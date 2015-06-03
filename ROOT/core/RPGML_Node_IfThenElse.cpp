/* This file is part of RPGML.
 *
 * Copyright (c) 2014, Gunnar Payer, All rights reserved.
 *
 * RPGML is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 3.0 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library.
 */
#include "RPGML_Node_IfThenElse.h"

// RPGML_CXXFLAGS=
// RPGML_LDFLAGS=

#include "RPGML_Block.h"
#include <algorithm>

using namespace std;

namespace RPGML {
namespace core {

IfThenElse::IfThenElse( GarbageCollector *_gc, const String &identifier, const RPGML::SharedObject *so )
: Node( _gc, identifier, so, NUM_INPUTS, NUM_OUTPUTS, NUM_PARAMS )
{
  DEFINE_INPUT ( INPUT_IN_IF  , "in_if"    );
  DEFINE_INPUT ( INPUT_IN_THEN, "in_then"  );
  DEFINE_INPUT ( INPUT_IN_ELSE, "in_else"  );
  DEFINE_OUTPUT( OUTPUT_OUT, "out" );
}

IfThenElse::~IfThenElse( void )
{}

const char *IfThenElse::getName( void ) const
{
  return "core.IfThenElse";
}

void IfThenElse::gc_clear( void )
{
  Base::gc_clear();
}

void IfThenElse::gc_getChildren( Children &children ) const
{
  Base::gc_getChildren( children );
}

namespace IfThenElse_impl {

template< class RetType >
class IfThenElseBlock : public Block< RetType >
{
  typedef Block< RetType > Base;
public:
  explicit
  IfThenElseBlock( const CountPtr< BlockBase > &in_if, const CountPtr< BlockBase > &in_then, const CountPtr< BlockBase > &in_else )
  : m_in_if  ( in_if  ->getAs< Block< bool    > >() )
  , m_in_then( in_then->getAs< Block< RetType > >() )
  , m_in_else( in_else->getAs< Block< RetType > >() )
  , m_buffer_if  ( nullptr, 1 )
  , m_buffer_then( nullptr, 1 )
  , m_buffer_else( nullptr, 1 )
  {}
  virtual ~IfThenElseBlock( void ) {}

  virtual bool next( index_t &n, index_t buffer_n, typename Array< RetType >::pointer buffer_p )
  {
    index_t n_if   = 0;
    index_t n_then = 0;
    index_t n_else = 0;
    m_buffer_if  .resize( buffer_n );
    m_buffer_then.resize( buffer_n );
    m_buffer_else.resize( buffer_n );
    const          Array< bool    >::pointer in_if   = m_buffer_if  .elements();
    const typename Array< RetType >::pointer in_then = m_buffer_then.elements();
    const typename Array< RetType >::pointer in_else = m_buffer_else.elements();

    m_in_if  ->next( n_if  , buffer_n, in_if   );
    m_in_then->next( n_then, buffer_n, in_then );
    m_in_else->next( n_else, buffer_n, in_else );

    if( n_if != n_then || n_then != n_else )
    {
      throw Exception()
        << "IfThenElseBlock: in_if, in_then and in_else delivered different n"
        << ": n_if   = " << n_if
        << ", n_then = " << n_then
        << ", n_else = " << n_else
        ;
    }

    if( n_if > buffer_n )
    {
      throw Exception()
        << "IfThenElseBlock: the n that was delivered is bigger than the block size"
        << ": n = " << n_if
        << ", block size = " << buffer_n
        ;
    }

    for( index_t i=0; i<n_if; ++i )
    {
      buffer_p[ i ] = ( in_if[ i ] ? in_then[ i ] : in_else[ i ] );
    }

    n = n_if;
    return ( n > 0 );
  }

private:
  const CountPtr< Block< bool    > > m_in_if  ;
  const CountPtr< Block< RetType > > m_in_then;
  const CountPtr< Block< RetType > > m_in_else;

  Array< bool    > m_buffer_if  ;
  Array< RetType > m_buffer_then;
  Array< RetType > m_buffer_else;
};


} // namespace IfThenElse_impl

using namespace IfThenElse_impl;

template< class RetType >
bool IfThenElse::tick2( const ArrayBase *in_if, const ArrayBase *in_then, const ArrayBase *in_else )
{
  const ArrayBase::Size &size = in_if->getSize();

  GET_OUTPUT_INIT( OUTPUT_OUT, out, RetType, size.getDims(), size.getCoords() );

  const Type ret_type( TypeOf< RetType >::E );
  CountPtr< Block< bool    > > cast_if   = createCastBlock< bool >( in_if );
  CountPtr< Block< RetType > > cast_then = createCastBlock< RetType >( in_then );
  CountPtr< Block< RetType > > cast_else = createCastBlock< RetType >( in_else );

  const bool then_is_scalar = ( in_then->getDims() == 0 );
  const bool else_is_scalar = ( in_else->getDims() == 0 );

  index_t n_left = in_if->size();

  const index_t block_size = min( n_left, index_t( 4096*4 ) );
  Array< bool    > block_if  ( nullptr, 1, block_size );
  Array< RetType > block_then( nullptr, 1, ( then_is_scalar ? 1 : block_size ) );
  Array< RetType > block_else( nullptr, 1, ( else_is_scalar ? 1 : block_size ) );

  if( then_is_scalar )
  {
    index_t n = 0;
    cast_then->next( n, 1, block_then.elements() );
    if( n != 1 ) throw Exception() << "Scalar sized 'then' could somehow not cast.";
  }
  if( else_is_scalar )
  {
    index_t n = 0;
    cast_else->next( n, 1, block_else.elements() );
    if( n != 1 ) throw Exception() << "Scalar sized 'else' could somehow not cast.";
  }

  typename Array< RetType >::iterator out_iter = out->begin();
  while( n_left )
  {
    const index_t buffer_n = min( n_left, block_size );

    {
      index_t n = 0;
      cast_if->next( n, buffer_n, block_if.elements() );
      if( n != buffer_n )
      {
        throw Exception() << "Could not get block from 'if'";
      }
    }

    if( !then_is_scalar )
    {
      index_t n = 0;
      cast_then->next( n, buffer_n, block_then.elements() );
      if( n != buffer_n )
      {
        throw Exception() << "Could not get block from 'then'";
      }
    }

    if( !else_is_scalar )
    {
      index_t n = 0;
      cast_else->next( n, buffer_n, block_else.elements() );
      if( n != buffer_n )
      {
        throw Exception() << "Could not get block from 'else'";
      }
    }

    Array< bool >::iterator             if_iter   = block_if  .begin();
    typename Array< RetType >::iterator then_iter = block_then.begin();
    typename Array< RetType >::iterator else_iter = block_else.begin();

    for( index_t i=0; i<buffer_n; ++i )
    {
      (*out_iter) = ( (*if_iter) ? (*then_iter) : (*else_iter) );
      ++if_iter;
      if( !then_is_scalar ) ++then_iter;
      if( !else_is_scalar ) ++else_iter;
      ++out_iter;
    }

    n_left -= buffer_n;
  }

  return true;
}

bool IfThenElse::tick( void )
{
  if( !hasAnyInputChanged() ) return true;
  setAllOutputChanged();

  GET_INPUT_BASE( INPUT_IN_IF  , in_if   );
  GET_INPUT_BASE( INPUT_IN_THEN, in_then );
  GET_INPUT_BASE( INPUT_IN_ELSE, in_else );

  const ArrayBase::Size in_if_size = in_if->getSize();

  if( 0 == in_if_size.getDims() )
  {
    const bool cond = getScalar< bool >( INPUT_IN_IF );
    getOutput( OUTPUT_OUT )->setData( const_cast< ArrayBase* >( cond ? in_then : in_else ) );
  }
  else
  {
    const ArrayBase::Size in_then_size = in_then->getSize();
    const ArrayBase::Size in_else_size = in_else->getSize();

    if( 0 != in_then_size.getDims() && in_then_size != in_if_size )
    {
      throw Exception()
        << "Size of 'then' must match the size of 'if', if neither is a scalar"
        << ": 'if' is " << in_if_size
        << ", 'else' is " << in_else_size
        ;
    }

    if( 0 != in_else_size.getDims() && in_else_size != in_if_size )
    {
      throw Exception()
        << "Size of 'else' must match the size of 'if', if neither is a scalar"
        << ": 'if' is " << in_if_size
        << ", 'else' is " << in_else_size
        ;
    }

    const Type ret_type = Type::Ret( in_then->getType(), in_else->getType() );

    switch( ret_type.getEnum() )
    {
      case Type::BOOL  : return tick2< bool     >( in_if, in_then, in_else );
      case Type::UINT8 : return tick2< uint8_t  >( in_if, in_then, in_else );
      case Type::INT8  : return tick2< int8_t   >( in_if, in_then, in_else );
      case Type::UINT16: return tick2< uint16_t >( in_if, in_then, in_else );
      case Type::INT16 : return tick2< int16_t  >( in_if, in_then, in_else );
      case Type::UINT32: return tick2< uint32_t >( in_if, in_then, in_else );
      case Type::INT32 : return tick2< int32_t  >( in_if, in_then, in_else );
      case Type::UINT64: return tick2< uint64_t >( in_if, in_then, in_else );
      case Type::INT64 : return tick2< int64_t  >( in_if, in_then, in_else );
      case Type::FLOAT : return tick2< float    >( in_if, in_then, in_else );
      case Type::DOUBLE: return tick2< double   >( in_if, in_then, in_else );
      case Type::STRING: return tick2< String   >( in_if, in_then, in_else );
      default:
        throw Exception()
          << "Return type of '" << ret_type << "' not supported (yet?) for IfThenElse"
          << ": Type of 'in_then' is " << in_then->getType()
          << ", Type of 'in_else' is " << in_else->getType()
          ;
    }

  }

  return true;
}

 } // namespace core {
} // namespace RPGML

RPGML_CREATE_NODE( IfThenElse, core:: )


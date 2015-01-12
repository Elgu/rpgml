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
#include "RPGML_Node_UnaryOp.h"

#include <algorithm>

using namespace std;

namespace RPGML {

UnaryOp::UnaryOp( GarbageCollector *_gc, const String &identifier, const RPGML::SharedObject *so )
: Node( _gc, identifier, so, NUM_INPUTS, NUM_OUTPUTS, NUM_PARAMS )
, m_op( UOP_PLUS )
{
  DEFINE_INPUT ( INPUT_IN  , "in"  );
  DEFINE_OUTPUT( OUTPUT_OUT, "out" );
  DEFINE_PARAM ( PARAM_OP  , "op", UnaryOp::set_op );
}

UnaryOp::~UnaryOp( void )
{}

const char *UnaryOp::getName( void ) const
{
  return "UnaryOp";
}

void UnaryOp::set_op( const Value &value, index_t, int, const index_t* )
{
  if( value.getType() != Type::String() )
  {
    throw Exception()
      << "Param 'op' must be set with string, is '" << value.getTypeName() << "'"
      ;
  }

  try
  {
    m_op = getUOP( value.getString() );
  }
  catch( const char * )
  {
    throw Exception() << "Invalid op '" << value << "'";
  }
}

namespace UnaryOp_impl {

template< class T, UOP OP > struct op_impl {};
template< class T > struct op_impl< T, UOP_MINUS   > { typedef T    Ret; Ret operator()( const T &x ) const { return  x; } };
template< class T > struct op_impl< T, UOP_PLUS    > { typedef T    Ret; Ret operator()( const T &x ) const { return -x; } };
template< class T > struct op_impl< T, UOP_LOG_NOT > { typedef bool Ret; Ret operator()( const T &x ) const { return !x; } };
template< class T > struct op_impl< T, UOP_BIT_NOT > { typedef T    Ret; Ret operator()( const T &x ) const { return ~x; } };

template<> struct op_impl< float, UOP_BIT_NOT > { float operator()( const float & ) const { throw UnaryOp::Exception( "binary not not supported for floating point types" ); } };

template< class T, UOP OP >
struct impl
{
  typedef op_impl< T, OP > Op;
  typedef typename Op::Ret Ret;

  bool operator()( const ArrayElements< T > *in, ArrayElements< Ret > *out ) const
  {
    const Op op;
    typename ArrayElements< Ret >::iterator o_iter = out->begin();
    typename ArrayElements< T >::const_iterator i_iter = in->begin();
    const index_t n = in->size();
    for( index_t i=0; i<n; ++i, ++i_iter, ++o_iter )
    {
      (*o_iter) = op( (*i_iter) );
    }

    return true;
  }

};

} // namespace UnaryOp_impl

template< UOP OP, class T >
bool UnaryOp::tick2( const ArrayBase *in_base )
{
  using namespace UnaryOp_impl;

  typedef impl< T, OP > Impl;
  typedef typename Impl::Ret Ret;
  const Impl impl;

  const ArrayElements< T > *in = 0;
  if( !in_base->getAs( in ) ) throw GetAsFailed( getInput( INPUT_IN ), in );

  const index_t dims = in->getDims();
  const index_t *const size = in->getSize();

  Output *output = getOutput( OUTPUT_OUT );
  output->initData< Ret >( dims, size );

  ArrayElements< Ret > *out = 0;
  if( !output->getData()->getAs( out ) ) throw GetAsFailed( getOutput( OUTPUT_OUT ), out );

  return impl( in, out );
}

template< UOP OP >
bool UnaryOp::tick1( void )
{
  const ArrayBase *const in_base = getInput( INPUT_IN )->getData();
  if( !in_base ) throw NotConnected( getInput( INPUT_IN ) );

  switch( in_base->getType() )
  {
    case Type::BOOL  : return tick2< OP, bool   >( in_base );
    case Type::INT   : return tick2< OP, int    >( in_base );
    case Type::FLOAT : return tick2< OP, float  >( in_base );
    default:
      throw Exception()
        << "unsupported type '" << in_base->getType().getTypeName() << "'"
        ;
  }

  return true;
}

template< UOP OP >
bool UnaryOp::tick1_int( void )
{
  const ArrayBase *const in_base = getInput( INPUT_IN )->getData();
  if( !in_base ) throw NotConnected( getInput( INPUT_IN ) );

  switch( in_base->getType() )
  {
    case Type::BOOL  : return tick2< OP, bool   >( in_base );
    case Type::INT   : return tick2< OP, int    >( in_base );
    default:
      throw Exception()
        << "unsupported type '" << in_base->getType().getTypeName() << "'"
        << ": Only integer types supported for op '" << getUOPStr( m_op ) << "'"
        ;
  }

  return true;
}

bool UnaryOp::tick( void )
{
  switch( m_op )
  {
    case UOP_MINUS  : return tick1    < UOP_MINUS   >();
    case UOP_PLUS   : return tick1    < UOP_PLUS    >();
    case UOP_LOG_NOT: return tick1    < UOP_LOG_NOT >();
    case UOP_BIT_NOT: return tick1_int< UOP_BIT_NOT >();
    default:
      throw Exception() << "unhandled uop";
  }

  return true;
}

void UnaryOp::gc_clear( void )
{
  Node::gc_clear();
}

void UnaryOp::gc_getChildren( Children &children ) const
{
  Node::gc_getChildren( children );
}

} // namespace RPGML

RPGML_CREATE_NODE( UnaryOp,  )


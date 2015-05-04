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
#include "RPGML_Node_Median.h"

// RPGML_CXXFLAGS=
// RPGML_LDFLAGS=

#include <algorithm>

using namespace std;

namespace RPGML {
namespace math {

Median::Median( GarbageCollector *_gc, const String &identifier, const RPGML::SharedObject *so )
: Node( _gc, identifier, so, NUM_INPUTS, NUM_OUTPUTS, NUM_PARAMS )
, m_in( new InputArray( _gc, 1, 1 ) )
, m_n( 1 )
{
  getInput( INPUT_IN0 )->init( String::Static( "in[0]" ) );
  (*m_in)[ 0 ] = getInput( INPUT_IN0 );
  push_back( String::Static( "in" ), Value( m_in ) );

  DEFINE_OUTPUT( OUTPUT_OUT, "out" );
  DEFINE_PARAM ( PARAM_N , "n", Median::set_n );
}

Median::~Median( void )
{}

const char *Median::getName( void ) const
{
  return "math.Median";
}

void Median::gc_clear( void )
{
  Base::gc_clear();
  m_in.clear();
}

void Median::gc_getChildren( Children &children ) const
{
  Base::gc_getChildren( children );
  children << m_in;
}

void Median::set_n( const Value &value, index_t, int, const index_t* )
{
  if( !value.isInteger() )
  {
    throw Exception() << "Param 'n' must be set with an integer.";
  }

  const index_t n = value.save_cast< index_t >();

  if( n < 1 ) throw Exception() << "Parameter 'n' must be greater than 0, is " << n;

  setNumInputs( INPUT_IN0+n );
  m_in->resize( n );
  for( index_t i=m_n; i<n; ++i )
  {
    Input *const in_i = getInput( INPUT_IN0+i );
    in_i->init( "in[" + toString( i ) + "]" );
    (*m_in)[ i ] = in_i;
  }

  m_n = n;
}

template< class T >
bool Median::tick2( const ArrayBase::Size &size )
{
  GET_OUTPUT_INIT( OUTPUT_OUT, out, T, size.getDims(), size.getCoords() );

  vector< T > buffer( m_n );
  vector< typename Array< T >::const_iterator > in_iter( m_n );

  for( index_t i=0; i<m_n; ++i )
  {
    GET_INPUT_AS_DIMS( INPUT_IN0+i, in_i, T, size.getDims() );
    in_iter[ i ] = in_i->begin();
  }

  const typename vector< T >::iterator buffer_begin = buffer.begin();
  const typename vector< T >::iterator buffer_end   = buffer.begin();
  const typename vector< T >::iterator buffer_nth   = buffer_begin + m_n/2;

  for(
    typename Array< T >::iterator
      out_iter( out->begin() )
    , out_end( out->end() )
    ; out_iter != out_end
    ; ++out_iter
    )
  {
    for( index_t i=0; i<m_n; ++i )
    {
      buffer[ i ] = (*in_iter[ i ]);
      ++in_iter[ i ];
    }

    nth_element( buffer_begin, buffer_nth, buffer_end );

    (*out_iter) = (*buffer_nth);
  }

  return true;
}

bool Median::tick( void )
{
  if( !hasAnyInputChanged() ) return true;
  setAllOutputChanged();

  if( m_n < 1 ) throw Exception() << "Parameter 'n' must be greater than 0, is " << m_n;

  GET_INPUT_BASE( INPUT_IN0, in0_base );

  const Type type = in0_base->getType();
  const ArrayBase::Size size = in0_base->getSize();

  for( index_t i=1; i<m_n; ++i )
  {
    GET_INPUT_BASE( INPUT_IN0+i, in_i_base );

    if( in_i_base->getType() != type )
    {
      throw Exception()
        << "All Inputs must have the same type"
        << ": in[ 0 ] is " << type
        << ", in[ " << i << " ] is " << in_i_base->getType()
        ;
    }

    if( in_i_base->getSize() != size )
    {
      throw Exception()
        << "All Inputs must have the same size"
        << ": in[ 0 ] is " << size
        << ", in[ " << i << " ] is " << in_i_base->getSize()
        ;
    }
  }

  switch( type.getEnum() )
  {
    case Type::BOOL  : return tick2< bool     >( size );
    case Type::UINT8 : return tick2< uint8_t  >( size );
    case Type::INT8  : return tick2< int8_t   >( size );
    case Type::UINT16: return tick2< uint16_t >( size );
    case Type::INT16 : return tick2< int16_t  >( size );
    case Type::UINT32: return tick2< uint32_t >( size );
    case Type::INT32 : return tick2< int32_t  >( size );
    case Type::UINT64: return tick2< uint64_t >( size );
    case Type::INT64 : return tick2< int64_t  >( size );
    case Type::FLOAT : return tick2< float    >( size );
    case Type::DOUBLE: return tick2< double   >( size );
    default:
      throw IncompatibleOutput( getInput( INPUT_IN0 ) );
  }

  return true;
}

 } // namespace math {
} // namespace RPGML

RPGML_CREATE_NODE( Median, math:: )


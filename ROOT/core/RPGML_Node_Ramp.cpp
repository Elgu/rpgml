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
#include "RPGML_Node_Ramp.h"

// RPGML_CXXFLAGS=
// RPGML_LDFLAGS=

#include <algorithm>

using namespace std;

namespace RPGML {
namespace core {

Ramp::Ramp( GarbageCollector *_gc, const String &identifier, const RPGML::SharedObject *so )
: Node( _gc, identifier, so, NUM_INPUTS, NUM_OUTPUTS, NUM_PARAMS )
{
  DEFINE_INPUT ( INPUT_SIZEX, "sizeX" );
  DEFINE_INPUT ( INPUT_SIZEY, "sizeY" );
  DEFINE_INPUT ( INPUT_SIZEZ, "sizeZ" );
  DEFINE_INPUT ( INPUT_SIZET, "sizeT" );
  DEFINE_INPUT ( INPUT_C0, "c0" );
  DEFINE_INPUT ( INPUT_CX, "cx" );
  DEFINE_INPUT ( INPUT_CY, "cy" );
  DEFINE_INPUT ( INPUT_CZ, "cz" );
  DEFINE_INPUT ( INPUT_CT, "ct" );
  DEFINE_OUTPUT( OUTPUT_OUT, "out" );
  DEFINE_PARAM ( PARAM_TYPE, "type", Ramp::set_type );
  DEFINE_PARAM ( PARAM_DIMS, "dims", Ramp::set_dims );

  CountPtr< InputArray > size_array = new InputArray( getGC(), 1, 4 );
  (*size_array)[ 0 ] = getInput( INPUT_SIZEX );
  (*size_array)[ 1 ] = getInput( INPUT_SIZEY );
  (*size_array)[ 2 ] = getInput( INPUT_SIZEZ );
  (*size_array)[ 3 ] = getInput( INPUT_SIZET );
  push_back( String::Static( "size" ), Value( size_array ) );

  CountPtr< InputArray > c_array = new InputArray( getGC(), 1, 4 );
  (*c_array)[ 0 ] = getInput( INPUT_CX );
  (*c_array)[ 1 ] = getInput( INPUT_CY );
  (*c_array)[ 2 ] = getInput( INPUT_CZ );
  (*c_array)[ 3 ] = getInput( INPUT_CT );
  push_back( String::Static( "c" ), Value( c_array ) );
}

Ramp::~Ramp( void )
{}

const char *Ramp::getName( void ) const
{
  return "core.Ramp";
}

void Ramp::gc_clear( void )
{
  Base::gc_clear();
}

void Ramp::gc_getChildren( Children &children ) const
{
  Base::gc_getChildren( children );
}

void Ramp::set_type( const Value &value, index_t, int, const index_t* )
{
  if( !value.isString() )
  {
    throw Exception() << "Param 'type' must be set with a string";
  }

  m_type = Type( value.getString().c_str() );

  if( !m_type.isScalar() )
  {
    throw Exception() << "Unsupported type " << m_type << ", must be scalar";
  }
}

void Ramp::set_dims( const Value &value, index_t, int, const index_t* )
{
  if( !value.isInteger() )
  {
    throw Exception() << "Param 'dims' must be set with an integer";
  }

  const int dims = value.to< int >();
  if( dims < 0 || dims > 4 )
  {
    throw Exception()
      << "Parameter 'dims' must be from 0 to 4, is " << dims
      ;
  }

  m_dims = dims;
}

template< class Element >
bool Ramp::tick2( const index_t *size )
{
  const int dims = m_dims;
  const Element c0 = getScalar< Element >( INPUT_C0 );
  Element c[ dims ];

  for( int d=0; d<m_dims; ++d )
  {
    c[ d ] = getScalar< Element >( INPUT_CX+d );
  }

  GET_OUTPUT_INIT( OUTPUT_OUT, out, Element, m_dims, size );
  out->resize_v( m_dims, size );

  typedef typename Array< Element >::iterator iterator;
  for( iterator i( out->begin() ), end( out->end() ); i != end; ++i )
  {
    const index_t *const pos = i.getPos();
    Element out_i = c0;
    for( int d=0; d<dims; ++d )
    {
      // Do not use += because of warning
      out_i = Element( out_i + Element( pos[ d ] ) * c[ d ] );
    }
    (*i) = out_i;
  }

  return true;
}

bool Ramp::tick( void )
{
  if( !hasAnyInputChanged() ) return true;
  setAllOutputChanged();

  const int dims = m_dims;
  index_t size[ dims ];

  for( int d=0; d<m_dims; ++d )
  {
    size[ d ] = getScalar< index_t >( INPUT_SIZEX+d );
  }

  switch( m_type.getEnum() )
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
      throw Exception() << "Unsupported type " << m_type;
  }

  return true;
}

 } // namespace core {
} // namespace RPGML

RPGML_CREATE_NODE( Ramp, core:: )


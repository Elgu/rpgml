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
#include "RPGML_Node_Random.h"

// RPGML_CXXFLAGS=
// RPGML_LDFLAGS=

#include <cstdlib>
#include <functional>

using namespace std;

namespace RPGML {
namespace util {

Random::Random( GarbageCollector *_gc, const String &identifier, const RPGML::SharedObject *so )
: Node( _gc, identifier, so, NUM_INPUTS, NUM_OUTPUTS, NUM_PARAMS )
, m_const_seed( 0 )
, m_dims( 0 )
{
  DEFINE_INPUT ( INPUT_SEED, "seed"  );
  DEFINE_INPUT ( INPUT_SIZEX, "sizeX" );
  DEFINE_INPUT ( INPUT_SIZEY, "sizeY" );
  DEFINE_INPUT ( INPUT_SIZEZ, "sizeZ" );
  DEFINE_INPUT ( INPUT_SIZET, "sizeT" );

  DEFINE_OUTPUT( OUTPUT_OUT, "out" );

  DEFINE_PARAM ( PARAM_CONST_SEED, "const_seed", Random::set_const_seed );
  DEFINE_PARAM ( PARAM_TYPE, "type", Random::set_type );
  DEFINE_PARAM ( PARAM_DIMS, "dims", Random::set_dims );

  CountPtr< InputArray > size_array = new InputArray( getGC(), 1, 4 );
  (*size_array)[ 0 ] = getInput( INPUT_SIZEX );
  (*size_array)[ 1 ] = getInput( INPUT_SIZEY );
  (*size_array)[ 2 ] = getInput( INPUT_SIZEZ );
  (*size_array)[ 3 ] = getInput( INPUT_SIZET );
  push_back( String::Static( "size" ), Value( size_array ) );
}

Random::~Random( void )
{}

const char *Random::getName( void ) const
{
  return "util.Random";
}

void Random::gc_clear( void )
{
  Base::gc_clear();
}

void Random::gc_getChildren( Children &children ) const
{
  Base::gc_getChildren( children );
}

void Random::set_const_seed( const Value &value, index_t, int, const index_t* )
{
  if( value.isString() )
  {
    m_const_seed = std::hash< std::string >()( value.getString().c_str() );
  }
  else if( value.isInteger() )
  {
    m_const_seed = value.to< uint64_t >();
  }
  else
  {
    throw Exception()
      << "Parameter 'const_seed' must be string or integer, is " << value.getType()
      ;
  }
}

void Random::set_dims( const Value &value, index_t, int, const index_t* )
{
  if( !value.isInteger() )
  {
    throw Exception() << "Param 'dims' must be set with an integer, is " << value.getType();
  }

  int dims = value.to< int >();

  if( dims < 0 || dims > 4 )
  {
    throw Exception() << "Param 'dims' must be from 0 to 4, is " << dims;
  }

  m_dims = dims;
}

void Random::set_type( const Value &value, index_t, int, const index_t* )
{
  if( !value.isString() )
  {
    throw Exception() << "Param 'type' must be set with a type string, is " << value.getType();
  }

  const Type type( value.getString().c_str() );

  if( !type.isScalar() )
  {
    throw Exception() << "Param 'type' must be a scalar, is '" << value << "', " << type;
  }

  m_type = type;
}

namespace Random_impl {

  static void rand48( unsigned short xsubi[3], bool     &ret ) { ret = static_cast< bool     >( nrand48( xsubi ) ); }
  static void rand48( unsigned short xsubi[3], uint8_t  &ret ) { ret = static_cast< uint8_t  >( nrand48( xsubi ) ); }
  static void rand48( unsigned short xsubi[3], int8_t   &ret ) { ret = static_cast< int8_t   >( jrand48( xsubi ) ); }
  static void rand48( unsigned short xsubi[3], uint16_t &ret ) { ret = static_cast< uint16_t >( nrand48( xsubi ) ); }
  static void rand48( unsigned short xsubi[3], int16_t  &ret ) { ret = static_cast< int16_t  >( jrand48( xsubi ) ); }
  static void rand48( unsigned short xsubi[3], uint32_t &ret ) { ret = static_cast< uint32_t >( nrand48( xsubi ) ); }
  static void rand48( unsigned short xsubi[3], int32_t  &ret ) { ret = static_cast< int32_t  >( jrand48( xsubi ) ); }
  static void rand48( unsigned short xsubi[3], uint64_t &ret ) { ret = static_cast< uint64_t >( nrand48( xsubi ) ); }
  static void rand48( unsigned short xsubi[3], int64_t  &ret ) { ret = static_cast< int64_t  >( jrand48( xsubi ) ); }
  static void rand48( unsigned short xsubi[3], float    &ret ) { ret = static_cast< float    >( erand48( xsubi ) ); }
  static void rand48( unsigned short xsubi[3], double   &ret ) { ret = static_cast< double   >( erand48( xsubi ) ); }

} // namespace Random_impl

template< class Element >
bool Random::tick2( const index_t *size, uint64_t seed )
{
  typedef Array< Element > OutArray;
  CountPtr< OutArray > out = new OutArray( getGC(), m_dims, size );

  unsigned short xsubi[ 3 ];
  xsubi[ 0 ] = (unsigned short)( seed >>  0 );
  xsubi[ 1 ] = (unsigned short)( seed >> 16 );
  xsubi[ 2 ] = (unsigned short)( seed >> 32 );

  Element r = Element();
  for( typename OutArray::iterator o( out->begin() ), end( out->end() ); o != end; ++o )
  {
    Random_impl::rand48( xsubi, r );
    (*o) = r;
  }

  getOutput( OUTPUT_OUT )->setData( out );
  return true;
}

bool Random::tick( void )
{
  if( !hasAnyInputChanged() ) return true;
  setAllOutputChanged();

  if( m_type.isNil() )
  {
    throw Exception() << "Param 'type' was not set";
  }

  index_t size[ m_dims ];
  for( int d=0; d<m_dims; ++d )
  {
    size[ d ] = getScalar< index_t >( INPUT_SIZEX+d );
  }

  uint64_t seed = m_const_seed;

  GET_INPUT_BASE_IF_CONNECTED( INPUT_SEED, seed_base );
  if( seed_base )
  {
    if( seed_base->getType().isString() )
    {
      seed = std::hash< std::string >()( getScalar< String >( INPUT_SEED ).c_str() );
    }
    else
    {
      seed = getScalar< uint64_t >( INPUT_SEED );
    }
  }

  switch( m_type.getEnum() )
  {
    case Type::BOOL  : return tick2< bool     >( size, seed );
    case Type::UINT8 : return tick2< uint8_t  >( size, seed );
    case Type::INT8  : return tick2< int8_t   >( size, seed );
    case Type::UINT16: return tick2< uint16_t >( size, seed );
    case Type::INT16 : return tick2< int16_t  >( size, seed );
    case Type::UINT32: return tick2< uint32_t >( size, seed );
    case Type::INT32 : return tick2< int32_t  >( size, seed );
    case Type::UINT64: return tick2< uint64_t >( size, seed );
    case Type::INT64 : return tick2< int64_t  >( size, seed );
    case Type::FLOAT : return tick2< float    >( size, seed );
    case Type::DOUBLE: return tick2< double   >( size, seed );
    default:
      throw Exception() << "Output type must be scalar, is " << m_type;
  }

  return true;
}

 } // namespace util {
} // namespace RPGML

RPGML_CREATE_NODE( Random, util:: )


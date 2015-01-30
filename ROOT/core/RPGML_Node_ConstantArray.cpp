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
#include "RPGML_Node_ConstantArray.h"

// RPGML_CXXFLAGS=
// RPGML_LDFLAGS=

#include <algorithm>
#include <iostream>

using namespace std;

namespace RPGML {
namespace core {

ConstantArray::ConstantArray( GarbageCollector *_gc, const String &identifier, const RPGML::SharedObject *so )
: Node( _gc, identifier, so, NUM_INPUTS, NUM_OUTPUTS, NUM_PARAMS )
, m_fill( Value() )
, m_type( Type::Nil() )
, m_dims( -1 )
, m_first( true )
{
  for( int i=0; i<4; ++i ) m_size[ i ] = unknown;

  DEFINE_OUTPUT( OUTPUT_OUT, "out" );
  DEFINE_PARAM ( PARAM_TYPE , "type", ConstantArray::set_type );
  DEFINE_PARAM ( PARAM_DIMS , "dims", ConstantArray::set_dims );
  DEFINE_PARAM ( PARAM_FILL , "fill", ConstantArray::set_fill );
  DEFINE_PARAM_INDEX( PARAM_SIZEX, "sizeX", ConstantArray::set_size, 0 );
  DEFINE_PARAM_INDEX( PARAM_SIZEY, "sizeY", ConstantArray::set_size, 1 );
  DEFINE_PARAM_INDEX( PARAM_SIZEZ, "sizeZ", ConstantArray::set_size, 2 );
  DEFINE_PARAM_INDEX( PARAM_SIZET, "sizeT", ConstantArray::set_size, 3 );
  DEFINE_PARAM_ARRAY( PARAM_IN   , "in"   , ConstantArray::set_in );
}

ConstantArray::~ConstantArray( void )
{}

const char *ConstantArray::getName( void ) const
{
  return "core.ConstantArray";
}

void ConstantArray::set_type( const Value &value, index_t, int, const index_t* )
{
  if( !value.isString() )
  {
    throw Exception()
      << "Param 'type' must be set with string, is " << value.getTypeName()
      ;
  }

  if( !m_type.isNil() )
  {
    throw Exception()
      << "Param 'type' must be set exactly once, was set to " << m_type
      ;
  }

  m_type = Type( Type::getTypeEnum( value.getString() ) );
  if( !m_type.isPrimitive() )
  {
    const Type tmp_type = m_type;
    m_type = Type::Nil();
    throw Exception()
      << "Param 'type' must be set to a primitive type, is " << tmp_type
      ;
  }

  if( all_set() ) create_array();
}

void ConstantArray::set_dims( const Value &value, index_t, int, const index_t* )
{
  if( !value.isInteger() )
  {
    throw Exception() << "Param 'dims' must be set with an integer, is " << value.getTypeName();
  }

  if( m_dims >= 0 )
  {
    throw Exception() << "Param 'dims' must be set exactly once, is " << m_dims;
  }

  m_dims = value.to( Type::Int() ).getInt();
  if( m_dims < 0 )
  {
    throw Exception()
      << "Param 'dims' must not be negative, is " << m_dims
      ;
  }

  if( all_set() ) create_array();
}

void ConstantArray::set_fill( const Value &value, index_t, int, const index_t* )
{
  if( value.isNil() ) return;

  if( !value.isPrimitive() )
  {
    throw Exception()
      << "Param 'value' must be primitive, is " << value.getTypeName()
      ;
  }

  if( !m_fill.isNil() )
  {
    throw Exception()
      << "Param 'fill' must be set at most once, is set to"
      << " '" << m_fill << "' (" << m_fill.getTypeName() << ")"
      ;
  }

  if( m_dims >= 0 || !m_type.isNil() || !m_array.isNull() )
  {
    throw Exception()
      << "Param 'fill' must be set before setting 'type', 'dims', or any array values"
      ;
  }

  m_fill = value;
}

void ConstantArray::set_size( const Value &value, index_t d, int, const index_t* )
{
  if( d > 4 ) throw Exception() << "Only 4 dimensions supported";

  if( m_size[ d ] != unknown )
  {
    throw Exception() << "Size Params must be set exactly once.";
  }

  try
  {
    m_size[ d ] = value.to( TypeOf< index_t >::E ).get< index_t >();
  }
  catch( const RPGML::Exception &e )
  {
    throw Exception()
      << e.what() << ": Sizes must be set with integers"
      ;
  }

  if( all_set() ) create_array();
}

void ConstantArray::set_in( const Value &value, index_t, int n_coords, const index_t *coords )
{
  if( m_array.isNull() ) create_array();

  if( n_coords != m_dims )
  {
    throw Exception()
      << "Param 'in' has " << m_dims << " coordinates, specified " << n_coords
      ;
  }

  try
  {
    m_array->setValue_v( value.to( m_type ), n_coords, coords );
  }
  catch( const ArrayBase::Exception &e )
  {
    throw Exception()
      << "Could not set Param 'in'"
      << ": Failed to set Array value"
      << ": " << e.what()
      ;
  }
  catch( const Value::Exception &e )
  {
    throw Exception()
      << "Could not set Param 'in'"
      << ": Failed to convert value"
      << ": " << e.what()
      ;
  }
}

bool ConstantArray::all_set( void ) const
{
  if( m_type.isNil() || m_dims < 0 )
  {
    return false;
  }
  for( int d=0; d<m_dims; ++d )
  {
    if( m_size[ d ] == unknown )
    {
      return false;
    }
  }
  return true;
}

void ConstantArray::create_array( void )
{
  if( m_type.isNil() || m_dims < 0 )
  {
    throw Exception()
      << "Either Parm 'type' or Param 'dims' was not set"
      << ", 'type' is " << m_type
      << ", 'dims' is " << m_dims
      ;
  }
  for( int d=0; d<m_dims; ++d )
  {
    if( m_size[ d ] == unknown )
    {
      throw Exception()
        << "Param 'size" << ("XYZT"[ d ]) << "' was not specified"
        ;
    }
  }

  m_array = new_Array( getGC(), m_type, m_dims );
  m_array->resize_v( m_dims, m_size );

  if( !m_fill.isNil() )
  {
    try
    {
      const Value fill_value = m_fill.save_cast( m_type );
      m_array->fillValue( fill_value );
    }
    catch( const RPGML::Exception &e )
    {
      throw Exception()
        << "Could not fill array with 'fill'"
        << ": " << e.what()
        ;
    }
  }
}

bool ConstantArray::tick( void )
{
  if( m_first )
  {
    if( m_array.isNull() ) create_array();
    getOutput( OUTPUT_OUT )->setData( m_array );
    m_first = false;
  }

  return true;
}

void ConstantArray::gc_clear( void )
{
  Base::gc_clear();
  m_array.reset();
}

void ConstantArray::gc_getChildren( Children &children ) const
{
  Base::gc_getChildren( children );
  children << m_array;
}

} // namespace core
} // namespace RPGML

RPGML_CREATE_NODE( ConstantArray, core:: )


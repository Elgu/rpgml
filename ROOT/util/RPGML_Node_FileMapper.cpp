/* This file is part of RPGML.
 *
 * Copyright (c) 2015, Gunnar Payer, All rights reserved.
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
#include "RPGML_Node_FileMapper.h"

#include "RPGML_util.h"

// RPGML_CXXFLAGS=
// RPGML_LDFLAGS=

#include <algorithm>

using namespace std;

namespace RPGML {
namespace util {

FileMapper::FileMapper( GarbageCollector *_gc, const String &identifier, const RPGML::SharedObject *so )
: Node( _gc, identifier, so, NUM_INPUTS, NUM_OUTPUTS, NUM_PARAMS )
{
  DEFINE_INPUT ( INPUT_FILENAME,     "filename"  );
  DEFINE_INPUT ( INPUT_TYPE,         "type"  );
  DEFINE_INPUT ( INPUT_DIMS,         "dims"  );
  DEFINE_INPUT ( INPUT_SX,           "sx"  );
  DEFINE_INPUT ( INPUT_SY,           "sy"  );
  DEFINE_INPUT ( INPUT_SZ,           "sz"  );
  DEFINE_INPUT ( INPUT_ST,           "st"  );
  DEFINE_INPUT ( INPUT_OFFSET_BYTES, "offset_bytes"  );
  DEFINE_OUTPUT( OUTPUT_OUT, "out" );

  fill( m_size, m_size+4, index_t( 0 ) );
}

FileMapper::~FileMapper( void )
{}

const char *FileMapper::getName( void ) const
{
  return "util.FileMapper";
}

void FileMapper::gc_clear( void )
{
  Base::gc_clear();
}

void FileMapper::gc_getChildren( Children &children ) const
{
  Base::gc_getChildren( children );
}

template< class Element >
void FileMapper::tick2( const char *filename, size_t offset_bytes, int dims, const index_t *size )
{
  CountPtr< MappedFile< Element > > mapped = new MappedFile< Element >( getGC(), dims, size );
  mapped->map_ro( filename, offset_bytes );

  CountPtr< Array< Element > > out = new Array< Element >( getGC(), mapped );

  getOutput( OUTPUT_OUT )->setData( out );
}

bool FileMapper::tick( void )
{
  if( !hasAnyInputChanged() ) return true;

  const String filename( getScalar< String >( INPUT_FILENAME ) );

  const Type    type    ( getScalar< String >( INPUT_TYPE     ).c_str() );
  if( !type.isScalar() || type.isBool() )
  {
    throw Exception()
      << "Input 'type' must be a scalar, but not bool"
      << ", is " << type
      ;
  }

  const int     dims    ( getScalar< int    >( INPUT_DIMS     ) );
  if( dims < 0 || dims > 4 )
  {
    throw Exception()
      << "Input 'dims' muste be from 0 to 4"
      << ", is " << dims
      ;
  }

  index_t size[ 4 ];
  for( int d=0; d<4; ++d )
  {
    size[ d ] = getScalarIfConnected< index_t >( INPUT_SX+d, unknown );
    if( d < dims && size[ d ] == unknown )
    {
#include "RPGML_util.h"
      throw Exception()
        << "Input 'dims' is " << dims
        << ", but s" << ("xyzt"[ d ]) << " is not connected"
        ;
    }
  }

  const size_t offset_bytes( getScalarIfConnected< size_t >( INPUT_OFFSET_BYTES, size_t( 0 ) ) );

  ArrayBase::Size old_size( m_dims, m_size );
  ArrayBase::Size new_size( dims, size );

  if(
       m_offset_bytes == offset_bytes
    && m_size == size
    && m_filename == filename
    )
  {
    // Nothing to do
    return true;
  }

  switch( type.getEnum() )
  {
    case Type::UINT8 : tick2< uint8_t  >( filename, offset_bytes, dims, size ); break;
    case Type::INT8  : tick2< int8_t   >( filename, offset_bytes, dims, size ); break;
    case Type::UINT16: tick2< uint16_t >( filename, offset_bytes, dims, size ); break;
    case Type::INT16 : tick2< int16_t  >( filename, offset_bytes, dims, size ); break;
    case Type::UINT32: tick2< uint32_t >( filename, offset_bytes, dims, size ); break;
    case Type::INT32 : tick2< int32_t  >( filename, offset_bytes, dims, size ); break;
    case Type::UINT64: tick2< uint64_t >( filename, offset_bytes, dims, size ); break;
    case Type::INT64 : tick2< int64_t  >( filename, offset_bytes, dims, size ); break;
    case Type::FLOAT : tick2< float    >( filename, offset_bytes, dims, size ); break;
    case Type::DOUBLE: tick2< double   >( filename, offset_bytes, dims, size ); break;
    default:
      throw Exception()
        << "Internal: Unexpected type " << type
        ;
  }


  m_offset_bytes = offset_bytes;
  m_filename = filename;
  copy( size, size+4, m_size );
  m_dims = dims;

  setAllOutputChanged();
  return true;
}

 } // namespace util {
} // namespace RPGML

RPGML_CREATE_NODE( FileMapper, util:: )


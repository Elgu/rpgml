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
#include "RPGML_Node_Get.h"

// RPGML_CXXFLAGS=
// RPGML_LDFLAGS=

#include <algorithm>

using namespace std;

namespace RPGML {
namespace core {

Get::Get( GarbageCollector *_gc, const String &identifier, const RPGML::SharedObject *so )
: Node( _gc, identifier, so, NUM_INPUTS, NUM_OUTPUTS, NUM_PARAMS )
{
  DEFINE_INPUT      ( INPUT_IN   , "in"  );
  DEFINE_OUTPUT_INIT( OUTPUT_ARRAY_TYPE, "array_type", String , 0 );
  DEFINE_OUTPUT_INIT( OUTPUT_DIMS, "dims", int    , 0 );
  DEFINE_OUTPUT_INIT( OUTPUT_SIZE, "size", index_t, 1 );
}

Get::~Get( void )
{}

const char *Get::getName( void ) const
{
  return "core.Get";
}

bool Get::tick( void )
{
  if( !hasAnyInputChanged() ) return true;

  GET_INPUT_DATA( INPUT_IN, in_base );

  GET_OUTPUT_IF_CONNECTED( OUTPUT_ARRAY_TYPE, array_type, String , 0 );
  GET_OUTPUT_IF_CONNECTED( OUTPUT_DIMS, dims, int    , 0 );
  GET_OUTPUT_IF_CONNECTED( OUTPUT_SIZE, size, index_t, 1 );

  const ArrayBase::Size in_size = in_base->getSize();
  const int in_dims = in_size.getDims();

  if( array_type )
  {
    const char *const in_type = in_base->getTypeName();
    if( array_type && (**array_type) != in_type )
    {
      (**array_type) = in_type;
      getOutput( OUTPUT_ARRAY_TYPE )->setChanged();
    }
  }

  if( dims )
  {
    if( (**dims) != in_dims )
    {
      (**dims) = in_dims;
      getOutput( OUTPUT_DIMS )->setChanged();
    }
  }

  if( size )
  {
    bool size_changed = false;

    const int size_n = max( 4, in_dims );
    if( index_t( size_n ) != size->size() )
    {
      size->resize( size_n );
      size_changed = true;
    }

    for( int i=0; i<in_dims; ++i )
    {
      index_t &size_i = size->at( i );
      if( size_i != in_size[ i ] )
      {
        size_i = in_size[ i ];
        size_changed = true;
      }
    }
    for( int i=in_dims; i<size_n; ++i )
    {
      index_t &size_i = size->at( i );
      if( size_i != 1 )
      {
        size_i = 1;
        size_changed = true;
      }
    }
    if( size_changed )
    {
      getOutput( OUTPUT_SIZE )->setChanged();
    }
  }

  return true;
}

void Get::gc_clear( void )
{
  Node::gc_clear();
}

void Get::gc_getChildren( Children &children ) const
{
  Node::gc_getChildren( children );
}

} // namespace core
} // namespace RPGML

RPGML_CREATE_NODE( Get, core:: )


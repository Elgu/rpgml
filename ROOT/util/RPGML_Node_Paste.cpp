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
#include "RPGML_Node_Paste.h"

// RPGML_CXXFLAGS=
// RPGML_LDFLAGS=

#include <algorithm>

using namespace std;

namespace RPGML {
namespace util {

Paste::Paste( GarbageCollector *_gc, const String &identifier, const RPGML::SharedObject *so )
: Node( _gc, identifier, so, NUM_INPUTS, NUM_OUTPUTS, NUM_PARAMS )
{
  DEFINE_INPUT ( INPUT_IN  , "in"  );
  DEFINE_INPUT ( INPUT_DEST, "dest"  );
  DEFINE_INPUT ( INPUT_X   , "x"  );
  DEFINE_INPUT ( INPUT_Y   , "y"  );
  DEFINE_INPUT ( INPUT_Z   , "z"  );
  DEFINE_INPUT ( INPUT_T   , "t"  );
  DEFINE_OUTPUT( OUTPUT_OUT, "out" );
}

Paste::~Paste( void )
{}

const char *Paste::getName( void ) const
{
  return "util.Paste";
}

void Paste::gc_clear( void )
{
  Base::gc_clear();
}

void Paste::gc_getChildren( Children &children ) const
{
  Base::gc_getChildren( children );
}

template< class T >
bool Paste::tick2(
    const ArrayBase *in_base
  , const ArrayBase *dest_base
  , int dims
  , const index_t *pos_in
  , const index_t *pos_dest
  , const index_t *size
  )
{
  const Array< T > *const in   = in_base  ->getAs< Array< T > >();
  const Array< T > *const dest = dest_base->getAs< Array< T > >();

  CountPtr< Array< T > > out = dest-> template cloneAs< T >();

  Array< T > roi_in ( (*in)  ); roi_in .setROI( dims, pos_in  , size );
  Array< T > roi_out( (*out) ); roi_out.setROI( dims, pos_dest, size );

  copy( roi_in.begin(), roi_in.end(), roi_out.begin() );

  getOutput( OUTPUT_OUT )->setData( out );
  return true;
}

bool Paste::tick( void )
{
  if( !hasAnyInputChanged() ) return true;
  setAllOutputChanged();

  GET_INPUT_BASE( INPUT_IN  , in_base   );
  GET_INPUT_BASE( INPUT_DEST, dest_base );

  const int dims = in_base->getDims();

  if( dest_base->getDims() != dims )
  {
    throw Exception()
      << "Inputs 'in' and 'dest' have different number of dimensions"
      << ": 'in' has " << dims
      << ", 'dest' has " << dest_base->getDims()
      ;
  }

  const Type type = in_base->getType();

  if( dest_base->getType() != type )
  {
    throw Exception()
      << "Inputs 'in' and 'dest' have different type, this is not supported (yet)"
      << ": 'in' is " << type
      << ", 'dest' is " << dest_base->getType()
      ;
  }

  if( 0 == dims )
  {
    getOutput( OUTPUT_OUT )->setData( const_cast< ArrayBase* >( in_base ) );
    return true;
  }

  const ArrayBase::Size size_in   = in_base->getSize();
  const ArrayBase::Size size_dest = dest_base->getSize();

  assert( dims > 0 );
  index_t pos_in  [ dims ];
  index_t pos_dest[ dims ];
  index_t size    [ dims ];

  for( int d=0; d<dims; ++d )
  {
    const stride_t pos_d = getScalar< stride_t >( INPUT_X+d );
    if(
         pos_d >=  stride_t( size_dest[ d ] )
      || pos_d <= -stride_t( size_in[ d ] )
    )
    {
      // Out of bounds
      getOutput( OUTPUT_OUT )->setData( const_cast< ArrayBase* >( dest_base ) );
      return true;
    }

    if( pos_d < 0 )
    {
      pos_dest[ d ] = 0;
      pos_in[ d ] = index_t( -pos_d );
    }
    else
    {
      pos_dest[ d ] = pos_d;
      pos_in[ d ] = 0;
    }

    size[ d ] = min( size_in[ d ] - pos_in[ d ], size_dest[ d ] - pos_dest[ d ] );
  }

  for( int d=dims; d<4; ++d )
  {
    if( getInput( INPUT_X+d )->isConnected() )
    {
      throw Exception()
        << "Input '" << ("xyzt"[ d ]) << "' must not be connected"
        << ", if dims is less than " << (d+1)
        << ": is " << dims
        ;
    }
  }

  switch( type.getEnum() )
  {
    case Type::BOOL  : return tick2< bool     >( in_base, dest_base, dims, pos_in, pos_dest, size );
    case Type::UINT8 : return tick2< uint8_t  >( in_base, dest_base, dims, pos_in, pos_dest, size );
    case Type::INT8  : return tick2< int8_t   >( in_base, dest_base, dims, pos_in, pos_dest, size );
    case Type::UINT16: return tick2< uint16_t >( in_base, dest_base, dims, pos_in, pos_dest, size );
    case Type::INT16 : return tick2< int16_t  >( in_base, dest_base, dims, pos_in, pos_dest, size );
    case Type::UINT32: return tick2< uint32_t >( in_base, dest_base, dims, pos_in, pos_dest, size );
    case Type::INT32 : return tick2< int32_t  >( in_base, dest_base, dims, pos_in, pos_dest, size );
    case Type::UINT64: return tick2< uint64_t >( in_base, dest_base, dims, pos_in, pos_dest, size );
    case Type::INT64 : return tick2< int64_t  >( in_base, dest_base, dims, pos_in, pos_dest, size );
    case Type::FLOAT : return tick2< float    >( in_base, dest_base, dims, pos_in, pos_dest, size );
    case Type::DOUBLE: return tick2< double   >( in_base, dest_base, dims, pos_in, pos_dest, size );
    case Type::STRING: return tick2< String   >( in_base, dest_base, dims, pos_in, pos_dest, size );
    default:
      throw IncompatibleOutput( getInput( INPUT_IN ) );
  }

  return true;
}

 } // namespace util {
} // namespace RPGML

RPGML_CREATE_NODE( Paste, util:: )


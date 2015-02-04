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
#include "RPGML_Node_At.h"

// RPGML_CXXFLAGS=
// RPGML_LDFLAGS=

#include <algorithm>
#include <iostream>

using namespace std;

namespace RPGML {
namespace core {

At::At( GarbageCollector *_gc, const String &identifier, const RPGML::SharedObject *so )
: Node( _gc, identifier, so, NUM_INPUTS, NUM_OUTPUTS, NUM_PARAMS )
{
  DEFINE_INPUT( INPUT_IN, "in"  );
  DEFINE_INPUT( INPUT_X , "x" );
  DEFINE_INPUT( INPUT_Y , "y" );
  DEFINE_INPUT( INPUT_Z , "z" );
  DEFINE_INPUT( INPUT_T , "t" );
  DEFINE_OUTPUT( OUTPUT_OUT, "out" );
}

At::~At( void )
{}

const char *At::getName( void ) const
{
  return "core.At";
}

template< class InType >
bool At::tick1( const ArrayBase *in_base, Type coord_type, int coord_dims )
{
  switch( coord_type.getEnum() )
  {
    case Type::BOOL  : return tick2< InType, bool     >( in_base, coord_dims );
    case Type::UINT8 : return tick2< InType, uint8_t  >( in_base, coord_dims );
    case Type::INT8  : return tick2< InType, int8_t   >( in_base, coord_dims );
    case Type::UINT16: return tick2< InType, uint16_t >( in_base, coord_dims );
    case Type::INT16 : return tick2< InType, int16_t  >( in_base, coord_dims );
    case Type::UINT32: return tick2< InType, uint32_t >( in_base, coord_dims );
    case Type::INT32 : return tick2< InType, int32_t  >( in_base, coord_dims );
    case Type::UINT64: return tick2< InType, uint64_t >( in_base, coord_dims );
    case Type::INT64 : return tick2< InType, int64_t  >( in_base, coord_dims );
    default:
      throw Exception() << "Internal: Unhandled case";
  }
}

template< class InType, class CoordType >
bool At::tick2( const ArrayBase *in_base, int coord_dims )
{
  if( coord_dims > 4 )
  {
    throw Exception()
      << "Coordinate Inputs must have at most 4 dimensions for now, has " << coord_dims
      ;
  }

  const Array< InType > *in = 0;
  if( !in_base->getAs( in ) ) throw GetAsFailed( getInput( INPUT_IN ), in );

  GET_INPUT_AS_IF_CONNECTED( INPUT_X, x, CoordType );
  GET_INPUT_AS_IF_CONNECTED( INPUT_Y, y, CoordType );
  GET_INPUT_AS_IF_CONNECTED( INPUT_Z, z, CoordType );
  GET_INPUT_AS_IF_CONNECTED( INPUT_T, t, CoordType );

  const int in_dims = in->getDims();

  const Array< CoordType > *const coord[ 4 ] = { x, y, z, t };
  const ArrayBase::Size coord_size = coord[ 0 ]->getSize();

  GET_OUTPUT_INIT( OUTPUT_OUT, out, InType, coord_dims, coord_size );

  typedef typename Array< InType >::iterator out_iterator;
  typedef typename Array< CoordType >::const_iterator coord_iterator;

  out_iterator out_iter = out->begin();
  coord_iterator iter[ in_dims ];
  for( int i=0; i<in_dims; ++i )
  {
    iter[ i ] = coord[ i ]->begin();
  }

  index_t C[ in_dims ];
  const index_t n = x->size();
  for( index_t p=0; p<n; ++p )
  {
    for( int i=0; i<in_dims; ++i )
    {
      C[ i ] = index_t( *iter[ i ] );
      ++iter[ i ];
    }

    (*out_iter) = in->at_v( in_dims, C );
    ++out_iter;
  }

  getOutput( OUTPUT_OUT )->resolve();
  return true;
}

bool At::tick( void )
{
  if( !hasAnyInputChanged() ) return true;
  setAllOutputChanged();

  GET_INPUT_BASE( INPUT_IN, in_base_tmp );

  const ArrayBase *in_base = resolve( in_base_tmp );

  const int in_dims = in_base->getDims();

  if( in_dims == 0 )
  {
    if(
         getInput( INPUT_X )->isConnected()
      || getInput( INPUT_Y )->isConnected()
      || getInput( INPUT_Z )->isConnected()
      || getInput( INPUT_T )->isConnected()
      )
    {
      throw Exception()
        << "Input 'in' is 0-dimensional (scalar), but at least"
        << " one coordinate Inputs 'x', 'y', 'z', 't' is connected."
        ;
    }
    getOutput( OUTPUT_OUT )->setData( const_cast< ArrayBase* >( in_base ) );
    return true;
  }

  if( in_dims > 4 )
  {
    throw Exception()
      << "Only up to 4 dimensions supported for Input 'in', is " << in_dims
      ;
  }

  const ArrayBase *const x_base = getInput( INPUT_X )->getData();
  if( !x_base ) throw NotConnected( getInput( INPUT_X ) );

  const Type coord_type = x_base->getType();
  if( !coord_type.isInteger() )
  {
    throw IncompatibleOutput( getInput( INPUT_X ) )
      << ": Coordinate Inputs must be integer"
      ;
  }

  const ArrayBase::Size coord_size = x_base->getSize();

  for( int d=1; d<in_dims; ++d )
  {
    const ArrayBase *const d_base = getInput( INPUT_X+d )->getData();
    if( !d_base)
    {
      throw NotConnected( getInput( INPUT_X+d ) )
        << ": Input 'in' has " << in_dims << " dimensions, so it should be connected"
        ;
    }
    if( d_base->getType() != coord_type )
    {
      throw NotConnected( getInput( INPUT_X+d ) )
        << ": Input 'in' has " << in_dims << " dimensions, so it should be connected"
        ;
    }
    if( d_base->getSize() != coord_size )
    {
      throw Exception()
        << "All coordinate Inputs must have the same size"
        << ": 'x' is " << coord_size
        << ", '" << ("xyzt"[ d ]) << "' is " << d_base->getSize()
        ;
    }
  }

  for( int d=in_dims; d<=(INPUT_T-INPUT_X); ++d )
  {
    const ArrayBase *const d_base = getInput( INPUT_X+d )->getData();
    if( d_base)
    {
      throw Exception()
        << "Input 'in' only has " << in_dims << " dimensions,"
        << " so Input '" << getInput( INPUT_X+d )->getIdentifier() << "' must not be connected."
        ;
    }
  }

  const int coord_dims = coord_size.getDims();
  switch( in_base->getType().getEnum() )
  {
    case Type::BOOL    : return tick1< bool     >( in_base, coord_type, coord_dims );
    case Type::UINT8   : return tick1< uint8_t  >( in_base, coord_type, coord_dims );
    case Type::INT8    : return tick1< int8_t   >( in_base, coord_type, coord_dims );
    case Type::UINT16  : return tick1< uint16_t >( in_base, coord_type, coord_dims );
    case Type::INT16   : return tick1< int16_t  >( in_base, coord_type, coord_dims );
    case Type::UINT32  : return tick1< uint32_t >( in_base, coord_type, coord_dims );
    case Type::INT32   : return tick1< int32_t  >( in_base, coord_type, coord_dims );
    case Type::UINT64  : return tick1< uint64_t >( in_base, coord_type, coord_dims );
    case Type::INT64   : return tick1< int64_t  >( in_base, coord_type, coord_dims );
    case Type::FLOAT   : return tick1< float    >( in_base, coord_type, coord_dims );
    case Type::DOUBLE  : return tick1< double   >( in_base, coord_type, coord_dims );
    case Type::STRING  : return tick1< String   >( in_base, coord_type, coord_dims );
    case Type::ARRAY   : return tick1< CountPtr< ArrayBase > >( in_base, coord_type, coord_dims );

    default:
      throw IncompatibleOutput( getInput( INPUT_IN ) );
  }

  return true;
}

void At::gc_clear( void )
{
  Node::gc_clear();
}

void At::gc_getChildren( Children &children ) const
{
  Base::gc_getChildren( children );
}

} // namespace core
} // namespace RPGML

RPGML_CREATE_NODE( At, core:: )


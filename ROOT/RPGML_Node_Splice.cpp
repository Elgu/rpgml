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
#include "RPGML_Node_Splice.h"

// RPGML_CXXFLAGS=
// RPGML_LDFLAGS=

#include <algorithm>

using namespace std;

namespace RPGML {


Splice::Splice( GarbageCollector *_gc, const String &identifier, const RPGML::SharedObject *so )
: Node( _gc, identifier, so, NUM_INPUTS, NUM_OUTPUTS, NUM_PARAMS )
, m_in( new InputArray( getGC(), 2 ) )
{
  DEFINE_INPUT( INPUT_LIKE_THIS, "like_this" );
  push_back( String::Static( "in" ), Value( m_in ) );
  DEFINE_OUTPUT( OUTPUT_OUT, "out" );
  DEFINE_PARAM ( PARAM_DIMS , "dims", Splice::set_dims );
  DEFINE_PARAM_INDEX( PARAM_NX, "nx"  , Splice::set_n, 0 );
  DEFINE_PARAM_INDEX( PARAM_NY, "ny"  , Splice::set_n, 1 );
  DEFINE_PARAM_INDEX( PARAM_NZ, "nz"  , Splice::set_n, 2 );
  DEFINE_PARAM_INDEX( PARAM_NT, "nt"  , Splice::set_n, 3 );
}

Splice::~Splice( void )
{}

const char *Splice::getName( void ) const
{
  return "Splice";
}

void Splice::gc_clear( void )
{
  Base::gc_clear();
  m_in.reset();
}

void Splice::gc_getChildren( Children &children ) const
{
  Base::gc_getChildren( children );
  children
    << m_in
    ;
}

void Splice::set_dims( const Value &value, index_t, int, const index_t* )
{
  const int dims = value.save_cast< int >();
  if( dims == m_dims ) return;

  if( dims < 1 || dims > 4 )
  {
    throw Exception()
      << "Parameter 'dims' must be greater 0 and at most 4"
      << ", is " << dims << " (" << value << ")"
      ;
  }

  m_dims = dims;
  alloc_in();
}

void Splice::set_n( const Value &value, index_t d, int, const index_t* )
{
  const int64_t n = value.save_cast< int64_t >();
  if( n < 0 )
  {
    throw Exception()
      << "Parameter 'n" << ("xyzt"[ d ]) << "' must be greater than 0"
      << ", is " << n
      ;
  }
  if( int( d ) >= m_dims )
  {
    throw Exception()
      << "Parameter 'n" << ("xyzt"[ d ]) << "' must not be set, if 'dims' is less than " << d
      << ", is " << m_dims
      ;
  }

  m_n[ d ] = index_t( n );
  alloc_in();
}

void Splice::alloc_in( void )
{
  if( !m_in.isNull() )
  {
    for( const auto &i : (*m_in) )
    {
      if( !i.isNull() && i->isConnected() )
      {
        throw Exception()
          << "Once an Input is connected, 'in' must not be resized anymore."
          ;
      }
    }

    m_in->resize_v( m_dims, m_n );
  }
  else
  {
    m_in.reset( new InputArray( getGC(), m_dims, m_n ) );
  }

  setNumInputs( INPUT_IN0 + m_in->size() );

  index_t input_index = INPUT_IN0;
  for( auto i( m_in->begin() ), end( m_in->end() ); i != end; ++i )
  {
    Input *const in_i = getInput( input_index++ );
    in_i->setIdentifier( "in" + toString( ArrayBase::Size( m_dims, i.getPos() ) ) );
    (*i).reset( in_i );
  }

  getVariable( "in" )->set( m_in );
}

template< class Element >
bool Splice::tick2( const index_t *size, const index_t *like_this_size )
{
  const int dims = m_dims;
  const index_t *const n = m_n;
  const index_t N = m_in->size();

  index_t out_size[ dims ];
  for( int d=0; d<dims; ++d )
  {
    out_size[ d ] = size[ d ] * n[ d ];
  }

  GET_OUTPUT_INIT( OUTPUT_OUT, out, Element, dims, ( like_this_size ? like_this_size : out_size ) );

  // Get iterators to all in[] into in_iter[]
  typedef vector< typename Array< Element >::const_iterator > iterators_t;
  iterators_t in_iter( N );
  for( index_t i=0; i<N; ++i )
  {
    GET_INPUT_AS_DIMS( INPUT_IN0+i, in_i, Element, dims );
    in_iter[ i ] = in_i->begin();
  }

  index_t out_roi_pos[ dims ];
  CountPtr< Array< Element > > out_roi( new Array< Element >( getGC(), dims ) );

  GET_INPUT_AS_DIMS( INPUT_IN0, in0, Element, dims );
  for( auto i0( in0->begin() ), i0end( in0->end() ); i0 != i0end; ++i0 )
  {
    const index_t *const pos = i0.getPos();
    for( int d=0; d<dims; ++d )
    {
      out_roi_pos[ d ] = pos[ d ] * n[ d ];
    }

    out->getROI( dims, out_roi_pos, n, out_roi );

    index_t i=0;
    for( typename Array< Element >::reference o : (*out_roi) )
    {
      o = *in_iter[ i ];
      ++in_iter[ i ];
      ++i;
    }
  }

  return true;
}

bool Splice::tick( void )
{
  if( !hasAnyInputChanged() ) return true;
  setAllOutputChanged();

  if( m_in.isNull() || m_in->empty() )
  {
    throw Exception()
      << "Parameters 'dims' and corresponding sizes must be set."
      ;
  }

  GET_INPUT_BASE_IF_CONNECTED( INPUT_LIKE_THIS, like_this );
  const index_t *const like_this_size = ( like_this ? like_this->getSize().getCoords() : nullptr );

  const ArrayBase::Size n( m_dims, m_n );

  Type type;
  const index_t *size = nullptr;

  const int N = int( m_in->size() );

  for( int i=0; i<N; ++i )
  {
    GET_INPUT_BASE( INPUT_IN0+i, in_i );

    if( 0 == i )
    {
      if( in_i->getDims() != m_dims )
      {
        throw Exception()
          << "Input '" << getInput( INPUT_IN0+i )->getIdentifier() << "' does have the wrong number of dimensions"
          << ": Expected " << m_dims
          << ", has " << in_i->getDims()
          ;
      }
      type = in_i->getType();
      size = in_i->getSize();
    }
    else
    {
      const ArrayBase::Size expected( m_dims, size );
      if( expected != in_i->getSize() )
      {
        throw Exception()
          << "Input '" << getInput( INPUT_IN0+i )->getIdentifier() << "' does not match in dimension and size"
          << ": Expected " << expected
          << ", has " << in_i->getSize()
          ;
      }
      if( type != in_i->getType() )
      {
        throw Exception()
          << "All Inputs must have the same type"
          << ": '" << getInput( INPUT_IN0+i )->getIdentifier() << "' is " << in_i->getType()
          << ", first was is " << type
          ;
      }
    }
  }

  switch( type.getEnum() )
  {
    case Type::BOOL    : return tick2< bool     >( size, like_this_size );
    case Type::UINT8   : return tick2< uint8_t  >( size, like_this_size );
    case Type::INT8    : return tick2< int8_t   >( size, like_this_size );
    case Type::UINT16  : return tick2< uint16_t >( size, like_this_size );
    case Type::INT16   : return tick2< int16_t  >( size, like_this_size );
    case Type::UINT32  : return tick2< uint32_t >( size, like_this_size );
    case Type::INT32   : return tick2< int32_t  >( size, like_this_size );
    case Type::UINT64  : return tick2< uint64_t >( size, like_this_size );
    case Type::INT64   : return tick2< int64_t  >( size, like_this_size );
    case Type::FLOAT   : return tick2< float    >( size, like_this_size );
    case Type::DOUBLE  : return tick2< double   >( size, like_this_size );
    case Type::STRING  : return tick2< String   >( size, like_this_size );
    case Type::ARRAY   : return tick2< CountPtr< ArrayBase > >( size, like_this_size );
    default:
      throw IncompatibleOutput( getInput( INPUT_IN0 ) );
  }




  return true;
}

 //
} // namespace RPGML

RPGML_CREATE_NODE( Splice,  )


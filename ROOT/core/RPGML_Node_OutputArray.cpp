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
#include "RPGML_Node_OutputArray.h"

// RPGML_CXXFLAGS=
// RPGML_LDFLAGS=

#include <algorithm>
#include <sstream>

using namespace std;

namespace RPGML {
namespace core {

OutputArray::OutputArray( GarbageCollector *_gc, const String &identifier, const RPGML::SharedObject *so )
: Node( _gc, identifier, so, NUM_INPUTS, NUM_OUTPUTS, NUM_PARAMS )
, m_dims( -1 )
{
  for( int i=0; i<4; ++i ) m_size[ i ] = unknown;

  push_back( String::Static( "in" ), Value( new InputArray( getGC(), 0 ) ) );

  DEFINE_OUTPUT( OUTPUT_OUT, "out" );
  DEFINE_PARAM ( PARAM_DIMS , "dims", OutputArray::set_dims );
  DEFINE_PARAM_INDEX( PARAM_SIZEX, "sizeX", OutputArray::set_size, 0 );
  DEFINE_PARAM_INDEX( PARAM_SIZEY, "sizeY", OutputArray::set_size, 1 );
  DEFINE_PARAM_INDEX( PARAM_SIZEZ, "sizeZ", OutputArray::set_size, 2 );
  DEFINE_PARAM_INDEX( PARAM_SIZET, "sizeT", OutputArray::set_size, 3 );
}

OutputArray::~OutputArray( void )
{}

const char *OutputArray::getName( void ) const
{
  return "core.OutputArray";
}

void OutputArray::gc_clear( void )
{
  Base::gc_clear();
  m_out.reset();
  m_in.reset();
}

void OutputArray::gc_getChildren( Children &children ) const
{
  Base::gc_getChildren( children );
  children
    << m_out
    << m_in
    ;
}

void OutputArray::set_dims( const Value &value, index_t, int, const index_t* )
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

void OutputArray::set_size( const Value &value, index_t d, int, const index_t* )
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

bool OutputArray::all_set( void ) const
{
  if( m_dims < 0 )
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

void OutputArray::create_array( void )
{
  if( m_dims < 0 )
  {
    throw Exception() << "Param 'dims' was not set";
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

  m_out = new ArrayArray( getGC(), m_dims );
  m_out->resize_v( m_dims, m_size );

  getOutput( OUTPUT_OUT )->setData( m_out );

  m_in = new InputArray( getGC(), m_dims );
  m_in->resize_v( m_dims, m_size );

  setNumInputs( INPUT_IN0 + m_in->size() );

  typedef ArrayBase::CoordinatesIterator C;
  int i = INPUT_IN0;
  InputArray::iterator in_iter = m_in->begin();
  for( CountPtr< C > c( m_in->getCoordinatesIterator() ); !c->done(); c->next(), ++i, ++in_iter )
  {
    std::ostringstream name;
    name << "in" << c->get();
    Input *const in_i = getInput( i );
    in_i->init( name.str() );
    (*in_iter) = in_i;
  }

  getVariable( "in" )->set( Value( m_in ) );
}

bool OutputArray::tick( void )
{
  if( m_out.isNull() ) create_array();

  bool changed = false;

  InputArray::const_iterator       in_iter = m_in->begin();
  InputArray::const_iterator const in_end  = m_in->end();
  ArrayArray::iterator out_iter = m_out->begin();
  for( ; in_iter != in_end; ++in_iter, ++out_iter )
  {
    const Input *const in_i = (*in_iter);
    if( in_i->hasChanged() )
    {
      (*out_iter) = const_cast< ArrayBase* >( in_i->getData() );
      changed = true;
    }
  }

  if( changed ) getOutput( OUTPUT_OUT )->setChanged();
  return true;
}

 } // namespace core {
} // namespace RPGML

RPGML_CREATE_NODE( OutputArray, core:: )


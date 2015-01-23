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
#include "RPGML_Node_Counter.h"

#include <algorithm>

using namespace std;

namespace RPGML {
namespace core {

Counter::Counter( GarbageCollector *_gc, const String &identifier, const RPGML::SharedObject *so )
: Node( _gc, identifier, so, NUM_INPUTS, NUM_OUTPUTS, NUM_PARAMS )
, m_start( 0 )
, m_step( 1 )
, m_count( 0 )
{
  DEFINE_OUTPUT_INIT( OUTPUT_OUT, "out", int, 0 );
  DEFINE_PARAM ( PARAM_START , "start", Counter::set_start );
  DEFINE_PARAM ( PARAM_STEP  , "step" , Counter::set_step  );
}

Counter::~Counter( void )
{}

const char *Counter::getName( void ) const
{
  return "core.Counter";
}

void Counter::set_start( const Value &value, index_t, int, const index_t* )
{
  try
  {
    m_start = value.to( Type::Int() );
  }
  catch( const Value::Exception &e )
  {
    throw Exception()
      << "Could not set parameter 'start'"
      << ": " << e.what()
      ;
  }
}

void Counter::set_step( const Value &value, index_t, int, const index_t* )
{
  try
  {
    m_step = value.to( Type::Int() );
  }
  catch( const Value::Exception &e )
  {
    throw Exception()
      << "Could not set parameter 'step'"
      << ": " << e.what()
      ;
  }
}

bool Counter::tick( void )
{
  GET_OUTPUT_AS( OUTPUT_OUT, out, int, 0 );
  (**out) = m_start + m_count * m_step;
  getOutput( OUTPUT_OUT )->setChanged();
  ++m_count;
  return true;
}

void Counter::gc_clear( void )
{
  Node::gc_clear();
}

void Counter::gc_getChildren( Children &children ) const
{
  Node::gc_getChildren( children );
}

} // namespace core
} // namespace RPGML

RPGML_CREATE_NODE( Counter, core:: )


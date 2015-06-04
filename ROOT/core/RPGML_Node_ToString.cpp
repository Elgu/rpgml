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
#include "RPGML_Node_ToString.h"

// RPGML_CXXFLAGS=
// RPGML_LDFLAGS=

#include <algorithm>

using namespace std;

namespace RPGML {
namespace core {

ToString::ToString( GarbageCollector *_gc, const String &identifier, const RPGML::SharedObject *so )
: Node( _gc, identifier, so, NUM_INPUTS, NUM_OUTPUTS, NUM_PARAMS )
{
  DEFINE_INPUT ( INPUT_IN  , "in"  );
  DEFINE_OUTPUT_INIT( OUTPUT_OUT, "out", String, 0 );
}

ToString::~ToString( void )
{}

const char *ToString::getName( void ) const
{
  return "core.ToString";
}

void ToString::gc_clear( void )
{
  Base::gc_clear();
}

void ToString::gc_getChildren( Children &children ) const
{
  Base::gc_getChildren( children );
}

bool ToString::tick( void )
{
  if( !hasAnyInputChanged() ) return true;

  GET_INPUT_BASE( INPUT_IN, in );
  GET_OUTPUT_AS( OUTPUT_OUT, out, String );

  if( out->getDims() != 0 )
  {
    throw Exception()
      << "Internal: Expected 'out' to be 0-dimensional"
      << ", is " << out->getDims()
      ;
  }

  if( 0 == in->getDims() )
  {
    (**out) = toString( in->getValue() );
  }
  else
  {
    (**out) = toString( (*in) );
  }

  setAllOutputChanged();
  return true;
}

 } // namespace core {
} // namespace RPGML

RPGML_CREATE_NODE( ToString, core:: )


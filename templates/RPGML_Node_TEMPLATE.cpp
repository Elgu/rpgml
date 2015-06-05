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
#include "RPGML_Node_TEMPLATE.h"

// RPGML_CXXFLAGS=
// RPGML_LDFLAGS=

#include <algorithm>

using namespace std;

namespace RPGML {
ADDITIONAL_NAMESPACES_BEGIN

TEMPLATE::TEMPLATE( GarbageCollector *_gc, const String &identifier, const RPGML::SharedObject *so )
: Node( _gc, identifier, so, NUM_INPUTS, NUM_OUTPUTS, NUM_PARAMS )
{
  DEFINE_INPUT ( INPUT_IN  , "in"  );
  DEFINE_OUTPUT( OUTPUT_OUT, "out" );
  DEFINE_PARAM ( PARAM_FOO , "foo", TEMPLATE::set_foo );
}

TEMPLATE::~TEMPLATE( void )
{}

const char *TEMPLATE::getName( void ) const
{
  return "TEMPLATE_NAME";
}

void TEMPLATE::gc_clear( void )
{
  Base::gc_clear();
}

void TEMPLATE::gc_getChildren( Children &children ) const
{
  Base::gc_getChildren( children );
}

void TEMPLATE::set_foo( const Value &value, index_t, int, const index_t* )
{
  if( !value.isString() )
  {
    throw Exception() << "Param 'foo' must be set with string.";
  }

  m_foo = value.getString();
}

bool TEMPLATE::tick( void )
{
  if( !hasAnyInputChanged() ) return true;
  setAllOutputChanged();

  return true;
}

ADDITIONAL_NAMESPACES_END
} // namespace RPGML

RPGML_CREATE_NODE( TEMPLATE, EXTRA_NAMESPACES )


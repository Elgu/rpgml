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
#include "RPGML_Node_Print.h"

#include <iostream>

namespace RPGML {
namespace core {

Print::Print( GarbageCollector *_gc, const String &identifier, const RPGML::SharedObject *so )
: Node( _gc, identifier, so, NUM_INPUTS, NUM_OUTPUTS, NUM_PARAMS )
{
  DEFINE_INPUT( INPUT_IN, "in" );
  DEFINE_INPUT( INPUT_PREV, "prev" );
  DEFINE_OUTPUT( OUTPUT_NEXT, "next" );
}

Print::~Print( void )
{}

const char *Print::getName( void ) const
{
  return "core.Print";
}

bool Print::tick( void )
{
  GET_INPUT_BASE( INPUT_IN, in_base );
  in_base->print( std::cout, true );
  return true;
}

void Print::gc_clear( void )
{
  Base::gc_clear();
}

void Print::gc_getChildren( Children &children ) const
{
  Base::gc_getChildren( children );
}

} // namespace core
} // namespace RPGML

RPGML_CREATE_NODE( Print, core:: )


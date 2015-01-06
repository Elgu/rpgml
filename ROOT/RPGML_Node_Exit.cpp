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
#include "RPGML_Node_Exit.h"

// RPGML_CXXFLAGS=
// RPGML_LDFLAGS=

#include <algorithm>

using namespace std;

namespace RPGML {


Exit::Exit( GarbageCollector *_gc, const String &identifier, const RPGML::SharedObject *so )
: Node( _gc, identifier, so, NUM_INPUTS, NUM_OUTPUTS, NUM_PARAMS )
{
  DEFINE_INPUT ( INPUT_CONDITION  , "condition"  );
}

Exit::~Exit( void )
{}

const char *Exit::getName( void ) const
{
  return "Exit";
}

bool Exit::tick( void )
{
  if( !hasAnyInputChanged() ) return true;

  const Input *const input_condition = getInput( INPUT_CONDITION );
  const ArrayBase *const condition_base = input_condition->getData();

  if( !condition_base )
  {
    throw ExitRequest();
  }

  if( condition_base->getDims() > 0 )
  {
    throw IncompatibleOutput( input_condition )
      << ": Must be 0-dimensional (scalar)"
      ;
  }

  try
  {
    const bool exit_requested = condition_base->getValue().to( Type::Bool() ).getBool();
    if( exit_requested )
    {
      throw ExitRequest();
    }
  }
  catch( const Value::CastFailed &e )
  {
    throw IncompatibleOutput( input_condition )
      << ": Type must be scalar"
      ;
  }

  return true;
}

void Exit::gc_clear( void )
{
  Node::gc_clear();
}

void Exit::gc_getChildren( Children &children ) const
{
  Node::gc_getChildren( children );
}

 //
} // namespace RPGML

RPGML_CREATE_NODE( Exit,  )


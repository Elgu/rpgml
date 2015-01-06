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
#include "RPGML_Function_TEMPLATE.h"

// RPGML_CXXFLAGS=
// RPGML_LDFLAGS=

#include <RPGML/String.h>
#include <RPGML/Scope.h>
#include <RPGML/Node.h>
#include <RPGML/ParseException.h>

using namespace std;

namespace RPGML {
ADDITIONAL_NAMESPACES_BEGIN

Function_TEMPLATE::Function_TEMPLATE( GarbageCollector *_gc, Frame *parent, const SharedObject *so )
: Function( _gc, parent, genDeclArgs(), so )
{}

Function_TEMPLATE::~Function_TEMPLATE( void )
{}

const char *Function_TEMPLATE::getName( void ) const
{
  return "TEMPLATE";
}

void Function_TEMPLATE::gc_clear( void )
{
  Base::gc_clear();
}

void Function_TEMPLATE::gc_getChildren( Children &children ) const
{
  Base::gc_getChildren( children );
}

bool Function_TEMPLATE::call_impl( const Location *loc, index_t recursion_depth, Scope *scope, Value &ret, index_t n_args, const Value *args )
{
  if( recursion_depth > MAX_RECURSION_DEPTH )
  {
    throw ParseException( loc ) << "Maximum recursion depth reached";
  }

  if( n_args != NUM_ARGS ) throw ParseException( loc ) << "Function TEMPLATE requires " << NUM_ARGS << " arguments.";
  const Value &in = args[ ARG_IN ];

  switch( in.getType().getEnum() )
  {
    default:
      break;
    //  throw "Invalid type for Function_TEMPLATE argument 'str'";
  }

  ret = Value( true );
  return true;
}

CountPtr< Function::Args > Function_TEMPLATE::genDeclArgs( void )
{
  CountPtr< Args > args = new Args( NUM_ARGS );
  args->at( ARG_IN ) = Arg( String::Static( "in" ) );
  return args;
}

ADDITIONAL_NAMESPACES_END
} // namespace RPGML

RPGML_CREATE_FUNCTION( TEMPLATE,  )

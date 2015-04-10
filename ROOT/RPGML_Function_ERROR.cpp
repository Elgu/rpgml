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
#include "RPGML_Function_ERROR.h"

// RPGML_CXXFLAGS=
// RPGML_LDFLAGS=

#include <RPGML/String.h>
#include <RPGML/Scope.h>
#include <RPGML/Node.h>
#include <RPGML/ParseException.h>

using namespace std;

namespace RPGML {


Function_ERROR::Function_ERROR( GarbageCollector *_gc, Frame *parent, const SharedObject *so )
: Function( _gc, new Location( __FILE__ ), parent, genDeclArgs(), so )
{}

Function_ERROR::~Function_ERROR( void )
{}

const char *Function_ERROR::getName( void ) const
{
  return "ERROR";
}

void Function_ERROR::gc_clear( void )
{
  Base::gc_clear();
}

void Function_ERROR::gc_getChildren( Children &children ) const
{
  Base::gc_getChildren( children );
}

Value Function_ERROR::call_impl( const Location *loc, index_t recursion_depth, Scope *, index_t n_args, const Value *args )
{
  if( recursion_depth > MAX_RECURSION_DEPTH )
  {
    throw ParseException( loc ) << "Maximum recursion depth reached";
  }

  if( n_args != NUM_ARGS ) throw ParseException( loc ) << "Function ERROR requires " << NUM_ARGS << " arguments.";
  const Value &in = args[ ARG_IN ];

  if( in.isPrimitive() )
  {
    throw Exception() << in;
  }
  else
  {
    throw Exception()
      << "Incompatible value for Argument 'in' for ERROR(), is " << in.getType()
      ;
  }

  return Value( true );
}

CountPtr< Function::Args > Function_ERROR::genDeclArgs( void )
{
  CountPtr< Args > args = new Args( NUM_ARGS );
  args->at( ARG_IN ) = Arg( String::Static( "in" ) );
  return args;
}

 //
} // namespace RPGML

RPGML_CREATE_FUNCTION( ERROR,  )

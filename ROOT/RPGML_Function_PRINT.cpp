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
#include "RPGML_Function_PRINT.h"

// RPGML_CXXFLAGS=
// RPGML_LDFLAGS=

#include <RPGML/String.h>
#include <RPGML/Scope.h>
#include <RPGML/Node.h>
#include <RPGML/ParseException.h>

using namespace std;

namespace RPGML {


Function_PRINT::Function_PRINT( GarbageCollector *_gc, Frame *parent, const SharedObject *so )
: Function( _gc, new Location( __FILE__ ), parent, genDeclArgs(), so )
{}

Function_PRINT::~Function_PRINT( void )
{}

const char *Function_PRINT::getName( void ) const
{
  return "PRINT";
}

void Function_PRINT::gc_clear( void )
{
  Base::gc_clear();
}

void Function_PRINT::gc_getChildren( Children &children ) const
{
  Base::gc_getChildren( children );
}

Value Function_PRINT::call_impl( const Location *loc, index_t recursion_depth, Scope *scope, index_t n_args, const Value *args )
{
  if( recursion_depth > MAX_RECURSION_DEPTH )
  {
    throw ParseException( loc ) << "Maximum recursion depth reached";
  }

  if( n_args != NUM_ARGS ) throw ParseException( loc ) << "Function PRINT requires " << NUM_ARGS << " arguments.";
  const Value &in = args[ ARG_IN ];

  std::cout << in;
  return Value( true );
}

CountPtr< Function::Args > Function_PRINT::genDeclArgs( void )
{
  CountPtr< Args > args = new Args( NUM_ARGS );
  args->at( ARG_IN ) = Arg( String::Static( "in" ) );
  return args;
}

 //
} // namespace RPGML

RPGML_CREATE_FUNCTION( PRINT,  )

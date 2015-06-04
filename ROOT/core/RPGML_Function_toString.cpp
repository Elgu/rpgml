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
#include "RPGML_Function_toString.h"

// RPGML_CXXFLAGS=
// RPGML_LDFLAGS=

#include <RPGML/String.h>
#include <RPGML/Scope.h>
#include <RPGML/Node.h>
#include <RPGML/ParseException.h>

using namespace std;

namespace RPGML {
namespace core {

Function_toString::Function_toString( GarbageCollector *_gc, Frame *parent, const SharedObject *so )
: Function( _gc, new Location( __FILE__ ), parent, genDeclArgs(), so )
{}

Function_toString::~Function_toString( void )
{}

const char *Function_toString::getName( void ) const
{
  return "core.toString";
}

void Function_toString::gc_clear( void )
{
  Base::gc_clear();
}

void Function_toString::gc_getChildren( Children &children ) const
{
  Base::gc_getChildren( children );
}

Value Function_toString::call_impl( const Location *loc, index_t recursion_depth, Scope *scope, index_t n_args, const Value *args )
{
  if( recursion_depth > MAX_RECURSION_DEPTH )
  {
    throw ParseException( loc ) << "Maximum recursion depth reached";
  }

  if( n_args != NUM_ARGS ) throw ParseException( loc ) << "Function toString requires " << NUM_ARGS << " arguments.";
  const Value &in = args[ ARG_IN ];

  if( in.isOutput() )
  {
    CountPtr< Node > node = scope->createNode( loc, recursion_depth+1, String::Static( ".core.ToString" ) );
    in.getOutput()->connect( node->getInput( "in" ) );
    return Value( node->getOutput( "out" ) );
  }

  return Value( toString( in ) );
}

CountPtr< Function::Args > Function_toString::genDeclArgs( void )
{
  CountPtr< Args > args = new Args( NUM_ARGS );
  args->at( ARG_IN ) = Arg( String::Static( "in" ) );
  return args;
}

 } // namespace core {
} // namespace RPGML

RPGML_CREATE_FUNCTION( toString, core:: )

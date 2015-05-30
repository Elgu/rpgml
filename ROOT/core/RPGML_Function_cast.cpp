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
#include "RPGML_Function_cast.h"

#include "RPGML_cast.h"

// RPGML_CXXFLAGS=
// RPGML_LDFLAGS=

#include <RPGML/String.h>
#include <RPGML/Scope.h>
#include <RPGML/Node.h>
#include <RPGML/ParseException.h>

using namespace std;

namespace RPGML {
namespace core {

Function_cast::Function_cast( GarbageCollector *_gc, Frame *parent, const SharedObject *so )
: Function( _gc, new Location( __FILE__ ), parent, genDeclArgs(), so )
{}

Function_cast::~Function_cast( void )
{}

const char *Function_cast::getName( void ) const
{
  return "core.cast";
}

void Function_cast::gc_clear( void )
{
  Base::gc_clear();
}

void Function_cast::gc_getChildren( Children &children ) const
{
  Base::gc_getChildren( children );
}

Value Function_cast::call_impl( const Location *loc, index_t rd, Scope *scope, index_t n_args, const Value *args )
{
  if( rd > MAX_RECURSION_DEPTH )
  {
    throw ParseException( loc ) << "Maximum recursion depth reached";
  }

  if( n_args != NUM_ARGS ) throw ParseException( loc ) << "Function cast requires " << NUM_ARGS << " arguments.";
  const Value &to = args[ ARG_TO ];
  const Value &in = args[ ARG_IN ];

  if( !to.isString() )
  {
    throw Exception()
      << "Argument 'to' must be string, is " << to.getType()
      ;
  }

  if( in.isOutput() )
  {
    CountPtr< Node > node = scope->createNode( loc, rd+1, String::Static( ".core.Cast" ) );
    node->getParam( "to" )->set( to );
    in.getOutput()->connect( node->getInput( "in" ) );
    return Value( node->getOutput( "out" ) );
  }

  const Type to_type( Type::getTypeEnum( to.getString() ) );
  if( in.isArray() )
  {
    return Value( castTo( to_type, in.getArray() ) );
  }
  else
  {
    return castTo( to_type, in );
  }
}

CountPtr< Function::Args > Function_cast::genDeclArgs( void )
{
  CountPtr< Args > args = new Args( NUM_ARGS );
  args->at( ARG_TO ) = Arg( String::Static( "to" ) );
  args->at( ARG_IN ) = Arg( String::Static( "in" ) );
  return args;
}

 } // namespace core {
} // namespace RPGML

RPGML_CREATE_FUNCTION( cast, core:: )

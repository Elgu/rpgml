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
#include "RPGML_Function_mathOp1.h"

#include "RPGML_math.h"

// RPGML_CXXFLAGS=
// RPGML_LDFLAGS=-lm

#include <RPGML/String.h>
#include <RPGML/Scope.h>
#include <RPGML/Node.h>
#include <RPGML/ParseException.h>

using namespace std;

namespace RPGML {
namespace math {

Function_mathOp1::Function_mathOp1( GarbageCollector *_gc, Frame *parent, const SharedObject *so )
: Function( _gc, new Location( __FILE__ ), parent, genDeclArgs(), so )
{}

Function_mathOp1::~Function_mathOp1( void )
{}

const char *Function_mathOp1::getName( void ) const
{
  return "math.mathOp1";
}

void Function_mathOp1::gc_clear( void )
{
  Base::gc_clear();
}

void Function_mathOp1::gc_getChildren( Children &children ) const
{
  Base::gc_getChildren( children );
}

Value Function_mathOp1::call_impl( const Location *loc, index_t recursion_depth, Scope *scope, index_t n_args, const Value *args )
{
  if( recursion_depth > MAX_RECURSION_DEPTH )
  {
    throw ParseException( loc ) << "Maximum recursion depth reached";
  }

  if( n_args != NUM_ARGS ) throw ParseException( loc ) << "Function mathOp1 requires " << NUM_ARGS << " arguments.";

  const Value &op_v = args[ ARG_OP ];
  const Value &in_v = args[ ARG_IN ];

  if( !op_v.isString() )
  {
    throw ParseException( loc ) << "Argument 'op' must be string, is " << op_v.getType();
  }

  if( in_v.isOutput() )
  {
    CountPtr< Node > node = scope->createNode( loc, recursion_depth+1, String::Static( ".math.MathOp1" ) );
    node->getParam( "op" )->set( op_v );
    node->getInput( "in" )->connect( in_v.getOutput() );
    return Value( node->getOutput( "out" ) );
  }

  try
  {
    const MOP1 op = getMOP1( op_v.getString() );
    return Value( mathOp1( op, in_v ) );
  }
  catch( const RPGML::Exception &e )
  {
    throw ParseException( loc, e );
  }
}

CountPtr< Function::Args > Function_mathOp1::genDeclArgs( void )
{
  CountPtr< Args > args = new Args( NUM_ARGS );
  args->at( ARG_OP ) = Arg( String::Static( "op" ) );
  args->at( ARG_IN ) = Arg( String::Static( "in" ) );
  return args;
}

 } // namespace math {
} // namespace RPGML

RPGML_CREATE_FUNCTION( mathOp1, math:: )

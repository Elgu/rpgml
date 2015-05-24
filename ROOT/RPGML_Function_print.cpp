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
#include "RPGML_Function_print.h"

#include <RPGML/String.h>
#include <RPGML/ParserEnums.h>
#include <RPGML/Scope.h>
#include <RPGML/Node.h>
#include <RPGML/ParseException.h>

#include <iostream>

namespace RPGML {

Function_print::Function_print( GarbageCollector *_gc, Frame *parent, const SharedObject *so )
: Function( _gc, new Location( __FILE__ ), parent, genDeclArgs(), so )
{}

Function_print::~Function_print( void )
{}

void Function_print::gc_clear( void )
{
  Base::gc_clear();
  m_prev.reset();
}

void Function_print::gc_getChildren( Children &children ) const
{
  Base::gc_getChildren( children );
  children
    << m_prev
    ;
}

const char *Function_print::getName( void ) const
{
  return "print";
}

Value Function_print::call_impl( const Location *loc, index_t recursion_depth, Scope *scope, index_t n_args, const Value *args )
{
  if( recursion_depth > MAX_RECURSION_DEPTH )
  {
    throw ParseException( loc ) << "Maximum recursion depth reached";
  }

  if( n_args != 1 ) throw Exception() << "Function 'print' requires 1 argument, specified " << n_args;
  const Value &in = args[ ARG_IN ];

  CountPtr< Node > node( scope->createNode( loc, recursion_depth+1, String::Static( ".core.Print" ) ) );

  scope->toOutput( loc, recursion_depth+1, in )->connect( node->getInput( "in" ) );

  scope->call( loc, recursion_depth+1, String::Static( ".needing" ), Value( node.get() ) );

  if( !m_prev.isNull() )
  {
    m_prev->connect( node->getInput( "prev" ) );
  }
  m_prev = node->getOutput( "next" );

  return Value( true );
}

CountPtr< Function::Args > Function_print::genDeclArgs( void )
{
  CountPtr< Args > args = new Args( NUM_ARGS );
  args->at( ARG_IN ) = Arg( String::Static( "in" ) );
  return args;
}

} // namespace RPGML

RPGML_CREATE_FUNCTION( print,  )

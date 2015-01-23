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
#include "RPGML_Function_needing.h"

// RPGML_CXXFLAGS=
// RPGML_LDFLAGS=

#include <RPGML/String.h>
#include <RPGML/Scope.h>
#include <RPGML/Node.h>
#include <RPGML/ParseException.h>

using namespace std;

namespace RPGML {


Function_needing::Function_needing( GarbageCollector *_gc, Frame *parent, const SharedObject *so )
: Function( _gc, new Location( __FILE__ ), parent, genDeclArgs(), so )
{}

Function_needing::~Function_needing( void )
{}

const char *Function_needing::getName( void ) const
{
  return "needing";
}

void Function_needing::gc_clear( void )
{
  Base::gc_clear();
  m_needed.reset();
}

void Function_needing::gc_getChildren( Children &children ) const
{
  Base::gc_getChildren( children );
  children.add( m_needed );
}

bool Function_needing::call_impl( const Location *loc, index_t recursion_depth, Scope *scope, Value &ret, index_t n_args, const Value *args )
{
  if( recursion_depth > MAX_RECURSION_DEPTH )
  {
    throw ParseException( loc ) << "Maximum recursion depth reached";
  }

  if( n_args != NUM_ARGS ) throw ParseException( loc ) << "Function needing requires " << NUM_ARGS << " arguments.";
  const Value &in = args[ ARG_IN ];

  if( in.isNode() )
  {
    if( m_needed.isNull() )
    {
      const Value *const needed_v = scope->lookup( "@needed" );
      if( !needed_v ) throw ParseException( loc ) << "Looking up '@needed' failed";

      if( !needed_v->isArray() )
      {
        throw ParseException( loc ) << "'@needed' is not an Array, is " << needed_v->getTypeName();
      }

      ArrayBase *const needed_base = needed_v->getArray();
      if( !needed_base )
      {
        throw ParseException( loc ) << "Found Array '@needed', but is null";
      }

      NodeArray1D *needed_p = 0;
      if( !needed_base->getAs( needed_p ) )
      {
        throw ParseException( loc )
          << "'@needed' is an Array, but not of the correct type, is "
          << needed_base->getTypeName()
          << needed_base->getSize()
          ;
      }

      m_needed.reset( needed_p );
    }

    m_needed->push_back( in.getNode() );
  }
  else
  {
    throw ParseException( loc )
      << "Argument 'in' must be Node, is " << in.getTypeName()
      ;
  }

  ret = Value( true );
  return true;
}

CountPtr< Function::Args > Function_needing::genDeclArgs( void )
{
  CountPtr< Args > args = new Args( NUM_ARGS );
  args->at( ARG_IN ) = Arg( String::Static( "in" ) );
  return args;
}

 //
} // namespace RPGML

RPGML_CREATE_FUNCTION( needing,  )

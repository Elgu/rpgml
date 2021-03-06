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
#include "RPGML_Function_get.h"

// RPGML_CXXFLAGS=
// RPGML_LDFLAGS=

#include <RPGML/String.h>
#include <RPGML/Scope.h>
#include <RPGML/Node.h>

using namespace std;

namespace RPGML {
namespace core {

Function_get::Function_get( GarbageCollector *_gc, Frame *parent, const SharedObject *so )
: Function( _gc, new Location( __FILE__ ), parent, genDeclArgs(), so )
{}

Function_get::~Function_get( void )
{}

const char *Function_get::getName( void ) const
{
  return "core.get";
}

Value Function_get::call_impl( const Location *loc, index_t recursion_depth, Scope *scope, index_t n_args, const Value *args )
{
  if( n_args != NUM_ARGS ) throw Exception() << "Function get requires " << NUM_ARGS << " arguments.";
  const Value &what  = args[ ARG_WHAT  ];
  const Value &in    = args[ ARG_IN    ];

  if( !what.isString() )
  {
    throw Exception()
      << "Type of 'what' must be string, is " << what.getType()
      ;
  }
  const String &what_str = what.getString();

  if( what_str == "type" )
  {
    return Value( scope->unify( in.getTypeName() ) );
  }

  if( in.isOutput() )
  {
    if( what_str == "isNull" )
    {
      return Value( in.getOutput() == nullptr );
    }

    CountPtr< Node > node( scope->createNode( new Location( LOCATION, loc ), recursion_depth+1, String::Static( ".core.Get" ) ) );

    in.getOutput()->connect( node->getInput( "in" ) );

    if( what_str == "array_type" )
    {
      return Value( node->getOutput( String::Static( "array_type" ) ) );
    }

    if( what_str == "dims" )
    {
      return Value( node->getOutput( String::Static( "dims" ) ) );
    }

    if( what_str == "size" )
    {
      const Value *const size = node->getVariable( "size" );
      if( !size ) throw Exception() << "Internal: Could not get 'size' from Node 'Get'.";
      return (*size);
    }

    throw Exception()
      << "Unknown 'what'-string '" << what_str << "'"
      ;
  }
  else if( in.isArray() )
  {
    const ArrayBase *const in_array = in.getArray();

    if( what_str == "isNull" )
    {
      return Value( in.getArray() == nullptr );
    }

    if( what_str == "array_type" )
    {
      return Value( scope->unify( in_array->getTypeName() ) );
    }

    const ArrayBase::Size in_size = in_array->getSize();
    const int in_dims = in_size.getDims();

    if( what_str == "dims" )
    {
      return Value( in_dims );
    }

    if( what_str == "size" )
    {
      const index_t size_n = index_t( max( in_dims, 4 ) );
      CountPtr< Array< index_t > > size_array = new Array< index_t >( getGC(), 1, size_n );

      for( index_t i=0; i<index_t( in_dims ); ++i )
      {
        size_array->at( i ) = in_size[ i ];
      }
      for( index_t i=index_t( in_dims ); i < size_n; ++i )
      {
        size_array->at( i ) = 1;
      }

      return Value( size_array );
    }

    throw Exception()
      << "Unknown 'what'-string '" << what_str << "'"
      ;
  }
  else
  {
    if( what_str == "array_type" )
    {
      throw Exception()
        << "Type of 'in' must be Output or Array for 'array_type', is " << in.getTypeName()
        ;
    }

    if( what_str == "isNull" )
    {
      throw Exception()
        << "'what' string 'isNull' only valid for Output or Array, is " << in.getType()
        ;
    }

    if( what_str == "dims" )
    {
      return Value( int( 0 ) );
    }

    if( what_str == "size" )
    {
      return Value( new Array< index_t >( getGC(), 1 ) );
    }

    throw Exception()
      << "Unknown 'what'-string '" << what_str << "'"
      ;
  }

  // Never reached
  return Value();
}

CountPtr< Function::Args > Function_get::genDeclArgs( void )
{
  CountPtr< Args > args = new Args( NUM_ARGS );
  args->at( ARG_WHAT  ) = Arg( String::Static( "what"  ) );
  args->at( ARG_IN    ) = Arg( String::Static( "in"    ) );
  return args;
}

} // namespace core
} // namespace RPGML

RPGML_CREATE_FUNCTION( get, core:: )

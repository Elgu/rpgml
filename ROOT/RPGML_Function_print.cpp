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

#include <iostream>

namespace RPGML {

Function_print::Function_print( GarbageCollector *_gc, Frame *parent, const SharedObject *so )
: Function( _gc, new Location( __FILE__ ), parent, genDeclArgs(), so )
{}

Function_print::~Function_print( void )
{}

bool Function_print::call_impl( const Location *loc, index_t recursion_depth, Scope *scope, Value &ret, index_t n_args, const Value *args )
{
  if( n_args != 1 ) throw Exception() << "Function 'print' requires 1 argument, specified " << n_args;
  const Value &in = args[ ARG_IN ];

  switch( in.getType().getEnum() )
  {
    case Type::OUTPUT:
      {
        CountPtr< Node > node( scope->createNode( loc, recursion_depth+1, String::Static( ".core.Print" ) ) );

        in.getOutput()->connect( node->getInput( "in" ) );
        scope->call( loc, recursion_depth+1, String::Static( ".needing" ), ret, Value( node.get() ) );

        ret = Value( true );
        return true;
      }
      break;

    default:
      std::cout << in;
      break;
    //  throw "Invalid type for Function_print argument 'str'";
  }

  ret = Value( true );
  return true;
}

CountPtr< Function::Args > Function_print::genDeclArgs( void )
{
  CountPtr< Args > args = new Args( NUM_ARGS );
  args->at( ARG_IN ) = Arg( String::Static( "in" ) );
  return args;
}

} // namespace RPGML

RPGML_CREATE_FUNCTION( print,  )

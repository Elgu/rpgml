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
#include "RPGML_Function_binaryOp.h"

#include <RPGML/String.h>
#include <RPGML/ParserEnums.h>
#include <RPGML/Scope.h>
#include <RPGML/Node.h>

#include <sstream>

namespace RPGML {

Function_binaryOp::Function_binaryOp( GarbageCollector *_gc, Frame *parent, const SharedObject *so )
: Function( _gc, new Location( __FILE__ ), parent, genDeclArgs(), so )
{}

Function_binaryOp::~Function_binaryOp( void )
{}

namespace binaryOp_impl {

  template< class IntType1, class IntType2 >
  static inline
  IntType1 shift_left( IntType1 x, IntType2 n )
  {
    return ( x << n );
  }

  template< class IntType1, class IntType2 >
  static inline
  IntType1 shift_right( IntType1 x, IntType2 n )
  {
    return ( x >> n );
  }

  template< class Type >
  static inline
  bool lt( Type left, Type right )
  {
    return ( left < right );
  }

  template< class Type >
  static inline
  bool gt( Type left, Type right )
  {
    return ( left > right );
  }

  template< class Type >
  static inline
  bool ge( Type left, Type right )
  {
    return ( left >= right );
  }

  template< class Type >
  static inline
  bool le( Type left, Type right )
  {
    return ( left <= right );
  }

  template< class Type >
  static inline
  bool eq( Type left, Type right )
  {
    return ( left == right );
  }

  template< class Type >
  static inline
  bool ne( Type left, Type right )
  {
    return ( left != right );
  }

  static inline
  bool log_and( bool left, bool right )
  {
    return ( left && right );
  }

  static inline
  bool log_or ( bool left, bool right )
  {
    return ( left || right );
  }

  static inline
  bool log_xor ( bool left, bool right )
  {
    return ( left ^ right );
  }

  template< class IntType >
  static inline
  IntType bit_and( IntType left, IntType right )
  {
    return ( left & right );
  }

  template< class IntType >
  static inline
  IntType bit_or ( IntType left, IntType right )
  {
    return ( left | right );
  }

  template< class IntType >
  static inline
  IntType bit_xor( IntType left, IntType right )
  {
    return ( left | right );
  }

  template< class Type >
  static inline
  Type mul( Type left, Type right )
  {
    return ( left * right );
  }

  template< class Type >
  static inline
  Type div( Type left, Type right )
  {
    return ( left * right );
  }

  template< class Type >
  static inline
  Type add( Type left, Type right )
  {
    return ( left + right );
  }

  template< class Type >
  static inline
  Type sub( Type left, Type right )
  {
    return ( left - right );
  }

  template< class IntType >
  static inline
  IntType mod( IntType left, IntType right )
  {
    return ( left % right );
  }

  template< class FloatType >
  static inline
  FloatType modf( FloatType left, FloatType right )
  {
    return left - int( left / right ) * right;
  }

  static inline
  Value bop( const Value &left, const BOP op, const Value &right )
  {
    switch( op )
    {
      case BOP_LEFT   : return Value( left << right );
      case BOP_RIGHT  : return Value( left >> right );
      case BOP_LT     : return Value( left <  right );
      case BOP_LE     : return  Value( left <= right );
      case BOP_GT     : return Value( left >  right );
      case BOP_GE     : return Value( left >= right );
      case BOP_EQ     : return Value( left == right );
      case BOP_NE     : return Value( left != right );
      case BOP_LOG_AND: return Value( left && right );
      case BOP_LOG_OR : return Value( left || right );
      case BOP_LOG_XOR: return Value( left.log_xor( right ) );
      case BOP_BIT_AND: return Value( left & right );
      case BOP_BIT_OR : return Value( left | right );
      case BOP_BIT_XOR: return Value( left ^ right );
      case BOP_MUL    : return Value( left * right );
      case BOP_DIV    : return Value( left / right );
      case BOP_ADD    : return Value( left + right );
      case BOP_SUB    : return Value( left - right );
      case BOP_MOD    : return Value( left % right );
      case BOP_MIN    : return ( left < right ? left : right );
      case BOP_MAX    : return ( left > right ? left : right );
      default:
        throw "Invalid op";
    }
  }
} // namespace binaryOp_impl

Value Function_binaryOp::call_impl( const Location *loc, index_t recursion_depth, Scope *scope, index_t n_args, const Value *args )
{
  using namespace binaryOp_impl;

  if( n_args != NUM_ARGS ) throw Exception() << "Function 'binaryOp' requires 2 arguments, specified " << n_args;

  const Value &op_v  = args[ ARG_OP   ];
  const Value &left  = args[ ARG_LEFT ];
  const Value &right = args[ ARG_RIGHT ];

  if( !op_v.isString() ) throw Exception() << "Argument 'op' must be string, is " << op_v.getType();

  if( left.isOutput() || right.isOutput() )
  {
    CountPtr< Node > node( scope->createNode( loc, recursion_depth+1, String::Static( ".BinaryOp" ) ) );

    CountPtr< Output > left_output = scope->toOutput( loc, recursion_depth, left );
    CountPtr< Output > right_output = scope->toOutput( loc, recursion_depth, right );

    node->getInput( "in1" )->connect( left_output );
    node->getParam( "op"  )->set( op_v );
    node->getInput( "in2" )->connect( right_output );

    return Value( node->getOutput( "out" ) );
  }

  const BOP op = getBOP( op_v.getString() );

  return bop( left, op, right );
}

CountPtr< Function::Args > Function_binaryOp::genDeclArgs( void )
{
  CountPtr< Args > args = new Args( NUM_ARGS );
  args->at( ARG_LEFT  ) = Arg( String::Static( "left"  ) );
  args->at( ARG_OP    ) = Arg( String::Static( "op"    ) );
  args->at( ARG_RIGHT ) = Arg( String::Static( "right" ) );
  return args;
}

} // namespace RPGML

RPGML_CREATE_FUNCTION( binaryOp,  )


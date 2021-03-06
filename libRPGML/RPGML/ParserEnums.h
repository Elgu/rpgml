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
#ifndef RPGML_ParserEnums_h
#define RPGML_ParserEnums_h

#include "Exception.h"

namespace RPGML {

enum BOP
{
    BOP_UNDEFINED
  , BOP_LEFT
  , BOP_RIGHT
  , BOP_LT
  , BOP_LE
  , BOP_GT
  , BOP_GE
  , BOP_EQ
  , BOP_NE
  , BOP_LOG_AND
  , BOP_LOG_OR
  , BOP_LOG_XOR
  , BOP_BIT_AND
  , BOP_BIT_OR
  , BOP_BIT_XOR
  , BOP_MUL
  , BOP_DIV
  , BOP_ADD
  , BOP_SUB
  , BOP_MOD
  , BOP_MAX
  , BOP_MIN
};

static inline
const char *getBOPStr( BOP bop )
{
  static
  const char *const op[] =
  {
      "undefined"
    , "<<", ">>"
    , "<", "<="
    , ">", ">="
    , "==" , "!="
    , "&&", "||", "^^"
    , "&", "|", "^"
    , "*", "/", "+", "-"
    , "%"
    , "min", "max"
  };
  return op[ bop ];
}

static inline
BOP getBOP( const char *const bop )
{
  switch( bop[ 0 ] )
  {
    case '<':
      switch( bop[ 1 ] )
      {
        case '\0': return BOP_LT;
        case  '<': return BOP_LEFT;
        case  '=': return BOP_LE;
      }
      break;
    case '>':
      switch( bop[ 1 ] )
      {
        case '\0': return BOP_GT;
        case  '>': return BOP_RIGHT;
        case  '=': return BOP_GE;
      }
      break;
    case '=':
      switch( bop[ 1 ] )
      {
        case  '=': return BOP_EQ;
      }
      break;
    case '!':
      switch( bop[ 1 ] )
      {
        case  '=': return BOP_NE;
      }
      break;
    case '&':
      switch( bop[ 1 ] )
      {
        case '\0': return BOP_BIT_AND;
        case  '&': return BOP_LOG_AND;
      }
      break;
    case '|':
      switch( bop[ 1 ] )
      {
        case '\0': return BOP_BIT_OR;
        case  '|': return BOP_LOG_OR;
      }
      break;
    case '^':
      switch( bop[ 1 ] )
      {
        case '\0': return BOP_BIT_XOR;
        case  '^': return BOP_LOG_XOR;
      }
      break;
    case '*':
      switch( bop[ 1 ] )
      {
        case '\0': return BOP_MUL;
      }
      break;
    case '/':
      switch( bop[ 1 ] )
      {
        case '\0': return BOP_DIV;
      }
      break;
    case '+':
      switch( bop[ 1 ] )
      {
        case '\0': return BOP_ADD;
      }
      break;
    case '-':
      switch( bop[ 1 ] )
      {
        case '\0': return BOP_SUB;
      }
      break;
    case '%':
      switch( bop[ 1 ] )
      {
        case '\0': return BOP_MOD;
      }
      break;
    case 'm':
      if( 0 == ::strcmp( bop, "min" ) ) return BOP_MIN;
      if( 0 == ::strcmp( bop, "max" ) ) return BOP_MAX;
      break;
  }

  throw Exception() << "Invalid binary op '" << bop << "'";
}

enum UOP
{
    UOP_UNDEFINED
  , UOP_MINUS
  , UOP_PLUS
  , UOP_LOG_NOT
  , UOP_BIT_NOT
};

static inline
const char *getUOPStr( UOP uop )
{
  static
  const char *const op[] =
  {
      "undefined"
    , "-", "+", "!", "~"
  };
  return op[ uop ];
}

static inline
UOP getUOP( const char *uop )
{
  if( !uop[ 0 ] || uop[ 1 ] ) goto fail;

  switch( uop[ 0 ] )
  {
    case '-': return UOP_MINUS;
    case '+': return UOP_PLUS;
    case '!': return UOP_LOG_NOT;
    case '~': return UOP_BIT_NOT;
  }

fail:
  throw Exception() << "Invalid unary op '" << uop << "'";
}

enum ASSIGN
{
    ID_ASSIGN
  , MUL_ASSIGN
  , DIV_ASSIGN
  , MOD_ASSIGN
  , ADD_ASSIGN
  , SUB_ASSIGN
  , LEFT_ASSIGN
  , RIGHT_ASSIGN
  , AND_ASSIGN
  , XOR_ASSIGN
  , OR_ASSIGN
};

static inline
const char *getAssign( ASSIGN assign )
{
  static
  const char *const op[] =
  {
      "=", "*=", "/="
    , "%=", "+=", "-="
    , "<<=", ">>="
    , "&=", "^=", "|="
  };
  return op[ assign ];
}

static inline
BOP getAssignBOP( ASSIGN assign )
{
  assert( assign >= MUL_ASSIGN );
  assert( assign <= OR_ASSIGN );

  static
  const BOP bop[] =
  {
      BOP_MUL
    , BOP_DIV
    , BOP_MOD
    , BOP_ADD
    , BOP_SUB
    , BOP_LEFT
    , BOP_RIGHT
    , BOP_BIT_AND
    , BOP_BIT_XOR
    , BOP_BIT_OR
  };

  return bop[ assign-1 ];
}

} // namespace RPGML

#endif


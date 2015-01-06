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
#include "Scanner.h"

#include <RPGML/rpgml.tab.hh>

#include <ctype.h>
#include <iostream>

namespace RPGML {

typedef _Parser::location_type location_type;
typedef _Parser::semantic_type semantic_type;

Source::Source( void )
: m_prevChar( '\0' )
, m_putBackChar( '\0' )
, m_charWasPutBack( false )
{}

Source::~Source( void )
{}

char Source::next( location_type *loc )
{
  if( m_charWasPutBack )
  {
    const char c = m_putBackChar;
    m_charWasPutBack = false;;
    return ( m_prevChar = c );
  }

  trackLocation( m_prevChar, loc );
  return ( m_prevChar = nextChar() );
}

void Source::putBackChar( void )
{
  m_putBackChar = m_prevChar;
  m_charWasPutBack = true;
  m_prevChar = '\0';
}

void Source::tokenBegin( location_type *loc )
{
  trackLocation( m_prevChar, loc );
  m_prevChar = '\0';
  loc->step();
}

void Source::tokenEnd( location_type *loc )
{
  trackLocation( m_prevChar, loc );
  m_prevChar = '\0';
}

char Source::trackLocation( char c, location_type *loc )
{
  if( '\n' == c )
  {
    loc->lines();
  }
  else if( '\0' == c )
  {
    // do nothing
  }
  else
  {
    loc->columns();
  }

  return c;
}

Scanner::Scanner( StringUnifier *unifier, Source *source )
: m_source( source )
, m_unifier( unifier )
{
  setFilename( "" );
}

Scanner::~Scanner( void )
{}

Scanner &Scanner::getScanner( void )
{
  return (*this);
}

Scanner &Scanner::setSource( Source *source )
{
  m_source = source;
  return (*this);
}

Scanner &Scanner::setFilename( const char *_filename )
{
  unified_filename = unify( _filename );
  m_filename = _filename;
  filename = &m_filename;
  return (*this);
}

Scanner &Scanner::setFilename( const std::string &_filename )
{
  unified_filename = unify( _filename );
  m_filename = _filename;
  filename = &m_filename;
  return (*this);
}

Scanner &Scanner::setFilename( const String &_filename )
{
  unified_filename = unify( _filename );
  m_filename = _filename;
  filename = &m_filename;
  return (*this);
}

static
char waste_whitespace( Source *s, location_type *loc )
{
  char c;
  while( isspace( c = s->next( loc ) ) )
  {
    if( '\0' == c ) break;
  }
  return c;
}

struct Keyword
{
  const char *const keyword;
  const _Parser::token::yytokentype token;
  const Type::Enum type_enum;
};

static
const Keyword keywords[] =
{
  { "bool"    , _Parser::token::BOOL    , Type::BOOL     },
  { "int"     , _Parser::token::INT     , Type::INT32    },
  { "uint8"   , _Parser::token::UINT8   , Type::UINT8    },
  { "int8"    , _Parser::token::INT8    , Type::INT8     },
  { "uint16"  , _Parser::token::UINT16  , Type::UINT16   },
  { "int16"   , _Parser::token::INT16   , Type::INT16    },
  { "uint32"  , _Parser::token::UINT32  , Type::UINT32   },
  { "int32"   , _Parser::token::INT32   , Type::INT32    },
  { "uint64"  , _Parser::token::UINT64  , Type::UINT64   },
  { "int64"   , _Parser::token::INT64   , Type::INT64    },
  { "float"   , _Parser::token::FLOAT   , Type::FLOAT    },
  { "double"  , _Parser::token::DOUBLE  , Type::DOUBLE   },
  { "string"  , _Parser::token::STRING  , Type::STRING   },
  { "nil"     , _Parser::token::NIL     , Type::NIL      },
  { "Array"   , _Parser::token::ARRAY   , Type::ARRAY    },
  { "Frame"   , _Parser::token::FRAME   , Type::FRAME    },
  { "Function", _Parser::token::FUNCTION, Type::FUNCTION },
  { "Output"  , _Parser::token::OUTPUT  , Type::OUTPUT   },
  { "Input"   , _Parser::token::INPUT   , Type::INPUT    },
  { "if"      , _Parser::token::IF      , Type::NIL  },
  { "else"    , _Parser::token::ELSE    , Type::NIL  },
  { "for"     , _Parser::token::FOR     , Type::NIL  },
  { "in"      , _Parser::token::IN      , Type::NIL  },
  { "to"      , _Parser::token::TO      , Type::NIL  },
  { "step"    , _Parser::token::STEP    , Type::NIL  },
  { "true"    , _Parser::token::TRUE    , Type::NIL  },
  { "false"   , _Parser::token::FALSE   , Type::NIL  },
  { "this"    , _Parser::token::THIS    , Type::NIL  },
  { "return"  , _Parser::token::RETURN  , Type::NIL  },
  { 0         , _Parser::token::END     , Type::NIL  }
};

static
int parse_line_comment( char c, Source *s, StringUnifier *, semantic_type *, location_type *loc )
{
  s->tokenBegin( loc );
  for(;;)
  {
    c = s->next( loc );
    if( '\n' == c ) break;
    if( '\0' == c ) { s->putBackChar(); break; }
  }
  s->tokenEnd( loc );
  return -1;
}

static
int parse_block_comment( char c, Source *s, StringUnifier *, semantic_type *, location_type *loc )
{
  s->tokenBegin( loc );
  // waste tokens until "*/"
  for(;;)
  {
    // search for '*'
    while( '*' != ( c = s->next( loc ) ) ) {}
    for(;;)
    {
      c = s->next( loc );
      if( '*' == c ) break;
      if( '\0' == c ) { s->putBackChar(); s->tokenEnd( loc ); return -1; }
    }
    // if next is '/', break
    if( '/' == ( c = s->next( loc ) ) ) break;
  }
  s->tokenEnd( loc );
  return -1;
}

static
int parse_identifier( char c, Source *s, StringUnifier *u, semantic_type *token, location_type *loc )
{
  s->tokenBegin( loc );

  std::string str; str.reserve( 8 );

  do
  {
    str += c;
  }
  while( isalnum( c = s->next( loc ) ) || '_' == c );

  s->putBackChar();

  s->tokenEnd( loc );

  // Check for keywords
  for( const Keyword *i = keywords; i->token != _Parser::token::END; ++i )
  {
    if( str == i->keyword )
    {
      if( i->type_enum )
      {
        token->type_enum = i->type_enum;
      }
      return i->token;
    }
  }

  // Not a keyword
  token->str = u->unify_move( str );
  return _Parser::token::IDENTIFIER;
}

static
int parse_number( char c, Source *s, StringUnifier *, semantic_type *token, location_type *loc )
{
  s->tokenBegin( loc );

  std::string str; str.reserve( 8 );

  bool has_dot = false;
  bool has_exp = false;
  bool has_exp_sign = false;
  bool is_float = false;
  bool is_hex = false;

  for(;;)
  {
    str += c;
    c = s->next( loc );

    if( c >= '0' && c <= '9' )
    {
      // accept
    }
    else if( !is_hex && 'e' == c )
    {
      if( has_exp )
      {
        throw "only one exponent 'e' allowed.";
      }
      has_exp = true;
      is_float = true;
    }
    else if( 'p' == c )
    {
      if( !is_hex )
      {
        throw "'p' exponent only allowed in floating point hex numbers";
      }
      if( has_exp )
      {
        throw "only one exponent 'p' allowed.";
      }
      has_exp = true;
      is_float = true;
    }
    else if( ( c >= 'a' && c <= 'z' ) || ( c >= 'A' && c <= 'Z' ) )
    {
      if( !is_hex )
      {
        throw "hex numbers must start with '0x'";
      }
    }
    else if( '.' == c )
    {
      if( has_dot ) throw "Invalid float with two dots";
      has_dot = true;
      is_float = true;
    }
    else if( 'x' == c )
    {
      if( str.length() != 1 || str[ 0 ] != '0' )
      {
        throw "hex numbers must begin with '0x', no further 'x' allowed.";
      }
      is_hex = true;
    }
    else if( '+' == c || '-' == c )
    {
      if( !has_exp )
      {
        // reject
        break;
      }
      if( has_exp_sign )
      {
        // reject
        break;
      }
      has_exp_sign = true;
    }
    else
    {
      // reject
      break;
    }
  }

  s->putBackChar();

  s->tokenEnd( loc );

  if( is_float )
  {
    const double f = strtod( str.c_str(), 0 );
    token->fval = f;
    return _Parser::token::F_CONSTANT;
  }
  else
  {
    const int base = ( is_hex ? 16 : 10 );
    const int64_t i = int64_t( strtoll( str.c_str(), 0, base ) );
    token->ival = i;
    return _Parser::token::I_CONSTANT;
  }
}

static
int parse_string( char c, Source *s, StringUnifier *u, semantic_type *token, location_type *loc )
{
  const char parenthis = c;

  s->tokenBegin( loc );

  std::string str; str.reserve( 8 );
  while( parenthis != ( c = s->next( loc ) ) )
  {
    if( c == '\\' )
    {
      c = s->next( loc );
      switch( c )
      {
        case 'a': str += '\a'; break;
        case 'b': str += '\b'; break;
        case 'f': str += '\f'; break;
        case 'n': str += '\n'; break;
        case 'r': str += '\r'; break;
        case 't': str += '\t'; break;
        case 'v': str += '\v'; break;
        case '0': str += '\0'; break;
        case '\\': str += '\\'; break;
        case '\'': str += '\''; break;
        case '\"': str += '\"'; break;
        default:
          throw "invalid escape character";
      }
    }
    else if( '\0' == c )
    {
      s->putBackChar();
      break;
    }
    else
    {
      str += c;
    }
  }

  s->tokenEnd( loc );

  token->str = u->unify_move( str );
  return _Parser::token::S_CONSTANT;
}

static
int parse_minus( char c, Source *s, StringUnifier *, semantic_type *token, location_type *loc )
{
  int ret = 0;

  s->tokenBegin( loc );

  const char second = s->next( loc );

  switch( second )
  {
    case '>': ret = _Parser::token::ARROW; break;
    case '-': ret = _Parser::token::DEC_OP; break;

    case '=':
       token->assign = SUB_ASSIGN;
       ret = _Parser::token::SUB_ASSIGN;
       break;

    default:
      s->putBackChar();
      ret = c;
      break;
  }

  s->tokenEnd( loc );

  return ret;
}

static
int parse_plus( char c, Source *s, StringUnifier *, semantic_type *token, location_type *loc )
{
  int ret = 0;

  s->tokenBegin( loc );

  const char second = s->next( loc );

  switch( second )
  {
    case '+': ret = _Parser::token::INC_OP; break;

    case '=':
       token->assign = ADD_ASSIGN;
       ret = _Parser::token::ADD_ASSIGN;
       break;

    default:
      s->putBackChar();
      ret = c;
      break;
  }

  s->tokenEnd( loc );

  return ret;
}

static
int parse_lt( char c, Source *s, StringUnifier *, semantic_type *token, location_type *loc )
{
  int ret = 0;

  s->tokenBegin( loc );

  const char second = s->next( loc );

  switch( second )
  {
    case '<':
      {
        const char third = s->next( loc );
        if( '=' == third )
        {
          token->assign = LEFT_ASSIGN;
          ret = _Parser::token::LEFT_ASSIGN;
        }
        else
        {
          s->putBackChar();
          ret = _Parser::token::LEFT_OP;
        }
      }
      break;

    case '=':
       ret = _Parser::token::LE_OP;
       break;

    default:
      s->putBackChar();
      ret = c;
      break;
  }

  s->tokenEnd( loc );

  return ret;
}

static
int parse_gt( char c, Source *s, StringUnifier *, semantic_type *token, location_type *loc )
{
  int ret = 0;

  s->tokenBegin( loc );

  const char second = s->next( loc );

  switch( second )
  {
    case '>':
      {
        const char third = s->next( loc );
        if( '=' == third )
        {
          token->assign = RIGHT_ASSIGN;
          ret = _Parser::token::RIGHT_ASSIGN;
        }
        else
        {
          s->putBackChar();
          ret = _Parser::token::RIGHT_OP;
        }
      }
      break;

    case '=':
       ret = _Parser::token::GE_OP;
       break;

    default:
      s->putBackChar();
      ret = c;
      break;
  }

  s->tokenEnd( loc );

  return ret;
}

static
int parse_eq( char c, Source *s, StringUnifier *, semantic_type *, location_type *loc )
{
  int ret = 0;

  s->tokenBegin( loc );

  const char second = s->next( loc );

  switch( second )
  {
    case '=':
       ret = _Parser::token::EQ_OP;
       break;

    default:
      s->putBackChar();
      ret = c;
      break;
  }

  s->tokenEnd( loc );

  return ret;
}

static
int parse_excl( char c, Source *s, StringUnifier *, semantic_type *, location_type *loc )
{
  int ret = 0;

  s->tokenBegin( loc );

  const char second = s->next( loc );

  switch( second )
  {
    case '=':
       ret = _Parser::token::NE_OP;
       break;

    default:
      s->putBackChar();
      ret = c;
      break;
  }

  s->tokenEnd( loc );

  return ret;
}

static
int parse_amp( char c, Source *s, StringUnifier *, semantic_type *token, location_type *loc )
{
  int ret = 0;

  s->tokenBegin( loc );

  const char second = s->next( loc );

  switch( second )
  {
    case '=':
      token->assign = AND_ASSIGN;
      ret = _Parser::token::AND_ASSIGN;
      break;

    case '&':
       ret = _Parser::token::AND_OP;
       break;

    default:
      s->putBackChar();
      ret = c;
      break;
  }

  s->tokenEnd( loc );

  return ret;
}

static
int parse_hat( char c, Source *s, StringUnifier *, semantic_type *token, location_type *loc )
{
  int ret = 0;

  s->tokenBegin( loc );

  const char second = s->next( loc );

  switch( second )
  {
    case '=':
      token->assign = XOR_ASSIGN;
      ret = _Parser::token::XOR_ASSIGN;
      break;

    case '^':
       ret = _Parser::token::XOR_OP;
       break;

    default:
      s->putBackChar();
      ret = c;
      break;
  }

  s->tokenEnd( loc );

  return ret;
}

static
int parse_pipe( char c, Source *s, StringUnifier *, semantic_type *token, location_type *loc )
{
  int ret = 0;

  s->tokenBegin( loc );

  const char second = s->next( loc );

  switch( second )
  {
    case '=':
      token->assign = OR_ASSIGN;
      ret = _Parser::token::OR_ASSIGN;
      break;

    case '|':
       ret = _Parser::token::OR_OP;
       break;

    default:
      s->putBackChar();
      ret = c;
      break;
  }

  s->tokenEnd( loc );

  return ret;
}

static
int parse_slash( char c, Source *s, StringUnifier *u, semantic_type *token, location_type *loc )
{
  int ret = 0;

  s->tokenBegin( loc );

  const char second = s->next( loc );

  switch( second )
  {
    case '=':
      token->assign = DIV_ASSIGN;
      ret = _Parser::token::DIV_ASSIGN;
      break;

    case '/':
       ret = parse_line_comment( second, s, u, token, loc );
       break;

    case '*':
       ret = parse_block_comment( second, s, u, token, loc );
       break;

    default:
      s->putBackChar();
      ret = c;
      break;
  }

  s->tokenEnd( loc );

  return ret;
}

static
int parse_dot( char c, Source *s, StringUnifier *, semantic_type *, location_type *loc )
{
  int ret = 0;

  s->tokenBegin( loc );

  const char second = s->next( loc );

  switch( second )
  {
    case '.':
      {
        const char third = s->next( loc );
        if( '.' == third )
        {
          ret = _Parser::token::ELLIPSIS;
        }
        else
        {
          s->putBackChar();
          ret = _Parser::token::DOTDOT;
        }
      }
      break;

    default:
      s->putBackChar();
      ret = c;
      break;
  }

  s->tokenEnd( loc );

  return ret;
}

int Scanner::yylex( semantic_type *token, location_type *loc )
{
  Source *const s = m_source.get();
  StringUnifier *const u = m_unifier.get();

  int ret = -1;

  do
  {
    const char c = waste_whitespace( s, loc );

    if( isalpha( c ) || '_' == c )
    {
      ret = parse_identifier( c, s, u, token, loc );
    }
    else if( isdigit( c ) )
    {
      ret = parse_number( c, s, u, token, loc );
    }
    else if( '\'' == c || '\"' == c )
    {
      ret = parse_string( c, s, u, token, loc );
    }
    else if( '-' == c )
    {
      ret = parse_minus( c, s, u, token, loc );
    }
    else if( '+' == c )
    {
      ret = parse_plus( c, s, u, token, loc );
    }
    else if( '<' == c )
    {
      ret = parse_lt( c, s, u, token, loc );
    }
    else if( '>' == c )
    {
      ret = parse_gt( c, s, u, token, loc );
    }
    else if( '=' == c )
    {
      ret = parse_eq( c, s, u, token, loc );
    }
    else if( '!' == c )
    {
      ret = parse_excl( c, s, u, token, loc );
    }
    else if( '&' == c )
    {
      ret = parse_amp( c, s, u, token, loc );
    }
    else if( '^' == c )
    {
      ret = parse_hat( c, s, u, token, loc );
    }
    else if( '|' == c )
    {
      ret = parse_pipe( c, s, u, token, loc );
    }
    else if( '/' == c )
    {
      ret = parse_slash( c, s, u, token, loc );
    }
    else if( '.' == c )
    {
      ret = parse_dot( c, s, u, token, loc );
    }
    else if( '#' == c )
    {
      ret = parse_line_comment( c, s, u, token, loc );
    }
    else if( '\0' == c )
    {
//      std::cerr << "Encountered 0" << std::endl;
      ret = c;
    }
    else
    {
      ret = c;
    }
  }
  while( ret < 0 );

  return ret;
}

} // namespace RPGML


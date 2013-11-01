#ifndef RPGML_Union_h
#define RPGML_Union_h

#include "String.h"
#include "AST.h"

#include <string>
#include <cassert>

namespace RPGML {

class Union
{
public:
  Union( void )
  : p( 0 )
  , t( T_INVALID )
  {}

  Union( const Union &other )
  : p( other.p )
  , t( other.t )
  {
    switch( t )
    {
      case T_INVALID   : break;
      case T_INT32     : break;
      case T_FLOAT32   : break;
      case T_STRING    : str->ref(); break;
      case T_STATEMENT : stmt->ref(); break;
      case T_EXPRESSION: expr->ref(); break;
    }
  }

  explicit Union( int _i )
  : i( _i )
  , t( T_INT32 )
  {}

  explicit Union( float _f )
  : f( _f )
  , t( T_FLOAT32 )
  {}

  explicit Union( String *_str )
  : str( _str )
  , t( T_STRING )
  {
    str->ref();
  }

  explicit Union( Statement *_stmt )
  : stmt( _stmt )
  , t( T_STATEMENT )
  {
    stmt->ref();
  }

  explicit Union( Expression  *_expr )
  : expr( _expr )
  , t( T_EXPRESSION )
  {
    expr->ref();
  }

  ~Union( void )
  {
    clear();
  }

  Union &operator=( Union other )
  {
    this->swap( other );
    return (*this);
  }

  Union &set( int _i )
  {
    clear();
    i = _i;
    t = T_INT32;
    return (*this);
  }

  Union &set( float _f )
  {
    clear();
    f = _f;
    t = T_FLOAT32;
    return (*this);
  }

  Union &set( String *_str )
  {
    if( t != T_STRING || _str != str )
    {
      clear();
      str = _str;
      t = T_STRING;
      str->ref();
    }
    return (*this);
  }

  Union &set( Statement *_stmt )
  {
    if( t != T_STATEMENT || _stmt != stmt )
    {
      clear();
      stmt = _stmt;
      t = T_STATEMENT;
      stmt->ref();
    }
    return (*this);
  }

  Union &set( Expression  *_expr )
  {
    if( t != T_EXPRESSION || _expr != expr )
    {
      clear();
      expr = _expr;
      t = T_EXPRESSION;
      expr->ref();
    }
    return (*this);
  }

  Union &clear( void )
  {
    switch( t )
    {
      case T_INVALID   : break;
      case T_INT32     : break;
      case T_FLOAT32   : break;
      case T_STRING    : if( !str->unref()  ) delete str ; break;
      case T_STATEMENT : if( !stmt->unref() ) delete stmt; break;
      case T_EXPRESSION: if( !expr->unref() ) delete expr; break;
    }
    p = 0;
    t = T_INVALID;
    return (*this);
  }

  void swap( Union &other )
  {
    std::swap( t, other.t );
    std::swap( p, other.p );
  }

  void moveTo( Union &other )
  {
    if( this != &other )
    {
      other.t = t; t = T_INVALID;
      other.p = p; p = 0;
    }
  }

  int getInt( void ) const
  {
    assert( t == T_INT32 );
    return i;
  }

  float getFloat( void ) const
  {
    assert( t == T_FLOAT32 );
    return f;
  }

  String *getString( void ) const
  {
    assert( t == T_STRING );
    return str;
  }

  Statement *getStatement( void ) const
  {
    assert( t == T_STATEMENT );
    return stmt;
  }

  Expression *getExpression( void ) const
  {
    assert( t == T_EXPRESSION );
    return expr;
  }

private:
  enum Type
  {
      T_INVALID
    , T_INT32
    , T_FLOAT32
    , T_STRING
    , T_STATEMENT
    , T_EXPRESSION
  };

  union
  {
    int i;
    float f;
    String      *str;
    Statement   *stmt;
    Expression  *expr;
    void        *p;
  };
  Type t;
};

} // namespace RPGML

namespace std
{
  void swap( RPGML::Union &x1, RPGML::Union &x2 )
  {
    x1.swap( x2 );
  }
}

#endif


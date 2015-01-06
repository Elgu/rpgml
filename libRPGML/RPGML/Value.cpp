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
#include "Value.h"

#include "String.h"
#include "Array.h"
#include "Frame.h"
#include "Function.h"
#include "Node.h"
#include "Sequence.h"

#include <sstream>
#include <cmath>

namespace RPGML {

Value::CastException::CastException( Type _from, Type _to )
: from( _from )
, to( _to )
{
  (*this)
    << "Cannot cast directly from " << from
    << " to " << to
    ;
}

Value::Value( void )
: p( 0 )
{}

Value::~Value( void )
{
  clear();
}

Value::Value( const Value &other )
: p( other.p )
, m_type( other.m_type )
{
  switch( m_type.getEnum() )
  {
    case Type::INVALID : break;
    case Type::BOOL    : break;
    case Type::UINT8   : break;
    case Type::INT8    : break;
    case Type::UINT16  : break;
    case Type::INT16   : break;
    case Type::UINT32  : break;
    case Type::INT32   : break;
    case Type::UINT64  : break;
    case Type::INT64   : break;
    case Type::FLOAT   : break;
    case Type::DOUBLE  : break;
    case Type::STRING  : str ->ref(); break;
    case Type::ARRAY   : arr ->ref(); break;
    case Type::FRAME   : frame->ref(); break;
    case Type::FUNCTION: func->ref(); break;
    case Type::NODE    : node->ref(); break;
    case Type::OUTPUT  : out ->ref(); break;
    case Type::INPUT   : in  ->ref(); break;
    case Type::PARAM   : param->ref(); break;
    case Type::SEQUENCE: seq ->ref(); break;
    case Type::OTHER   : break;
  }
}

Value::Value( bool      _b    ) : m_type( Type::BOOL     ) { b    = _b   ; }
Value::Value( int       _i    ) : m_type( Type::INT      ) { i    = _i   ; }
Value::Value( index_t   _i    ) : m_type( Type::INT      ) { i    = int( _i ); }
Value::Value( float     _f    ) : m_type( Type::FLOAT    ) { f    = _f   ; }
Value::Value( std::string const &_str  ) : m_type( Type::STRING   ) { String s(_str); str = s.getData(); str->ref(); }
Value::Value( char        const *_str  ) : m_type( Type::STRING   ) { String s(_str); str = s.getData(); str->ref(); }
Value::Value( String      const &_str  ) : m_type( Type::STRING   ) { str  = _str.getData(); str->ref(); }
Value::Value( StringData  const *_str  ) : m_type( Type::STRING   ) { str  = _str; str->ref(); }
Value::Value( ArrayBase         *_arr   ) : m_type( Type::ARRAY    ) { arr   = _arr  ; arr  ->ref(); }
Value::Value( Frame          *_frame ) : m_type( Type::FRAME    ) { frame = _frame; frame->ref(); }
Value::Value( Function       *_func  ) : m_type( Type::FUNCTION ) { func  = _func ; func ->ref(); }
Value::Value( Node           *_node  ) : m_type( Type::NODE     ) { node  = _node ; node ->ref(); }
Value::Value( Output         *_out   ) : m_type( Type::OUTPUT   ) { out   = _out  ; out  ->ref(); }
Value::Value( Input          *_in    ) : m_type( Type::INPUT    ) { in    = _in   ; in   ->ref(); }
Value::Value( Param          *_param ) : m_type( Type::PARAM    ) { param = _param; param->ref(); }
Value::Value( Sequence const *_seq   ) : m_type( Type::SEQUENCE ) { seq   = _seq  ; seq  ->ref(); }

Value &Value::set( bool               _b    ) { return (*this) = Value( _b    ); }
Value &Value::set( int                _i    ) { return (*this) = Value( _i    ); }
Value &Value::set( index_t            _i    ) { return (*this) = Value( _i    ); }
Value &Value::set( float              _f    ) { return (*this) = Value( _f    ); }
Value &Value::set( std::string const &_str  ) { return (*this) = Value( _str  ); }
Value &Value::set( char        const *_str  ) { return (*this) = Value( _str  ); }
Value &Value::set( String      const &_str  ) { return (*this) = Value( _str  ); }
Value &Value::set( StringData  const *_str  ) { return (*this) = Value( _str  ); }
Value &Value::set( ArrayBase         *_arr  ) { return (*this) = Value( _arr  ); }
Value &Value::set( Frame               *_frame  ) { return (*this) = Value( _frame  ); }
Value &Value::set( Function          *_func ) { return (*this) = Value( _func ); }
Value &Value::set( Node              *_node ) { return (*this) = Value( _node ); }
Value &Value::set( Output            *_out  ) { return (*this) = Value( _out  ); }
Value &Value::set( Input             *_in   ) { return (*this) = Value( _in   ); }
Value &Value::set( Param             *_param) { return (*this) = Value( _param); }
Value &Value::set( Sequence const    *_seq  ) { return (*this) = Value( _seq  ); }
Value &Value::set( const Value       &v     ) { return (*this) = v;              }

bool Value::get( bool            &x ) const { if( isBool    () ) { x = getBool    (); return true; } else return false; }
bool Value::get( int             &x ) const { if( isInt     () ) { x = getInt     (); return true; } else return false; }
bool Value::get( float           &x ) const { if( isFloat   () ) { x = getFloat   (); return true; } else return false; }
bool Value::get( String          &x ) const { if( isString  () ) { x = getString  (); return true; } else return false; }
bool Value::get( ArrayBase      *&x ) const { if( isArray   () ) { x = getArray   (); return true; } else return false; }
bool Value::get( Frame          *&x ) const { if( isFrame   () ) { x = getFrame   (); return true; } else return false; }
bool Value::get( Function       *&x ) const { if( isFunction() ) { x = getFunction(); return true; } else return false; }
bool Value::get( Node           *&x ) const { if( isNode    () ) { x = getNode    (); return true; } else return false; }
bool Value::get( Output         *&x ) const { if( isOutput  () ) { x = getOutput  (); return true; } else return false; }
bool Value::get( Input          *&x ) const { if( isInput   () ) { x = getInput   (); return true; } else return false; }
bool Value::get( Param          *&x ) const { if( isParam   () ) { x = getParam   (); return true; } else return false; }
bool Value::get( Sequence const *&x ) const { if( isSequence() ) { x = getSequence(); return true; } else return false; }
bool Value::get( CountPtr< ArrayBase > &x ) const { if( isArray   () ) { x = getArray   (); return true; } else return false; }
bool Value::get( CountPtr< Frame     > &x ) const { if( isFrame   () ) { x = getFrame   (); return true; } else return false; }
bool Value::get( CountPtr< Function  > &x ) const { if( isFunction() ) { x = getFunction(); return true; } else return false; }
bool Value::get( CountPtr< Node      > &x ) const { if( isNode    () ) { x = getNode    (); return true; } else return false; }
bool Value::get( CountPtr< Output    > &x ) const { if( isOutput  () ) { x = getOutput  (); return true; } else return false; }
bool Value::get( CountPtr< Input     > &x ) const { if( isInput   () ) { x = getInput   (); return true; } else return false; }
bool Value::get( CountPtr< const Sequence > &x ) const { if( isSequence() ) { x = getSequence(); return true; } else return false; }

Value::operator CountPtr< ArrayBase      >( void ) const { return getArray   (); }
Value::operator CountPtr< Frame          >( void ) const { return getFrame   (); }
Value::operator CountPtr< Function       >( void ) const { return getFunction(); }
Value::operator CountPtr< Node           >( void ) const { return getNode    (); }
Value::operator CountPtr< Output         >( void ) const { return getOutput  (); }
Value::operator CountPtr< Input          >( void ) const { return getInput   (); }
Value::operator CountPtr< Param          >( void ) const { return getParam   (); }
Value::operator CountPtr< Sequence const >( void ) const { return getSequence(); }

Value &Value::operator=( const Value &other )
{
  Value tmp( other );
  this->swap( tmp );
  return (*this);
}

void Value::clear( void )
{
  switch( m_type.getEnum() )
  {
    case Type::INVALID : break;
    case Type::BOOL    : break;
    case Type::UINT8   : break;
    case Type::INT8    : break;
    case Type::UINT16  : break;
    case Type::INT16   : break;
    case Type::UINT32  : break;
    case Type::INT32   : break;
    case Type::UINT64  : break;
    case Type::INT64   : break;
    case Type::FLOAT   : break;
    case Type::DOUBLE  : break;
    case Type::STRING  : if( !str ->unref() ) delete str ; break;
    case Type::ARRAY   : if( !arr ->unref() ) delete arr ; break;
    case Type::FRAME   : if( !frame->unref() ) delete frame ; break;
    case Type::FUNCTION: if( !func->unref() ) delete func; break;
    case Type::NODE    : if( !node->unref() ) delete node; break;
    case Type::OUTPUT  : if( !out ->unref() ) delete out ; break;
    case Type::INPUT   : if( !in  ->unref() ) delete in  ; break;
    case Type::PARAM   : if( !param->unref() ) delete param; break;
    case Type::SEQUENCE: if( !seq ->unref() ) delete seq ; break;
    case Type::OTHER   : break;
  }
  p = 0;
  m_type = Type::INVALID;
}

void Value::swap( Value &other )
{
  std::swap( m_type, other.m_type );
  std::swap( p, other.p );
}

bool Value::isCollectable( void ) const
{
  static const int flags =
      ( 0 << Type::INVALID  )
    | ( 0 << Type::BOOL     )
    | ( 0 << Type::INT      )
    | ( 0 << Type::FLOAT    )
    | ( 0 << Type::STRING   )
    | ( 1 << Type::ARRAY    )
    | ( 1 << Type::FRAME    )
    | ( 1 << Type::FUNCTION )
    | ( 1 << Type::NODE     )
    | ( 1 << Type::OUTPUT   )
    | ( 1 << Type::INPUT    )
    | ( 1 << Type::PARAM    )
    | ( 1 << Type::SEQUENCE )
    ;
  return flags & ( 1 << m_type.getEnum() );
}

const Collectable *Value::getCollectable( void ) const
{
  switch( m_type.getEnum() )
  {
    case Type::INVALID : return 0;
    case Type::BOOL    : return 0;
    case Type::UINT8   : return 0;
    case Type::INT8    : return 0;
    case Type::UINT16  : return 0;
    case Type::INT16   : return 0;
    case Type::UINT32  : return 0;
    case Type::INT32   : return 0;
    case Type::UINT64  : return 0;
    case Type::INT64   : return 0;
    case Type::FLOAT   : return 0;
    case Type::DOUBLE  : return 0;
    case Type::STRING  : return 0;
    case Type::ARRAY   : return arr ;
    case Type::FRAME   : return frame ;
    case Type::FUNCTION: return func;
    case Type::NODE    : return node;
    case Type::OUTPUT  : return out ;
    case Type::INPUT   : return in  ;
    case Type::PARAM   : return param;
    case Type::SEQUENCE: return seq ;
    case Type::OTHER   : return 0;
  }
  return 0;
}

Value Value::to( Type type ) const
{
  const Value &x = (*this);

  if( type == x.getType() ) return x;

  switch( type.getEnum() )
  {
    case Type::BOOL  :
      switch( x.getEnum() )
      {
        case Type::INT   : return Value( bool( getInt  () ) );
        case Type::FLOAT : return Value( bool( getFloat() ) );
        case Type::STRING: throw CastException( x.getType(), type );
        default:
          throw CastException( x.getType(), type );
      }

    case Type::INT   :
      switch( x.getEnum() )
      {
        case Type::BOOL  : return Value( int( getBool()  ) );
        case Type::FLOAT : return Value( int( getFloat() ) );
        case Type::STRING: return Value( atoi( getString().c_str() ) );
        default:
          throw CastException( x.getType(), type );
      }

    case Type::FLOAT :
      switch( x.getEnum() )
      {
        case Type::BOOL  : return Value( float( getBool() ) );
        case Type::INT   : return Value( float( getInt()  ) );
        case Type::STRING: return Value( float( atof( getString().c_str() ) ) );
        default:
          throw CastException( x.getType(), type );
      }

    case Type::STRING:
      {
        std::ostringstream s;

        switch( x.getEnum() )
        {
          case Type::BOOL  : s << getBool(); break;
          case Type::INT   : s << getInt(); break;
          case Type::FLOAT : s << getFloat(); break;
          default:
            throw CastException( x.getType(), type );
        }

        return Value( String( s.str() ) );
      }

    default:
      throw CastException( x.getType(), type );
  }
}

bool Value::operator< ( const Value &right ) const
{
  const Value &left = (*this);
  if( left.isScalar() && right.isScalar() )
  {
    const Type ret_type = std::max( left.getType(), right.getType() );

    const Value l = left .to( ret_type );
    const Value r = right.to( ret_type );

    switch( ret_type.getEnum() )
    {
      case Type::BOOL : return ( l.getBool()  < r.getBool() );
      case Type::INT  : return ( l.getInt ()  < r.getInt() );
      case Type::FLOAT: return ( l.getFloat() < r.getFloat() );
      default:
        throw Exception( "Internal error: unhandled scalar" );
    }
  }
  else if( left.isString() && right.isString() )
  {
    return ( left.getString() < right.getString() );
  }
  else if( left.getType() == right.getType() )
  {
    return ( left.getP() < right.getP() );
  }
  else
  {
    throw Exception()
      << "Cannot compare " << left.getTypeName()
      << " and " << right.getTypeName()
      << " directly"
      ;
  }
}

bool Value::operator<=( const Value &right ) const
{
  const Value &left = (*this);
  if( left.isScalar() && right.isScalar() )
  {
    const Type ret_type = std::max( left.getType(), right.getType() );

    const Value l = left .to( ret_type );
    const Value r = right.to( ret_type );

    switch( ret_type.getEnum() )
    {
      case Type::BOOL : return ( l.getBool()  <= r.getBool() );
      case Type::INT  : return ( l.getInt ()  <= r.getInt() );
      case Type::FLOAT: return ( l.getFloat() <= r.getFloat() );
      default:
        throw Exception( "Internal error: unhandled scalar" );
    }
  }
  else if( left.isString() && right.isString() )
  {
    return ( left.getString() <= right.getString() );
  }
  else if( left.getType() == right.getType() )
  {
    return ( left.getP() <= right.getP() );
  }
  else
  {
    throw Exception()
      << "Cannot compare " << left.getTypeName()
      << " and " << right.getTypeName()
      << " directly"
      ;
  }
}

bool Value::operator> ( const Value &right ) const
{
  const Value &left = (*this);
  if( left.isScalar() && right.isScalar() )
  {
    const Type ret_type = std::max( left.getType(), right.getType() );

    const Value l = left .to( ret_type );
    const Value r = right.to( ret_type );

    switch( ret_type.getEnum() )
    {
      case Type::BOOL : return ( l.getBool()  > r.getBool() );
      case Type::INT  : return ( l.getInt ()  > r.getInt() );
      case Type::FLOAT: return ( l.getFloat() > r.getFloat() );
      default:
        throw Exception( "Internal error: unhandled scalar" );
    }
  }
  else if( left.isString() && right.isString() )
  {
    return ( left.getString() > right.getString() );
  }
  else if( left.getType() == right.getType() )
  {
    return ( left.getP() > right.getP() );
  }
  else
  {
    throw Exception()
      << "Cannot compare " << left.getTypeName()
      << " and " << right.getTypeName()
      <<" directly"
      ;
  }
}

bool Value::operator>=( const Value &right ) const
{
  const Value &left = (*this);
  if( left.isScalar() && right.isScalar() )
  {
    const Type ret_type = std::max( left.getType(), right.getType() );

    const Value l = left .to( ret_type );
    const Value r = right.to( ret_type );

    switch( ret_type.getEnum() )
    {
      case Type::BOOL : return ( l.getBool()  >= r.getBool() );
      case Type::INT  : return ( l.getInt ()  >= r.getInt() );
      case Type::FLOAT: return ( l.getFloat() >= r.getFloat() );
      default:
        throw Exception( "Internal error: unhandled scalar" );
    }
  }
  else if( left.isString() && right.isString() )
  {
    return ( left.getString() >= right.getString() );
  }
  else if( left.getType() == right.getType() )
  {
    return ( left.getP() >= right.getP() );
  }
  else
  {
    throw Exception()
      << "Cannot compare " << left.getTypeName()
      << " and " << right.getTypeName()
      << " directly"
      ;
  }
}

bool Value::operator==( const Value &right ) const
{
  const Value &left = (*this);
  if( left.isScalar() && right.isScalar() )
  {
    const Type ret_type = std::max( left.getType(), right.getType() );

    const Value l = left .to( ret_type );
    const Value r = right.to( ret_type );

    switch( ret_type.getEnum() )
    {
      case Type::BOOL : return ( l.getBool()  == r.getBool() );
      case Type::INT  : return ( l.getInt ()  == r.getInt() );
      case Type::FLOAT: return ( l.getFloat() == r.getFloat() );
      default:
        throw Exception( "Internal error: unhandled scalar" );
    }
  }
  else if( left.isString() && right.isString() )
  {
    return ( left.getString() == right.getString() );
  }
  else if( left.getType() == right.getType() )
  {
    return ( left.getP() == right.getP() );
  }
  else
  {
    throw Exception()
      << "Cannot compare " << left.getTypeName()
      << " and " << right.getTypeName()
      << " directly"
      ;
  }
}

bool Value::operator!=( const Value &right ) const
{
  const Value &left = (*this);
  if( left.isScalar() && right.isScalar() )
  {
    const Type ret_type = std::max( left.getType(), right.getType() );

    const Value l = left .to( ret_type );
    const Value r = right.to( ret_type );

    switch( ret_type.getEnum() )
    {
      case Type::BOOL : return ( l.getBool()  != r.getBool() );
      case Type::INT  : return ( l.getInt ()  != r.getInt() );
      case Type::FLOAT: return ( l.getFloat() != r.getFloat() );
      default:
        throw Exception( "Internal error: unhandled scalar" );
    }
  }
  else if( left.isString() && right.isString() )
  {
    return ( left.getString() != right.getString() );
  }
  else if( left.getType() == right.getType() )
  {
    return ( left.getP() != right.getP() );
  }
  else
  {
    throw Exception()
      << "Cannot compare " << left.getTypeName()
      << " and " << right.getTypeName()
      << " directly"
      ;
  }
}

bool Value::operator&&( const Value &right ) const
{
  const Value &left = (*this);
  if( left.isScalar() && right.isScalar() )
  {
    const Value l = left .to( Type::BOOL );
    const Value r = right.to( Type::BOOL );
    return ( l && r );
  }
  else if( !left.isScalar() )
  {
    throw Exception( "left must be scalar" );
  }
  else if( !right.isScalar() )
  {
    throw Exception( "right must be scalar" );
  }
  else
  {
    throw Exception( "left and right must be scalar" );
  }
}

bool Value::operator||( const Value &right ) const
{
  const Value &left = (*this);
  if( left.isScalar() && right.isScalar() )
  {
    const Value l = left .to( Type::BOOL );
    const Value r = right.to( Type::BOOL );
    return ( l || r );
  }
  else if( !left.isScalar() )
  {
    throw Exception( "left must be scalar" );
  }
  else if( !right.isScalar() )
  {
    throw Exception( "right must be scalar" );
  }
  else
  {
    throw Exception( "left and right must be scalar" );
  }
}

bool Value::log_xor   ( const Value &right ) const
{
  const Value &left = (*this);
  if( left.isScalar() && right.isScalar() )
  {
    const Value l = left .to( Type::BOOL );
    const Value r = right.to( Type::BOOL );
    return ( l.getBool() ^ r.getBool() );
  }
  else if( !left.isScalar() )
  {
    throw Exception( "left must be scalar" );
  }
  else if( !right.isScalar() )
  {
    throw Exception( "right must be scalar" );
  }
  else
  {
    throw Exception( "left and right must be scalar" );
  }
}

Value Value::operator<<( const Value &right ) const
{
  const Value &left = (*this);
  if( !left.isInteger() ) throw Exception( "left must be integer" );
  if( !right.isInteger() ) throw Exception( "right must be integer" );

  const Value l = left .to( Type::INT );
  const Value r = right.to( Type::INT );

  return Value( l.getInt() << r.getInt() );
}

Value Value::operator>>( const Value &right ) const
{
  const Value &left = (*this);
  if( !left.isInteger() ) throw Exception( "left must be integer" );
  if( !right.isInteger() ) throw Exception( "right must be integer" );

  const Value l = left .to( Type::INT );
  const Value r = right.to( Type::INT );

  return Value( l.getInt() >> r.getInt() );
}

Value Value::operator& ( const Value &right ) const
{
  const Value &left = (*this);
  if( !left.isInteger() ) throw Exception( "left must be integer" );
  if( !right.isInteger() ) throw Exception( "right must be integer" );

  const Value l = left .to( Type::INT );
  const Value r = right.to( Type::INT );

  return Value( l.getInt() & r.getInt() );
}

Value Value::operator| ( const Value &right ) const
{
  const Value &left = (*this);
  if( !left.isInteger() ) throw Exception( "left must be integer" );
  if( !right.isInteger() ) throw Exception( "right must be integer" );

  const Value l = left .to( Type::INT );
  const Value r = right.to( Type::INT );

  return Value( l.getInt() | r.getInt() );
}

Value Value::operator^ ( const Value &right ) const
{
  const Value &left = (*this);
  if( !left.isInteger() ) throw Exception( "left must be integer" );
  if( !right.isInteger() ) throw Exception( "right must be integer" );

  const Value l = left .to( Type::INT );
  const Value r = right.to( Type::INT );

  return Value( l.getInt() ^ r.getInt() );
}

Value Value::operator* ( const Value &right ) const
{
  const Value &left = (*this);
  if( left.isScalar() && right.isScalar() )
  {
    const Type ret_type = std::max( left.getType(), right.getType() );

    const Value l = left .to( ret_type );
    const Value r = right.to( ret_type );

    switch( ret_type.getEnum() )
    {
      case Type::BOOL : return Value( l.getBool()  * r.getBool() );
      case Type::INT  : return Value( l.getInt ()  * r.getInt() );
      case Type::FLOAT: return Value( l.getFloat() * r.getFloat() );
      default:
        throw Exception( "Internal error: unhandled scalar" );
    }
  }
  else
  {
    throw Exception( "left and right must be scalar" );
  }
}

Value Value::operator/ ( const Value &right ) const
{
  const Value &left = (*this);
  if( left.isScalar() && right.isScalar() )
  {
    const Type ret_type = std::max( left.getType(), right.getType() );

    const Value l = left .to( ret_type );
    const Value r = right.to( ret_type );

    switch( ret_type.getEnum() )
    {
      case Type::BOOL :
        if( r.getBool()  )
          return Value( l.getBool()  / r.getBool() );
        else
          throw Exception( "Division by zero" );

      case Type::INT  :
        if( r.getInt()   )
          return Value( l.getInt ()  / r.getInt() );
        else
          throw Exception( "Division by zero" );

      case Type::FLOAT:
        if( r.getFloat() )
          return Value( l.getFloat() / r.getFloat() );
        else
          throw Exception( "Division by zero" );

      default:
        throw Exception( "Internal error: unhandled scalar" );
    }
  }
  else
  {
    throw Exception( "left and right must be scalar." );
  }
}

Value Value::operator+ ( const Value &right ) const
{
  const Value &left = (*this);
  if( left.isScalar() && right.isScalar() )
  {
    const Type ret_type = std::max( Type::Int(), std::max( left.getType(), right.getType() ) );

    const Value l = left .to( ret_type );
    const Value r = right.to( ret_type );

    switch( ret_type.getEnum() )
    {
      case Type::BOOL : return Value( l.getBool()  + r.getBool() );
      case Type::INT  : return Value( l.getInt ()  + r.getInt() );
      case Type::FLOAT: return Value( l.getFloat() + r.getFloat() );
      default:
        throw Exception( "Internal error: unhandled scalar" );
    }
  }
  else if( left.isString() && right.isString() )
  {
    const Value &l = left;
    const Value &r = right;
    return Value( l.getString() + r.getString() );
  }
  else if( left.isString() && right.isScalar() )
  {
    const Value &l = left;
    const Value  r = right.to( Type::String() );
    return Value( l.getString() + r.getString() );
  }
  else if( left.isScalar() && right.isString() )
  {
    const Value  l = left.to( Type::String() );
    const Value &r = right;
    return Value( l.getString() + r.getString() );
  }
  else
  {
    throw Exception( "left and right must be scalar or string" );
  }
}

Value Value::operator- ( const Value &right ) const
{
  const Value &left = (*this);
  if( left.isScalar() && right.isScalar() )
  {
    const Type ret_type = std::max( left.getType(), right.getType() );

    const Value l = left .to( ret_type );
    const Value r = right.to( ret_type );

    switch( ret_type.getEnum() )
    {
      case Type::BOOL : return Value( l.getBool()  - r.getBool() );
      case Type::INT  : return Value( l.getInt ()  - r.getInt() );
      case Type::FLOAT: return Value( l.getFloat() - r.getFloat() );
      default:
        throw Exception( "Internal error: unhandled scalar" );
    }
  }
  else
  {
    throw Exception( "left and right must be scalar" );
  }
}

Value Value::operator% ( const Value &right ) const
{
  const Value &left = (*this);
  switch( left.getEnum() )
  {
    case Type::BOOL : return Value( false );

    case Type::INT  :
      switch( right.getEnum() )
      {
        case Type::BOOL : return Value( false );
        case Type::INT  : return Value( left.getInt() % right.getInt() );
        case Type::FLOAT:
          {
            const float l = float( left .getInt() );
            const float r = float( right.getFloat() );
            if( r )
              return Value( l - floorf( l / r ) * r );
            else
              throw Exception( "Modulo by zero" );
          }
        default:
          throw Exception( "left and right must be scalar" );
      }

    case Type::FLOAT:
      {
        float l;
        float r;

        switch( right.getEnum() )
        {
          case Type::BOOL : return Value( false );

          case Type::INT  :
            l = float( left .getFloat() );
            r = float( right.getInt() );
            break;

          case Type::FLOAT:
            l = float( left .getFloat() );
            r = float( right.getFloat() );
            break;

          default:
            throw Exception( "left and right must be scalar" );
        }

        if( r )
          return Value( l - floorf( l / r ) * r );
        else
          throw Exception( "Modulo by zero" );
      }

    default:
      throw Exception( "left and right must be scalar" );
  }
}

std::ostream &Value::print( std::ostream &o ) const
{
  switch( getType().getEnum() )
  {
    case Type::INVALID : o << "nil"; break;
    case Type::BOOL    : o << ( getBool() ? "true" : "false" ); break;
//    case Type::UINT8   : o << getUInt8 (); break;
//    case Type::INT8    : o << getInt8  (); break;
//    case Type::UINT16  : o << getUInt16(); break;
//    case Type::INT16   : o << getInt16 (); break;
//    case Type::UINT32  : o << getUInt32(); break;
    case Type::INT32   : o << getInt32 (); break;
//    case Type::UINT64  : o << getUInt64(); break;
//    case Type::INT64   : o << getInt64 (); break;
    case Type::FLOAT   : o << getFloat (); break;
//    case Type::DOUBLE  : o << getDouble(); break;
    case Type::STRING  : o << "\"" << getString() << "\""; break;

    case Type::ARRAY   :
      {
        o << "[ TODO: print Array ]";
        /*
        const Array &a = *getArray();
        o << "[ ";
        for( index_t j( 0 ), end( a.size() ); j < end; ++j )
        {
          if( j > 0 ) o << ", ";
          o << a[ j ];
        }
        o << " ]";
        */
      }
      break;

    case Type::FRAME:
      {
        const Frame *m = getFrame();
        o << "{ ";
        for( CountPtr< RPGML::Frame::Iterator > j( m->getIterator() ); !j->done(); j->next() )
        {
          const Frame::Iterator::Type v( j->get() );
          o << " " << v.second.getTypeName() << " " << v.first << " = " << v.second << ";";
        }
        o << " }";
      }
      break;

    case Type::FUNCTION: o << "Function( '" << getFunction()->getName() << "' (" << (void*)getFunction() << ") )"; break;
    case Type::NODE    : o << "Node( " << getNode()->getName() << ", " << getNode()->getIdentifier() << " )"; break;

    case Type::OUTPUT  :
      {
        const Output *const output = getOutput();
        o
          << "Output( " << output->getParent()->getIdentifier()
          << ", " << output->getIdentifier() << " )"
          ;
      }
      break;

    case Type::INPUT   :
      {
        const Input *const input = getInput();
        o
          << "Input( " << input->getParent()->getIdentifier()
          << ", " << input->getIdentifier() << " )"
          ;
      }
      break;

    case Type::PARAM   :
      {
        const Param *const pm = getParam();
        o
          << "Param( " << pm->getParent()->getIdentifier()
          << ", " << pm->getIdentifier() << " )"
          ;
      }
      break;

    case Type::SEQUENCE: getSequence()->print( o ); break;

    default:
      throw Exception( "Invalid Type for Value::print()" );
  }

  return o;
}


} // namespace RPGML

namespace std {

std::ostream &operator<<( std::ostream &o, const RPGML::Value &v )
{
  return v.print( o );
}

} // namespace std

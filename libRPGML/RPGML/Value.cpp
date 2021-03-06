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
#include "StringUnifier.h"
#include "ParseException.h"
#include "make_printable.h"

#include <sstream>
#include <cmath>

namespace RPGML {

Value::CastFailed::CastFailed( Type _from, Type _to )
: from( _from )
, to( _to )
{
  (*this)
    << "Cannot cast directly from " << from
    << " to " << to
    ;
}

Value::GetFailed::GetFailed( Type _is, Type _tried )
: is( _is )
, tried( _tried )
{
  (*this)
    << "Could not get Value as " << tried
    << ", is " << is
    ;
}

EXCEPTION_DERIVED_DEFINE_FIXED_TEXT( Value, DivisionByZero, "division by zero" );

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
    case Type::NIL     : break;
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
    case Type::STRING  : if( str   ) str  ->ref(); break;
    case Type::ARRAY   : if( arr   ) arr  ->ref(); break;
    case Type::FRAME   : if( frame ) frame->ref(); break;
    case Type::FUNCTION: if( func  ) func ->ref(); break;
    case Type::NODE    : if( node  ) node ->ref(); break;
    case Type::OUTPUT  : if( out   ) out  ->ref(); break;
    case Type::INPUT   : if( in    ) in   ->ref(); break;
    case Type::INOUT   : if( inout ) inout->ref(); break;
    case Type::PARAM   : if( param ) param->ref(); break;
    case Type::SEQUENCE: if( seq   ) seq  ->ref(); break;
    case Type::REF     : if( ref   ) ref  ->ref(); break;
    case Type::OTHER   : break;
  }
}

Value::Value( bool     _x ) : m_type( Type::BOOL   ) { b    = _x; }
Value::Value( uint8_t  _x ) : m_type( Type::UINT8  ) { ui8  = _x; }
Value::Value( int8_t   _x ) : m_type( Type::INT8   ) {  i8  = _x; }
Value::Value( uint16_t _x ) : m_type( Type::UINT16 ) { ui16 = _x; }
Value::Value( int16_t  _x ) : m_type( Type::INT16  ) {  i16 = _x; }
Value::Value( uint32_t _x ) : m_type( Type::UINT32 ) { ui32 = _x; }
Value::Value( int32_t  _x ) : m_type( Type::INT32  ) {  i32 = _x; }
Value::Value( uint64_t _x ) : m_type( Type::UINT64 ) { ui64 = _x; }
Value::Value( int64_t  _x ) : m_type( Type::INT64  ) {  i64 = _x; }
Value::Value( float    _x ) : m_type( Type::FLOAT  ) { f    = _x; }
Value::Value( double   _x ) : m_type( Type::DOUBLE ) { d    = _x; }
Value::Value( std::string const &_str  ) : m_type( Type::STRING   ) { String s(_str); str = s.getData(); str->ref(); }
Value::Value( char        const *_str  ) : m_type( Type::STRING   ) { String s(_str); str = s.getData(); str->ref(); }
Value::Value( String      const &_str  ) : m_type( Type::STRING   ) { CountPtr< const StringData > s( _str.getData() ); str = s.get(); str->ref(); }
Value::Value( StringData  const *_str   ) : m_type( Type::STRING   ) { str   = _str  ; if( str   ) str  ->ref(); }
Value::Value( Frame             *_frame ) : m_type( Type::FRAME    ) { frame = _frame; if( frame ) frame->ref(); }
Value::Value( Function          *_func  ) : m_type( Type::FUNCTION ) { func  = _func ; if( func  ) func ->ref(); }
Value::Value( Node              *_node  ) : m_type( Type::NODE     ) { node  = _node ; if( node  ) node ->ref(); }
Value::Value( Output            *_out   ) : m_type( Type::OUTPUT   ) { out   = _out  ; if( out   ) out  ->ref(); }
Value::Value( Input             *_in    ) : m_type( Type::INPUT    ) { in    = _in   ; if( in    ) in   ->ref(); }
Value::Value( InOut             *_inout ) : m_type( Type::INOUT    ) { inout = _inout; if( inout ) inout->ref(); }
Value::Value( Param             *_param ) : m_type( Type::PARAM    ) { param = _param; if( param ) param->ref(); }
Value::Value( Sequence    const *_seq   ) : m_type( Type::SEQUENCE ) { seq   = _seq  ; if( seq   ) seq  ->ref(); }
Value::Value( ArrayBase         *_arr   ) : m_type( Type::ARRAY    ) { arr   = _arr  ; if( arr   ) arr  ->ref(); }
Value::Value( const Reference   &_ref   ) : m_type( Type::REF      ) { ref = new Reference( _ref ); ref->ref(); }
Value::Value( Reference         *_ref   ) : m_type( Type::REF      ) { ref   = _ref  ; if( ref   ) ref  ->ref(); }

Value::Value( const void * )
{
  throw Exception()
    << "Internal: Tried to set a Value with a const pointer"
    << ": Try using a const_cast<>, but be careful, that enables possible writes"
    ;
}

Value &Value::set( bool               _x    ) { return (*this) = Value( _x ); }
Value &Value::set( uint8_t            _x    ) { return (*this) = Value( _x ); }
Value &Value::set( int8_t             _x    ) { return (*this) = Value( _x ); }
Value &Value::set( uint16_t           _x    ) { return (*this) = Value( _x ); }
Value &Value::set( int16_t            _x    ) { return (*this) = Value( _x ); }
Value &Value::set( uint32_t           _x    ) { return (*this) = Value( _x ); }
Value &Value::set( int32_t            _x    ) { return (*this) = Value( _x ); }
Value &Value::set( uint64_t           _x    ) { return (*this) = Value( _x ); }
Value &Value::set( int64_t            _x    ) { return (*this) = Value( _x ); }
Value &Value::set( float              _x    ) { return (*this) = Value( _x ); }
Value &Value::set( double             _x    ) { return (*this) = Value( _x ); }
Value &Value::set( std::string const &_str  ) { return (*this) = Value( _str  ); }
Value &Value::set( char        const *_str  ) { return (*this) = Value( _str  ); }
Value &Value::set( String      const &_str  ) { return (*this) = Value( _str  ); }
Value &Value::set( StringData  const *_str  ) { return (*this) = Value( _str  ); }
Value &Value::set( Frame               *_frame  ) { return (*this) = Value( _frame  ); }
Value &Value::set( Function          *_func ) { return (*this) = Value( _func ); }
Value &Value::set( Node              *_node ) { return (*this) = Value( _node ); }
Value &Value::set( Output            *_out  ) { return (*this) = Value( _out  ); }
Value &Value::set( Input             *_in   ) { return (*this) = Value( _in   ); }
Value &Value::set( InOut             *_inout) { return (*this) = Value( _inout); }
Value &Value::set( Param             *_param) { return (*this) = Value( _param); }
Value &Value::set( Sequence const    *_seq  ) { return (*this) = Value( _seq  ); }
Value &Value::set( ArrayBase         *_arr  ) { return (*this) = Value( _arr  ); }
Value &Value::set( const Reference   &_ref  ) { return (*this) = Value( _ref  ); }
Value &Value::set( Reference         *_ref  ) { return (*this) = Value( _ref  ); }

Value &Value::set( const Value       &v     ) { return (*this) = v;              }

Value &Value::set( const void *vp )
{
  return (*this) = Value( vp );
}

Output *Value::getOutput  ( void ) const { Value_assert( isOutput  () ); return ( is( Type::OUTPUT ) ? out : inout->getOutput() ); }
Input  *Value::getInput   ( void ) const { Value_assert( isInput   () ); return ( is( Type::INPUT  ) ? in  : inout->getInput()  ); }

void Value::get( bool            &x ) const { if( isBool  () ) x = getBool  (); else throw GetFailed( m_type, typeOf( x ) ); }
void Value::get( uint8_t         &x ) const { if( isUInt8 () ) x = getUInt8 (); else throw GetFailed( m_type, typeOf( x ) ); }
void Value::get( int8_t          &x ) const { if( isInt8  () ) x = getInt8  (); else throw GetFailed( m_type, typeOf( x ) ); }
void Value::get( uint16_t        &x ) const { if( isUInt16() ) x = getUInt16(); else throw GetFailed( m_type, typeOf( x ) ); }
void Value::get( int16_t         &x ) const { if( isInt16 () ) x = getInt16 (); else throw GetFailed( m_type, typeOf( x ) ); }
void Value::get( uint32_t        &x ) const { if( isUInt32() ) x = getUInt32(); else throw GetFailed( m_type, typeOf( x ) ); }
void Value::get( int32_t         &x ) const { if( isInt32 () ) x = getInt32 (); else throw GetFailed( m_type, typeOf( x ) ); }
void Value::get( uint64_t        &x ) const { if( isUInt64() ) x = getUInt64(); else throw GetFailed( m_type, typeOf( x ) ); }
void Value::get( int64_t         &x ) const { if( isInt64 () ) x = getInt64 (); else throw GetFailed( m_type, typeOf( x ) ); }
void Value::get( float           &x ) const { if( isFloat () ) x = getFloat (); else throw GetFailed( m_type, typeOf( x ) ); }
void Value::get( double          &x ) const { if( isDouble() ) x = getDouble(); else throw GetFailed( m_type, typeOf( x ) ); }
void Value::get( String          &x ) const { if( isString  () ) x = getString  (); else throw GetFailed( m_type, typeOf( x ) ); }
void Value::get( Frame          *&x ) const { if( isFrame   () ) x = getFrame   (); else throw GetFailed( m_type, typeOf( x ) ); }
void Value::get( Function       *&x ) const { if( isFunction() ) x = getFunction(); else throw GetFailed( m_type, typeOf( x ) ); }
void Value::get( Node           *&x ) const { if( isNode    () ) x = getNode    (); else throw GetFailed( m_type, typeOf( x ) ); }
void Value::get( Output         *&x ) const { if( isOutput  () ) x = getOutput  (); else throw GetFailed( m_type, typeOf( x ) ); }
void Value::get( Input          *&x ) const { if( isInput   () ) x = getInput   (); else throw GetFailed( m_type, typeOf( x ) ); }
void Value::get( InOut          *&x ) const { if( isInOut   () ) x = getInOut   (); else throw GetFailed( m_type, typeOf( x ) ); }
void Value::get( Param          *&x ) const { if( isParam   () ) x = getParam   (); else throw GetFailed( m_type, typeOf( x ) ); }
void Value::get( Sequence const *&x ) const { if( isSequence() ) x = getSequence(); else throw GetFailed( m_type, typeOf( x ) ); }
void Value::get( ArrayBase      *&x ) const { if( isArray   () ) x = getArray   (); else throw GetFailed( m_type, typeOf( x ) ); }
void Value::get( Reference      *&x ) const { if( isRef     () ) x = getRef     (); else throw GetFailed( m_type, typeOf( x ) ); }

void Value::get( CountPtr< ArrayBase > &x ) const { if( isArray   () ) x = getArray   (); else throw GetFailed( m_type, typeOf( x ) ); }
void Value::get( CountPtr< Frame     > &x ) const { if( isFrame   () ) x = getFrame   (); else throw GetFailed( m_type, typeOf( x ) ); }
void Value::get( CountPtr< Function  > &x ) const { if( isFunction() ) x = getFunction(); else throw GetFailed( m_type, typeOf( x ) ); }
void Value::get( CountPtr< Node      > &x ) const { if( isNode    () ) x = getNode    (); else throw GetFailed( m_type, typeOf( x ) ); }
void Value::get( CountPtr< Output    > &x ) const { if( isOutput  () ) x = getOutput  (); else throw GetFailed( m_type, typeOf( x ) ); }
void Value::get( CountPtr< Input     > &x ) const { if( isInput   () ) x = getInput   (); else throw GetFailed( m_type, typeOf( x ) ); }
void Value::get( CountPtr< InOut     > &x ) const { if( isInOut   () ) x = getInOut   (); else throw GetFailed( m_type, typeOf( x ) ); }
void Value::get( CountPtr< Param     > &x ) const { if( isParam   () ) x = getParam   (); else throw GetFailed( m_type, typeOf( x ) ); }
void Value::get( CountPtr< const Sequence > &x ) const { if( isSequence() ) x = getSequence(); else throw GetFailed( m_type, typeOf( x ) ); }
void Value::get( CountPtr< Reference > &x ) const { if( isRef() ) x = getRef(); else throw GetFailed( m_type, typeOf( x ) ); }

Value::operator bool            ( void ) const { return save_cast< bool     >(); }
Value::operator uint8_t         ( void ) const { return save_cast< uint8_t  >(); }
Value::operator int8_t          ( void ) const { return save_cast< int8_t   >(); }
Value::operator uint16_t        ( void ) const { return save_cast< uint16_t >(); }
Value::operator int16_t         ( void ) const { return save_cast< int16_t  >(); }
Value::operator uint32_t        ( void ) const { return save_cast< uint32_t >(); }
Value::operator int32_t         ( void ) const { return save_cast< int32_t  >(); }
Value::operator uint64_t        ( void ) const { return save_cast< uint64_t >(); }
Value::operator int64_t         ( void ) const { return save_cast< int64_t  >(); }
Value::operator float           ( void ) const { return save_cast< float    >(); }
Value::operator double          ( void ) const { return save_cast< double   >(); }
Value::operator String          ( void ) const { return save_cast< String   >(); }
Value::operator Frame          *( void ) const { return getFrame   (); }
Value::operator Function       *( void ) const { return getFunction(); }
Value::operator Node           *( void ) const { return getNode    (); }
Value::operator Output         *( void ) const { return getOutput  (); }
Value::operator Input          *( void ) const { return getInput   (); }
Value::operator InOut          *( void ) const { return save_cast( Type::InOut() ).get< InOut* >(); }
Value::operator Param          *( void ) const { return getParam   (); }
Value::operator Sequence const *( void ) const { return getSequence(); }
Value::operator ArrayBase      *( void ) const { return getArray   (); }
Value::operator const Reference&( void ) const { return *getRef     (); }
Value::operator Reference      *( void ) const { return getRef     (); }
Value::operator CountPtr< Frame          >( void ) const { return getFrame   (); }
Value::operator CountPtr< Function       >( void ) const { return getFunction(); }
Value::operator CountPtr< Node           >( void ) const { return getNode    (); }
Value::operator CountPtr< Output         >( void ) const { return getOutput  (); }
Value::operator CountPtr< Input          >( void ) const { return getInput   (); }
Value::operator CountPtr< InOut          >( void ) const { return getInOut   (); }
Value::operator CountPtr< Param          >( void ) const { return getParam   (); }
Value::operator CountPtr< Sequence const >( void ) const { return getSequence(); }
Value::operator CountPtr< ArrayBase      >( void ) const { return getArray   (); }
Value::operator CountPtr< Reference      >( void ) const { return getRef     (); }

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
    case Type::NIL     : break;
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
    case Type::STRING  : if( str   && !str  ->unref() ) delete str  ; break;
    case Type::ARRAY   : if( arr   && !arr  ->unref() ) delete arr  ; break;
    case Type::FRAME   : if( frame && !frame->unref() ) delete frame; break;
    case Type::FUNCTION: if( func  && !func ->unref() ) delete func ; break;
    case Type::NODE    : if( node  && !node ->unref() ) delete node ; break;
    case Type::OUTPUT  : if( out   && !out  ->unref() ) delete out  ; break;
    case Type::INPUT   : if( in    && !in   ->unref() ) delete in   ; break;
    case Type::INOUT   : if( inout && !inout->unref() ) delete inout; break;
    case Type::PARAM   : if( param && !param->unref() ) delete param; break;
    case Type::SEQUENCE: if( seq   && !seq  ->unref() ) delete seq  ; break;
    case Type::REF     : if( ref   && !ref  ->unref() ) delete ref  ; break;
    case Type::OTHER   : break;
  }
  p = 0;
  m_type = Type::NIL;
}

void Value::swap( Value &other )
{
  std::swap( m_type, other.m_type );
  std::swap( p, other.p );
}

bool Value::isCollectable( void ) const
{
  static const uint64_t _1 = uint64_t( 1 );
  static const uint64_t flags =
      ( _1 << Type::FRAME    )
    | ( _1 << Type::FUNCTION )
    | ( _1 << Type::NODE     )
    | ( _1 << Type::OUTPUT   )
    | ( _1 << Type::INPUT    )
    | ( _1 << Type::INOUT    )
    | ( _1 << Type::PARAM    )
    | ( _1 << Type::SEQUENCE )
    | ( _1 << Type::ARRAY    )
    ;
  return flags & ( _1 << m_type.getEnum() );
}

const Collectable *Value::getCollectable( void ) const
{
  if( isCollectable() ) return col;
  return 0;
}

Value &Value::unify( StringUnifier *unifier )
{
  if( isString() )
  {
    set( unifier->unify( getString() ) );
  }
  return (*this);
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
        case Type::NIL    : return Value( false );
        case Type::BOOL   : return Value( bool( getBool  () ) );
        case Type::UINT8  : return Value( bool( getUInt8 () ) );
        case Type::INT8   : return Value( bool( getInt8  () ) );
        case Type::UINT16 : return Value( bool( getUInt16() ) );
        case Type::INT16  : return Value( bool( getInt16 () ) );
        case Type::UINT32 : return Value( bool( getUInt32() ) );
        case Type::INT32  : return Value( bool( getInt32 () ) );
        case Type::UINT64 : return Value( bool( getUInt64() ) );
        case Type::INT64  : return Value( bool( getInt64 () ) );
        case Type::FLOAT  : return Value( bool( getFloat () ) );
        case Type::DOUBLE : return Value( bool( getDouble() ) );
        default:
          throw CastFailed( x.getType(), type );
      }

    case Type::UINT8   :
      switch( x.getEnum() )
      {
        case Type::NIL    : return Value( uint8_t( 0 ) );
        case Type::BOOL   : return Value( uint8_t( getBool  () ) );
        case Type::UINT8  : return Value( uint8_t( getUInt8 () ) );
        case Type::INT8   : return Value( uint8_t( getInt8  () ) );
        case Type::UINT16 : return Value( uint8_t( getUInt16() ) );
        case Type::INT16  : return Value( uint8_t( getInt16 () ) );
        case Type::UINT32 : return Value( uint8_t( getUInt32() ) );
        case Type::INT32  : return Value( uint8_t( getInt32 () ) );
        case Type::UINT64 : return Value( uint8_t( getUInt64() ) );
        case Type::INT64  : return Value( uint8_t( getInt64 () ) );
        case Type::FLOAT  : return Value( uint8_t( getFloat () ) );
        case Type::DOUBLE : return Value( uint8_t( getDouble() ) );
        case Type::STRING: return Value( uint8_t( atoi( getString().c_str() ) ) );
        default:
          throw CastFailed( x.getType(), type );
      }

    case Type::INT8   :
      switch( x.getEnum() )
      {
        case Type::NIL    : return Value( int8_t( 0 ) );
        case Type::BOOL   : return Value( int8_t( getBool  () ) );
        case Type::UINT8  : return Value( int8_t( getUInt8 () ) );
        case Type::INT8   : return Value( int8_t( getInt8  () ) );
        case Type::UINT16 : return Value( int8_t( getUInt16() ) );
        case Type::INT16  : return Value( int8_t( getInt16 () ) );
        case Type::UINT32 : return Value( int8_t( getUInt32() ) );
        case Type::INT32  : return Value( int8_t( getInt32 () ) );
        case Type::UINT64 : return Value( int8_t( getUInt64() ) );
        case Type::INT64  : return Value( int8_t( getInt64 () ) );
        case Type::FLOAT  : return Value( int8_t( getFloat () ) );
        case Type::DOUBLE : return Value( int8_t( getDouble() ) );
        case Type::STRING: return Value( int8_t( atoi( getString().c_str() ) ) );
        default:
          throw CastFailed( x.getType(), type );
      }

    case Type::UINT16   :
      switch( x.getEnum() )
      {
        case Type::NIL    : return Value( uint16_t( 0 ) );
        case Type::BOOL   : return Value( uint16_t( getBool  () ) );
        case Type::UINT8  : return Value( uint16_t( getUInt8 () ) );
        case Type::INT8   : return Value( uint16_t( getInt8  () ) );
        case Type::UINT16 : return Value( uint16_t( getUInt16() ) );
        case Type::INT16  : return Value( uint16_t( getInt16 () ) );
        case Type::UINT32 : return Value( uint16_t( getUInt32() ) );
        case Type::INT32  : return Value( uint16_t( getInt32 () ) );
        case Type::UINT64 : return Value( uint16_t( getUInt64() ) );
        case Type::INT64  : return Value( uint16_t( getInt64 () ) );
        case Type::FLOAT  : return Value( uint16_t( getFloat () ) );
        case Type::DOUBLE : return Value( uint16_t( getDouble() ) );
        case Type::STRING: return Value( uint16_t( atoi( getString().c_str() ) ) );
        default:
          throw CastFailed( x.getType(), type );
      }

    case Type::INT16   :
      switch( x.getEnum() )
      {
        case Type::NIL    : return Value( int16_t( 0 ) );
        case Type::BOOL   : return Value( int16_t( getBool  () ) );
        case Type::UINT8  : return Value( int16_t( getUInt8 () ) );
        case Type::INT8   : return Value( int16_t( getInt8  () ) );
        case Type::UINT16 : return Value( int16_t( getUInt16() ) );
        case Type::INT16  : return Value( int16_t( getInt16 () ) );
        case Type::UINT32 : return Value( int16_t( getUInt32() ) );
        case Type::INT32  : return Value( int16_t( getInt32 () ) );
        case Type::UINT64 : return Value( int16_t( getUInt64() ) );
        case Type::INT64  : return Value( int16_t( getInt64 () ) );
        case Type::FLOAT  : return Value( int16_t( getFloat () ) );
        case Type::DOUBLE : return Value( int16_t( getDouble() ) );
        case Type::STRING: return Value( int16_t( atoi( getString().c_str() ) ) );
        default:
          throw CastFailed( x.getType(), type );
      }

    case Type::UINT32   :
      switch( x.getEnum() )
      {
        case Type::NIL    : return Value( uint32_t( 0 ) );
        case Type::BOOL   : return Value( uint32_t( getBool  () ) );
        case Type::UINT8  : return Value( uint32_t( getUInt8 () ) );
        case Type::INT8   : return Value( uint32_t( getInt8  () ) );
        case Type::UINT16 : return Value( uint32_t( getUInt16() ) );
        case Type::INT16  : return Value( uint32_t( getInt16 () ) );
        case Type::UINT32 : return Value( uint32_t( getUInt32() ) );
        case Type::INT32  : return Value( uint32_t( getInt32 () ) );
        case Type::UINT64 : return Value( uint32_t( getUInt64() ) );
        case Type::INT64  : return Value( uint32_t( getInt64 () ) );
        case Type::FLOAT  : return Value( uint32_t( getFloat () ) );
        case Type::DOUBLE : return Value( uint32_t( getDouble() ) );
        case Type::STRING: return Value( uint32_t( atoi( getString().c_str() ) ) );
        default:
          throw CastFailed( x.getType(), type );
      }

    case Type::INT32   :
      switch( x.getEnum() )
      {
        case Type::NIL    : return Value( int32_t( 0 ) );
        case Type::BOOL   : return Value( int32_t( getBool  () ) );
        case Type::UINT8  : return Value( int32_t( getUInt8 () ) );
        case Type::INT8   : return Value( int32_t( getInt8  () ) );
        case Type::UINT16 : return Value( int32_t( getUInt16() ) );
        case Type::INT16  : return Value( int32_t( getInt16 () ) );
        case Type::UINT32 : return Value( int32_t( getUInt32() ) );
        case Type::INT32  : return Value( int32_t( getInt32 () ) );
        case Type::UINT64 : return Value( int32_t( getUInt64() ) );
        case Type::INT64  : return Value( int32_t( getInt64 () ) );
        case Type::FLOAT  : return Value( int32_t( getFloat () ) );
        case Type::DOUBLE : return Value( int32_t( getDouble() ) );
        case Type::STRING: return Value( int32_t( atoi( getString().c_str() ) ) );
        default:
          throw CastFailed( x.getType(), type );
      }

    case Type::UINT64   :
      switch( x.getEnum() )
      {
        case Type::NIL    : return Value( uint64_t( 0 ) );
        case Type::BOOL   : return Value( uint64_t( getBool  () ) );
        case Type::UINT8  : return Value( uint64_t( getUInt8 () ) );
        case Type::INT8   : return Value( uint64_t( getInt8  () ) );
        case Type::UINT16 : return Value( uint64_t( getUInt16() ) );
        case Type::INT16  : return Value( uint64_t( getInt16 () ) );
        case Type::UINT32 : return Value( uint64_t( getUInt32() ) );
        case Type::INT32  : return Value( uint64_t( getInt32 () ) );
        case Type::UINT64 : return Value( uint64_t( getUInt64() ) );
        case Type::INT64  : return Value( uint64_t( getInt64 () ) );
        case Type::FLOAT  : return Value( uint64_t( getFloat () ) );
        case Type::DOUBLE : return Value( uint64_t( getDouble() ) );
        case Type::STRING: return Value( uint64_t( atoi( getString().c_str() ) ) );
        default:
          throw CastFailed( x.getType(), type );
      }

    case Type::INT64   :
      switch( x.getEnum() )
      {
        case Type::NIL    : return Value( int64_t( 0 ) );
        case Type::BOOL   : return Value( int64_t( getBool  () ) );
        case Type::UINT8  : return Value( int64_t( getUInt8 () ) );
        case Type::INT8   : return Value( int64_t( getInt8  () ) );
        case Type::UINT16 : return Value( int64_t( getUInt16() ) );
        case Type::INT16  : return Value( int64_t( getInt16 () ) );
        case Type::UINT32 : return Value( int64_t( getUInt32() ) );
        case Type::INT32  : return Value( int64_t( getInt32 () ) );
        case Type::UINT64 : return Value( int64_t( getUInt64() ) );
        case Type::INT64  : return Value( int64_t( getInt64 () ) );
        case Type::FLOAT  : return Value( int64_t( getFloat () ) );
        case Type::DOUBLE : return Value( int64_t( getDouble() ) );
        case Type::STRING: return Value( int64_t( atoi( getString().c_str() ) ) );
        default:
          throw CastFailed( x.getType(), type );
      }

    case Type::FLOAT :
      switch( x.getEnum() )
      {
        case Type::NIL   : return Value( float( 0 ) );
        case Type::BOOL  : return Value( float( getBool() ) );
        case Type::UINT8 : return Value( float( getUInt8 () ) );
        case Type::INT8  : return Value( float( getInt8  () ) );
        case Type::UINT16: return Value( float( getUInt16() ) );
        case Type::INT16 : return Value( float( getInt16 () ) );
        case Type::UINT32: return Value( float( getUInt32() ) );
        case Type::INT32 : return Value( float( getInt32 () ) );
        case Type::UINT64: return Value( float( getUInt64() ) );
        case Type::INT64 : return Value( float( getInt64 () ) );
        case Type::FLOAT : return Value( float( getFloat () ) );
        case Type::DOUBLE: return Value( float( getDouble() ) );
        case Type::STRING: return Value( float( atof( getString().c_str() ) ) );
        default:
          throw CastFailed( x.getType(), type );
      }

    case Type::DOUBLE :
      switch( x.getEnum() )
      {
        case Type::NIL   : return Value( double( 0 ) );
        case Type::BOOL  : return Value( double( getBool() ) );
        case Type::UINT8 : return Value( double( getUInt8 () ) );
        case Type::INT8  : return Value( double( getInt8  () ) );
        case Type::UINT16: return Value( double( getUInt16() ) );
        case Type::INT16 : return Value( double( getInt16 () ) );
        case Type::UINT32: return Value( double( getUInt32() ) );
        case Type::INT32 : return Value( double( getInt32 () ) );
        case Type::UINT64: return Value( double( getUInt64() ) );
        case Type::INT64 : return Value( double( getInt64 () ) );
        case Type::FLOAT : return Value( double( getFloat () ) );
        case Type::DOUBLE: return Value( double( getDouble() ) );
        case Type::STRING: return Value( double( atof( getString().c_str() ) ) );
        default:
          throw CastFailed( x.getType(), type );
      }

    case Type::STRING:
      {
        std::ostringstream s;

        switch( x.getEnum() )
        {
          case Type::NIL    : return Value( String::Static() );
          case Type::BOOL   : s << getBool  (); break;
          case Type::UINT8  : s << int( getUInt8 () ); break;
          case Type::INT8   : s << int( getInt8  () ); break;
          case Type::UINT16 : s << getUInt16(); break;
          case Type::INT16  : s << getInt16 (); break;
          case Type::UINT32 : s << getUInt32(); break;
          case Type::INT32  : s << getInt32 (); break;
          case Type::UINT64 : s << getUInt64(); break;
          case Type::INT64  : s << getInt64 (); break;
          case Type::FLOAT  : s << getFloat (); break;
          case Type::DOUBLE : s << getDouble(); break;
          default:
            throw CastFailed( x.getType(), type );
        }

        std::string str_tmp( s.str() );
        return Value( String::MoveFrom( str_tmp ) );
      }

    case Type::OUTPUT:
      switch( x.getEnum() )
      {
        case Type::NIL   : return Value( (Output*)( 0 ) );
        default:
          throw CastFailed( x.getType(), type );
      }

    default:
      throw CastFailed( x.getType(), type );
  }
}

Value Value::save_cast( Type type ) const
{
  const Value &x = (*this);
  const Type xType = x.getType();

  if( xType == type ) return x;
  if( type.isNil() ) return x;

  if( xType.isInteger() )
  {
    if( type.isScalar() )
    {
      return x.to( type );
    }
  }
  else if( xType.isFloatingPoint() )
  {
    if( type.isFloatingPoint() )
    {
      return x.to( type );
    }
  }
  else if( xType.isInOut() )
  {
    if( type.isInput() )
    {
      return Value( x.getInOut()->getInput() );
    }
    else if( type.isOutput() )
    {
      return Value( x.getInOut()->getOutput() );
    }
  }
  else if( type.isFrame() )
  {
    if( xType.isNode() )
    {
      return Value( static_cast< Frame* >( x.getNode() ) );
    }
  }

  throw Value::CastFailed( x.getType(), type )
    << ": Cannot be cast savely directly, probably needs an explicit cast"
    ;
}

bool Value::operator< ( const Value &right ) const
{
  const Value &left = (*this);
  if( left.isScalar() && right.isScalar() )
  {
    const Type ret_type = Type::Ret( left.getType(), right.getType() );

    const Value l = left .to( ret_type );
    const Value r = right.to( ret_type );

    switch( ret_type.getEnum() )
    {
      case Type::BOOL  : return ( l.getBool  () < r.getBool  () );
      case Type::UINT8 : return ( l.getUInt8 () < r.getUInt8 () );
      case Type::INT8  : return ( l.getInt8  () < r.getInt8  () );
      case Type::UINT16: return ( l.getUInt16() < r.getUInt16() );
      case Type::INT16 : return ( l.getInt16 () < r.getInt16 () );
      case Type::UINT32: return ( l.getUInt32() < r.getUInt32() );
      case Type::INT32 : return ( l.getInt32 () < r.getInt32 () );
      case Type::UINT64: return ( l.getUInt64() < r.getUInt64() );
      case Type::INT64 : return ( l.getInt64 () < r.getInt64 () );
      case Type::FLOAT : return ( l.getFloat () < r.getFloat () );
      case Type::DOUBLE: return ( l.getDouble() < r.getDouble() );
      default:
        throw ParseException( new Location( __FILE__, __LINE__ ) ) << "Internal: Unhandled case";
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

  throw Exception()
    << "Operator '<': Cannot compare " << left.getTypeName()
    << " and " << right.getTypeName()
    << " directly"
    ;
}

bool Value::operator<=( const Value &right ) const
{
  const Value &left = (*this);
  if( left.isScalar() && right.isScalar() )
  {
    const Type ret_type = Type::Ret( left.getType(), right.getType() );

    const Value l = left .to( ret_type );
    const Value r = right.to( ret_type );

    switch( ret_type.getEnum() )
    {
      case Type::BOOL  : return ( l.getBool  () <= r.getBool  () );
      case Type::UINT8 : return ( l.getUInt8 () <= r.getUInt8 () );
      case Type::INT8  : return ( l.getInt8  () <= r.getInt8  () );
      case Type::UINT16: return ( l.getUInt16() <= r.getUInt16() );
      case Type::INT16 : return ( l.getInt16 () <= r.getInt16 () );
      case Type::UINT32: return ( l.getUInt32() <= r.getUInt32() );
      case Type::INT32 : return ( l.getInt32 () <= r.getInt32 () );
      case Type::UINT64: return ( l.getUInt64() <= r.getUInt64() );
      case Type::INT64 : return ( l.getInt64 () <= r.getInt64 () );
      case Type::FLOAT : return ( l.getFloat () <= r.getFloat () );
      case Type::DOUBLE: return ( l.getDouble() <= r.getDouble() );
      default:
        throw ParseException( new Location( __FILE__, __LINE__ ) ) << "Internal: Unhandled case";
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

  throw Exception()
    << "Operator '<=': Cannot compare " << left.getTypeName()
    << " and " << right.getTypeName()
    << " directly"
    ;
}

bool Value::operator> ( const Value &right ) const
{
  const Value &left = (*this);
  if( left.isScalar() && right.isScalar() )
  {
    const Type ret_type = Type::Ret( left.getType(), right.getType() );

    const Value l = left .to( ret_type );
    const Value r = right.to( ret_type );

    switch( ret_type.getEnum() )
    {
      case Type::BOOL  : return ( l.getBool  () > r.getBool  () );
      case Type::UINT8 : return ( l.getUInt8 () > r.getUInt8 () );
      case Type::INT8  : return ( l.getInt8  () > r.getInt8  () );
      case Type::UINT16: return ( l.getUInt16() > r.getUInt16() );
      case Type::INT16 : return ( l.getInt16 () > r.getInt16 () );
      case Type::UINT32: return ( l.getUInt32() > r.getUInt32() );
      case Type::INT32 : return ( l.getInt32 () > r.getInt32 () );
      case Type::UINT64: return ( l.getUInt64() > r.getUInt64() );
      case Type::INT64 : return ( l.getInt64 () > r.getInt64 () );
      case Type::FLOAT : return ( l.getFloat () > r.getFloat () );
      case Type::DOUBLE: return ( l.getDouble() > r.getDouble() );
      default:
        throw ParseException( new Location( __FILE__, __LINE__ ) ) << "Internal: Unhandled case";
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

  throw Exception()
    << "Operator '>': Cannot compare " << left.getTypeName()
    << " and " << right.getTypeName()
    << " directly"
    ;
}

bool Value::operator>=( const Value &right ) const
{
  const Value &left = (*this);
  if( left.isScalar() && right.isScalar() )
  {
    const Type ret_type = Type::Ret( left.getType(), right.getType() );

    const Value l = left .to( ret_type );
    const Value r = right.to( ret_type );

    switch( ret_type.getEnum() )
    {
      case Type::BOOL  : return ( l.getBool  () >= r.getBool  () );
      case Type::UINT8 : return ( l.getUInt8 () >= r.getUInt8 () );
      case Type::INT8  : return ( l.getInt8  () >= r.getInt8  () );
      case Type::UINT16: return ( l.getUInt16() >= r.getUInt16() );
      case Type::INT16 : return ( l.getInt16 () >= r.getInt16 () );
      case Type::UINT32: return ( l.getUInt32() >= r.getUInt32() );
      case Type::INT32 : return ( l.getInt32 () >= r.getInt32 () );
      case Type::UINT64: return ( l.getUInt64() >= r.getUInt64() );
      case Type::INT64 : return ( l.getInt64 () >= r.getInt64 () );
      case Type::FLOAT : return ( l.getFloat () >= r.getFloat () );
      case Type::DOUBLE: return ( l.getDouble() >= r.getDouble() );
      default:
        throw ParseException( new Location( __FILE__, __LINE__ ) ) << "Internal: Unhandled case";
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

  throw Exception()
    << "Operator '>=': Cannot compare " << left.getTypeName()
    << " and " << right.getTypeName()
    << " directly"
    ;
}

bool Value::operator==( const Value &right ) const
{
  const Value &left = (*this);
  if( left.isScalar() && right.isScalar() )
  {
    const Type ret_type = Type::Ret( left.getType(), right.getType() );

    const Value l = left .to( ret_type );
    const Value r = right.to( ret_type );

    switch( ret_type.getEnum() )
    {
      case Type::BOOL  : return ( l.getBool  () == r.getBool  () );
      case Type::UINT8 : return ( l.getUInt8 () == r.getUInt8 () );
      case Type::INT8  : return ( l.getInt8  () == r.getInt8  () );
      case Type::UINT16: return ( l.getUInt16() == r.getUInt16() );
      case Type::INT16 : return ( l.getInt16 () == r.getInt16 () );
      case Type::UINT32: return ( l.getUInt32() == r.getUInt32() );
      case Type::INT32 : return ( l.getInt32 () == r.getInt32 () );
      case Type::UINT64: return ( l.getUInt64() == r.getUInt64() );
      case Type::INT64 : return ( l.getInt64 () == r.getInt64 () );
      case Type::FLOAT : return ( l.getFloat () == r.getFloat () );
      case Type::DOUBLE: return ( l.getDouble() == r.getDouble() );
      default:
        throw ParseException( new Location( __FILE__, __LINE__ ) ) << "Internal: Unhandled case";
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

  throw Exception()
    << "Operator '==': Cannot compare " << left.getTypeName()
    << " and " << right.getTypeName()
    << " directly"
    ;
}

bool Value::operator!=( const Value &right ) const
{
  const Value &left = (*this);
  if( left.isScalar() && right.isScalar() )
  {
    const Type ret_type = Type::Ret( left.getType(), right.getType() );

    const Value l = left .to( ret_type );
    const Value r = right.to( ret_type );

    switch( ret_type.getEnum() )
    {
      case Type::BOOL  : return ( l.getBool  () != r.getBool  () );
      case Type::UINT8 : return ( l.getUInt8 () != r.getUInt8 () );
      case Type::INT8  : return ( l.getInt8  () != r.getInt8  () );
      case Type::UINT16: return ( l.getUInt16() != r.getUInt16() );
      case Type::INT16 : return ( l.getInt16 () != r.getInt16 () );
      case Type::UINT32: return ( l.getUInt32() != r.getUInt32() );
      case Type::INT32 : return ( l.getInt32 () != r.getInt32 () );
      case Type::UINT64: return ( l.getUInt64() != r.getUInt64() );
      case Type::INT64 : return ( l.getInt64 () != r.getInt64 () );
      case Type::FLOAT : return ( l.getFloat () != r.getFloat () );
      case Type::DOUBLE: return ( l.getDouble() != r.getDouble() );
      default:
        throw ParseException( new Location( __FILE__, __LINE__ ) ) << "Internal: Unhandled case";
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

  throw Exception()
    << "Operator '!=': Cannot compare " << left.getTypeName()
    << " and " << right.getTypeName()
    << " directly"
    ;
}

bool Value::operator&&( const Value &right ) const
{
  const Value &left = (*this);
  if( left.isScalar() && right.isScalar() )
  {
    const bool l = left .save_cast< bool >();
    const bool r = right.save_cast< bool >();
    return ( l && r );
  }

  throw Exception()
    << "Operator '&&': left and right must be scalar"
    << ": left is " << left.getType()
    << ", right is " << right.getType()
    ;
}

bool Value::operator||( const Value &right ) const
{
  const Value &left = (*this);
  if( left.isScalar() && right.isScalar() )
  {
    const bool l = left .save_cast< bool >();
    const bool r = right.save_cast< bool >();
    return ( l || r );
  }

  throw Exception()
    << "Operator '||': left and right must be scalar"
    << ": left is " << left.getType()
    << ", right is " << right.getType()
    ;
}

bool Value::log_xor   ( const Value &right ) const
{
  const Value &left = (*this);
  if( left.isScalar() && right.isScalar() )
  {
    const bool l = left .save_cast< bool >();
    const bool r = right.save_cast< bool >();
    return ( l ^ r );
  }

  throw Exception()
    << "Operator '^^': left and right must be scalar"
    << ": left is " << left.getType()
    << ", right is " << right.getType()
    ;
}

Value Value::operator<<( const Value &right ) const
{
  const Value &left = (*this);

  if( left.isInteger() && right.isInteger() )
  {
    const int r = right.save_cast< int >();

    switch( left.getType().getEnum() )
    {
      case Type::BOOL  : return Value( left.getBool  () << r );
      case Type::UINT8 : return Value( left.getUInt8 () << r );
      case Type::INT8  : return Value( left.getInt8  () << r );
      case Type::UINT16: return Value( left.getUInt16() << r );
      case Type::INT16 : return Value( left.getInt16 () << r );
      case Type::UINT32: return Value( left.getUInt32() << r );
      case Type::INT32 : return Value( left.getInt32 () << r );
      case Type::UINT64: return Value( left.getUInt64() << r );
      case Type::INT64 : return Value( left.getInt64 () << r );
      default:
        throw ParseException( new Location( __FILE__, __LINE__ ) ) << "Internal: Unhandled case";
    }
  }

  throw Exception()
    << "Operator '<<': left and right must be integer"
    << ": left is " << left.getType()
    << ", right is " << right.getType()
    ;
}

Value Value::operator>>( const Value &right ) const
{
  const Value &left = (*this);

  if( left.isInteger() && right.isInteger() )
  {
    const int r = right.save_cast< int >();

    switch( left.getType().getEnum() )
    {
      case Type::BOOL  : return Value( left.getBool  () >> r );
      case Type::UINT8 : return Value( left.getUInt8 () >> r );
      case Type::INT8  : return Value( left.getInt8  () >> r );
      case Type::UINT16: return Value( left.getUInt16() >> r );
      case Type::INT16 : return Value( left.getInt16 () >> r );
      case Type::UINT32: return Value( left.getUInt32() >> r );
      case Type::INT32 : return Value( left.getInt32 () >> r );
      case Type::UINT64: return Value( left.getUInt64() >> r );
      case Type::INT64 : return Value( left.getInt64 () >> r );
      default:
        throw ParseException( new Location( __FILE__, __LINE__ ) ) << "Internal: Unhandled case";
    }
  }

  throw Exception()
    << "Operator '>>': left and right must be integer"
    << ": left is " << left.getType()
    << ", right is " << right.getType()
    ;
}

Value Value::operator& ( const Value &right ) const
{
  const Value &left = (*this);

  if( left.isInteger() && right.isInteger() )
  {
    const Type ret_type = Type::Ret( left.getType(), right.getType() );

    const Value r = right.to( ret_type );
    const Value l = left .to( ret_type );

    switch( ret_type.getEnum() )
    {
      case Type::BOOL  : return Value( l.getBool  () & r.getBool  () );
      case Type::UINT8 : return Value( l.getUInt8 () & r.getUInt8 () );
      case Type::INT8  : return Value( l.getInt8  () & r.getInt8  () );
      case Type::UINT16: return Value( l.getUInt16() & r.getUInt16() );
      case Type::INT16 : return Value( l.getInt16 () & r.getInt16 () );
      case Type::UINT32: return Value( l.getUInt32() & r.getUInt32() );
      case Type::INT32 : return Value( l.getInt32 () & r.getInt32 () );
      case Type::UINT64: return Value( l.getUInt64() & r.getUInt64() );
      case Type::INT64 : return Value( l.getInt64 () & r.getInt64 () );
      default:
        throw ParseException( new Location( __FILE__, __LINE__ ) ) << "Internal: Unhandled case";
    }
  }

  throw Exception()
    << "Operator '&': left and right must be integer"
    << ": left is " << left.getType()
    << ", right is " << right.getType()
    ;
}

Value Value::operator| ( const Value &right ) const
{
  const Value &left = (*this);

  if( left.isInteger() && right.isInteger() )
  {
    const Type ret_type = Type::Ret( left.getType(), right.getType() );

    const Value r = right.to( ret_type );
    const Value l = left .to( ret_type );

    switch( ret_type.getEnum() )
    {
      case Type::BOOL  : return Value( l.getBool  () | r.getBool  () );
      case Type::UINT8 : return Value( l.getUInt8 () | r.getUInt8 () );
      case Type::INT8  : return Value( l.getInt8  () | r.getInt8  () );
      case Type::UINT16: return Value( l.getUInt16() | r.getUInt16() );
      case Type::INT16 : return Value( l.getInt16 () | r.getInt16 () );
      case Type::UINT32: return Value( l.getUInt32() | r.getUInt32() );
      case Type::INT32 : return Value( l.getInt32 () | r.getInt32 () );
      case Type::UINT64: return Value( l.getUInt64() | r.getUInt64() );
      case Type::INT64 : return Value( l.getInt64 () | r.getInt64 () );
      default:
        throw ParseException( new Location( __FILE__, __LINE__ ) ) << "Internal: Unhandled case";
    }
  }

  throw Exception()
    << "Operator '|': left and right must be integer"
    << ": left is " << left.getType()
    << ", right is " << right.getType()
    ;
}

Value Value::operator^ ( const Value &right ) const
{
  const Value &left = (*this);

  if( left.isInteger() && right.isInteger() )
  {
    const Type ret_type = Type::Ret( left.getType(), right.getType() );

    const Value r = right.to( ret_type );
    const Value l = left .to( ret_type );

    switch( ret_type.getEnum() )
    {
      case Type::BOOL  : return Value( l.getBool  () ^ r.getBool  () );
      case Type::UINT8 : return Value( l.getUInt8 () ^ r.getUInt8 () );
      case Type::INT8  : return Value( l.getInt8  () ^ r.getInt8  () );
      case Type::UINT16: return Value( l.getUInt16() ^ r.getUInt16() );
      case Type::INT16 : return Value( l.getInt16 () ^ r.getInt16 () );
      case Type::UINT32: return Value( l.getUInt32() ^ r.getUInt32() );
      case Type::INT32 : return Value( l.getInt32 () ^ r.getInt32 () );
      case Type::UINT64: return Value( l.getUInt64() ^ r.getUInt64() );
      case Type::INT64 : return Value( l.getInt64 () ^ r.getInt64 () );
      default:
        throw ParseException( new Location( __FILE__, __LINE__ ) ) << "Internal: Unhandled case";
    }
  }

  throw Exception()
    << "Operator '^': left and right must be integer"
    << ": left is " << left.getType()
    << ", right is " << right.getType()
    ;
}

Value Value::operator* ( const Value &right ) const
{
  const Value &left = (*this);

  if( left.isScalar() && right.isScalar() )
  {
    const Type ret_type = Type::Ret( left.getType(), right.getType() );

    const Value l = left .to( ret_type );
    const Value r = right.to( ret_type );

    switch( ret_type.getEnum() )
    {
      case Type::BOOL   : return Value( l.getBool  () * r.getBool  () );
      case Type::UINT8  : return Value( l.getUInt8 () * r.getUInt8 () );
      case Type::INT8   : return Value( l.getInt8  () * r.getInt8  () );
      case Type::UINT16 : return Value( l.getUInt16() * r.getUInt16() );
      case Type::INT16  : return Value( l.getInt16 () * r.getInt16 () );
      case Type::UINT32 : return Value( l.getUInt32() * r.getUInt32() );
      case Type::INT32  : return Value( l.getInt32 () * r.getInt32 () );
      case Type::UINT64 : return Value( l.getUInt64() * r.getUInt64() );
      case Type::INT64  : return Value( l.getInt64 () * r.getInt64 () );
      case Type::FLOAT  : return Value( l.getFloat () * r.getFloat () );
      case Type::DOUBLE : return Value( l.getDouble() * r.getDouble() );
      default:
        throw ParseException( new Location( __FILE__, __LINE__ ) ) << "Internal: Unhandled case";
    }
  }

  throw Exception()
    << "Operator '*': left and right must be scalar"
    << ": left is " << left.getType()
    << ", right is " << right.getType()
    ;
}

Value Value::operator/ ( const Value &right ) const
{
  const Value &left = (*this);
  if( left.isScalar() && right.isScalar() )
  {
    const Type ret_type = Type::Ret( left.getType(), right.getType() );

    const Value l = left .to( ret_type );
    const Value r = right.to( ret_type );

    switch( ret_type.getEnum() )
    {
      case Type::BOOL   : if( r.getBool  ()  ) return Value( l.getBool  ()  / r.getBool  () ); else throw DivisionByZero();
      case Type::UINT8  : if( r.getUInt8 ()  ) return Value( l.getUInt8 ()  / r.getUInt8 () ); else throw DivisionByZero();
      case Type::INT8   : if( r.getInt8  ()  ) return Value( l.getInt8  ()  / r.getInt8  () ); else throw DivisionByZero();
      case Type::UINT16 : if( r.getUInt16()  ) return Value( l.getUInt16()  / r.getUInt16() ); else throw DivisionByZero();
      case Type::INT16  : if( r.getInt16 ()  ) return Value( l.getInt16 ()  / r.getInt16 () ); else throw DivisionByZero();
      case Type::UINT32 : if( r.getUInt32()  ) return Value( l.getUInt32()  / r.getUInt32() ); else throw DivisionByZero();
      case Type::INT32  : if( r.getInt32 ()  ) return Value( l.getInt32 ()  / r.getInt32 () ); else throw DivisionByZero();
      case Type::UINT64 : if( r.getUInt64()  ) return Value( l.getUInt64()  / r.getUInt64() ); else throw DivisionByZero();
      case Type::INT64  : if( r.getInt64 ()  ) return Value( l.getInt64 ()  / r.getInt64 () ); else throw DivisionByZero();
      case Type::FLOAT  : if( r.getFloat ()  ) return Value( l.getFloat ()  / r.getFloat () ); else throw DivisionByZero();
      case Type::DOUBLE : if( r.getDouble()  ) return Value( l.getDouble()  / r.getDouble() ); else throw DivisionByZero();
      default:
        throw ParseException( new Location( __FILE__, __LINE__ ) ) << "Internal: Unhandled case";
    }
  }

  throw Exception()
    << "Operator '/': left and right must be scalar"
    << ": left is " << left.getType()
    << ", right is " << right.getType()
    ;
}

Value Value::operator+ ( const Value &right ) const
{
  const Value &left = (*this);
  if( left.isScalar() && right.isScalar() )
  {
    const Type ret_type = Type::Ret( left.getType(), right.getType() );

    const Value l = left .to( ret_type );
    const Value r = right.to( ret_type );

    switch( ret_type.getEnum() )
    {
      case Type::BOOL   : return Value( l.getBool  () + r.getBool  () );
      case Type::UINT8  : return Value( l.getUInt8 () + r.getUInt8 () );
      case Type::INT8   : return Value( l.getInt8  () + r.getInt8  () );
      case Type::UINT16 : return Value( l.getUInt16() + r.getUInt16() );
      case Type::INT16  : return Value( l.getInt16 () + r.getInt16 () );
      case Type::UINT32 : return Value( l.getUInt32() + r.getUInt32() );
      case Type::INT32  : return Value( l.getInt32 () + r.getInt32 () );
      case Type::UINT64 : return Value( l.getUInt64() + r.getUInt64() );
      case Type::INT64  : return Value( l.getInt64 () + r.getInt64 () );
      case Type::FLOAT  : return Value( l.getFloat () + r.getFloat () );
      case Type::DOUBLE : return Value( l.getDouble() + r.getDouble() );
      default:
        throw ParseException( new Location( __FILE__, __LINE__ ) ) << "Internal: Unhandled case";
    }
  }
  else if( left.isString() && right.isPrimitive() )
  {
    const Value &l = left;
    const Value  r = right.to( Type::String() );
    return Value( l.getString() + r.getString() );
  }
  else if( left.isPrimitive() && right.isString() )
  {
    const Value  l = left.to( Type::String() );
    const Value &r = right;
    return Value( l.getString() + r.getString() );
  }

  throw Exception()
    << "Operator '+': left and right must be scalar or string"
    << ": left is " << left.getType()
    << ", right is " << right.getType()
    ;
}

Value Value::operator- ( const Value &right ) const
{
  const Value &left = (*this);

  if( left.isScalar() && right.isScalar() )
  {
    const Type ret_type = Type::Ret( left.getType(), right.getType() );

    const Value l = left .to( ret_type );
    const Value r = right.to( ret_type );

    switch( ret_type.getEnum() )
    {
      case Type::BOOL   : return Value( l.getBool  () - r.getBool  () );
      case Type::UINT8  : return Value( l.getUInt8 () - r.getUInt8 () );
      case Type::INT8   : return Value( l.getInt8  () - r.getInt8  () );
      case Type::UINT16 : return Value( l.getUInt16() - r.getUInt16() );
      case Type::INT16  : return Value( l.getInt16 () - r.getInt16 () );
      case Type::UINT32 : return Value( l.getUInt32() - r.getUInt32() );
      case Type::INT32  : return Value( l.getInt32 () - r.getInt32 () );
      case Type::UINT64 : return Value( l.getUInt64() - r.getUInt64() );
      case Type::INT64  : return Value( l.getInt64 () - r.getInt64 () );
      case Type::FLOAT  : return Value( l.getFloat () - r.getFloat () );
      case Type::DOUBLE : return Value( l.getDouble() - r.getDouble() );
      default:
        throw ParseException( new Location( __FILE__, __LINE__ ) ) << "Internal: Unhandled case";
    }
  }

  throw Exception()
    << "Operator '*': left and right must be scalar"
    << ": left is " << left.getType()
    << ", right is " << right.getType()
    ;
}

namespace Value_impl {

  static inline float  floor_t( float  x ) { return floorf( x ); }
  static inline double floor_t( double x ) { return floor ( x ); }

  template< class F >
  static inline
  F modulo( F l, F r )
  {
    if( r )
      return Value( l - floor_t( l / r ) * r );
    else
      throw Value::DivisionByZero() << ": At modulo";
  }

} // namespace Value_impl

Value Value::operator% ( const Value &right ) const
{
  const Value &left = (*this);

  if( left.isInteger() && right.isInteger() )
  {
    const Type ret_type = Type::Ret( left.getType(), right.getType() );

    const Value l = left .to( ret_type );
    const Value r = right.to( ret_type );

    switch( ret_type.getEnum() )
    {
      case Type::BOOL   : return Value( l.getBool  () % r.getBool  () );
      case Type::UINT8  : return Value( l.getUInt8 () % r.getUInt8 () );
      case Type::INT8   : return Value( l.getInt8  () % r.getInt8  () );
      case Type::UINT16 : return Value( l.getUInt16() % r.getUInt16() );
      case Type::INT16  : return Value( l.getInt16 () % r.getInt16 () );
      case Type::UINT32 : return Value( l.getUInt32() % r.getUInt32() );
      case Type::INT32  : return Value( l.getInt32 () % r.getInt32 () );
      case Type::UINT64 : return Value( l.getUInt64() % r.getUInt64() );
      case Type::INT64  : return Value( l.getInt64 () % r.getInt64 () );
      default:
        throw Exception() << "Internal error: unhandled scalar";
    }
  }
  else if( left.isScalar() && right.isScalar() )
  {
    const Type ret_type = Type::Ret( left.getType(), right.getType() );

    const Value l = left .to( ret_type );
    const Value r = right.to( ret_type );

    switch( ret_type.getEnum() )
    {
      case Type::FLOAT  : return Value( Value_impl::modulo( l.getFloat (), r.getFloat () ) );
      case Type::DOUBLE : return Value( Value_impl::modulo( l.getDouble(), r.getDouble() ) );
      default:
        throw Exception() << "Internal error: unhandled scalar";
    }
  }

  throw Exception()
    << "Operator '%': left and right must be scalar"
    << ": left is " << left.getType()
    << ", right is " << right.getType()
    ;
}

namespace Value_impl {

  template< class T >
  static inline
  int cmp( const T &x1, const T &x2 )
  {
    if( x1 == x2 ) return  0;
    if( x1 <  x2 ) return -1;
    return 1;
  }

} // namespace Value_impl

int Value::compare_exactly( const Value &other ) const
{
  using namespace Value_impl;

  const int cmp_type = m_type.compare( other.m_type );
  if( 0 != cmp_type) return cmp_type;

  switch( m_type.getEnum() )
  {
    case Type::NIL     : return 0;
    case Type::BOOL    : return cmp( b   , other.b    );
    case Type::UINT8   : return cmp( ui8 , other.ui8  );
    case Type::INT8    : return cmp(  i8 , other. i8  );
    case Type::UINT16  : return cmp( ui16, other.ui16 );
    case Type::INT16   : return cmp(  i16, other. i16 );
    case Type::UINT32  : return cmp( ui32, other.ui32 );
    case Type::INT32   : return cmp(  i32, other. i32 );
    case Type::UINT64  : return cmp( ui64, other.ui64 );
    case Type::INT64   : return cmp(  i64, other. i64 );
    case Type::FLOAT   : return cmp( f   , other.f    );
    case Type::DOUBLE  : return cmp( d   , other.d    );
    case Type::STRING  : return str->compare( *other.str );
    case Type::FRAME   : return cmp( frame, other.frame );
    case Type::FUNCTION: return cmp( func , other.func  );
    case Type::NODE    : return cmp( node , other.node  );
    case Type::OUTPUT  : return cmp( out  , other.out   );
    case Type::INPUT   : return cmp( in   , other.in    );
    case Type::INOUT   : return cmp( inout, other.inout );
    case Type::PARAM   : return cmp( param, other.param );
    case Type::SEQUENCE: return cmp( seq  , other.seq   );
    case Type::ARRAY   : return cmp( arr  , other.arr   );
    default: return 0;
  }
}

std::ostream &Value::print_Type( std::ostream &o ) const
{
  if( isArray() )
  {
    const ArrayBase *const array = getArray();
    const Type array_type = array->getType();

    if( array_type.isArray() )
    {
      if( !array->empty() )
      {
        static const index_t pos[ 4 ] = { 0, 0, 0, 0 };
        array->getValue_v( array->getDims(), pos ).print_Type( o );
      }
      else
      {
        o << "int[]";
      }
    }
    else
    {
      o << array_type;
    }

    o << "[]";
  }
  else
  {
    o << getTypeName();
  }

  return o;
}

std::ostream &Value::print( std::ostream &o ) const
{
  using namespace Value_impl;

  switch( getType().getEnum() )
  {
    case Type::NIL     : o << "nil"; break;
    case Type::BOOL    : o << ( getBool() ? "true" : "false" ); break;
    case Type::UINT8   : o << int( getUInt8 () ); break;
    case Type::INT8    : o << int( getInt8  () ); break;
    case Type::UINT16  : o << getUInt16(); break;
    case Type::INT16   : o << getInt16 (); break;
    case Type::UINT32  : o << getUInt32(); break;
    case Type::INT32   : o << getInt32 (); break;
    case Type::UINT64  : o << getUInt64(); break;
    case Type::INT64   : o << getInt64 (); break;
    case Type::FLOAT   : o << getFloat (); break;
    case Type::DOUBLE  : o << getDouble(); break;
    case Type::STRING  : o << getString(); break;
    case Type::ARRAY   : getArray()->print( o ); break;

    case Type::FRAME:
      {
        const Frame *m = getFrame();
        o << "Frame{ ";
        for( CountPtr< RPGML::Frame::Iterator > j( m->getIterator() ); !j->done(); j->next() )
        {
          const Frame::Iterator::Type v( j->get() );
          v.second.print_Type( o );
          o
            << " " << v.first
            << " = ";

          if( v.second.isString() )
          {
            o << "\"" << make_printable( v.second.getString() ) << "\"";
          }
          else
          {
            o << v.second;
          }

          o << "; ";
        }
        o << "}";
      }
      break;

    case Type::FUNCTION: o << "Function( '" << getFunction()->getName() << "' )"; break;
    case Type::NODE    : o << "Node( '" << getNode()->getName() << "', '" << getNode()->getIdentifier() << "' )"; break;

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
          << "Input( '" << input->getParent()->getIdentifier() << "'"
          << ", '" << input->getIdentifier() << "' )"
          ;
      }
      break;

    case Type::INOUT   :
      {
        const InOut *const io = getInOut();
        o
          << "InOut( '" << io->getIdentifier() << "' )"
          ;
      }
      break;

    case Type::PARAM   :
      {
        const Param *const pm = getParam();
        o
          << "Param( '" << pm->getParent()->getIdentifier() << "'"
          << ", '" << pm->getIdentifier() << "' )"
          ;
      }
      break;

    case Type::SEQUENCE: getSequence()->print( o ); break;

    default:
      throw Exception() << "Invalid Type for Value::print()";
  }

  return o;
}

CountPtr< ArrayBase > new_Array( GarbageCollector *gc, Type type, int dims, const Value &fill_value )
{
  switch( type.getEnum() )
  {
    case Type::NIL     : throw ArrayBase::Exception() << "Cannot create an Array of type 'nil'";
    case Type::BOOL    : return new_Array<           bool             >( gc, dims, fill_value );
    case Type::UINT8   : return new_Array<           uint8_t          >( gc, dims, fill_value );
    case Type::INT8    : return new_Array<           int8_t           >( gc, dims, fill_value );
    case Type::UINT16  : return new_Array<           uint16_t         >( gc, dims, fill_value );
    case Type::INT16   : return new_Array<           int16_t          >( gc, dims, fill_value );
    case Type::UINT32  : return new_Array<           uint32_t         >( gc, dims, fill_value );
    case Type::INT32   : return new_Array<           int32_t          >( gc, dims, fill_value );
    case Type::UINT64  : return new_Array<           uint64_t         >( gc, dims, fill_value );
    case Type::INT64   : return new_Array<           int64_t          >( gc, dims, fill_value );
    case Type::FLOAT   : return new_Array<           float            >( gc, dims, fill_value );
    case Type::DOUBLE  : return new_Array<           double           >( gc, dims, fill_value );
    case Type::STRING  : return new_Array<           String           >( gc, dims, fill_value );
    case Type::FRAME   : return new_Array< CountPtr< Frame          > >( gc, dims, fill_value );
    case Type::FUNCTION: return new_Array< CountPtr< Function       > >( gc, dims, fill_value );
    case Type::NODE    : return new_Array< CountPtr< Node           > >( gc, dims, fill_value );
    case Type::OUTPUT  : return new_Array< CountPtr< Output         > >( gc, dims, fill_value );
    case Type::INPUT   : return new_Array< CountPtr< Input          > >( gc, dims, fill_value );
    case Type::INOUT   : return new_Array< CountPtr< InOut          > >( gc, dims, fill_value );
    case Type::PARAM   : return new_Array< CountPtr< Param          > >( gc, dims, fill_value );
    case Type::SEQUENCE: return new_Array< CountPtr< Sequence const > >( gc, dims, fill_value );
    case Type::ARRAY   : return new_Array< CountPtr< ArrayBase      > >( gc, dims, fill_value );
    case Type::REF     : throw ArrayBase::Exception() << "Cannot create an Array of 'Ref' type";
    case Type::OTHER   : throw ArrayBase::Exception() << "Cannot create an Array of 'other' custom type";
    // no default
  }
  throw ArrayBase::Exception() << "Internal: Unhandled Type::Enum";
}


} // namespace RPGML

namespace std {

std::ostream &operator<<( std::ostream &o, const RPGML::Value &v )
{
  return v.print( o );
}

} // namespace std


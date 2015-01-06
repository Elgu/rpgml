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
#ifndef RPGML_Value_h
#define RPGML_Value_h

#include "Type.h"
#include "String.h"
#include "Exception.h"

#include <cassert>

namespace RPGML {

class String  ;
class StringData;
class ArrayBase   ;
class Frame     ;
class Function;
class Node    ;
class Output  ;
class Input   ;
class Param   ;
class Sequence;
class Collectable;

class Value
{
public:
  EXCEPTION_BASE( Exception );
  EXCEPTION_DERIVED_FIXED_TEXT( DivisionByZero, Exception );

  class CastFailed : public Exception
  {
  public:
    typedef Exception Base;
    CastFailed( Type _from, Type _to );
    EXCEPTION_BODY( CastFailed )
    Type const from;
    Type const to  ;
  };

  class GetFailed : public Exception
  {
  public:
    typedef Exception Base;
    GetFailed( Type _is, Type _tried );
    EXCEPTION_BODY( GetFailed )
    Type const is;
    Type const tried;
  };

  Value( void );
  ~Value( void );

  Value( const Value &other );
  Value &operator=( const Value &other );

  Value to( Type type ) const;
  Value save_cast( Type type ) const;

  bool operator< ( const Value &right ) const;
  bool operator<=( const Value &right ) const;
  bool operator> ( const Value &right ) const;
  bool operator>=( const Value &right ) const;
  bool operator==( const Value &right ) const;
  bool operator!=( const Value &right ) const;
  bool operator&&( const Value &right ) const;
  bool operator||( const Value &right ) const;
  bool log_xor   ( const Value &right ) const;
  Value operator<<( const Value &right ) const;
  Value operator>>( const Value &right ) const;
  Value operator& ( const Value &right ) const;
  Value operator| ( const Value &right ) const;
  Value operator^ ( const Value &right ) const;
  Value operator* ( const Value &right ) const;
  Value operator/ ( const Value &right ) const;
  Value operator+ ( const Value &right ) const;
  Value operator- ( const Value &right ) const;
  Value operator% ( const Value &right ) const;

  void clear( void );
  void swap( Value &other );

  std::ostream &print( std::ostream &o ) const;

  explicit Value ( bool     _x );
  explicit Value ( uint8_t  _x );
  explicit Value ( int8_t   _x );
  explicit Value ( uint16_t _x );
  explicit Value ( int16_t  _x );
  explicit Value ( uint32_t _x );
  explicit Value ( int32_t  _x );
  explicit Value ( uint64_t _x );
  explicit Value ( int64_t  _x );
  explicit Value ( float    _x );
  explicit Value ( double   _x );

  explicit Value ( char const *_str  );
  explicit Value ( std::string const &_str  );
  explicit Value ( String   const &_str  );
  explicit Value ( StringData const *_str );

  explicit Value ( ArrayBase      *_arr  );
  explicit Value ( Frame          *_frame );
  explicit Value ( Function       *_func );
  explicit Value ( Node           *_node );
  explicit Value ( Output         *_out  );
  explicit Value ( Input          *_in   );
  explicit Value ( Param          *_param );
  explicit Value ( Sequence const *_seq  );

  Value &set( bool     _x );
  Value &set( uint8_t  _x );
  Value &set( int8_t   _x );
  Value &set( uint16_t _x );
  Value &set( int16_t  _x );
  Value &set( uint32_t _x );
  Value &set( int32_t  _x );
  Value &set( uint64_t _x );
  Value &set( int64_t  _x );
  Value &set( float    _x );
  Value &set( double   _x );
  Value &set( std::string const &_str  );
  Value &set( char        const *_str  );
  Value &set( String      const &_str  );
  Value &set( StringData  const *_str );
  Value &set( ArrayBase      *_arr  );
  Value &set( Frame          *_frame );
  Value &set( Function       *_func );
  Value &set( Node           *_node );
  Value &set( Output         *_out  );
  Value &set( Input          *_in   );
  Value &set( Param          *_param );
  Value &set( Sequence const *_seq  );
  Value &set( const Value &v );

  Type        getType    ( void ) const { return m_type; }
  Type::Enum  getEnum    ( void ) const { return m_type.getEnum(); }
  const char *getTypeName( void ) const { return m_type.getTypeName(); }

  bool is( Type t       ) const { return ( m_type == t ); }
  bool is( Type::Enum e ) const { return ( m_type == e ); }
  bool isNil     ( void ) const { return is( Type::NIL      ); }

  bool isBool    ( void ) const { return is( Type::BOOL     ); }
  bool isUInt8   ( void ) const { return is( Type::UINT8    ); }
  bool isInt8    ( void ) const { return is( Type::INT8     ); }
  bool isUInt16  ( void ) const { return is( Type::UINT16   ); }
  bool isInt16   ( void ) const { return is( Type::INT16    ); }
  bool isUInt32  ( void ) const { return is( Type::UINT32   ); }
  bool isInt32   ( void ) const { return is( Type::INT32    ); }
  bool isUInt64  ( void ) const { return is( Type::UINT64   ); }
  bool isInt64   ( void ) const { return is( Type::INT64    ); }
  bool isFloat   ( void ) const { return is( Type::FLOAT    ); }
  bool isDouble  ( void ) const { return is( Type::DOUBLE   ); }

  bool isString  ( void ) const { return is( Type::STRING   ); }
  bool isArray   ( void ) const { return is( Type::ARRAY    ); }
  bool isFrame   ( void ) const { return is( Type::FRAME    ); }
  bool isFunction( void ) const { return is( Type::FUNCTION ); }
  bool isNode    ( void ) const { return is( Type::NODE     ); }
  bool isOutput  ( void ) const { return is( Type::OUTPUT   ); }
  bool isInput   ( void ) const { return is( Type::INPUT    ); }
  bool isParam   ( void ) const { return is( Type::PARAM    ); }
  bool isSequence( void ) const { return is( Type::SEQUENCE ); }

  bool isInt( void ) const { return isInt32(); }
  bool isScalar( void ) const { return getType().isScalar(); }
  bool isInteger( void ) const { return getType().isInteger(); }
  bool isPrimitive( void ) const { return getType().isPrimitive(); }

  bool            getBool     ( void ) const { assert( isBool  () ); return b   ; }
  uint8_t         getUInt8    ( void ) const { assert( isUInt8 () ); return ui8 ; }
  int8_t          getInt8     ( void ) const { assert( isInt8  () ); return  i8 ; }
  uint16_t        getUInt16   ( void ) const { assert( isUInt16() ); return ui16; }
  int16_t         getInt16    ( void ) const { assert( isInt16 () ); return  i16; }
  uint32_t        getUInt32   ( void ) const { assert( isUInt32() ); return ui32; }
  int32_t         getInt32    ( void ) const { assert( isInt32 () ); return  i32; }
  uint64_t        getUInt64   ( void ) const { assert( isUInt64() ); return ui64; }
  int64_t         getInt64    ( void ) const { assert( isInt64 () ); return  i64; }
  float           getFloat    ( void ) const { assert( isFloat () ); return f   ; }
  double          getDouble   ( void ) const { assert( isDouble() ); return d   ; }
  String          getString  ( void ) const { assert( isString  () ); return String( str ); }
  ArrayBase      *getArray   ( void ) const { assert( isArray   () ); return arr ; }
  Frame          *getFrame   ( void ) const { assert( isFrame   () ); return frame; }
  Function       *getFunction( void ) const { assert( isFunction() ); return func; }
  Node           *getNode    ( void ) const { assert( isNode    () ); return node; }
  Output         *getOutput  ( void ) const { assert( isOutput  () ); return out ; }
  Input          *getInput   ( void ) const { assert( isInput   () ); return in  ; }
  Param          *getParam   ( void ) const { assert( isParam   () ); return param  ; }
  Sequence const *getSequence( void ) const { assert( isSequence() ); return seq ; }

  int             getInt     ( void ) const { return getInt32(); }

  operator bool            ( void ) const { return getBool     (); }
  operator uint8_t         ( void ) const { return getUInt8    (); }
  operator int8_t          ( void ) const { return getInt8     (); }
  operator uint16_t        ( void ) const { return getUInt16   (); }
  operator int16_t         ( void ) const { return getInt16    (); }
  operator uint32_t        ( void ) const { return getUInt32   (); }
  operator int32_t         ( void ) const { return getInt32    (); }
  operator uint64_t        ( void ) const { return getUInt64   (); }
  operator int64_t         ( void ) const { return getInt64    (); }
  operator float           ( void ) const { return getFloat    (); }
  operator double          ( void ) const { return getDouble   (); }
  operator String          ( void ) const { return getString  (); }
  operator ArrayBase      *( void ) const { return getArray   (); }
  operator Frame          *( void ) const { return getFrame   (); }
  operator Function       *( void ) const { return getFunction(); }
  operator Node           *( void ) const { return getNode    (); }
  operator Output         *( void ) const { return getOutput  (); }
  operator Input          *( void ) const { return getInput   (); }
  operator Param          *( void ) const { return getParam   (); }
  operator Sequence const *( void ) const { return getSequence(); }

  operator CountPtr< ArrayBase      >( void ) const;
  operator CountPtr< Frame          >( void ) const;
  operator CountPtr< Function       >( void ) const;
  operator CountPtr< Node           >( void ) const;
  operator CountPtr< Output         >( void ) const;
  operator CountPtr< Input          >( void ) const;
  operator CountPtr< Param          >( void ) const;
  operator CountPtr< Sequence const >( void ) const;

  void get( bool     &x ) const;
  void get( uint8_t  &x ) const;
  void get( int8_t   &x ) const;
  void get( uint16_t &x ) const;
  void get( int16_t  &x ) const;
  void get( uint32_t &x ) const;
  void get( int32_t  &x ) const;
  void get( uint64_t &x ) const;
  void get( int64_t  &x ) const;
  void get( float    &x ) const;
  void get( double   &x ) const;
  void get( String   &x ) const;

  void get( ArrayBase      *&x ) const;
  void get( Frame          *&x ) const;
  void get( Function       *&x ) const;
  void get( Node           *&x ) const;
  void get( Output         *&x ) const;
  void get( Input          *&x ) const;
  void get( Param          *&x ) const;
  void get( Sequence const *&x ) const;
  void get( CountPtr< ArrayBase > &x ) const;
  void get( CountPtr< Frame     > &x ) const;
  void get( CountPtr< Function  > &x ) const;
  void get( CountPtr< Node      > &x ) const;
  void get( CountPtr< Output    > &x ) const;
  void get( CountPtr< Input     > &x ) const;
  void get( CountPtr< Param     > &x ) const;
  void get( CountPtr< const Sequence > &x ) const;
  void get( Value &x ) const { x = (*this); }
  template< class T > void get( T &x ) const { throw GetFailed( m_type, typeOf( x ) ); }

  template< class T >
  T get( void ) const
  {
    T ret;
    get( ret );
    return ret;
  }

  const void *getP( void ) const { return p; }

  bool isCollectable( void ) const;
  const Collectable *getCollectable( void ) const;
private:
  template< class PrimitiveType >
  static int primitive_compare( PrimitiveType x, PrimitiveType y );

  union
  {
    void        *p  ;
    Collectable *col;

    bool     b   ;
    uint8_t  ui8 ;
    int8_t    i8 ;
    uint16_t ui16;
    int16_t   i16;
    uint32_t ui32;
    int32_t   i32;
    uint64_t ui64;
    int64_t   i64;
    float    f   ;
    double   d   ;

    StringData const *str ;
    ArrayBase      *arr ;
    Frame          *frame;
    Function       *func;
    Node           *node;
    Output         *out ;
    Input          *in  ;
    Param          *param  ;
    Sequence const *seq ;
  };
  Type m_type;
};

CountPtr< ArrayBase > new_Array( GarbageCollector *gc, Type type, int dims, const Value &fill_value = Value() );

} // namespace RPGML

namespace std {

inline
void swap( RPGML::Value &x1, RPGML::Value &x2 )
{
  x1.swap( x2 );
}

  std::ostream &operator<<( std::ostream &o, const RPGML::Value &s );

} // namespace std

#endif


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
#ifndef RPGML_Type_h
#define RPGML_Type_h

#include "types.h"

#include <ostream>

namespace RPGML {

class String;
class Frame;
class Function;
class Node;
class Output;
class Input;
class Param;
class Sequence;
class ArrayBase;
class Collectable;
template< class RefcountedType > class CountPtr;

class Type
{
public:
  enum Enum
  {
      NIL      // no type specified
    , BOOL     // bool
    , UINT8    // uint8_t
    , INT8     // int8_t
    , UINT16   // uint16_t
    , INT16    // int16_t
    , UINT32   // uint32_t
    , INT32    // int32_t
    , UINT64   // uint64_t
    , INT64    // int64_t
    , FLOAT    // float
    , DOUBLE   // double
    , STRING   // String
    , FRAME    // CountPtr< Frame          >
    , FUNCTION // CountPtr< Function       >
    , NODE     // CountPtr< Node           >
    , OUTPUT   // CountPtr< Output         >
    , INPUT    // CountPtr< Input          >
    , PARAM    // CountPtr< Param          >
    , SEQUENCE // CountPtr< const Sequence >
    , ARRAY    // CountPtr< ArrayBase      >
    , OTHER    // anything else

    , INT = INT32
  };

  Type( Enum e=NIL ) : m_e( e ) {}
  Type &operator=( Enum e ) { m_e = e; return (*this); }

  const char *getTypeName( void ) const
  {
    static
    const char *const names[] =
    {
        "nil"
      , "bool"
      , "uint8"
      , "int8"
      , "uint16"
      , "int16"
      , "uint32"
      , "int32"
      , "uint64"
      , "int64"
      , "float"
      , "double"
      , "string"
      , "Frame"
      , "Function"
      , "Node"
      , "Output"
      , "Input"
      , "Param"
      , "Sequence"
      , "Array"
      , "other"
    };

    return names[ m_e ];
  }

  static
  Enum getTypeEnum( const char *typeName )
  {
    switch( typeName[ 0 ] )
    {
      case 'A': return ARRAY;
      case 'F':
        if( typeName[ 1 ] == 'r' ) return FRAME;
        if( typeName[ 1 ] == 'u' ) return FUNCTION;
        return OTHER;
      case 'I': return INPUT;
      case 'N': return NODE;
      case 'O': return OUTPUT;
      case 'P': return PARAM;
      case 'S': return SEQUENCE;
      case 'b': return BOOL;
      case 'd': return DOUBLE;
      case 'f': return FLOAT;
      case 'i':
        if( typeName[ 3 ] == '8' ) return INT8;
        if( typeName[ 3 ] == '1' ) return INT16;
        if( typeName[ 3 ] == '3' ) return INT32;
        if( typeName[ 3 ] == '6' ) return INT64;
        if( typeName[ 3 ] == '\0' ) return INT;
        return OTHER;
      case 'n': return NIL;
      case 'o': return OTHER;
      case 's': return STRING;
      case 'u':
        if( typeName[ 4 ] == '8' ) return UINT8;
        if( typeName[ 4 ] == '1' ) return UINT16;
        if( typeName[ 4 ] == '3' ) return UINT32;
        if( typeName[ 4 ] == '6' ) return UINT64;
        return OTHER;
      default:
        return OTHER;
    }
  }

  explicit
  Type( const char *typeName )
  : m_e( getTypeEnum( typeName ) )
  {}

  bool operator==( Enum e ) const { return m_e == e; }
  bool operator!=( Enum e ) const { return m_e != e; }
  bool operator< ( Enum e ) const { return m_e <  e; }
  bool operator> ( Enum e ) const { return m_e >  e; }
  bool operator<=( Enum e ) const { return m_e <= e; }
  bool operator>=( Enum e ) const { return m_e >= e; }

  int compare( const Type &other ) const
  {
    if( m_e == other.m_e ) return  0;
    if( m_e <  other.m_e ) return -1;
    return 1;
  }

  Enum getEnum( void ) const { return m_e; }
  operator Enum( void ) const { return getEnum(); }

  bool isScalar( void ) const
  {
    static const uint64_t _1 = uint64_t( 1 );
    static const uint64_t flags =
        ( _1 << Type::BOOL   )
      | ( _1 << Type::UINT8  )
      | ( _1 << Type::INT8   )
      | ( _1 << Type::UINT16 )
      | ( _1 << Type::INT16  )
      | ( _1 << Type::UINT32 )
      | ( _1 << Type::INT32  )
      | ( _1 << Type::UINT64 )
      | ( _1 << Type::INT64  )
      | ( _1 << Type::FLOAT  )
      | ( _1 << Type::DOUBLE )
      ;
    return flags & ( _1 << m_e );
  }

  bool isInteger( void ) const
  {
    static const uint64_t _1 = uint64_t( 1 );
    static const uint64_t flags =
        ( _1 << Type::BOOL   )
      | ( _1 << Type::UINT8  )
      | ( _1 << Type::INT8   )
      | ( _1 << Type::UINT16 )
      | ( _1 << Type::INT16  )
      | ( _1 << Type::UINT32 )
      | ( _1 << Type::INT32  )
      | ( _1 << Type::UINT64 )
      | ( _1 << Type::INT64  )
      ;
    return flags & ( _1 << m_e );
  }

  bool isFloatingPoint( void ) const
  {
    static const uint64_t _1 = uint64_t( 1 );
    static const uint64_t flags =
        ( _1 << Type::FLOAT  )
      | ( _1 << Type::DOUBLE )
      ;
    return flags & ( _1 << m_e );
  }

  bool isPrimitive( void ) const
  {
    static const uint64_t _1 = uint64_t( 1 );
    static const uint64_t flags =
        ( _1 << Type::BOOL   )
      | ( _1 << Type::UINT8  )
      | ( _1 << Type::INT8   )
      | ( _1 << Type::UINT16 )
      | ( _1 << Type::INT16  )
      | ( _1 << Type::UINT32 )
      | ( _1 << Type::INT32  )
      | ( _1 << Type::UINT64 )
      | ( _1 << Type::INT64  )
      | ( _1 << Type::FLOAT  )
      | ( _1 << Type::DOUBLE )
      | ( _1 << Type::STRING )
      ;
    return flags & ( _1 << m_e );
  }

  bool hasSign( void ) const
  {
    static const uint64_t _1 = uint64_t( 1 );
    static const uint64_t flags =
        ( _1 << Type::INT8   )
      | ( _1 << Type::INT16  )
      | ( _1 << Type::INT32  )
      | ( _1 << Type::INT64  )
      | ( _1 << Type::FLOAT  )
      | ( _1 << Type::DOUBLE )
      ;
    return flags & ( _1 << m_e );
  }

  static Type Nil      ( void ) { return Type( Type::NIL      ); }
  static Type Bool     ( void ) { return Type( Type::BOOL     ); }
  static Type UInt8    ( void ) { return Type( Type::UINT8    ); }
  static Type Int8     ( void ) { return Type( Type::INT8     ); }
  static Type UInt16   ( void ) { return Type( Type::UINT16   ); }
  static Type Int16    ( void ) { return Type( Type::INT16    ); }
  static Type UInt32   ( void ) { return Type( Type::UINT32   ); }
  static Type Int32    ( void ) { return Type( Type::INT32    ); }
  static Type UInt64   ( void ) { return Type( Type::UINT64   ); }
  static Type Int64    ( void ) { return Type( Type::INT64    ); }
  static Type Float    ( void ) { return Type( Type::FLOAT    ); }
  static Type Double   ( void ) { return Type( Type::DOUBLE   ); }
  static Type String   ( void ) { return Type( Type::STRING   ); }
  static Type Frame    ( void ) { return Type( Type::FRAME    ); }
  static Type Function ( void ) { return Type( Type::FUNCTION ); }
  static Type Node     ( void ) { return Type( Type::NODE     ); }
  static Type Output   ( void ) { return Type( Type::OUTPUT   ); }
  static Type Input    ( void ) { return Type( Type::INPUT    ); }
  static Type Param    ( void ) { return Type( Type::PARAM    ); }
  static Type Sequence ( void ) { return Type( Type::SEQUENCE ); }
  static Type Array    ( void ) { return Type( Type::ARRAY    ); }
  static Type Other    ( void ) { return Type( Type::OTHER    ); }

  static Type Int     ( void ) { return Type::Int32(); }

  explicit Type( const bool               & ) : m_e( Type::BOOL     ) {}
  explicit Type( const uint8_t            & ) : m_e( Type::UINT8    ) {}
  explicit Type( const int8_t             & ) : m_e( Type::INT8     ) {}
  explicit Type( const uint16_t           & ) : m_e( Type::UINT16   ) {}
  explicit Type( const int16_t            & ) : m_e( Type::INT16    ) {}
  explicit Type( const uint32_t           & ) : m_e( Type::UINT32   ) {}
  explicit Type( const int32_t            & ) : m_e( Type::INT32    ) {}
  explicit Type( const uint64_t           & ) : m_e( Type::UINT64   ) {}
  explicit Type( const int64_t            & ) : m_e( Type::INT64    ) {}
  explicit Type( const float              & ) : m_e( Type::FLOAT    ) {}
  explicit Type( const double             & ) : m_e( Type::DOUBLE   ) {}
  explicit Type( const RPGML::String      & ) : m_e( Type::STRING   ) {}
  explicit Type( const RPGML::Frame       & ) : m_e( Type::FRAME    ) {}
  explicit Type( const RPGML::Function    & ) : m_e( Type::FUNCTION ) {}
  explicit Type( const RPGML::Node        & ) : m_e( Type::NODE     ) {}
  explicit Type( const RPGML::Output      & ) : m_e( Type::OUTPUT   ) {}
  explicit Type( const RPGML::Input       & ) : m_e( Type::INPUT    ) {}
  explicit Type( const RPGML::Param       & ) : m_e( Type::PARAM    ) {}
  explicit Type( const RPGML::Sequence    & ) : m_e( Type::SEQUENCE ) {}
  explicit Type( const RPGML::ArrayBase   & ) : m_e( Type::ARRAY    ) {}

  bool isNil     ( void ) const { return ( m_e == Type::NIL      ); }
  bool isBool    ( void ) const { return ( m_e == Type::BOOL     ); }
  bool isUInt8   ( void ) const { return ( m_e == Type::UINT8    ); }
  bool isInt8    ( void ) const { return ( m_e == Type::INT8     ); }
  bool isUInt16  ( void ) const { return ( m_e == Type::UINT16   ); }
  bool isInt16   ( void ) const { return ( m_e == Type::INT16    ); }
  bool isUInt32  ( void ) const { return ( m_e == Type::UINT32   ); }
  bool isInt32   ( void ) const { return ( m_e == Type::INT32    ); }
  bool isUInt64  ( void ) const { return ( m_e == Type::UINT64   ); }
  bool isInt64   ( void ) const { return ( m_e == Type::INT64    ); }
  bool isFloat   ( void ) const { return ( m_e == Type::FLOAT    ); }
  bool isDouble  ( void ) const { return ( m_e == Type::DOUBLE   ); }
  bool isString  ( void ) const { return ( m_e == Type::STRING   ); }
  bool isFrame   ( void ) const { return ( m_e == Type::FRAME    ); }
  bool isFunction( void ) const { return ( m_e == Type::FUNCTION ); }
  bool isNode    ( void ) const { return ( m_e == Type::NODE     ); }
  bool isOutput  ( void ) const { return ( m_e == Type::OUTPUT   ); }
  bool isInput   ( void ) const { return ( m_e == Type::INPUT    ); }
  bool isParam   ( void ) const { return ( m_e == Type::PARAM    ); }
  bool isSequence( void ) const { return ( m_e == Type::SEQUENCE ); }
  bool isArray   ( void ) const { return ( m_e == Type::ARRAY    ); }
  bool isOther   ( void ) const { return ( m_e == Type::OTHER    ); }

  bool isInt      ( void ) const { return isInt32(); }

  static
  Type WithSign( Type type )
  {
    static const Enum ret[] =
    {
        NIL      // NIL
      , BOOL     // BOOL
      , INT8     // UINT8
      , INT8     // INT8
      , INT16    // UINT16
      , INT16    // INT16
      , INT32    // UINT32
      , INT32    // INT32
      , INT64    // UINT64
      , INT64    // INT64
      , FLOAT    // FLOAT
      , DOUBLE   // DOUBLE
      , NIL      // STRING
      , NIL      // FRAME
      , NIL      // FUNCTION
      , NIL      // NODE
      , NIL      // OUTPUT
      , NIL      // INPUT
      , NIL      // PARAM
      , NIL      // SEQUENCE
      , NIL      // ARRAY
      , NIL      // OTHER
    };

    if( type <= Type::Other() )
    {
      return Type( ret[ type ] );
    }
    else
    {
      return Nil();
    }
  }

  static
  Type Ret( Type left, Type right )
  {
    if( left.isScalar() && right.isScalar() )
    {
      if( left.hasSign() || right.hasSign() )
      {
        return WithSign( std::max( left, right ) );
      }
      else
      {
        return std::max( left, right );
      }
    }
    else if( left.isString() && right.isPrimitive() )
    {
      return String();
    }
    else if( left.isPrimitive() && right.isString() )
    {
      return String();
    }
    else
    {
      return Nil();
    }
  }

private:
  Enum m_e;
};

template< typename T > struct TypeOf              { static const Type::Enum E = Type::OTHER; };

template<> struct TypeOf< bool                  > { static const Type::Enum E = Type::BOOL    ; };
template<> struct TypeOf< uint8_t               > { static const Type::Enum E = Type::UINT8   ; };
template<> struct TypeOf< int8_t                > { static const Type::Enum E = Type::INT8    ; };
template<> struct TypeOf< uint16_t              > { static const Type::Enum E = Type::UINT16  ; };
template<> struct TypeOf< int16_t               > { static const Type::Enum E = Type::INT16   ; };
template<> struct TypeOf< uint32_t              > { static const Type::Enum E = Type::UINT32  ; };
template<> struct TypeOf< int32_t               > { static const Type::Enum E = Type::INT32   ; };
template<> struct TypeOf< uint64_t              > { static const Type::Enum E = Type::UINT64  ; };
template<> struct TypeOf< int64_t               > { static const Type::Enum E = Type::INT64   ; };
template<> struct TypeOf< float                 > { static const Type::Enum E = Type::FLOAT   ; };
template<> struct TypeOf< double                > { static const Type::Enum E = Type::DOUBLE  ; };
template<> struct TypeOf< String                > { static const Type::Enum E = Type::STRING  ; };
template<> struct TypeOf< CountPtr< Frame     > > { static const Type::Enum E = Type::FRAME   ; };
template<> struct TypeOf< CountPtr< Function  > > { static const Type::Enum E = Type::FUNCTION; };
template<> struct TypeOf< CountPtr< Node      > > { static const Type::Enum E = Type::NODE    ; };
template<> struct TypeOf< CountPtr< Output    > > { static const Type::Enum E = Type::OUTPUT  ; };
template<> struct TypeOf< CountPtr< Input     > > { static const Type::Enum E = Type::INPUT   ; };
template<> struct TypeOf< CountPtr< Param     > > { static const Type::Enum E = Type::PARAM   ; };
template<> struct TypeOf< CountPtr< const Sequence  > > { static const Type::Enum E = Type::SEQUENCE; };
template<> struct TypeOf< CountPtr< ArrayBase > > { static const Type::Enum E = Type::ARRAY   ; };

template< Type::Enum E > struct EnumType {};

template<> struct EnumType< Type::BOOL     >{ typedef bool                  T; };
template<> struct EnumType< Type::UINT8    >{ typedef uint8_t               T; };
template<> struct EnumType< Type::INT8     >{ typedef int8_t                T; };
template<> struct EnumType< Type::UINT16   >{ typedef uint16_t              T; };
template<> struct EnumType< Type::INT16    >{ typedef int16_t               T; };
template<> struct EnumType< Type::UINT32   >{ typedef uint32_t              T; };
template<> struct EnumType< Type::INT32    >{ typedef int32_t               T; };
template<> struct EnumType< Type::UINT64   >{ typedef uint64_t              T; };
template<> struct EnumType< Type::INT64    >{ typedef int64_t               T; };
template<> struct EnumType< Type::FLOAT    >{ typedef float                 T; };
template<> struct EnumType< Type::DOUBLE   >{ typedef double                T; };
template<> struct EnumType< Type::STRING   >{ typedef String                T; };
template<> struct EnumType< Type::FRAME    >{ typedef CountPtr< Frame     > T; };
template<> struct EnumType< Type::FUNCTION >{ typedef CountPtr< Function  > T; };
template<> struct EnumType< Type::NODE     >{ typedef CountPtr< Node      > T; };
template<> struct EnumType< Type::OUTPUT   >{ typedef CountPtr< Output    > T; };
template<> struct EnumType< Type::INPUT    >{ typedef CountPtr< Input     > T; };
template<> struct EnumType< Type::PARAM    >{ typedef CountPtr< Param     > T; };
template<> struct EnumType< Type::SEQUENCE >{ typedef CountPtr< const Sequence  > T; };
template<> struct EnumType< Type::ARRAY    >{ typedef CountPtr< ArrayBase > T; };

template< typename T > struct IsSigned { static const bool B = false; };

template<> struct IsSigned< int8_t  > { static const bool B = true; };
template<> struct IsSigned< int16_t > { static const bool B = true; };
template<> struct IsSigned< int32_t > { static const bool B = true; };
template<> struct IsSigned< int64_t > { static const bool B = true; };
template<> struct IsSigned< float   > { static const bool B = true; };
template<> struct IsSigned< double  > { static const bool B = true; };

template< typename T > struct IsScalar
{
  static const Type::Enum _E = TypeOf< T >::E;
  static const bool B = ( _E >= Type::BOOL && _E <= Type::DOUBLE );
};

template< typename T > struct IsPrimitive
{
  static const Type::Enum _E = TypeOf< T >::E;
  static const bool B = ( _E >= Type::BOOL && _E <= Type::STRING );
};

template< typename T > struct IsInteger
{
  static const Type::Enum _E = TypeOf< T >::E;
  static const bool B = ( _E >= Type::BOOL && _E < Type::FLOAT );
};

template< typename T > struct IsFloatingPoint
{
  static const Type::Enum _E = TypeOf< T >::E;
  static const bool B = ( _E >= Type::FLOAT && _E <= Type::DOUBLE );
};

template< typename _T > struct MakeSigned { typedef _T T; static const Type::Enum E = Type::OTHER; static const bool B = false; };

template<> struct MakeSigned< bool     >{ typedef int8_t  T; static const Type::Enum E = Type::INT8  ; static const bool B = true; };
template<> struct MakeSigned< uint8_t  >{ typedef int8_t  T; static const Type::Enum E = Type::INT8  ; static const bool B = true; };
template<> struct MakeSigned< int8_t   >{ typedef int8_t  T; static const Type::Enum E = Type::INT8  ; static const bool B = true; };
template<> struct MakeSigned< uint16_t >{ typedef int16_t T; static const Type::Enum E = Type::INT16 ; static const bool B = true; };
template<> struct MakeSigned< int16_t  >{ typedef int16_t T; static const Type::Enum E = Type::INT16 ; static const bool B = true; };
template<> struct MakeSigned< uint32_t >{ typedef int32_t T; static const Type::Enum E = Type::INT32 ; static const bool B = true; };
template<> struct MakeSigned< int32_t  >{ typedef int32_t T; static const Type::Enum E = Type::INT32 ; static const bool B = true; };
template<> struct MakeSigned< uint64_t >{ typedef int64_t T; static const Type::Enum E = Type::INT64 ; static const bool B = true; };
template<> struct MakeSigned< int64_t  >{ typedef int64_t T; static const Type::Enum E = Type::INT64 ; static const bool B = true; };
template<> struct MakeSigned< float    >{ typedef float   T; static const Type::Enum E = Type::FLOAT ; static const bool B = true; };
template<> struct MakeSigned< double   >{ typedef double  T; static const Type::Enum E = Type::DOUBLE; static const bool B = true; };

template< int SELECT, typename T0, typename T1, typename T2=void, typename T3=void > struct SelectType {};
template< typename T0, typename T1, typename T2, typename T3 > struct SelectType< 0, T0, T1, T2, T3 > { typedef T0 T; };
template< typename T0, typename T1, typename T2, typename T3 > struct SelectType< 1, T0, T1, T2, T3 > { typedef T1 T; };
template< typename T0, typename T1, typename T2, typename T3 > struct SelectType< 2, T0, T1, T2, T3 > { typedef T2 T; };
template< typename T0, typename T1, typename T2, typename T3 > struct SelectType< 3, T0, T1, T2, T3 > { typedef T3 T; };

template< class X, class Y >
struct MaxType
{
  static const Type::Enum _EX = TypeOf< X >::E;
  static const Type::Enum _EY = TypeOf< Y >::E;
  static const Type::Enum E = ( _EX > _EY ? _EX : _EY );
  typedef typename EnumType< E >::T T;
};

template< class X, class Y >
struct RetType
{
  static const Type::Enum EX = TypeOf< X >::E;
  static const Type::Enum EY = TypeOf< Y >::E;

  // See Type::Ret()
  static const Type::Enum E =
    ( IsScalar< X >::B && IsScalar< X >::B
    ? ( IsSigned< X >::B || IsSigned< X >::B
      ? MakeSigned< typename MaxType< X, Y >::T >::E
      : MaxType< X, Y >::E
      )
    : ( EX == Type::STRING && IsPrimitive< Y >::B
      ? Type::STRING
      : ( IsPrimitive< X >::B && EY == Type::STRING
        ? Type::STRING
        : Type::NIL
        )
      )
    );

  typedef typename EnumType< E >::T T;
};

template< class IN >
struct AppropFloat
{
  static const Type::Enum IN_E = TypeOf< IN >::E;

  static const int is_scalar = IsScalar< IN >::B;

  static const Type::Enum ScalarE =
    ( IN_E < Type::UINT64
    ? Type::FLOAT // integers with less than 64 bit
    : ( IN_E == Type::FLOAT
      ? Type::FLOAT // float
      : Type::DOUBLE // double and 64 bit integers and others
      )
    );
  typedef typename EnumType< ScalarE >::T ScalarT;

  typedef typename SelectType< is_scalar, IN, ScalarT >::T T;
  static const Type::Enum E = TypeOf< T >::E;
};

template< class T >
static inline
Type typeOf( const T & ) { return Type( TypeOf< T >::E  ); }

template< class T >
static inline
const char *getTypeName( void ) { return Type( TypeOf< T >::E  ).getTypeName(); }

} // namespace RPGML

namespace std {

  static inline
  std::ostream &operator<<( std::ostream &o, const RPGML::Type &t )
  {
    return o << t.getTypeName();
  }

} // namespace std
#endif


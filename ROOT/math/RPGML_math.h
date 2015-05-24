/* This file is part of RPGML.
 *
 * Copyright (c) 2015, Gunnar Payer, All rights reserved.
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
#ifndef RPGML_math_h
#define RPGML_math_h

#include "RPGML_MapOp.h"

#include <cmath>
#include <cstring>

namespace RPGML {
namespace math {

enum MOP1
{
    MOP1_UNDEFINED
  , MOP1_MINUS
  , MOP1_PLUS
  , MOP1_LOG_NOT
  , MOP1_BIT_NOT
  , MOP1_SIN
  , MOP1_COS
  , MOP1_TAN
  , MOP1_ASIN
  , MOP1_ACOS
  , MOP1_ATAN
  , MOP1_EXP
  , MOP1_EXP10
  , MOP1_EXP2
  , MOP1_EXPM1
  , MOP1_SQRT
  , MOP1_LOG
  , MOP1_LOG10
  , MOP1_LOG2
  , MOP1_LOG1P
  , MOP1_ABS
  , MOP1_SQR
};

static inline
const char *getMOP1Str( MOP1 op )
{
  static
  const char *const str[] =
  {
      "undefined"
    , "-", "+", "!", "~"
    , "sin", "cos", "tan"
    , "asin", "acos", "atan"
    , "exp", "exp10", "exp2", "expm1"
    , "sqrt"
    , "log", "log10", "log2", "log1p"
    , "abs"
    , "sqr"
  };
  return str[ op ];
};

static inline
MOP1 getMOP1( const char *op )
{
  switch( op[ 0 ] )
  {
    case '-': return MOP1_MINUS;
    case '+': return MOP1_PLUS;
    case '!': return MOP1_LOG_NOT;
    case '~': return MOP1_BIT_NOT;

    case 'a':
      switch( op[ 1 ] )
      {
        case 'b': return MOP1_ABS;
        case 's': return MOP1_ASIN;
        case 'c': return MOP1_ACOS;
        case 't': return MOP1_ATAN;
      }
      break;

    case 'c': return MOP1_COS;

    case 'e':
      if( op[ 1 ] == 'x' && op[ 2 ] == 'p' )
      {
        switch( op[ 3 ] )
        {
          case '\0': return MOP1_EXP;
          case '1': return MOP1_EXP10;
          case '2': return MOP1_EXP2;
          case 'm': return MOP1_EXPM1;
        }
      }
      break;

    case 'l':
      if( op[ 1 ] == 'o' && op[ 2 ] == 'g' )
      {
        switch( op[ 3 ] )
        {
          case '\0': return MOP1_LOG;
          case '1':
            switch( op[ 4 ] )
            {
              case '0': return MOP1_LOG10;
              case 'p': return MOP1_LOG1P;
            }
            break;

          case '2': return MOP1_LOG2;
        }
      }
      break;

    case 's':
      switch( op[ 1 ] )
      {
        case 'i': return MOP1_SIN;
        case 'q':
          if( op[ 2 ] == 'r' )
          {
            if     ( op[ 3 ] == 't'  ) return MOP1_SQRT;
            else if( op[ 3 ] == '\0' ) return MOP1_SQR;
          }
      }
      break;

    case 't': return MOP1_TAN;
  }

  throw Exception() << "Invalid math op '" << op << "'";
};

template< class _T, MOP1 op > struct MathOp1_op { typedef _T T, Ret; };

template< class _T >
struct MathOp1_op< _T, MOP1_MINUS >
{
  static const MOP1 op = MOP1_MINUS;
  template< class OtherT >
  struct Other
  {
    typedef MathOp1_op< OtherT, op > Op;
  };
  typedef _T T;
  typedef typename MakeSigned< _T >::T Ret;
  Ret operator()( const T &x ) const { return Ret( -Ret( x ) ); }
};

template< class _T >
struct  MathOp1_op< _T, MOP1_PLUS >
{
  static const MOP1 op = MOP1_PLUS;
  template< class OtherT >
  struct Other
  {
    typedef MathOp1_op< OtherT, op > Op;
  };
  typedef _T T, Ret;
  Ret operator()( const T &x ) const { return x; }
};

template< class _T >
struct MathOp1_op< _T, MOP1_LOG_NOT >
{
  static const MOP1 op = MOP1_LOG_NOT;
  template< class OtherT >
  struct Other
  {
    typedef MathOp1_op< OtherT, op > Op;
  };
  typedef _T T;
  typedef bool Ret;
  Ret operator()( const T &x ) const { return bool( x ); }
};

template< class _T >
struct MathOp1_op< _T, MOP1_BIT_NOT >
{
  static const MOP1 op = MOP1_BIT_NOT;
  template< class OtherT >
  struct Other
  {
    typedef MathOp1_op< OtherT, op > Op;
  };
  typedef _T T;
  typedef typename SelectType< int( IsInteger< T >::B ), uint8_t, T >::T Ret;
  Ret operator()( const T &x ) const
  {
    if( !IsInteger< T >::B )
    {
      throw Exception()
        << "Operator '~' only supported for integer types, is " << getTypeName< T >()
        ;
    }
    return Ret( ~Ret( x ) );
  }
};

#define DEFINE_FLOAT_MATHOP1( mop1, func ) \
  template< class _T > \
  struct MathOp1_op< _T, mop1 > \
  { \
    static const MOP1 op = mop1; \
    template< class OtherT > \
    struct Other \
    { \
      typedef MathOp1_op< OtherT, op > Op; \
    }; \
    typedef _T T; \
    typedef typename AppropFloat< T >::T Ret; \
    Ret f( float x ) const { return func ## f( x ); } \
    Ret f( double x ) const { return func( x ); } \
    Ret operator()( const T &x ) const { return f( Ret( x ) ); } \
  }

DEFINE_FLOAT_MATHOP1( MOP1_SIN  , sin   );
DEFINE_FLOAT_MATHOP1( MOP1_COS  , cos   );
DEFINE_FLOAT_MATHOP1( MOP1_TAN  , tan   );
DEFINE_FLOAT_MATHOP1( MOP1_ASIN , asin  );
DEFINE_FLOAT_MATHOP1( MOP1_ACOS , acos  );
DEFINE_FLOAT_MATHOP1( MOP1_ATAN , atan  );
DEFINE_FLOAT_MATHOP1( MOP1_EXP  , exp   );
DEFINE_FLOAT_MATHOP1( MOP1_EXP10, exp10 );
DEFINE_FLOAT_MATHOP1( MOP1_EXP2 , exp2  );
DEFINE_FLOAT_MATHOP1( MOP1_EXPM1, expm1 );
DEFINE_FLOAT_MATHOP1( MOP1_SQRT , sqrt  );
DEFINE_FLOAT_MATHOP1( MOP1_LOG  , log   );
DEFINE_FLOAT_MATHOP1( MOP1_LOG10, log10 );
DEFINE_FLOAT_MATHOP1( MOP1_LOG2 , log2  );
DEFINE_FLOAT_MATHOP1( MOP1_LOG1P, log1p );

bool     abs( const bool     &x ) { return x; }
uint8_t  abs( const uint8_t  &x ) { return x; }
int8_t   abs( const int8_t   &x ) { return int8_t( std::abs( int( x ) ) ); }
uint16_t abs( const uint16_t &x ) { return x; }
int16_t  abs( const int16_t  &x ) { return int16_t( std::abs( int( x ) ) ); }
uint32_t abs( const uint32_t &x ) { return x; }
int32_t  abs( const int32_t  &x ) { return std::abs( x ); }
uint64_t abs( const uint64_t &x ) { return x; }
int64_t  abs( const int64_t  &x ) { return std::abs( x ); }
float    abs( const float    &x ) { return ::fabsf( x ); }
double   abs( const double   &x ) { return ::fabs( x ); }

template< class _T >
struct  MathOp1_op< _T, MOP1_ABS >
{
  static const MOP1 op = MOP1_ABS;
  template< class OtherT >
  struct Other
  {
    typedef MathOp1_op< OtherT, op > Op;
  };
  typedef _T T, Ret;
  Ret operator()( const T &x ) const { return RPGML::math::abs( x ); }
};

template< class _T >
struct  MathOp1_op< _T, MOP1_SQR >
{
  static const MOP1 op = MOP1_SQR;
  template< class OtherT >
  struct Other
  {
    typedef MathOp1_op< OtherT, op > Op;
  };
  typedef _T T, Ret;
  Ret operator()( const T &x ) const { return Ret(x*x); }
};

template< class T, class Ret=T >
static
Ret mathOp1( MOP1 op, const T &x )
{
  switch( op )
  {
    case MOP1_MINUS  : return Ret( MapOp< T, MathOp1_op< T, MOP1_MINUS   > >()( x ) );
    case MOP1_PLUS   : return Ret( MapOp< T, MathOp1_op< T, MOP1_PLUS    > >()( x ) );
    case MOP1_LOG_NOT: return Ret( MapOp< T, MathOp1_op< T, MOP1_LOG_NOT > >()( x ) );
    case MOP1_BIT_NOT: return Ret( MapOp< T, MathOp1_op< T, MOP1_BIT_NOT > >()( x ) );
    case MOP1_SIN    : return Ret( MapOp< T, MathOp1_op< T, MOP1_SIN     > >()( x ) );
    case MOP1_COS    : return Ret( MapOp< T, MathOp1_op< T, MOP1_COS     > >()( x ) );
    case MOP1_TAN    : return Ret( MapOp< T, MathOp1_op< T, MOP1_TAN     > >()( x ) );
    case MOP1_ASIN   : return Ret( MapOp< T, MathOp1_op< T, MOP1_ASIN    > >()( x ) );
    case MOP1_ACOS   : return Ret( MapOp< T, MathOp1_op< T, MOP1_ACOS    > >()( x ) );
    case MOP1_ATAN   : return Ret( MapOp< T, MathOp1_op< T, MOP1_ATAN    > >()( x ) );
    case MOP1_EXP    : return Ret( MapOp< T, MathOp1_op< T, MOP1_EXP     > >()( x ) );
    case MOP1_EXP10  : return Ret( MapOp< T, MathOp1_op< T, MOP1_EXP10   > >()( x ) );
    case MOP1_EXP2   : return Ret( MapOp< T, MathOp1_op< T, MOP1_EXP2    > >()( x ) );
    case MOP1_EXPM1  : return Ret( MapOp< T, MathOp1_op< T, MOP1_EXPM1   > >()( x ) );
    case MOP1_SQRT   : return Ret( MapOp< T, MathOp1_op< T, MOP1_SQRT    > >()( x ) );
    case MOP1_LOG    : return Ret( MapOp< T, MathOp1_op< T, MOP1_LOG     > >()( x ) );
    case MOP1_LOG10  : return Ret( MapOp< T, MathOp1_op< T, MOP1_LOG10   > >()( x ) );
    case MOP1_LOG2   : return Ret( MapOp< T, MathOp1_op< T, MOP1_LOG2    > >()( x ) );
    case MOP1_LOG1P  : return Ret( MapOp< T, MathOp1_op< T, MOP1_LOG1P   > >()( x ) );
    case MOP1_ABS    : return Ret( MapOp< T, MathOp1_op< T, MOP1_ABS     > >()( x ) );
    case MOP1_SQR    : return Ret( MapOp< T, MathOp1_op< T, MOP1_SQR     > >()( x ) );
    default:
      throw Exception() << "Undefined op";
  }
}

} // namespace RPGML
} // namespace math

#endif


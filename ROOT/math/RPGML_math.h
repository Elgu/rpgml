#ifndef RPGML_math_h
#define RPGML_math_h

#include <RPGML/Value.h>
#include <RPGML/Array.h>
#include <RPGML/Type.h>
#include <RPGML/Exception.h>

#include <cmath>

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
        case 'q': return MOP1_SQRT;
      }
      break;

    case 't': return MOP1_TAN;
  }

  throw Exception() << "Invalid math op '" << op << "'";
};

template< class _T, MOP1 op > struct MathOp1 { typedef _T T, Ret; };

template< class _T >
struct MathOp1< _T, MOP1_MINUS >
{
  typedef _T T;
  typedef typename MakeSigned< _T >::T Ret;
  Ret operator()( const T &x ) const { return Ret( -Ret( x ) ); }
};

template< class _T >
struct  MathOp1< _T, MOP1_PLUS >
{
  typedef _T T, Ret;
  Ret operator()( const T &x ) const { return x; }
};

template< class _T >
struct MathOp1< _T, MOP1_LOG_NOT >
{
  typedef _T T;
  typedef bool Ret;
  Ret operator()( const T &x ) const { return bool( x ); }
};

template< class _T >
struct MathOp1< _T, MOP1_BIT_NOT >
{
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
  struct MathOp1< _T, mop1 > \
  { \
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

template< class _T, MOP1 _op >
struct mathOp1_t
{
  typedef _T T;
  static const MOP1 op = _op;
  typedef MathOp1< T, op > Op;
  typedef typename Op::Ret Ret;

  Ret operator()( const T &x ) const
  {
    return Op()( x );
  }
};

template< MOP1 _op > struct mathOp1_t< Value, _op >;
template< MOP1 _op > struct mathOp1_t< CountPtr< ArrayBase >, _op >;
template< MOP1 _op > struct mathOp1_t< CountPtr< ArrayElements< Value > >, _op >;
template< MOP1 _op, class ArrayType > struct mathOp1_t< CountPtr< ArrayElements< ArrayType > >, _op >;

template< MOP1 _op >
struct mathOp1_t< Value, _op >
{
  typedef Value T;
  static const MOP1 op = _op;
  typedef Value Ret;

  Ret operator()( const T &x ) const
  {
    switch( x.getType().getEnum() )
    {
      case Type::BOOL  : return Value( mathOp1_t< bool    , op >()( x.getBool  () ) );
      case Type::UINT8 : return Value( mathOp1_t< uint8_t , op >()( x.getUInt8 () ) );
      case Type::INT8  : return Value( mathOp1_t< int8_t  , op >()( x.getInt8  () ) );
      case Type::UINT16: return Value( mathOp1_t< uint16_t, op >()( x.getUInt16() ) );
      case Type::INT16 : return Value( mathOp1_t< int16_t , op >()( x.getInt16 () ) );
      case Type::UINT32: return Value( mathOp1_t< uint32_t, op >()( x.getUInt32() ) );
      case Type::INT32 : return Value( mathOp1_t< int32_t , op >()( x.getInt32 () ) );
      case Type::UINT64: return Value( mathOp1_t< uint64_t, op >()( x.getUInt64() ) );
      case Type::INT64 : return Value( mathOp1_t< int64_t , op >()( x.getInt64 () ) );
      case Type::FLOAT : return Value( mathOp1_t< float   , op >()( x.getFloat () ) );
      case Type::DOUBLE: return Value( mathOp1_t< double  , op >()( x.getDouble() ) );
      case Type::ARRAY : return Value( mathOp1_t< CountPtr< ArrayBase >, op >()( x.getArray() ) );
      default:
        throw Exception() << "Unsupported Type";
    }
  }
};

template< MOP1 _op >
struct mathOp1_t< CountPtr< ArrayBase >, _op >
{
  typedef const ArrayBase *T;
  static const MOP1 op = _op;
  typedef CountPtr< ArrayBase > Ret;

  Ret operator()( const T &x ) const
  {
    if( x->isValue() )
    {
      return mathOp1_t< CountPtr< ArrayElements< Value > >, op >()( x );
    }
    switch( x->getType().getEnum() )
    {
      case Type::BOOL  : return Value( mathOp1_t< CountPtr< ArrayElements< bool     > >, op >()( x ) );
      case Type::UINT8 : return Value( mathOp1_t< CountPtr< ArrayElements< uint8_t  > >, op >()( x ) );
      case Type::INT8  : return Value( mathOp1_t< CountPtr< ArrayElements< int8_t   > >, op >()( x ) );
      case Type::UINT16: return Value( mathOp1_t< CountPtr< ArrayElements< uint16_t > >, op >()( x ) );
      case Type::INT16 : return Value( mathOp1_t< CountPtr< ArrayElements< int16_t  > >, op >()( x ) );
      case Type::UINT32: return Value( mathOp1_t< CountPtr< ArrayElements< uint32_t > >, op >()( x ) );
      case Type::INT32 : return Value( mathOp1_t< CountPtr< ArrayElements< int32_t  > >, op >()( x ) );
      case Type::UINT64: return Value( mathOp1_t< CountPtr< ArrayElements< uint64_t > >, op >()( x ) );
      case Type::INT64 : return Value( mathOp1_t< CountPtr< ArrayElements< int64_t  > >, op >()( x ) );
      case Type::FLOAT : return Value( mathOp1_t< CountPtr< ArrayElements< float    > >, op >()( x ) );
      case Type::DOUBLE: return Value( mathOp1_t< CountPtr< ArrayElements< double   > >, op >()( x ) );
      case Type::ARRAY : return Value( mathOp1_t< CountPtr< ArrayElements< CountPtr< ArrayBase > > >, op >()( x ) );
      default:
        throw Exception() << "Unsupported Array Type";
    }
  }
};

template< MOP1 _op >
struct mathOp1_t< CountPtr< ArrayElements< Value > >, _op >
{
  typedef const ArrayBase *T;
  static const MOP1 op = _op;
  typedef CountPtr< ArrayElements< Value > > Ret;

  Ret operator()( const T &x ) const
  {
    typedef ArrayElements< Value > ValueArrayElements;
    const ValueArrayElements *e = 0;
    if( !x->getAs( e ) ) throw Exception() << "x is not a Value Array";

    const ArrayBase::Size size = x->getSize();
    CountPtr< ValueArrayElements > ret( new_Array< Value >( x->getGC(), size.getDims() ) );
    ret->resize( size );

    ValueArrayElements::const_iterator e_iter = e->begin();
    ValueArrayElements::const_iterator e_end  = e->end();
    ValueArrayElements::iterator ret_iter = ret->begin();
    for( ; e_iter != e_end; ++e_iter, ++ret_iter )
    {
      (*ret_iter ) = mathOp1_t< Value, op >()( (*e_iter) );
    }

    return ret;
  }
};

template< MOP1 _op, class Element >
struct mathOp1_t< CountPtr< ArrayElements< Element > >, _op >
{
  typedef const ArrayBase *T;
  static const MOP1 op = _op;
  typedef mathOp1_t< Element, op > ElementOp;
  typedef typename ElementOp::Ret RetElement;
  typedef CountPtr< ArrayElements< RetElement > > Ret;

  Ret operator()( const T &x ) const
  {
    typedef ArrayElements< Element > InElements;
    typedef ArrayElements< RetElement > RetElements;

    const InElements *e = 0;
    if( !x->getAs( e ) )
    {
      throw Exception()
        << "x is not of specified element type " << getTypeName< T >()
        << ", is " << x->getTypeName()
        ;
    }

    const ArrayBase::Size size = x->getSize();
    CountPtr< RetElements > ret( new_Array< RetElement >( x->getGC(), size.getDims() ) );
    ret->resize( size );

    typename InElements::const_iterator e_iter = e->begin();
    typename InElements::const_iterator e_end  = e->end();
    typename RetElements::iterator ret_iter = ret->begin();
    for( ; e_iter != e_end; ++e_iter, ++ret_iter )
    {
      (*ret_iter ) = ElementOp()( Element(*e_iter) );
    }

    return ret;
  }
};

template< class T >
static
typename MathOp1< T, MOP1_UNDEFINED >::Ret mathOp1( MOP1 op, const T &x )
{
  switch( op )
  {
    case MOP1_MINUS  : return mathOp1_t< T, MOP1_MINUS   >()( x );
    case MOP1_PLUS   : return mathOp1_t< T, MOP1_PLUS    >()( x );
    case MOP1_LOG_NOT: return mathOp1_t< T, MOP1_LOG_NOT >()( x );
    case MOP1_BIT_NOT: return mathOp1_t< T, MOP1_BIT_NOT >()( x );
    case MOP1_SIN    : return mathOp1_t< T, MOP1_SIN     >()( x );
    case MOP1_COS    : return mathOp1_t< T, MOP1_COS     >()( x );
    case MOP1_TAN    : return mathOp1_t< T, MOP1_TAN     >()( x );
    case MOP1_ASIN   : return mathOp1_t< T, MOP1_ASIN    >()( x );
    case MOP1_ACOS   : return mathOp1_t< T, MOP1_ACOS    >()( x );
    case MOP1_ATAN   : return mathOp1_t< T, MOP1_ATAN    >()( x );
    case MOP1_EXP    : return mathOp1_t< T, MOP1_EXP     >()( x );
    case MOP1_EXP10  : return mathOp1_t< T, MOP1_EXP10   >()( x );
    case MOP1_EXP2   : return mathOp1_t< T, MOP1_EXP2    >()( x );
    case MOP1_EXPM1  : return mathOp1_t< T, MOP1_EXPM1   >()( x );
    case MOP1_SQRT   : return mathOp1_t< T, MOP1_SQRT    >()( x );
    case MOP1_LOG    : return mathOp1_t< T, MOP1_LOG     >()( x );
    case MOP1_LOG10  : return mathOp1_t< T, MOP1_LOG10   >()( x );
    case MOP1_LOG2   : return mathOp1_t< T, MOP1_LOG2    >()( x );
    case MOP1_LOG1P  : return mathOp1_t< T, MOP1_LOG1P   >()( x );
    default:
      throw Exception() << "Undefined op";
  }
}

} // namespace RPGML
} // namespace math

#endif


#include "BinaryOp.h"

#include "ParserEnums.h"
#include "String.h"

#include <sstream>

namespace RPGML {

BinaryOp::BinaryOp( GarbageCollector *_gc, Map *parent )
: Function( _gc, parent, genDeclArgs() )
{}

BinaryOp::~BinaryOp( void )
{}

namespace BinaryOp_impl {

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

} // namespace BinaryOp_impl

bool BinaryOp::call_impl( Scope *, Value &ret, index_t n_args, const Value *args, index_t )
{
  using namespace BinaryOp_impl;

  if( n_args != NUM_ARGS ) throw "wrong number of arguments.";

  if( !args[ ARG_OP ].isInt() ) throw "'op' argument must be int";

  const BOP op = BOP( args[ ARG_OP ].getInt() );
  const Value &left  = args[ ARG_LEFT ];
  const Value &right = args[ ARG_RIGHT ];

  switch( op )
  {
    case BOP_LEFT   : ret = ( left << right ); break;
    case BOP_RIGHT  : ret = ( left >> right ); break;
    case BOP_LT     : ret = ( left <  right ); break;
    case BOP_LE     : ret = ( left <= right ); break;
    case BOP_GT     : ret = ( left >  right ); break;
    case BOP_GE     : ret = ( left >= right ); break;
    case BOP_EQ     : ret = ( left == right ); break;
    case BOP_NE     : ret = ( left != right ); break;
    case BOP_LOG_AND: ret = ( left && right ); break;
    case BOP_LOG_OR : ret = ( left || right ); break;
    case BOP_LOG_XOR: ret = ( left.log_xor( right ) ); break;
    case BOP_BIT_AND: ret = ( left & right ); break;
    case BOP_BIT_OR : ret = ( left | right ); break;
    case BOP_BIT_XOR: ret = ( left ^ right ); break;
    case BOP_MUL    : ret = ( left * right ); break;
    case BOP_DIV    : ret = ( left / right ); break;
    case BOP_ADD    : ret = ( left + right ); break;
    case BOP_SUB    : ret = ( left - right ); break;
    case BOP_MOD    : ret = ( left % right ); break;
    default:
      throw "Invalid op";
  }

  return true;
}

CountPtr< Function::Args > BinaryOp::genDeclArgs( void )
{
  CountPtr< Args > args = new Args( NUM_ARGS );
  args->at( ARG_LEFT  ) = Arg( new String( "left"  ) );
  args->at( ARG_OP    ) = Arg( new String( "op"    ) );
  args->at( ARG_RIGHT ) = Arg( new String( "right" ) );
  return args;
}

} // namespace RPGML



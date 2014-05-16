#include "RPGML_binaryOp.h"

#include "String.h"
#include "ParserEnums.h"
#include "Scope.h"

#include <sstream>

namespace RPGML {

binaryOp::binaryOp( GarbageCollector *_gc, Frame *parent, const SharedObject *so )
: Function( _gc, parent, genDeclArgs(), so )
{}

binaryOp::~binaryOp( void )
{}

namespace binaryOp_impl {

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

  static inline
  void bop( Value &ret, const Value &left, const BOP op, const Value &right )
  {
    switch( op )
    {
      case BOP_LEFT   : ret = Value( left << right ); break;
      case BOP_RIGHT  : ret = Value( left >> right ); break;
      case BOP_LT     : ret = Value( left <  right ); break;
      case BOP_LE     : ret = Value( left <= right ); break;
      case BOP_GT     : ret = Value( left >  right ); break;
      case BOP_GE     : ret = Value( left >= right ); break;
      case BOP_EQ     : ret = Value( left == right ); break;
      case BOP_NE     : ret = Value( left != right ); break;
      case BOP_LOG_AND: ret = Value( left && right ); break;
      case BOP_LOG_OR : ret = Value( left || right ); break;
      case BOP_LOG_XOR: ret = Value( left.log_xor( right ) ); break;
      case BOP_BIT_AND: ret = Value( left & right ); break;
      case BOP_BIT_OR : ret = Value( left | right ); break;
      case BOP_BIT_XOR: ret = Value( left ^ right ); break;
      case BOP_MUL    : ret = Value( left * right ); break;
      case BOP_DIV    : ret = Value( left / right ); break;
      case BOP_ADD    : ret = Value( left + right ); break;
      case BOP_SUB    : ret = Value( left - right ); break;
      case BOP_MOD    : ret = Value( left % right ); break;
      default:
        throw "Invalid op";
    }
  }
} // namespace binaryOp_impl

bool binaryOp::call_impl( const Location *loc, Scope *scope, Value &ret, index_t n_args, const Value *args, index_t )
{
  using namespace binaryOp_impl;

  if( n_args != NUM_ARGS ) throw "wrong number of arguments.";

  const Value &op_v  = args[ ARG_OP   ];
  const Value &left  = args[ ARG_LEFT ];
  const Value &right = args[ ARG_RIGHT ];

  if( !op_v.isString() ) throw "'op' argument must be string";

  if( left.isOutput() || right.isOutput() )
  {
    const String global_name =
      scope->genGlobalName(
           op_v.getString()
         + "@" + toString( loc->withoutFilename() )
         + "#" + toString( scope->getNr() )
         );
    ret = Value( new Node( getGC(), global_name, n_args, args, getSO() ) );
    return true;
  }

  const BOP op = getBOP( op_v.getString() );

  bop( ret, left, op, right );
  return true;
}

CountPtr< Function::Args > binaryOp::genDeclArgs( void )
{
  CountPtr< Args > args = new Args( NUM_ARGS );
  args->at( ARG_LEFT  ) = Arg( String::Static( "left"  ) );
  args->at( ARG_OP    ) = Arg( String::Static( "op"    ) );
  args->at( ARG_RIGHT ) = Arg( String::Static( "right" ) );
  return args;
}

binaryOp::Node::Node( GarbageCollector *_gc, const String &global_name, index_t n_args, const Value *args, const RPGML::SharedObject *so )
: RPGML::Node( _gc, global_name, n_args, args, so )
{}

binaryOp::Node::~Node( void )
{}

void binaryOp::Node::gc_clear( void )
{
  RPGML::Node::gc_clear();
}

void binaryOp::Node::gc_getChildren( Children &children ) const
{
  RPGML::Node::gc_getChildren( children );
}

} // namespace RPGML

RPGML_CREATE_FUNCTION( binaryOp )
RPGML_CREATE_NODE( binaryOp )


#include "UnaryOp.h"

#include "String.h"
#include "ParserEnums.h"

namespace RPGML {

UnaryOp::UnaryOp( GarbageCollector *_gc, Map *parent )
: Function( _gc, parent, genDeclArgs() )
{}

UnaryOp::~UnaryOp( void )
{}

namespace UnaryOp_impl {

  static inline
  Value minus( const Value &x )
  {
    switch( x.getType().getEnum() )
    {
      case Type::BOOL : return Value( -int( x.getBool () ) );
      case Type::INT  : return Value( -     x.getInt  ()   );
      case Type::FLOAT: return Value( -     x.getFloat()   );
      default:
        throw "Invalid type for unary '-'";
    }
  }

  static inline
  Value plus( const Value &x )
  {
    switch( x.getType().getEnum() )
    {
      case Type::BOOL :
      case Type::INT  :
      case Type::FLOAT:
        return x;
      default:
        throw "Invalid type for unary '+'";
    }
  }

  static inline
  Value log_not( const Value &x )
  {
    switch( x.getType().getEnum() )
    {
      case Type::BOOL : return Value( bool( !x.getBool () ) );
      case Type::INT  : return Value( bool( !x.getInt  () ) );
      case Type::FLOAT: return Value( bool( !x.getFloat() ) );
      default:
        throw "Invalid type for unary '!'";
    }
  }

  static inline
  Value bit_not( const Value &x )
  {
    switch( x.getType().getEnum() )
    {
      case Type::BOOL : return Value( ~x.getBool () );
      case Type::INT  : return Value( ~x.getInt  () );
      default:
        throw "Invalid type for unary '~'";
    }
  }

} // namespace UnaryOp_impl

bool UnaryOp::call_impl( Scope *, Value &ret, index_t n_args, const Value *args, index_t )
{
  if( n_args != 2 ) throw "UnaryOp requires 2 arguments.";

  if( !args[ 0 ].isInt() ) throw "First argument must be int";

  const UOP op = UOP( args[ 0 ].getInt() );
  const Value &x = args[ 1 ];

  using namespace UnaryOp_impl;

  switch( op )
  {
    case UOP_MINUS  : ret = minus  ( x ); break;
    case UOP_PLUS   : ret = plus   ( x ); break;
    case UOP_LOG_NOT: ret = log_not( x ); break;
    case UOP_BIT_NOT: ret = bit_not( x ); break;
    default:
      throw "Invalid op";
  }

  return true;
}

CountPtr< Function::Args > UnaryOp::genDeclArgs( void )
{
  CountPtr< Args > args = new Args( 2 );
  args->at( 0 ) = Arg( String::Static( "op" ) );
  args->at( 1 ) = Arg( String::Static( "x" ) );
  return args;
}

} // namespace RPGML


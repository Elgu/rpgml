#include "RPGML_Function_unaryOp.h"

#include "String.h"
#include "Scope.h"
#include "Node.h"

namespace RPGML {

Function_unaryOp::Function_unaryOp( GarbageCollector *_gc, Frame *parent, const SharedObject *so )
: Function( _gc, parent, genDeclArgs(), so )
{}

Function_unaryOp::~Function_unaryOp( void )
{}

namespace unaryOp_impl {

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

} // namespace unaryOp_impl

bool Function_unaryOp::call_impl( const Location *loc, index_t recursion_depth, Scope *scope, Value &ret, index_t n_args, const Value *args )
{
  using namespace unaryOp_impl;

  if( n_args != 2 ) throw "Function_unaryOp requires 2 arguments.";

  const Value &op_v = args[ ARG_OP ];
  const Value &in   = args[ ARG_IN  ];

  if( !op_v.isString() ) throw "argument 'op' must be string";

  if( in.isOutput() )
  {
    CountPtr< Node > node( scope->create_Node( loc, recursion_depth+1, String::Static( "UnaryOp" ) ) );
    node->getInput( "in" )->connect( in.getOutput() );
    node->getParam( "op" )->set( op_v );
    ret = Value( node->getOutput( "out" ) );
    return true;
  }

  const UOP op = getUOP( op_v.getString() );

  switch( op )
  {
    case UOP_MINUS  : ret = minus  ( in ); break;
    case UOP_PLUS   : ret = plus   ( in ); break;
    case UOP_LOG_NOT: ret = log_not( in ); break;
    case UOP_BIT_NOT: ret = bit_not( in ); break;
    default:
      throw "Invalid op";
  }

  return true;
}

CountPtr< Function::Args > Function_unaryOp::genDeclArgs( void )
{
  CountPtr< Args > args = new Args( NUM_ARGS );
  args->at( ARG_OP ) = Arg( String::Static( "op" ) );
  args->at( ARG_IN ) = Arg( String::Static( "in" ) );
  return args;
}

} // namespace RPGML

RPGML_CREATE_FUNCTION( unaryOp )


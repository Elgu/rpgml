#include "RPGML_unaryOp.h"

#include "String.h"
#include "Scope.h"
#include "ParserEnums.h"

namespace RPGML {

unaryOp::unaryOp( GarbageCollector *_gc, Frame *parent, const SharedObject *so )
: Function( _gc, parent, genDeclArgs(), so )
{}

unaryOp::~unaryOp( void )
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

bool unaryOp::call_impl( const Location *loc, Scope *scope, Value &ret, index_t n_args, const Value *args, index_t )
{
  using namespace unaryOp_impl;

  if( n_args != 2 ) throw "unaryOp requires 2 arguments.";

  const Value &op_v = args[ ARG_OP ];
  const Value &x_v  = args[ ARG_X  ];

  if( !op_v.isString() ) throw "First argument must be string";

  if( x_v.isOutput() )
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

  const UOP op = getUOP( op_v.getString() );
  const Value &x = x_v;

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

CountPtr< Function::Args > unaryOp::genDeclArgs( void )
{
  CountPtr< Args > args = new Args( NUM_ARGS );
  args->at( ARG_OP ) = Arg( String::Static( "op" ) );
  args->at( ARG_X  ) = Arg( String::Static( "x" ) );
  return args;
}

unaryOp::Node::Node( GarbageCollector *_gc, const String &global_name, index_t n_args, const Value *args, const RPGML::SharedObject *so )
: RPGML::Node( _gc, global_name, n_args, args, so )
{}

unaryOp::Node::~Node( void )
{}

void unaryOp::Node::gc_clear( void )
{
  RPGML::Node::gc_clear();
}

void unaryOp::Node::gc_getChildren( Children &children ) const
{
  RPGML::Node::gc_getChildren( children );
}

} // namespace RPGML

RPGML_CREATE_FUNCTION( unaryOp )
RPGML_CREATE_NODE( unaryOp )


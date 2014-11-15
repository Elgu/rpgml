#include "RPGML_unaryOp.h"

#include "String.h"
#include "Scope.h"

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

bool unaryOp::call_impl( const Location *loc, index_t, Scope *scope, Value &ret, index_t n_args, const Value *args )
{
  using namespace unaryOp_impl;

  if( n_args != 2 ) throw "unaryOp requires 2 arguments.";

  const Value &op_v = args[ ARG_OP ];
  const Value &in   = args[ ARG_IN  ];

  if( !op_v.isString() ) throw "argument 'op' must be string";

  if( in.isOutput() )
  {
    const String global_name =
      scope->genGlobalName(
           op_v.getString()
         + "@" + toString( loc->withoutFilename() )
         + "#" + toString( scope->getNr() )
         );
    CountPtr< Node > node( new Node( getGC(), global_name, getSO() ) );
    node->getInput( "in" )->connect( in.getOutput() );
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

CountPtr< Function::Args > unaryOp::genDeclArgs( void )
{
  CountPtr< Args > args = new Args( NUM_ARGS );
  args->at( ARG_OP ) = Arg( String::Static( "op" ) );
  args->at( ARG_IN ) = Arg( String::Static( "in" ) );
  return args;
}

unaryOp::Node::Node( GarbageCollector *_gc, const String &identifier, const RPGML::SharedObject *so )
: RPGML::Node( _gc, identifier, so )
, m_op( UOP_PLUS )
{
  setNumInputs( NUM_INPUTS );
  getInput( INPUT_IN )->setIdentifier( String::Static( "in" ) );

  setNumOutputs( NUM_OUTPUTS );
  getOutput( OUTPUT_OUT )->setIdentifier( String::Static( "out" ) );

  push_back( String::Static( "in"  ), Value( getInput( INPUT_IN ) ) );
  push_back( String::Static( "out" ), Value( getOutput( OUTPUT_OUT ) ) );
  push_back( String::Static( "op" ), Value( new NodeParam< Node >( getGC(), this, String::Static( "op" ), &Node::set_op ) ) );
}

unaryOp::Node::~Node( void )
{}

bool unaryOp::Node::set_op( const Value &value, index_t )
{
  if( value.getType() != Type::String() )
  {
    throw "Param 'op' must be set with string.";
  }

  try
  {
    m_op = getUOP( value.getString() );
  }
  catch( const char * )
  {
    throw "Invalid op";
  }

  return true;
}

bool unaryOp::Node::tick( void )
{


  return true;
}

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


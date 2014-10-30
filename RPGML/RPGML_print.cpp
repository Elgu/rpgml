#include "RPGML_print.h"

#include "String.h"
#include "ParserEnums.h"
#include "Scope.h"

#include <iostream>

namespace RPGML {

print::print( GarbageCollector *_gc, Frame *parent, const SharedObject *so )
: Function( _gc, parent, genDeclArgs(), so )
{}

print::~print( void )
{}

bool print::call_impl( const Location *loc, index_t, Scope *scope, Value &ret, index_t n_args, const Value *args )
{
  if( n_args != 1 ) throw "print requires 1 argument.";
  const Value &in = args[ 0 ];

  switch( in.getType().getEnum() )
  {
    case Type::BOOL  : std::cout << in.getBool  (); break;
    case Type::INT   : std::cout << in.getInt   (); break;
    case Type::FLOAT : std::cout << in.getFloat (); break;
    case Type::STRING: std::cout << in.getString(); break;
    case Type::OUTPUT:
      {
        const String global_name =
          scope->genGlobalName(
               "print@" + toString( loc->withoutFilename() )
             + "#" + toString( scope->getNr() )
             );
        CountPtr< Node > node( new Node( getGC(), global_name, getSO() ) );
        node->getInput( "in" )->connect( in.getOutput() );

        ret = Value( true );
        return true;
      }
      break;
    default:
      std::cout << in;
      break;
    //  throw "Invalid type for print argument 'str'";
  }

  ret = Value( true );
  return true;
}

CountPtr< Function::Args > print::genDeclArgs( void )
{
  CountPtr< Args > args = new Args( 1 );
  args->at( 0 ) = Arg( String::Static( "in" ) );
  return args;
}

print::Node::Node( GarbageCollector *_gc, const String &identifier, const RPGML::SharedObject *so )
: RPGML::Node( _gc, String::Static( "Print" ), identifier, so )
{
  setNumInputs( NUM_INPUTS );
  getInput( INPUT_IN )->setIdentifier( String::Static( "in" ) );

  setNumOutputs( NUM_OUTPUTS );
}

print::Node::~Node( void )
{}

bool print::Node::tick( void )
{
  return true;
}

void print::Node::gc_clear( void )
{
  RPGML::Node::gc_clear();
}

void print::Node::gc_getChildren( Children &children ) const
{
  RPGML::Node::gc_getChildren( children );
}

} // namespace RPGML

RPGML_CREATE_FUNCTION( print )
RPGML_CREATE_NODE( print )

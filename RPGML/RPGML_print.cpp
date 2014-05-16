#include "RPGML_print.h"

#include "String.h"
#include "ParserEnums.h"

#include <iostream>

namespace RPGML {

print::print( GarbageCollector *_gc, Frame *parent, const SharedObject *so )
: Function( _gc, parent, genDeclArgs(), so )
{}

print::~print( void )
{}

bool print::call_impl( const Location *, Scope *, Value &ret, index_t n_args, const Value *args, index_t )
{
  if( n_args != 1 ) throw "print requires 1 argument.";
  const Value &str = args[ 0 ];

  switch( str.getType().getEnum() )
  {
    case Type::BOOL  : std::cout << str.getBool  (); break;
    case Type::INT   : std::cout << str.getInt   (); break;
    case Type::FLOAT : std::cout << str.getFloat (); break;
    case Type::STRING: std::cout << str.getString(); break;
    default:
      std::cout << str;
      break;
    //  throw "Invalid type for print argument 'str'";
  }

  ret = Value( true );
  return true;
}

CountPtr< Function::Args > print::genDeclArgs( void )
{
  CountPtr< Args > args = new Args( 1 );
  args->at( 0 ) = Arg( String::Static( "str" ) );
  return args;
}

print::Node::Node( GarbageCollector *_gc, const String &global_name, index_t n_args, const Value *args, const RPGML::SharedObject *so )
: RPGML::Node( _gc, global_name, n_args, args, so )
{}

print::Node::~Node( void )
{}

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

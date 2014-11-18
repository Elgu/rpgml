#include "RPGML_Function_print.h"

#include "String.h"
#include "ParserEnums.h"
#include "Scope.h"
#include "Node.h"

#include <iostream>

namespace RPGML {

Function_print::Function_print( GarbageCollector *_gc, Frame *parent, const SharedObject *so )
: Function( _gc, parent, genDeclArgs(), so )
{}

Function_print::~Function_print( void )
{}

bool Function_print::call_impl( const Location *loc, index_t recursion_depth, Scope *scope, Value &ret, index_t n_args, const Value *args )
{
  if( n_args != 1 ) throw "Function_print requires 1 argument.";
  const Value &in = args[ 0 ];

  switch( in.getType().getEnum() )
  {
    case Type::BOOL  : std::cout << in.getBool  (); break;
    case Type::INT   : std::cout << in.getInt   (); break;
    case Type::FLOAT : std::cout << in.getFloat (); break;
    case Type::STRING: std::cout << in.getString(); break;
    case Type::OUTPUT:
      {
        CountPtr< Node > node( scope->create_Node( loc, recursion_depth+1, String::Static( "Print" ) ) );

        scope->toOutput( loc, recursion_depth+1, in )->connect( node->getInput( "in" ) );

        ret = Value( String::Static() );
        return true;
      }
      break;
    default:
      std::cout << in;
      break;
    //  throw "Invalid type for Function_print argument 'str'";
  }

  ret = Value( true );
  return true;
}

CountPtr< Function::Args > Function_print::genDeclArgs( void )
{
  CountPtr< Args > args = new Args( 1 );
  args->at( 0 ) = Arg( String::Static( "in" ) );
  return args;
}

} // namespace RPGML

RPGML_CREATE_FUNCTION( print )

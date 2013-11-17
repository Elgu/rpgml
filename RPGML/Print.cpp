#include "Print.h"

#include "String.h"
#include "ParserEnums.h"

#include <iostream>

namespace RPGML {

Print::Print( GarbageCollector *_gc, Map *parent )
: Function( _gc, parent, genDeclArgs() )
{}

Print::~Print( void )
{}

bool Print::call_impl( Scope *, Value &ret, index_t n_args, const Value *args, index_t )
{
  if( n_args != 1 ) throw "Print requires 1 argument.";
  const Value &str = args[ 0 ];

  switch( str.getType().getEnum() )
  {
    case Type::BOOL  : std::cout << str.getBool  (); break;
    case Type::INT   : std::cout << str.getInt   (); break;
    case Type::FLOAT : std::cout << str.getFloat (); break;
    case Type::STRING: std::cout << str.getString()->get(); break;
    default:
      throw "Invalid type for Print argument 'str'";
  }

  ret = true;
  return true;
}

CountPtr< Function::Args > Print::genDeclArgs( void )
{
  CountPtr< Args > args = new Args( 1 );
  args->at( 0 ) = Arg( new String( "str" ) );
  return args;
}

} // namespace RPGML



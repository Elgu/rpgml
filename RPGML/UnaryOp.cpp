#include "UnaryOp.h"

namespace RPGML {

UnaryOp::UnaryOp( GarbageCollector *gc, Map *parent )
: Function( gc, parent, genDeclArgs() )
{}

UnaryOp::~UnaryOp( void )
{}

bool UnaryOp::call_impl( Scope *, Value &ret, index_t n_args, const Value *args, index_t )
{
}

CountPtr< Args > UnaryOp::genDeclArgs( void )
{
  CountPtr< Args > args = new Args( 3 );
  args->at( 0 ) = new Arg( new String( "left" ) );
  args->at( 1 ) = new Arg( new String( "op" ) );
  args->at( 2 ) = new Arg( new String( "right" ) );
  return args;
}

} // namespace RPGML


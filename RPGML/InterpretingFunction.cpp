#include "InterpretingFunction.h"

#include "InterpretingASTVisitor.h"

namespace RPGML {

InterpretingFunction::InterpretingFunction( GarbageCollector *_gc, Map *parent, const Args *decl_args, const AST::CompoundStatement *body )
: Function( _gc, parent, decl_args )
, m_body( body )
{}

InterpretingFunction::~InterpretingFunction( void )
{}

bool InterpretingFunction::call_impl( Scope *scope, Value &ret, index_t, const Value *, index_t recursion_depth )
{
  InterpretingASTVisitor interpreter( scope, recursion_depth+1 );
  if( !m_body->invite( &interpreter ) ) return false;

  if( !interpreter.return_encountered )
  {
    throw "No return statement";
  }

  ret.swap( interpreter.return_value );
  return true;
}

} // namespace RPGML


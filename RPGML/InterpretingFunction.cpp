#include "InterpretingFunction.h"

#include "InterpretingASTVisitor.h"
#include "Frame.h"
#include "Scope.h"

namespace RPGML {

InterpretingFunction::InterpretingFunction( GarbageCollector *_gc, Frame *parent, const String &name, const Args *decl_args, const AST::CompoundStatement *body )
: Function( _gc, parent, decl_args )
, m_body( body )
, m_name( name )
{}

InterpretingFunction::~InterpretingFunction( void )
{}

bool InterpretingFunction::call_impl( const Location *, index_t recursion_depth, Scope *scope, Value &ret, index_t, const Value * )
{
  CountPtr< Frame > frame = new Frame( scope->getGC(), scope->getCurr() );
  frame->setThis( true );

  Scope::EnterLeaveGuard guard( scope, frame );

  InterpretingASTVisitor interpreter( scope, recursion_depth+1 );
  if( !m_body->invite( &interpreter ) ) return false;

  if( !interpreter.get_return_encountered() )
  {
    throw "No return statement";
  }

  ret = interpreter.get_return_value();
  return true;
}

} // namespace RPGML


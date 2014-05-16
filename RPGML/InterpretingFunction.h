#ifndef RPGML_InterpretingFunction_h
#define RPGML_InterpretingFunction_h

#include "Function.h"
#include "AST.h"

namespace RPGML {

class Scope;

class InterpretingFunction : public Function
{
public:
  InterpretingFunction( GarbageCollector *gc, Frame *parent, const Args *decl_args, const AST::CompoundStatement *body );
  virtual ~InterpretingFunction( void );

  virtual bool call_impl( const Location *loc, Scope *scope, Value &ret, index_t n_args, const Value *args, index_t recursion_depth );

private:
  const CountPtr< const AST::CompoundStatement > m_body;
};


} // namespace RPGML

#endif


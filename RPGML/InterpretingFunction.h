#ifndef RPGML_InterpretingFunction_h
#define RPGML_InterpretingFunction_h

#include "Function.h"
#include "AST.h"

namespace RPGML {

class Scope;

class InterpretingFunction : public Function
{
public:
  InterpretingFunction( GarbageCollector *gc, Frame *parent, const String &name, const Args *decl_args, const AST::CompoundStatement *body );
  virtual ~InterpretingFunction( void );

  virtual bool call_impl( const Location *loc, index_t recursion_depth, Scope *scope, Value &ret, index_t n_args, const Value *args );
  virtual const char *getName( void ) const { return m_name.get(); }

private:
  const CountPtr< const AST::CompoundStatement > m_body;
  const String m_name;
};


} // namespace RPGML

#endif


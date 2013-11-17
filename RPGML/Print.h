#ifndef RPGML_Print_h
#define RPGML_Print_h

#include "Function.h"

namespace RPGML {

class Print : public Function
{
public:
  Print( GarbageCollector *gc, Map *parent );
  virtual ~Print( void );

  virtual bool call_impl( Scope *scope, Value &ret, index_t n_args, const Value *args, index_t recursion_depth );
private:
  static
  CountPtr< Args > genDeclArgs( void );
};

} // namespace RPGML

#endif


#ifndef RPGML_UnaryOp_h
#define RPGML_UnaryOp_h

#include "Function.h"

namespace RPGML {

class UnaryOp : public Function
{
public:
  UnaryOp( GarbageCollector *gc, Map *parent );
  virtual ~UnaryOp( void );

  virtual bool call_impl( Scope *scope, Value &ret, index_t n_args, const Value *args, index_t recursion_depth );
private:
  static
  CountPtr< Args > genDeclArgs( void );
};

} // namespace RPGML

#endif


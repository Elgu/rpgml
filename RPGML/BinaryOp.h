#ifndef RPGML_BinaryOp_h
#define RPGML_BinaryOp_h

#include "Function.h"

namespace RPGML {

class BinaryOp : public Function
{
public:
  BinaryOp( GarbageCollector *gc, Map *parent );
  virtual ~BinaryOp( void );

  virtual bool call_impl( Scope *scope, Value &ret, index_t n_args, const Value *args, index_t recursion_depth );
private:
  static
  CountPtr< Args > genDeclArgs( void );
};

} // namespace RPGML

#endif


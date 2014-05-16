#ifndef RPGML_BinaryOp_h
#define RPGML_BinaryOp_h

#include "Function.h"
#include "Node.h"

namespace RPGML {

class BinaryOp : public Function
{
public:
  BinaryOp( GarbageCollector *gc, Map *parent );
  virtual ~BinaryOp( void );

  virtual bool call_impl( Scope *scope, Value &ret, index_t n_args, const Value *args, index_t recursion_depth );

private:
  enum ArgNr
  {
    ARG_LEFT ,
    ARG_OP   ,
    ARG_RIGHT,
    NUM_ARGS
  };

  static
  CountPtr< Args > genDeclArgs( void );
};

} // namespace RPGML

#endif


#ifndef RPGML_Function_binaryOp_h
#define RPGML_Function_binaryOp_h

#include "Function.h"

namespace RPGML {

class Function_binaryOp : public Function
{
public:
  Function_binaryOp( GarbageCollector *gc, Frame *parent, const SharedObject *so );
  virtual ~Function_binaryOp( void );

  virtual bool call_impl( const Location *loc, index_t recursion_depth, Scope *scope, Value &ret, index_t n_args, const Value *args );
  virtual const char *getName( void ) const { return "binaryOp"; }

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


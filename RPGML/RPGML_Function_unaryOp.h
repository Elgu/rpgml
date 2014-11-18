#ifndef RPGML_Function_unaryOp_h
#define RPGML_Function_unaryOp_h

#include "Function.h"
#include "ParserEnums.h"

namespace RPGML {

class Function_unaryOp : public Function
{
public:
  Function_unaryOp( GarbageCollector *gc, Frame *parent, const SharedObject *so );
  virtual ~Function_unaryOp( void );

  virtual bool call_impl( const Location *loc, index_t recursion_depth, Scope *scope, Value &ret, index_t n_args, const Value *args );

  virtual const char *getName( void ) const { return "unaryOp"; }

private:
  enum ArgNr
  {
    ARG_OP,
    ARG_IN,
    NUM_ARGS
  };

  static
  CountPtr< Args > genDeclArgs( void );
};

} // namespace RPGML

#endif


#ifndef RPGML_binaryOp_h
#define RPGML_binaryOp_h

#include "Function.h"
#include "Node.h"

namespace RPGML {

class binaryOp : public Function
{
public:
  binaryOp( GarbageCollector *gc, Frame *parent, const SharedObject *so );
  virtual ~binaryOp( void );

  virtual bool call_impl( const Location *loc, index_t recursion_depth, Scope *scope, Value &ret, index_t n_args, const Value *args );

  class Node : public RPGML::Node
  {
  public:
    Node( GarbageCollector *gc, const String &identifier, const SharedObject *so );
    virtual ~Node( void );

    virtual const char *getName( void ) const { return "binaryOp"; }

    virtual bool tick( void );

    virtual void gc_clear( void );
    virtual void gc_getChildren( Children &children ) const;
  private:
    enum Inputs
    {
      INPUT_LEFT ,
      INPUT_RIGHT,
      NUM_INPUTS
    };

    enum Outputs
    {
      OUTPUT_OUT,
      NUM_OUTPUTS
    };
  };

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


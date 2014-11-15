#ifndef RPGML_unaryOp_h
#define RPGML_unaryOp_h

#include "Function.h"
#include "Node.h"
#include "ParserEnums.h"

namespace RPGML {

class unaryOp : public Function
{
public:
  unaryOp( GarbageCollector *gc, Frame *parent, const SharedObject *so );
  virtual ~unaryOp( void );

  virtual bool call_impl( const Location *loc, index_t recursion_depth, Scope *scope, Value &ret, index_t n_args, const Value *args );

  class Node : public RPGML::Node
  {
  public:
    Node( GarbageCollector *gc, const String &identifier, const SharedObject *so );
    virtual ~Node( void );

    virtual const char *getName( void ) const { return "unaryOp"; }

    virtual bool tick( void );

    virtual void gc_clear( void );
    virtual void gc_getChildren( Children &children ) const;

    bool set_op( const Value &value, index_t );
  private:
    enum Inputs
    {
      INPUT_IN,
      NUM_INPUTS
    };

    enum Outputs
    {
      OUTPUT_OUT,
      NUM_OUTPUTS
    };

    enum Params
    {
      PARAM_OP,
      NUM_PARAMS
    };

    UOP m_op;
  };

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


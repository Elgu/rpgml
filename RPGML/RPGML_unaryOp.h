#ifndef RPGML_unaryOp_h
#define RPGML_unaryOp_h

#include "Function.h"
#include "Node.h"

namespace RPGML {

class unaryOp : public Function
{
public:
  unaryOp( GarbageCollector *gc, Frame *parent, const SharedObject *so );
  virtual ~unaryOp( void );

  virtual bool call_impl( const Location *loc, Scope *scope, Value &ret, index_t n_args, const Value *args, index_t recursion_depth );

  class Node : public RPGML::Node
  {
  public:
    Node( GarbageCollector *gc, const String &global_name, index_t n_args, const Value *args, const RPGML::SharedObject *so );
    virtual ~Node( void );

    virtual void gc_clear( void );
    virtual void gc_getChildren( Children &children ) const;
  };

private:
  enum ArgNr
  {
    ARG_OP,
    ARG_X ,
    NUM_ARGS
  };

  static
  CountPtr< Args > genDeclArgs( void );
};

} // namespace RPGML

#endif


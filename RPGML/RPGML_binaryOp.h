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


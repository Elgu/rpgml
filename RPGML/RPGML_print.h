#ifndef RPGML_print_h
#define RPGML_print_h

#include "Function.h"
#include "Node.h"

namespace RPGML {

class print : public Function
{
public:
  print( GarbageCollector *gc, Frame *parent, const SharedObject *so=0 );
  virtual ~print( void );

  virtual bool call_impl( const Location *loc, index_t recursion_depth, Scope *scope, Value &ret, index_t n_args, const Value *args );

  class Node : public RPGML::Node
  {
  public:
    Node( GarbageCollector *gc, const String &identifier, const SharedObject *so );
    virtual ~Node( void );

    virtual bool tick( void );

    virtual void gc_clear( void );
    virtual void gc_getChildren( Children &children ) const;
  private:
    enum Inputs
    {
      INPUT_IN,
      NUM_INPUTS
    };

    enum Outputs
    {
      NUM_OUTPUTS
    };
  };

private:
  static
  CountPtr< Args > genDeclArgs( void );
};

} // namespace RPGML

#endif


#ifndef RPGML_Node_BinaryOp_h
#define RPGML_Node_BinaryOp_h

#include "Node.h"
#include "ParserEnums.h"

namespace RPGML {

class BinaryOp : public Node
{
public:
  BinaryOp( GarbageCollector *gc, const String &identifier, const SharedObject *so );
  virtual ~BinaryOp( void );

  virtual const char *getName( void ) const { return "BinaryOp"; }

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

} // namespace RPGML

#endif


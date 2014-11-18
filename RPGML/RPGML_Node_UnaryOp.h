#ifndef RPGML_Node_UnaryOp_h
#define RPGML_Node_UnaryOp_h

#include "Node.h"
#include "ParserEnums.h"

namespace RPGML {

class UnaryOp : public Node
{
public:
  UnaryOp( GarbageCollector *gc, const String &identifier, const SharedObject *so );
  virtual ~UnaryOp( void );

  virtual const char *getName( void ) const { return "UnaryOp"; }

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

} // namespace RPGML

#endif


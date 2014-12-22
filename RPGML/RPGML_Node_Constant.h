#ifndef RPGML_Node_Constant_h
#define RPGML_Node_Constant_h

#include "Node.h"

namespace RPGML {

class Constant : public Node
{
public:
  Constant( GarbageCollector *gc, const String &identifier, const SharedObject *so );
  virtual ~Constant( void );

  virtual const char *getName( void ) const;

  virtual bool tick( void );

  virtual void gc_clear( void );
  virtual void gc_getChildren( Children &children ) const;

  void set_value( const Value &value, index_t );

private:
  typedef NodeParam< Constant > NParam;

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
    PARAM_VALUE,
    NUM_PARAMS
  };

  Value m_value;
  bool m_first;
};

} // namespace RPGML

#endif


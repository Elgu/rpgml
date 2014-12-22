#ifndef RPGML_Node_TEMPLATE_h
#define RPGML_Node_TEMPLATE_h

#include "Node.h"

namespace RPGML {

class TEMPLATE : public Node
{
public:
  EXCEPTION_BASE( Exception );

  TEMPLATE( GarbageCollector *gc, const String &identifier, const SharedObject *so );
  virtual ~TEMPLATE( void );

  virtual const char *getName( void ) const;

  virtual bool tick( void );

  virtual void gc_clear( void );
  virtual void gc_getChildren( Children &children ) const;

  void set_foo( const Value &value, index_t );

private:
  typedef NodeParam< TEMPLATE > NParam;

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
    PARAM_FOO,
    NUM_PARAMS
  };

  String m_foo;
};

} // namespace RPGML

#endif


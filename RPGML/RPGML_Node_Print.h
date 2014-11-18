#ifndef RPGML_Node_Print_h
#define RPGML_Node_Print_h

#include "Node.h"

namespace RPGML {

class Print : public RPGML::Node
{
public:
  Print( GarbageCollector *gc, const String &identifier, const SharedObject *so );
  virtual ~Print( void );

  virtual const char *getName( void ) const { return "Print"; }

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

} // namespace RPGML

#endif


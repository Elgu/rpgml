#ifndef RPGML_Node_Counter_h
#define RPGML_Node_Counter_h

#include <RPGML/Node.h>

namespace RPGML {

class Counter : public Node
{
public:
  EXCEPTION_BASE( Exception );

  Counter( GarbageCollector *gc, const String &identifier, const SharedObject *so );
  virtual ~Counter( void );

  virtual const char *getName( void ) const;

  virtual bool tick( void );

  virtual void gc_clear( void );
  virtual void gc_getChildren( Children &children ) const;

  void set_start( const Value &value, index_t );
  void set_step ( const Value &value, index_t );

private:
  typedef NodeParam< Counter > NParam;

  enum Inputs
  {
    NUM_INPUTS
  };

  enum Outputs
  {
    OUTPUT_OUT,
    NUM_OUTPUTS
  };

  enum Params
  {
    PARAM_START,
    PARAM_STEP ,
    NUM_PARAMS
  };

  int m_start;
  int m_step;
  int m_count;
};

} // namespace RPGML

#endif


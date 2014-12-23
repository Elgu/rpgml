#ifndef RPGML_Node_UnaryOp_h
#define RPGML_Node_UnaryOp_h

#include <RPGML/Node.h>
#include <RPGML/ParserEnums.h>

namespace RPGML {

class UnaryOp : public Node
{
public:
  EXCEPTION_BASE( Exception );

  UnaryOp( GarbageCollector *gc, const String &identifier, const SharedObject *so );
  virtual ~UnaryOp( void );

  virtual const char *getName( void ) const;

  virtual bool tick( void );

  virtual void gc_clear( void );
  virtual void gc_getChildren( Children &children ) const;

  void set_op( const Value &value, index_t );
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

  typedef NodeParam< UnaryOp > NParam;

  template< UOP OP >
  bool tick1( void );

  template< UOP OP >
  bool tick1_int( void );

  template< UOP OP, class T >
  bool tick2( const ArrayBase *in_base );

  UOP m_op;
};

} // namespace RPGML

#endif


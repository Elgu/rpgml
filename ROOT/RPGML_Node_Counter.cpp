#include "RPGML_Node_Counter.h"

#include <algorithm>

using namespace std;

namespace RPGML {

Counter::Counter( GarbageCollector *_gc, const String &identifier, const RPGML::SharedObject *so )
: Node( _gc, identifier, so, NUM_INPUTS, NUM_OUTPUTS, NUM_PARAMS )
{
  DEFINE_OUTPUT_INIT( OUTPUT_OUT, "out", int, 0 );
  DEFINE_PARAM ( PARAM_START , "start", Counter::set_start );
  DEFINE_PARAM ( PARAM_STEP  , "step" , Counter::set_step  );
}

Counter::~Counter( void )
{}

const char *Counter::getName( void ) const
{
  return "Counter";
}

void Counter::set_start( const Value &value, index_t )
{
  try
  {
    m_start = value.to( Type::Int() );
  }
  catch( const Value::Exception &e )
  {
    throw Exception()
      << "Could not set parameter 'start'"
      << ": " << e.what()
      ;
  }
}

void Counter::set_step( const Value &value, index_t )
{
  try
  {
    m_step = value.to( Type::Int() );
  }
  catch( const Value::Exception &e )
  {
    throw Exception()
      << "Could not set parameter 'step'"
      << ": " << e.what()
      ;
  }
}

bool Counter::tick( void )
{
  Array< int, 0 > *out = 0;
  if( !getOutput( OUTPUT_OUT )->getAs( out ) ) throw Exception( "Could not getAs() 'out'" );

  (**out) = m_start + m_count * m_step;
  ++m_count;
  return true;
}

void Counter::gc_clear( void )
{
  Node::gc_clear();
}

void Counter::gc_getChildren( Children &children ) const
{
  Node::gc_getChildren( children );
}

} // namespace RPGML

RPGML_CREATE_NODE( Counter )


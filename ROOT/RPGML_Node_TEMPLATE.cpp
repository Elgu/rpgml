#include "RPGML_Node_TEMPLATE.h"

#include <algorithm>

using namespace std;

namespace RPGML {

TEMPLATE::TEMPLATE( GarbageCollector *_gc, const String &identifier, const RPGML::SharedObject *so )
: Node( _gc, identifier, so, NUM_INPUTS, NUM_OUTPUTS, NUM_PARAMS )
{
  DEFINE_INPUT ( INPUT_IN  , "in"  );
  DEFINE_OUTPUT( OUTPUT_OUT, "out" );
  DEFINE_PARAM ( PARAM_FOO , "foo", TEMPLATE::set_foo );
}

TEMPLATE::~TEMPLATE( void )
{}

const char *TEMPLATE::getName( void ) const
{
  return "TEMPLATE";
}

void TEMPLATE::set_foo( const Value &value, index_t )
{
  if( value.getType() != Type::String() )
  {
    throw Exception( "Param 'foo' must be set with string." );
  }

  m_foo = value.getString();
}

bool TEMPLATE::tick( void )
{
  return true;
}

void TEMPLATE::gc_clear( void )
{
  Node::gc_clear();
}

void TEMPLATE::gc_getChildren( Children &children ) const
{
  Node::gc_getChildren( children );
}

} // namespace RPGML

RPGML_CREATE_NODE( TEMPLATE )


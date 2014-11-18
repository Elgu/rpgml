#include "RPGML_Node_Print.h"

namespace RPGML {

Print::Print( GarbageCollector *_gc, const String &identifier, const RPGML::SharedObject *so )
: RPGML::Node( _gc, identifier, so )
{
  setNumInputs( NUM_INPUTS );
  getInput( INPUT_IN )->setIdentifier( String::Static( "in" ) );

  setNumOutputs( NUM_OUTPUTS );
}

Print::~Print( void )
{}

bool Print::tick( void )
{
  return true;
}

void Print::gc_clear( void )
{
  RPGML::Node::gc_clear();
}

void Print::gc_getChildren( Children &children ) const
{
  RPGML::Node::gc_getChildren( children );
}

} // namespace RPGML

RPGML_CREATE_NODE( Print )


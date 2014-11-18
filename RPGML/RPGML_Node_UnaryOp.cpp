#include "RPGML_Node_UnaryOp.h"

namespace RPGML {

UnaryOp::UnaryOp( GarbageCollector *_gc, const String &identifier, const RPGML::SharedObject *so )
: Node( _gc, identifier, so )
, m_op( UOP_PLUS )
{
  setNumInputs( NUM_INPUTS );
  getInput( INPUT_IN )->setIdentifier( String::Static( "in" ) );

  setNumOutputs( NUM_OUTPUTS );
  getOutput( OUTPUT_OUT )->setIdentifier( String::Static( "out" ) );

  push_back( String::Static( "in"  ), Value( getInput( INPUT_IN ) ) );
  push_back( String::Static( "out" ), Value( getOutput( OUTPUT_OUT ) ) );
  push_back( String::Static( "op" ), Value( new NodeParam< UnaryOp >( getGC(), this, String::Static( "op" ), &UnaryOp::set_op ) ) );
}

UnaryOp::~UnaryOp( void )
{}

bool UnaryOp::set_op( const Value &value, index_t )
{
  if( value.getType() != Type::String() )
  {
    throw "Param 'op' must be set with string.";
  }

  try
  {
    m_op = getUOP( value.getString() );
  }
  catch( const char * )
  {
    throw "Invalid op";
  }

  return true;
}

bool UnaryOp::tick( void )
{


  return true;
}

void UnaryOp::gc_clear( void )
{
  Node::gc_clear();
}

void UnaryOp::gc_getChildren( Children &children ) const
{
  Node::gc_getChildren( children );
}

} // namespace RPGML

RPGML_CREATE_NODE( UnaryOp )


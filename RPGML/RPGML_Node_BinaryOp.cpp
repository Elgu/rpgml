#include "RPGML_Node_BinaryOp.h"

namespace RPGML {

BinaryOp::BinaryOp( GarbageCollector *_gc, const String &identifier, const RPGML::SharedObject *so )
: Node( _gc, identifier, so )
{
  setNumInputs( NUM_INPUTS );
  setNumOutputs( NUM_OUTPUTS );

  getInput( INPUT_LEFT  )->setIdentifier( String::Static( "left"  ) );
  getInput( INPUT_RIGHT )->setIdentifier( String::Static( "right" ) );
  getOutput( OUTPUT_OUT )->setIdentifier( String::Static( "out" ) );

  this->reserve( NUM_INPUTS + NUM_OUTPUTS );
  this->push_back( String::Static( "left"  ), Value( getInput( INPUT_LEFT  ) ) );
  this->push_back( String::Static( "op"    ), Value( String::Static( "undefined" ) ) );
  this->push_back( String::Static( "right" ), Value( getInput( INPUT_RIGHT ) ) );

  this->push_back( String::Static( "out"   ), Value( getOutput( OUTPUT_OUT ) ) );
}

BinaryOp::~BinaryOp( void )
{}

bool BinaryOp::tick( void )
{
  return true;
}

void BinaryOp::gc_clear( void )
{
  Node::gc_clear();
}

void BinaryOp::gc_getChildren( Children &children ) const
{
  Node::gc_getChildren( children );
}


} // namespace RPGML

RPGML_CREATE_NODE( BinaryOp )


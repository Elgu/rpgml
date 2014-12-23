#include "RPGML_Node_Constant.h"

#include <algorithm>

using namespace std;

namespace RPGML {

Constant::Constant( GarbageCollector *_gc, const String &identifier, const RPGML::SharedObject *so )
: Node( _gc, identifier, so, NUM_INPUTS, NUM_OUTPUTS, NUM_PARAMS )
, m_first( true )
{
  DEFINE_INPUT ( INPUT_IN  , "in"  );
  DEFINE_OUTPUT( OUTPUT_OUT, "out" );
  DEFINE_PARAM ( PARAM_VALUE , "value", Constant::set_value );
}

Constant::~Constant( void )
{}

const char *Constant::getName( void ) const
{
  return "Constant";
}

void Constant::set_value( const Value &value, index_t )
{
  if( !value.getType().isPrimitive() )
  {
    throw Exception()
      << "Param 'value' must be set with a primitive type like bool, integers, floats or String, is '" << value.getTypeName() << "'"
      ;
  }

  m_value = value;
}

bool Constant::tick( void )
{
  if( m_first )
  {
    CountPtr< ArrayBase > out;

    switch( m_value.getType().getEnum() )
    {
      case Type::BOOL  : out.reset( new Array< bool    , 0 >( getGC() ) ); break;
//      case Type::UINT8 : out.reset( new Array< uint8_t , 0 >( getGC() ) ); break;
//      case Type::INT8  : out.reset( new Array< int8_t  , 0 >( getGC() ) ); break;
//      case Type::UINT16: out.reset( new Array< uint16_t, 0 >( getGC() ) ); break;
//      case Type::INT16 : out.reset( new Array< int16_t , 0 >( getGC() ) ); break;
//      case Type::UINT32: out.reset( new Array< uint32_t, 0 >( getGC() ) ); break;
      case Type::INT32 : out.reset( new Array< int32_t , 0 >( getGC() ) ); break;
//      case Type::UINT64: out.reset( new Array< uint64_t, 0 >( getGC() ) ); break;
//      case Type::INT64 : out.reset( new Array< int64_t , 0 >( getGC() ) ); break;
      case Type::FLOAT : out.reset( new Array< float   , 0 >( getGC() ) ); break;
//      case Type::DOUBLE: out.reset( new Array< double  , 0 >( getGC() ) ); break;
      case Type::STRING: out.reset( new Array< String  , 0 >( getGC() ) ); break;
      default:
        throw Exception() << "Parameter 'value' was not set or set to a non-primitive type.";
    }

    out->setValue( m_value );
    getOutput( OUTPUT_OUT )->setData( out );
    m_first = false;
  }
  return true;
}

void Constant::gc_clear( void )
{
  Node::gc_clear();
}

void Constant::gc_getChildren( Children &children ) const
{
  Node::gc_getChildren( children );
}

} // namespace RPGML

RPGML_CREATE_NODE( Constant )


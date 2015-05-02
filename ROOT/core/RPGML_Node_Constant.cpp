/* This file is part of RPGML.
 *
 * Copyright (c) 2014, Gunnar Payer, All rights reserved.
 *
 * RPGML is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 3.0 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library.
 */
#include "RPGML_Node_Constant.h"

#include <algorithm>

using namespace std;

namespace RPGML {
namespace core {

Constant::Constant( GarbageCollector *_gc, const String &identifier, const RPGML::SharedObject *so )
: Node( _gc, identifier, so, NUM_INPUTS, NUM_OUTPUTS, NUM_PARAMS )
, m_first( true )
{
  DEFINE_OUTPUT( OUTPUT_OUT, "out" );
  DEFINE_PARAM ( PARAM_VALUE , "value", Constant::set_value );
}

Constant::~Constant( void )
{}

const char *Constant::getName( void ) const
{
  return "core.Constant";
}

void Constant::set_value( const Value &value, index_t, int, const index_t* )
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
      case Type::BOOL  : out.reset( new Array< bool     >( getGC(), 0 ) ); break;
      case Type::UINT8 : out.reset( new Array< uint8_t  >( getGC(), 0 ) ); break;
      case Type::INT8  : out.reset( new Array< int8_t   >( getGC(), 0 ) ); break;
      case Type::UINT16: out.reset( new Array< uint16_t >( getGC(), 0 ) ); break;
      case Type::INT16 : out.reset( new Array< int16_t  >( getGC(), 0 ) ); break;
      case Type::UINT32: out.reset( new Array< uint32_t >( getGC(), 0 ) ); break;
      case Type::INT32 : out.reset( new Array< int32_t  >( getGC(), 0 ) ); break;
      case Type::UINT64: out.reset( new Array< uint64_t >( getGC(), 0 ) ); break;
      case Type::INT64 : out.reset( new Array< int64_t  >( getGC(), 0 ) ); break;
      case Type::FLOAT : out.reset( new Array< float    >( getGC(), 0 ) ); break;
      case Type::DOUBLE: out.reset( new Array< double   >( getGC(), 0 ) ); break;
      case Type::STRING: out.reset( new Array< String   >( getGC(), 0 ) ); break;

      default:
        throw Exception() << "Parameter 'value' was not set or set to a non-primitive type.";
    }

    out->setValue( m_value );
    getOutput( OUTPUT_OUT )->setData( out );
    getOutput( OUTPUT_OUT )->setChanged();
    m_first = false;
  }
  return true;
}

void Constant::gc_clear( void )
{
  Base::gc_clear();
}

void Constant::gc_getChildren( Children &children ) const
{
  Node::gc_getChildren( children );
}

} // namespace core
} // namespace RPGML

RPGML_CREATE_NODE( Constant, core:: )


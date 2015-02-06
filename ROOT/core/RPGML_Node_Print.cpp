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
#include "RPGML_Node_Print.h"

#include <iostream>

namespace RPGML {
namespace core {

Print::Print( GarbageCollector *_gc, const String &identifier, const RPGML::SharedObject *so )
: Node( _gc, identifier, so, NUM_INPUTS, NUM_OUTPUTS, NUM_PARAMS )
{
  DEFINE_INPUT( INPUT_IN, "in" );
  DEFINE_INPUT( INPUT_PREV, "prev" );
  DEFINE_OUTPUT( OUTPUT_NEXT, "next" );
}

Print::~Print( void )
{}

const char *Print::getName( void ) const
{
  return "core.Print";
}

template< class T >
bool Print::tick_scalar( const ArrayBase *in_base )
{
  const Array< T > *in = 0;
  if( !in_base->getAs( in ) ) throw Exception() << "Could not get 'in'";

  if( in->getDims() != 0 ) throw Exception() << "Only printing of 0-dimensional (scalar) data supported at the moment.";

  std::cout << (**in);
  return true;
}

bool Print::tick( void )
{
  const ArrayBase *const in_base = getInput( INPUT_IN )->getData();
  if( !in_base ) throw NotConnected( getInput( INPUT_IN ) );
  const index_t in_dims = in_base->getDims();

  if( in_dims > 4 )
  {
    throw Exception()
      << "Only dimensions up to 4 supported, has " << in_dims << " dimensions"
      ;
  }
  else if( in_dims == 0 )
  {
    switch( in_base->getType().getEnum() )
    {
      case Type::BOOL  : return tick_scalar< bool     >( in_base );
      case Type::UINT8 : return tick_scalar< uint8_t  >( in_base );
      case Type::INT8  : return tick_scalar< int8_t   >( in_base );
      case Type::UINT16: return tick_scalar< uint16_t >( in_base );
      case Type::INT16 : return tick_scalar< int16_t  >( in_base );
      case Type::UINT32: return tick_scalar< uint32_t >( in_base );
      case Type::INT32 : return tick_scalar< int32_t  >( in_base );
      case Type::UINT64: return tick_scalar< uint64_t >( in_base );
      case Type::INT64 : return tick_scalar< int64_t  >( in_base );
      case Type::FLOAT : return tick_scalar< float    >( in_base );
      case Type::DOUBLE: return tick_scalar< double   >( in_base );
      case Type::STRING: return tick_scalar< String   >( in_base );
      default:
        throw IncompatibleOutput( getInput( INPUT_IN ) );
    }
  }
  else
  {
    const Value array( const_cast< ArrayBase* >( in_base ) );
    array.print( std::cout );
  }

  return true;
}

void Print::gc_clear( void )
{
  RPGML::Node::gc_clear();
}

void Print::gc_getChildren( Children &children ) const
{
  Base::gc_getChildren( children );
}

} // namespace core
} // namespace RPGML

RPGML_CREATE_NODE( Print, core:: )


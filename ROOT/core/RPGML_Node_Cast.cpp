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
#include "RPGML_Node_Cast.h"

#include "RPGML_cast.h"

// RPGML_CXXFLAGS=
// RPGML_LDFLAGS=

#include <algorithm>

using namespace std;

namespace RPGML {
namespace core {

Cast::Cast( GarbageCollector *_gc, const String &identifier, const RPGML::SharedObject *so )
: Node( _gc, identifier, so, NUM_INPUTS, NUM_OUTPUTS, NUM_PARAMS )
{
  DEFINE_INPUT ( INPUT_IN  , "in"  );
  DEFINE_OUTPUT( OUTPUT_OUT, "out" );
  DEFINE_PARAM ( PARAM_TO , "to", Cast::set_to );
}

Cast::~Cast( void )
{}

const char *Cast::getName( void ) const
{
  return "core.Cast";
}

void Cast::gc_clear( void )
{
  Base::gc_clear();
}

void Cast::gc_getChildren( Children &children ) const
{
  Base::gc_getChildren( children );
}

void Cast::set_to( const Value &value, index_t, int, const index_t* )
{
  if( !value.isString() )
  {
    throw Exception() << "Param 'to' must be set with string, is " << value.getType();
  }

  const Type to_type( Type::getTypeEnum( value.getString() ) );

  if( !to_type.isPrimitive() )
  {
    throw Exception()
      << "Unsupported input type for cast, is " << to_type
      << ", op string is '" << value << "'"
      ;
  }

  m_to_type = to_type;
}

bool Cast::tick( void )
{
  const Input *const input_in = getInput( INPUT_IN );
  if( !input_in->isConnected() ) throw NotConnected( input_in );

  if( !hasAnyInputChanged() ) return true;
  setAllOutputChanged();

  GET_INPUT_BASE( INPUT_IN, in );

  Output *const output_out = getOutput( OUTPUT_OUT );
  output_out->setData( castTo( m_to_type, in ) );
  return true;
}

 } // namespace core {
} // namespace RPGML

RPGML_CREATE_NODE( Cast, core:: )


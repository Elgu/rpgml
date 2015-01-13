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
#include "RPGML_Node_MathOp1.h"

// RPGML_CXXFLAGS=
// RPGML_LDFLAGS=

#include <algorithm>

using namespace std;

namespace RPGML {
namespace math {

MathOp1::MathOp1( GarbageCollector *_gc, const String &identifier, const RPGML::SharedObject *so )
: Node( _gc, identifier, so, NUM_INPUTS, NUM_OUTPUTS, NUM_PARAMS )
, m_op( MOP1_UNDEFINED )
{
  DEFINE_INPUT ( INPUT_IN, "in"  );
  DEFINE_OUTPUT( OUTPUT_OUT, "out"  );
  DEFINE_PARAM ( PARAM_OP, "op", MathOp1::set_op );
}

MathOp1::~MathOp1( void )
{}

const char *MathOp1::getName( void ) const
{
  return "math.MathOp1";
}

void MathOp1::gc_clear( void )
{
  Base::gc_clear();
}

void MathOp1::gc_getChildren( Children &children ) const
{
  Base::gc_getChildren( children );
}

void MathOp1::set_op( const Value &value, index_t, int, const index_t* )
{
  if( !value.isString() )
  {
    throw Exception()
      << "Param 'op' must be set with a string, is " << value.getType()
      ;
  }

  try
  {
    m_op = getMOP1( value.getString() );
  }
  catch( const RPGML::Exception &e )
  {
    throw Exception() << "Could not set Param 'op': " << e.what();
  }
}

bool MathOp1::tick( void )
{
  if( !hasAnyInputChanged() ) return true;
  setAllOutputChanged();

  if( MOP1_UNDEFINED == m_op )
  {
    throw Exception() << "Param 'op' was not set";
  }

  if( !getInput( INPUT_IN )->isConnected() )
  {
    throw NotConnected( getInput( INPUT_IN ) );
  }

  CountPtr< const ArrayBase > in = getInput( INPUT_IN )->getData();
  if( in.isNull() ) throw Exception() << "Input 'in' has no valid data";

  try
  {
    getOutput( OUTPUT_OUT )->setData( mathOp1< const ArrayBase*, CountPtr< ArrayBase > >( m_op, in ) );
  }
  catch( const RPGML::Exception &e )
  {
    throw;
  }

  return true;
}

 } // namespace math {
} // namespace RPGML

RPGML_CREATE_NODE( MathOp1, math:: )


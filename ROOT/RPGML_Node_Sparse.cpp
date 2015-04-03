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
#include "RPGML_Node_Sparse.h"

// RPGML_CXXFLAGS=
// RPGML_LDFLAGS=

#include <algorithm>

using namespace std;

namespace RPGML {


Sparse::Sparse( GarbageCollector *_gc, const String &identifier, const RPGML::SharedObject *so )
: Node( _gc, identifier, so, NUM_INPUTS, NUM_OUTPUTS, NUM_PARAMS )
{
  DEFINE_INPUT ( INPUT_IN  , "in"  );
  DEFINE_INPUT ( INPUT_D   , "d"  );
  DEFINE_INPUT ( INPUT_NTH , "nth"  );
  DEFINE_INPUT ( INPUT_OFFSET, "offset"  );
  DEFINE_OUTPUT( OUTPUT_OUT, "out" );
}

Sparse::~Sparse( void )
{}

const char *Sparse::getName( void ) const
{
  return "Sparse";
}

void Sparse::gc_clear( void )
{
  Base::gc_clear();
}

void Sparse::gc_getChildren( Children &children ) const
{
  Base::gc_getChildren( children );
}

bool Sparse::tick( void )
{
  if( !hasAnyInputChanged() ) return true;
  setAllOutputChanged();

  GET_INPUT_BASE( INPUT_IN, in );
  const int d = getScalar< int >( INPUT_D );
  const index_t nth = getScalar< index_t >( INPUT_NTH );
  const index_t offset = getScalarIfConnected< index_t >( INPUT_OFFSET, 0 );

  CountPtr< ArrayBase > out = in->copy();
  out->setSparse( d, nth, offset );

  getOutput( OUTPUT_OUT )->setData( out );

  return true;
}

 //
} // namespace RPGML

RPGML_CREATE_NODE( Sparse,  )


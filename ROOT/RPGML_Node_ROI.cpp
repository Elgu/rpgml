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
#include "RPGML_Node_ROI.h"

// RPGML_CXXFLAGS=
// RPGML_LDFLAGS=

#include <algorithm>

using namespace std;

namespace RPGML {


ROI::ROI( GarbageCollector *_gc, const String &identifier, const RPGML::SharedObject *so )
: Node( _gc, identifier, so, NUM_INPUTS, NUM_OUTPUTS, NUM_PARAMS )
{
  DEFINE_INPUT ( INPUT_IN  , "in"  );
  DEFINE_INPUT ( INPUT_X , "x"  );
  DEFINE_INPUT ( INPUT_Y , "y"  );
  DEFINE_INPUT ( INPUT_Z , "z"  );
  DEFINE_INPUT ( INPUT_T , "t"  );
  DEFINE_INPUT ( INPUT_SX, "sx"  );
  DEFINE_INPUT ( INPUT_SY, "sy"  );
  DEFINE_INPUT ( INPUT_SZ, "sz"  );
  DEFINE_INPUT ( INPUT_SZ, "st"  );
  DEFINE_OUTPUT( OUTPUT_OUT, "out" );
}

ROI::~ROI( void )
{}

const char *ROI::getName( void ) const
{
  return "ROI";
}

void ROI::gc_clear( void )
{
  Base::gc_clear();
}

void ROI::gc_getChildren( Children &children ) const
{
  Base::gc_getChildren( children );
}

template< class Element >
bool ROI::tick2( int dims, const index_t *x, const index_t *s )
{
  GET_INPUT_AS( INPUT_IN, in, Element );
  CountPtr< const ArrayBase > roi( in->getROI( dims, x, s ) );
  getOutput( OUTPUT_OUT )->setData( const_cast< ArrayBase* >( roi.get() ) );
  return true;
}

bool ROI::tick( void )
{
  if( !hasAnyInputChanged() ) return true;
  setAllOutputChanged();

  GET_INPUT_BASE( INPUT_IN, in_base );

  const int dims = in_base->getDims();

  index_t x[ dims ];
  index_t s[ dims ];

  for( int d=0; d<dims; ++d )
  {
    x[ d ] = getScalar< index_t >( INPUT_X+d );
    s[ d ] = getScalar< index_t >( INPUT_SX+d );
  }
  for( int d=dims; d<4; ++d )
  {
    if( getInput( INPUT_X+d )->isConnected() )
    {
      throw Exception()
        << "Input 'in' is " << dims << " dimensional"
        << ", so Input '" << ("xyzt"[d]) << "' must not be connected."
        ;
    }
    if( getInput( INPUT_SX+d )->isConnected() )
    {
      throw Exception()
        << "Input 'in' is " << dims << " dimensional"
        << ", so Input 's" << ("xyzt"[d]) << "' must not be connected."
        ;
    }
  }

  switch( in_base->getType().getEnum() )
  {
    case Type::BOOL  : return tick2< bool     >( dims, x, s );
    case Type::UINT8 : return tick2< uint8_t  >( dims, x, s );
    case Type::INT8  : return tick2< int8_t   >( dims, x, s );
    case Type::UINT16: return tick2< uint16_t >( dims, x, s );
    case Type::INT16 : return tick2< int16_t  >( dims, x, s );
    case Type::UINT32: return tick2< uint32_t >( dims, x, s );
    case Type::INT32 : return tick2< int32_t  >( dims, x, s );
    case Type::UINT64: return tick2< uint64_t >( dims, x, s );
    case Type::INT64 : return tick2< int64_t  >( dims, x, s );
    case Type::FLOAT : return tick2< float    >( dims, x, s );
    case Type::DOUBLE: return tick2< double   >( dims, x, s );
    case Type::STRING: return tick2< String   >( dims, x, s );
    case Type::ARRAY : return tick2< CountPtr< ArrayBase > >( dims, x, s );
    default:
      throw IncompatibleOutput( getInput( INPUT_IN ) );
  }

  return true;
}

 //
} // namespace RPGML

RPGML_CREATE_NODE( ROI,  )


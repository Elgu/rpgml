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
#include "Context.h"

#include "Graph.h"
#include "Scope.h"
#include "Frame.h"
#include "StringUnifier.h"

namespace RPGML {

Context::Context( StringUnifier *unifier )
: m_unifier( unifier )
, m_nr( 0 )
{
  m_gc.reset( new GarbageCollector() );
  m_root = new Frame( getGC() );
}

Context::~Context( void )
{}

CountPtr< Scope > Context::createScope( void )
{
  return new Scope( this );
}

CountPtr< Graph > Context::createGraph( void ) const
{
  CountPtr< Graph > graph( new Graph( getGC() ) );

  // Just add every Node
  std::vector< const Frame* > stack;
  stack.push_back( m_root );

  while( !stack.empty() )
  {
    const Frame *frame = stack.back();
    stack.pop_back();
    if( !frame ) continue;

    for( index_t i( 0 ), end( frame->getSize() ); i < end; ++i )
    {
      const Value *const value = frame->getStack( i );
      if( !value ) continue;

      switch( value->getEnum() )
      {
        case Type::FRAME:
          stack.push_back( value->getFrame() );
          break;

        case Type::NODE:
          graph->addNode( value->getNode() );
          break;

        default: {}
      }
    }
  }

  return graph;
}

size_t Context::getNr( void )
{
  return m_nr++;
}

} // namespace RPGML


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

#include <string>
#include <cmath>

namespace RPGML {

Context::Context( GarbageCollector *_gc, StringUnifier *unifier, const String &searchPath )
: Collectable( _gc )
, m_unifier( unifier )
, m_nr( 0 )
{
  m_root = new Frame( getGC() );
  setSearchPath( searchPath );
  addGlobal( "@needed", new Array< CountPtr< Node > >( getGC(), 1 ) );
  addGlobal( "M_E"       , M_E        );
  addGlobal( "M_LOG10E"  , M_LOG10E   );
  addGlobal( "M_LN2"     , M_LN2      );
  addGlobal( "M_LN10"    , M_LN10     );
  addGlobal( "M_PI"      , M_PI       );
  addGlobal( "M_PI_2"    , M_PI_2     );
  addGlobal( "M_PI_4"    , M_PI_4     );
  addGlobal( "M_1_PI"    , M_1_PI     );
  addGlobal( "M_2_PI"    , M_2_PI     );
  addGlobal( "M_2_SQRTPI", M_2_SQRTPI );
  addGlobal( "M_SQRT2"   , M_SQRT2	  );
  addGlobal( "M_SQRT1_2" , M_SQRT1_2  );
}

template< class T >
void Context::addGlobal( const char *static_identifier, const T &value )
{
  m_root->push_back( String::Static( static_identifier ), Value( value ) );
}

Context::~Context( void )
{}

void Context::gc_clear( void )
{
  m_root.reset();
  m_unifier.reset();
  m_searchPaths.clear();
}

void Context::gc_getChildren( Children &children ) const
{
  children << m_root;
  children << m_unifier;
}

Context &Context::setSearchPath( const String &searchPath )
{
  m_searchPaths.clear();

  std::string s;
  for( const char *c = searchPath.c_str(); (*c) != '\0'; ++c )
  {
    if( (*c) == ':' )
    {
      if( !s.empty() )
      {
        m_searchPaths.push_back( String( s ) );
        s.clear();
      }
    }
    else
    {
      s += (*c);
    }
  }

  if( !s.empty() )
  {
    m_searchPaths.push_back( String( s ) );
  }

  return (*this);
}

const std::vector< String > &Context::getSearchPaths( void ) const
{
  return m_searchPaths;
}

CountPtr< Scope > Context::createScope( void )
{
  return new Scope( getGC(), this );
}

CountPtr< Graph > Context::createGraph( void ) const
{
  CountPtr< Graph > graph( new Graph( getGC() ) );

  /*
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
  */

  const Value *const needed_v = m_root->getVariable( "@needed" );
  if( !needed_v ) throw Exception() << "Looking up '@needed' failed";

  if( !needed_v->isArray() )
  {
    throw Exception() << "'@needed' is not an Array, is " << needed_v->getTypeName();
  }

  ArrayBase *const needed_base = needed_v->getArray();
  if( !needed_base )
  {
    throw Exception() << "Found Array '@needed', but is null";
  }

  NodeArray *needed_p = 0;
  if( !needed_base->getAs( needed_p ) )
  {
    throw Exception()
      << "'@needed' is an Array, but not of the correct type, is "
      << needed_base->getTypeName()
      << needed_base->getSize()
      ;
  }

  for( NodeArray::const_iterator n( needed_p->begin() ), end( needed_p->end() ); n != end; ++n )
  {
    Node *const node = n->get();
    if( !node ) continue;
    graph->addNode( node );
  }

  return graph;
}

size_t Context::getNr( void )
{
  return m_nr++;
}

} // namespace RPGML


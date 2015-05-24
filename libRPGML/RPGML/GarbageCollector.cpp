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
#include "GarbageCollector.h"

#include <cassert>
#include <memory>
#include <iostream>
#include <typeinfo>

using namespace std;

namespace RPGML {

GarbageCollector::GarbageCollector( uint8_t num_generations )
: m_lock( Mutex::Recursive() )
, m_num_generations( num_generations )
{}

GarbageCollector::~GarbageCollector( void )
{
  sweep( m_cs );
}

void GarbageCollector::add( const Collectable *c )
{
  if( 0 == this ) return;
  if( 0 == c ) return;
  if( c->gc == this ) return;
  if( c->gc ) c->gc->remove( c );

  { Mutex::ScopedLock lock( &m_lock );

    c->gc = this;
    c->gc_index = index_t( m_cs.size() );
    c->gc_generation = 0;
    m_cs.push_back( c );
  } // m_lock

#ifdef GC_DEBUG
  std::cerr << "add " << c << " to " << this << ", index = " << c->gc_index << std::endl;
#endif
}

void GarbageCollector::remove( const Collectable *c )
{
  if( 0 == this ) return;
  assert( c->gc == this );

  { Mutex::ScopedLock lock( &m_lock );

    const index_t index = c->gc_index;
    if( index < m_cs.size() )
    {
      if( m_cs[ index ] == c )
      {
#ifdef GC_DEBUG
        std::cerr << "remove " << c << " from " << this << ", index = " << c->gc_index << std::endl;
#endif

        m_cs[ index ] = 0;
        c->gc = 0;
        c->gc_index = 0;
      }
    }
  } // m_lock
}

void GarbageCollector::compact( CollectableArray &cs_new, uint8_t up_to_generation )
{
  const size_t n = m_cs.size();
  Collectable::Children children;
  children.reserve( 512 );

  // Find all root objects:
  // Those who are referenced from outside by "foreign" references
  // will have a different refcount than calculated here
  vector< refCount_t > refcount( n );
  for( size_t i=0; i<n; ++i )
  {
    const Collectable *const obj = m_cs[ i ];
    if( !obj ) continue;

    if( obj->gc_generation > up_to_generation )
    {
      // Ignore "older" objects at this point
      // They will just implicitly contribute to the "foreign" references.
      continue;
    }

    // Get the objects referenced by this object
    children.clear();
    obj->gc_getChildren( children );

    for( size_t j = 0; j < children.size(); ++j )
    {
      const Collectable *const child = children[ j ];
      if( !child ) continue;

      if( child->gc != this ) continue;

      ++refcount[ child->gc_index ];
    }
  }

  CollectableArray stack;
  stack.reserve( 512 );

  // NOTE: This could also be done the other way, just schedule for checking,
  //       but there would be the risk of having a very large schedule stack
  //       with dupplicate entries

  // push root objects
  for( size_t i=0; i<n; ++i )
  {
    const Collectable *const obj = m_cs[ i ];
    if( !obj ) continue;

    if( obj->gc_generation > up_to_generation )
    {
      // Just keep "older" objects, they will not play any role in the
      // rest of the garbage collecting process.
      removeFromOldAddToNew( obj, cs_new );
      continue;
    }

    // Only root objects are referenced from outside
    if( refcount[ obj->gc_index ] == obj->refCount() ) continue;

#ifdef GC_DEBUG
    std::cerr << "Root object " << obj << std::endl;
#endif // GC_DEBUG

    // Remove from old storage, add to new
    removeFromOldAddToNew( obj, cs_new );

    // Schedule root
    stack.push_back( obj );
  }

  // Process children of scheduled (already moved) Collectables
  while( !stack.empty() )
  {
    const Collectable *const c = stack.back();
    stack.pop_back();

    children.clear();
    c->gc_getChildren( children );

    for( size_t i( 0 ), end( children.size() ); i<end; ++i )
    {
      const Collectable *const child = children[ i ];
      if( !child ) continue;

      // Check, if not compacted yet
      if( child->gc == this && m_cs[ child->gc_index ] == child )
      {
        // Remove from old storage, add to new
        removeFromOldAddToNew( child, cs_new );

        // Schedule child
        stack.push_back( child );
      }
    }
  }
}

void GarbageCollector::sweep( CollectableArray &garbage )
{
  for( index_t i=0; i<garbage.size(); ++i )
  {
    const Collectable *const chunk = garbage[ i ];
    if( chunk ) chunk->deactivate_deletion();
//    if( chunk ) std::cerr << "Sweeping " << chunk << std::endl;
  }

  for( index_t i=0; i<garbage.size(); ++i )
  {
    const Collectable *const chunk = garbage[ i ];
    if( chunk ) const_cast< Collectable* >( chunk )->gc_clear();
  }

  for( index_t i=0; i<garbage.size(); ++i )
  {
    const Collectable *const chunk = garbage[ i ];
    if( chunk )
    {
      if( chunk->refCount() <= 0 )
      {
        delete const_cast< Collectable* >( chunk );
      }
      else
      {
        std::cerr
          << "warning: Object " << (void*)chunk << " should be sweeped"
          << ", but still has " << chunk->refCount() << " references to it"
          << ": is a '" << typeid(*chunk).name() << "'"
          << std::endl
          ;
      }
    }
  }
}

void GarbageCollector::removeFromOldAddToNew( const Collectable *obj, CollectableArray &cs_new )
{
  assert( obj );
  assert( obj->gc_index < m_cs.size() );
  assert( m_cs[ obj->gc_index ] != 0 );
  m_cs[ obj->gc_index ] = 0;
  obj->gc_index = index_t( cs_new.size() );
  cs_new.push_back( obj );

  if( obj->gc_generation < m_num_generations-1 )
  {
    ++obj->gc_generation;
  }
}

void GarbageCollector::run( uint8_t up_to_generation )
{
  if( m_cs.empty() ) return;

  CollectableArray cs_new;

  { Mutex::ScopedLock lock( &m_lock );

    // compact recursively
    compact( cs_new, up_to_generation );

    // make new cs current
    std::swap( cs_new, m_cs );
  } // m_lock

  // cs_new now contains garbage
  sweep( cs_new );
}

void GarbageCollector::moveObjectsTo( GarbageCollector *other )
{
  for( auto &c : m_cs )
  {
    CountPtr< const Collectable > tmp( c );
    remove( tmp );
    if( other )
    {
      other->add( tmp );
    }
  }
}

Collectable::Collectable( GarbageCollector *_gc )
: gc( 0 )
, m_refCount( 0 )
, gc_index( 0 )
, gc_generation( 0 )
{
#ifdef GC_DEBUG
  std::cerr << "create " << this << std::endl;
#endif
  if( _gc ) _gc->add( this );
}

Collectable::Collectable( const Collectable &other )
: gc( 0 )
, gc_index( 0 )
, gc_generation( 0 )
{
#ifdef GC_DEBUG
  std::cerr << "create " << this << std::endl;
#endif
  GarbageCollector *const _gc = other.gc;
  if( _gc ) _gc->add( this );
}

Collectable &Collectable::operator=( const Collectable & )
{
  // Do nothing, keep gc and index
  return (*this);
}

Collectable::~Collectable( void )
{
  if( gc ) gc->remove( this );
#ifdef GC_DEBUG
  std::cerr << "delete " << this << std::endl;
#endif
}

bool Collectable::Children::contains( const Collectable *c ) const
{
  return ( m_children.end() != std::find( m_children.begin(), m_children.end(), c ) );
}

} // namespace RPGML


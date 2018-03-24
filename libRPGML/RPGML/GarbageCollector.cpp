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

void GarbageCollector::deactivate_deletion( const Collectable *c )
{
  c->deactivate_deletion();
}

void GarbageCollector::setGC( const Collectable *c, GarbageCollector *gc )
{
  c->m_gc = gc;
}

void GarbageCollector::setGCIndex( const Collectable *c, index_t index )
{
  c->m_gc_index = index;
}

void GarbageCollector::incGCGeneration( const Collectable *c )
{
  ++c->m_gc_generation;
}

void GarbageCollector::setGCGeneration( const Collectable *c, uint8_t generation )
{
  c->m_gc_generation = generation;
}

class GenerationalGarbageCollector : public GarbageCollector
{
public:
  static const uint8_t MaxGenerations = 255;

  explicit
  GenerationalGarbageCollector( uint8_t num_generations = 1 );
  ~GenerationalGarbageCollector( void );

  void run( uint8_t up_to_generation = MaxGenerations );

  void moveObjectsTo( GarbageCollector *other );

  virtual void add( const Collectable *c );
  virtual void remove( const Collectable *c );

private:
  typedef std::vector< const Collectable* > CollectableArray;
  void compact( CollectableArray &cs_new, uint8_t up_to_generation );
  void sweep( CollectableArray &garbage );

  void removeFromOldAddToNew( const Collectable *obj, CollectableArray &cs_new );

  CollectableArray m_cs;
  Mutex m_lock;
  uint8_t m_num_generations;

private:
  GenerationalGarbageCollector( const GenerationalGarbageCollector &other );
  GenerationalGarbageCollector &operator=( const GenerationalGarbageCollector &other );
};

class GenerationalGarbageCollectorChildren : public Collectable::Children
{
public:
  GenerationalGarbageCollectorChildren( void ) {}
  virtual ~GenerationalGarbageCollectorChildren( void ) {}
  virtual void add( const Collectable *c ) { if( c ) m_children.push_back( c ); }
  //! For testing (utest) only
  virtual bool contains( const Collectable *c ) const
  {
    return ( m_children.end() != std::find( m_children.begin(), m_children.end(), c ) );
  }

  void reserve( size_t n ) { m_children.reserve( n ); }
  size_t size( void ) const { return m_children.size(); }
  void clear( void ) { m_children.clear(); }
  const Collectable *const &operator[]( size_t i ) const { return m_children[ i ]; }
  const Collectable *      &operator[]( size_t i ) { return m_children[ i ]; }
  std::vector< const Collectable* > m_children;
};

GenerationalGarbageCollector::GenerationalGarbageCollector( uint8_t num_generations )
: m_lock( Mutex::Recursive() )
, m_num_generations( num_generations )
{}

GenerationalGarbageCollector::~GenerationalGarbageCollector( void )
{
  sweep( m_cs );
}

void GenerationalGarbageCollector::add( const Collectable *c )
{
  if( 0 == c ) return;
  if( c->getGC() == this ) return;
  if( c->getGC() ) c->getGC()->remove( c );

  { Mutex::ScopedLock lock( &m_lock );

    setGC( c, this );
    setGCIndex( c, index_t( m_cs.size() ) );
    setGCGeneration( c, 0 );
    m_cs.push_back( c );
  } // m_lock

#ifdef GC_DEBUG
  std::cerr << "add " << c << " to " << this << ", index = " << c->gc_index << std::endl;
#endif
}

void GenerationalGarbageCollector::remove( const Collectable *c )
{
  assert( c->getGC() == this );

  { Mutex::ScopedLock lock( &m_lock );

    const index_t index = c->getGCIndex();
    if( index < m_cs.size() )
    {
      if( m_cs[ index ] == c )
      {
#ifdef GC_DEBUG
        std::cerr << "remove " << c << " from " << this << ", index = " << c->m_gc_index << std::endl;
#endif

        m_cs[ index ] = 0;
        setGC( c, nullptr );
        setGCIndex( c, 0 );
      }
    }
  } // m_lock
}

void GenerationalGarbageCollector::compact( CollectableArray &cs_new, uint8_t up_to_generation )
{
  const size_t n = m_cs.size();
  GenerationalGarbageCollectorChildren children;
  children.reserve( 512 );

  // Find all root objects:
  // Those who are referenced from outside by "foreign" references
  // will have a different refcount than calculated here
  vector< refCount_t > refcount( n );
  for( size_t i=0; i<n; ++i )
  {
    const Collectable *const obj = m_cs[ i ];
    if( !obj ) continue;

    if( obj->getGCGeneration() > up_to_generation )
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

      if( child->getGC() != this ) continue;

      ++refcount[ child->getGCIndex() ];
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

    if( obj->getGCGeneration() > up_to_generation )
    {
      // Just keep "older" objects, they will not play any role in the
      // rest of the garbage collecting process.
      removeFromOldAddToNew( obj, cs_new );
      continue;
    }

    // Only root objects are referenced from outside
    if( refcount[ obj->getGCIndex() ] == obj->refCount() ) continue;

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
      if( child->getGC() == this && m_cs[ child->getGCIndex() ] == child )
      {
        // Remove from old storage, add to new
        removeFromOldAddToNew( child, cs_new );

        // Schedule child
        stack.push_back( child );
      }
    }
  }
}

void GenerationalGarbageCollector::sweep( CollectableArray &garbage )
{
  for( index_t i=0; i<garbage.size(); ++i )
  {
    const Collectable *const chunk = garbage[ i ];
    if( chunk ) deactivate_deletion( chunk );
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

void GenerationalGarbageCollector::removeFromOldAddToNew( const Collectable *obj, CollectableArray &cs_new )
{
  assert( obj );
  assert( obj->getGCIndex() < m_cs.size() );
  assert( m_cs[ obj->getGCIndex() ] != 0 );
  m_cs[ obj->getGCIndex() ] = 0;

  setGCIndex( obj, index_t( cs_new.size() ) );
  cs_new.push_back( obj );

  if( obj->getGCGeneration() < m_num_generations-1 )
  {
    incGCGeneration( obj );
  }
}

void GenerationalGarbageCollector::run( uint8_t up_to_generation )
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

void GenerationalGarbageCollector::moveObjectsTo( GarbageCollector *other )
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

CountPtr< GarbageCollector > newGenerationalGarbageCollector( uint8_t up_to_generation )
{
  return new GenerationalGarbageCollector( up_to_generation );
}

Collectable::Collectable( GarbageCollector *gc )
: m_gc( nullptr )
, m_gc_refCount( 0 )
, m_gc_index( 0 )
, m_gc_generation( 0 )
{
#ifdef GC_DEBUG
  std::cerr << "create " << this << std::endl;
#endif
  if( gc ) gc->add( this );
}

Collectable::Collectable( const Collectable &other )
: m_gc( nullptr )
, m_gc_index( 0 )
, m_gc_generation( 0 )
{
#ifdef GC_DEBUG
  std::cerr << "create " << this << std::endl;
#endif
  GarbageCollector *const gc = other.m_gc;
  if( gc ) gc->add( this );
}

Collectable &Collectable::operator=( const Collectable & )
{
  // Do nothing, keep gc and index
  return (*this);
}

Collectable::~Collectable( void )
{
  if( m_gc ) m_gc->remove( this );
#ifdef GC_DEBUG
  std::cerr << "delete " << this << std::endl;
#endif
}

void Collectable::gc_clear( void )
{}

void Collectable::gc_getChildren( Children &children ) const
{
  (void)children;
}

} // namespace RPGML


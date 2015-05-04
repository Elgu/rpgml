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
#ifndef RPGML_GarbageCollector_h
#define RPGML_GarbageCollector_h

#include "types.h"
#include "Refcounted.h"
#include "Mutex.h"
#include "Atomic.h"

#include <vector>

#ifdef GC_DEBUG
#include <iostream>
#endif

namespace RPGML {

class Collectable;

class GarbageCollector
{
public:
  static const uint8_t MaxGenerations = 255;

  explicit
  GarbageCollector( uint8_t num_generations = 1 );
  ~GarbageCollector( void );

  void run( uint8_t up_to_generation = MaxGenerations );

protected:
  friend class Collectable;
  void add( const Collectable *c );
  void remove( const Collectable *c );
  void setRoot( const Collectable *c );

private:
  typedef std::vector< const Collectable* > CollectableArray;
  void compact( CollectableArray &cs_new, uint8_t up_to_generation );
  void sweep( CollectableArray &garbage );

  void removeFromOldAddToNew( const Collectable *obj, CollectableArray &cs_new );

  CollectableArray m_cs;
  Mutex m_lock;
  uint8_t m_num_generations;

private:
  GarbageCollector( const GarbageCollector &other );
  GarbageCollector &operator=( const GarbageCollector &other );
};

class Collectable
{
  friend class GarbageCollector;
public:
  typedef index_t refCount_t;

  explicit
  Collectable( GarbageCollector *_gc );
  Collectable( const Collectable &other );

  Collectable &operator=( const Collectable & );

  virtual ~Collectable( void );

  refCount_t ref( void ) const
  {
    return ++m_refCount;
  }

  refCount_t unref( void ) const
  {
    gc_generation = 0;
    return --m_refCount;
  }

  refCount_t refCount( void ) const
  {
    return m_refCount;
  }

  GarbageCollector *getGC( void ) const
  {
    return gc;
  }

  class Children
  {
    friend class GarbageCollector;
  public:
    Children( void ) {}
    ~Children( void ) {}
    void add( const void * ) {}
    void add( const Collectable *c ) { if( c ) m_children.push_back( c ); }
    //! For testing (utest) only
    bool contains( const Collectable *c ) const;
  private:
    void reserve( size_t n ) { m_children.reserve( n ); }
    size_t size( void ) const { return m_children.size(); }
    void clear( void ) { m_children.clear(); }
    const Collectable *const &operator[]( size_t i ) const { return m_children[ i ]; }
    const Collectable *      &operator[]( size_t i ) { return m_children[ i ]; }
    std::vector< const Collectable* > m_children;
  };

  //! Do not implement recursively, only clear what is not reachable over gc_getChildren(), clear references to those
  virtual void gc_clear( void ) {}
  //! Do not implement recursively
  virtual void gc_getChildren( Children &children ) const { (void)children; }

private:
  void deactivate_deletion( void ) const throw()
  {
    // That way the next one gets count_t(-1), which is not 0
    m_refCount = 0;
  }

  mutable GarbageCollector *gc;
  mutable Atomic< refCount_t > m_refCount;
  mutable index_t gc_index;
  mutable uint8_t gc_generation;
};

static inline bool isCollectable( const Collectable * ) { return true; }
static inline bool isCollectable( const void * ) { return false; }

template< class T >
inline bool isCollectable( const CountPtr< T > * ) { return isCollectable( (const T*)0 ); }

static inline
Collectable::Children &operator<<( Collectable::Children &children, Collectable *e )
{
  children.add( e );
  return children;
}

static inline
Collectable::Children &operator<<( Collectable::Children &children, const Collectable *e )
{
  children.add( e );
  return children;
}

static inline
Collectable::Children &operator<<( Collectable::Children &children, const void * )
{
  return children;
}

static inline
Collectable::Children &operator<<( Collectable::Children &children, const std::vector< bool >::reference & )
{
  return children;
}

static inline
Collectable::Children &operator<<( Collectable::Children &children, const std::vector< bool >::const_reference & )
{
  return children;
}

template< class T >
static inline
Collectable::Children &operator<<( Collectable::Children &children, const CountPtr<       T > &e )
{
  children.add( e.get() );
  return children;
}

template< class T >
static inline
Collectable::Children &operator<<( Collectable::Children &children, const CountPtr< const T > &e )
{
  children.add( e.get() );
  return children;
}

} // namespace RPGML

#endif


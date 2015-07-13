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

class GarbageCollector : public Refcounted
{
public:
  static const uint8_t MaxGenerations = 255;

  GarbageCollector( void ) {}
  virtual ~GarbageCollector( void ) {}

  virtual void run( uint8_t up_to_generation = MaxGenerations ) = 0;
  virtual void moveObjectsTo( GarbageCollector *other ) = 0;

  virtual void add( const Collectable *c ) = 0;
  virtual void remove( const Collectable *c ) = 0;

protected:
  static void deactivate_deletion( const Collectable *c );
  static void setGC( const Collectable *c, GarbageCollector *gc );
  static void setGCIndex( const Collectable *c, index_t index );
  static void incGCGeneration( const Collectable *c );
  static void setGCGeneration( const Collectable *c, uint8_t generation );

private:
  GarbageCollector( const GarbageCollector &other ) = delete;
  GarbageCollector &operator=( const GarbageCollector &other ) = delete;
};

extern
CountPtr< GarbageCollector > newGenerationalGarbageCollector( uint8_t up_to_generation = GarbageCollector::MaxGenerations );

class Collectable
{
  friend class GarbageCollector;
public:
  explicit
  Collectable( GarbageCollector *gc );
  Collectable( const Collectable &other );

  Collectable &operator=( const Collectable & );

  virtual ~Collectable( void );

  refCount_t ref( void ) const
  {
    return ++m_gc_refCount;
  }

  refCount_t unref( void ) const
  {
    m_gc_generation = 0;
    return --m_gc_refCount;
  }

  refCount_t refCount( void ) const
  {
    return m_gc_refCount;
  }

  GarbageCollector *getGC( void ) const
  {
    return m_gc;
  }

  class Children
  {
    friend class GarbageCollector;
  public:
    Children( void ) {}
    virtual ~Children( void ) {}
    void add( const void * ) const {}
    virtual void add( const Collectable *c ) = 0;
    //! For testing (utest) only
    virtual bool contains( const Collectable *c ) const = 0;
  };

  //! Do not implement recursively, only clear what is not reachable over gc_getChildren(), clear references to those
  virtual void gc_clear( void );
  //! Do not implement recursively
  virtual void gc_getChildren( Children &children ) const;

  index_t getGCIndex( void ) const { return m_gc_index; }
  uint8_t getGCGeneration( void ) const { return m_gc_generation; }

private:
  void deactivate_deletion( void ) const throw()
  {
    // That way the next one gets count_t(-1), which is not 0
    m_gc_refCount = 0;
  }

  mutable GarbageCollector *m_gc;
  mutable Atomic< refCount_t > m_gc_refCount;
  mutable index_t m_gc_index;
  mutable uint8_t m_gc_generation;
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


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
#include "Iterator.h"
#include "Refcounted.h"
#include "Mutex.h"

#include <vector>

#ifdef GC_DEBUG
#include <iostream>
#endif

namespace RPGML {

class Collectable;

class GarbageCollector
{
public:
  GarbageCollector( void );
  ~GarbageCollector( void );

  void run( void );

protected:
  friend class Collectable;
  void add( const Collectable *c );
  void remove( const Collectable *c );
  void setRoot( const Collectable *c );

private:
  void compact( std::vector< const Collectable* > &cs_new );
  void sweep( std::vector< const Collectable* > &garbage );

  std::vector< const Collectable* > m_cs;
  Mutex m_lock;

private:
  GarbageCollector( const GarbageCollector &other );
  GarbageCollector &operator=( const GarbageCollector &other );
};

class Collectable : public Refcounted
{
  friend class GarbageCollector;
public:
	explicit
  Collectable( GarbageCollector *_gc )
  : gc( 0 )
  , gc_index( 0 )
  {
#ifdef GC_DEBUG
    std::cerr << "create " << this << std::endl;
#endif
    if( _gc ) _gc->add( this );
  }

  Collectable( const Collectable &other )
  : gc( 0 )
  , gc_index( 0 )
  {
#ifdef GC_DEBUG
    std::cerr << "create " << this << std::endl;
#endif
    GarbageCollector *const _gc = other.gc;
    if( _gc ) _gc->add( this );
  }

  Collectable &operator=( const Collectable & )
  {
    // Do nothing, keep gc and index
    return (*this);
  }

  virtual ~Collectable( void )
  {
    if( gc ) gc->remove( this );
#ifdef GC_DEBUG
    std::cerr << "delete " << this << std::endl;
#endif
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
    void add( Collectable *c ) { if( c ) m_children.push_back( c ); }
    void add( const Collectable *c ) { if( c ) m_children.push_back( c ); }
    //! For testing (utest) only
    bool contains( Collectable *c ) const;
  private:
    void reserve( size_t n ) { m_children.reserve( n ); }
    size_t size( void ) const { return m_children.size(); }
    void clear( void ) { m_children.clear(); }
    const Collectable *const &operator[]( size_t i ) const { return m_children[ i ]; }
    const Collectable *      &operator[]( size_t i ) { return m_children[ i ]; }
    std::vector< const Collectable* > m_children;
  };

  //! Do not implement recursively, only clear what is not reachable over gc_getChildren(), clear references to those
  virtual void gc_clear( void ) = 0;
  //! Do not implement recursively
  virtual void gc_getChildren( Children &children ) const = 0;

private:
  mutable GarbageCollector *gc;
  mutable index_t gc_index;
};

static inline bool isCollectable( const Collectable * ) { return true; }
static inline bool isCollectable( const void * ) { return false; }

template< class T >
inline bool isCollectable( const CountPtr< T > * ) { return isCollectable( (const T*)0 ); }

} // namespace RPGML

#endif


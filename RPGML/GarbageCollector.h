#ifndef RPGML_GarbageCollector_h
#define RPGML_GarbageCollector_h

#include "Iterator.h"
#include "Refcounted.h"
#include "types.h"

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

private:
  void compact( std::vector< const Collectable* > &cs_new, const Collectable *root );
  void sweep( std::vector< const Collectable* > &garbage );

  std::vector< const Collectable* > m_cs;

private:
  GarbageCollector( const GarbageCollector &other );
  GarbageCollector &operator=( GarbageCollector other );
};

class Collectable : public Refcounted
{
  friend class GarbageCollector;
public:
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

  class Children : public std::vector< const Collectable* >
  {
  public:
    Children( void ) {}
    ~Children( void ) {}

    void add( const Collectable *c ) { if( c ) push_back( c ); }
    template< class Value >
    void add( const Value &v ) { add( v.getCollectable() ); }
  };

  //! Do not implement recursively, only clear, what is not reachable over getChildren(), clear references to those
  virtual void gc_clear( void ) = 0;
  //! Do not implement recursively
  virtual void getChildren( Children &children ) const = 0;

private:
  mutable GarbageCollector *gc;
  mutable index_t gc_index;
};

} // namespace RPGML

#endif


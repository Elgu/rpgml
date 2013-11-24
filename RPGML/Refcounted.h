#ifndef RPGML_Refcounted_h
#define RPGML_Refcounted_h

#include "types.h"

#include <cassert>
#include <cstddef>
#include <algorithm>

namespace RPGML {

class GarbageCollector;

class Refcounted
{
  friend class GarbageCollector;
public:
  Refcounted( void )
  : m_count( 0 )
  {}

  virtual ~Refcounted( void )
  {}

  index_t ref( void ) const
  {
    ++m_count;
    return m_count;
  }

  index_t unref( void ) const
  {
    --m_count;
    return m_count;
  }

  index_t count( void ) const
  {
    return m_count;
  }

private:
  void deactivate_deletion( void ) const throw()
  {
    // That way the next one gets index_t(-1), which is not 0
    m_count = 0;
  }

  mutable index_t m_count;
};

template< class RefcountedType >
class CountPtr
{
public:
  typedef RefcountedType Type;

  CountPtr( void )
  : m_p( 0 )
  {}

  CountPtr( Type *p )
  : m_p( p )
  {
    if( m_p ) m_p->ref();
  }

  CountPtr( const CountPtr &cp )
  : m_p( cp.m_p )
  {
    if( m_p ) m_p->ref();
  }

  ~CountPtr( void )
  {
    if( m_p && !m_p->unref() ) delete m_p;
  }

  CountPtr &operator=( CountPtr cp )
  {
    this->swap( cp );
    return (*this);
  }

  CountPtr &operator=( Type *p )
  {
    return this->reset( p );
  }

  bool operator==( const CountPtr &cp ) const
  {
    return m_p == cp.m_p;
  }

  bool operator<( const CountPtr &cp ) const
  {
    return m_p < cp.m_p;
  }

  CountPtr &reset( Type *p=0 )
  {
    CountPtr tmp( p );
    this->swap( tmp );
    return (*this);
  }

  void swap( CountPtr &cp )
  {
    std::swap( m_p, cp.m_p );
  }

  Type *get( void ) const
  {
    return m_p;
  }

  Type &operator*( void ) const
  {
    return *get();
  }

  Type *operator->( void ) const
  {
    return get();
  }

  operator Type*( void ) const
  {
    return get();
  }

private:
  Type *m_p;
};

} // namespace RPGML

namespace std {

  template< class RefcountedType >
  void swap( RPGML::CountPtr< RefcountedType > &x1, RPGML::CountPtr< RefcountedType > &x2 )
  {
    x1.swap( x2 );
  }

} // namespace std

#endif


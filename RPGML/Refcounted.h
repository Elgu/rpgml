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

template< class _RefcountedType >
class CountPtr
{
public:
  typedef _RefcountedType RefcountedType;

  CountPtr( void )
  : m_p( 0 )
  {}

  template< class CompatibleRefcountedType >
  CountPtr( CompatibleRefcountedType *p )
  : m_p( p )
  {
    if( m_p ) m_p->ref();
  }

  template< class CompatibleRefcountedType >
  CountPtr( const CountPtr< CompatibleRefcountedType > &other )
  : m_p( other.get() )
  {
    if( m_p ) m_p->ref();
  }

  ~CountPtr( void )
  {
		clear();
  }

	void clear( void )
	{
    if( m_p )
		{
			if( !m_p->unref() ) delete m_p;
			m_p = 0;
		}
	}

  CountPtr &operator=( CountPtr cp )
  {
    this->swap( cp );
    return (*this);
  }

  template< class CompatibleRefcountedType >
  CountPtr &operator=( CompatibleRefcountedType *p )
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

  template< class CompatibleRefcountedType >
  CountPtr &reset( CompatibleRefcountedType *p )
  {
    CountPtr tmp( p );
    this->swap( tmp );
    return (*this);
  }

  CountPtr &reset( void )
  {
    return reset( (RefcountedType*)0 );
  }

  void swap( CountPtr &cp )
  {
    std::swap( m_p, cp.m_p );
  }

  RefcountedType *get( void ) const
  {
    return m_p;
  }

  RefcountedType &operator*( void ) const
  {
    return *get();
  }

  RefcountedType *operator->( void ) const
  {
    return get();
  }

  operator RefcountedType*( void ) const
  {
    return get();
  }

  bool isNull( void ) const
  {
    return !get();
  }

private:
  RefcountedType *m_p;
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


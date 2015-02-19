/* This file is part of RPGML.
 *
 * Copyright (c) 2015, Gunnar Payer, All rights reserved.
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
#ifndef RPGML_ArrayData_h
#define RPGML_ArrayData_h

#include "ArrayBase.h"
#include "GarbageCollector.h"
#include "types.h"
#include "SaveBool.h"

#include <vector>
#include <algorithm>
#include <cassert>
#include <iostream>

namespace RPGML {

template< class _Element >
class ArrayContainer
{
  typedef std::vector< _Element > elements_t;
public:
  typedef _Element Element;
  typedef Element        *pointer;
  typedef const Element  *const_pointer;
  typedef Element        &reference;
  typedef const Element  &const_reference;
  typedef typename elements_t::      iterator       iterator;
  typedef typename elements_t::const_iterator const_iterator;

  explicit
  ArrayContainer( index_t n ) : m_elements( n ) {}
  ~ArrayContainer( void ) {}

  void clear( void ) { m_elements.clear(); }
  index_t size( void ) const { return index_t( m_elements.size() ); }
  bool empty( void ) const { return m_elements.empty(); }

  iterator       begin( void )       { return m_elements.begin(); }
  const_iterator begin( void ) const { return m_elements.begin(); }
  iterator       end( void )       { return m_elements.end(); }
  const_iterator end( void ) const { return m_elements.end(); }

  reference       operator[]( index_t i )       { return m_elements[ i ]; }
  const_reference operator[]( index_t i ) const { return m_elements[ i ]; }

  pointer       first( void )       { return &m_elements[ 0 ]; }
  const_pointer first( void ) const { return &m_elements[ 0 ]; }

private:
  elements_t m_elements;
};

template<>
class ArrayContainer< bool >
{
  typedef std::vector< uint8_t > elements_t;
public:
  typedef bool Element;

  class reference;
  class const_reference;
  class pointer;
  class const_pointer;

  class reference
  {
    friend class ArrayContainer< bool >;
  private:
    reference( void )
    : m_p( 0 )
    , m_b( 0 )
    {}

    reference( elements_t &elements, index_t i )
    : m_p( &elements[ i >> 3 ] )
    , m_b( i & 0x7 )
    {}

    reference( uint8_t *p, int8_t b )
    : m_p( p )
    , m_b( b )
    {}

  public:
    operator bool( void ) const { return ( (*m_p) >> m_b ) & 1; }

    pointer operator&( void ) const { return pointer( getP(), getB() ); }

    reference &operator=( bool b )
    {
      const uint8_t mask = uint8_t( ~( 1 << m_b ) );
      (*m_p) = ( (*m_p) & mask ) | uint8_t( uint8_t( b ) << m_b );
      return (*this);
    }

    uint8_t *getP( void ) const { return m_p; }
     int8_t  getB( void ) const { return m_b; }
  private:
    uint8_t *m_p;
     int8_t  m_b;
  };

  class const_reference
  {
    friend class ArrayContainer< bool >;
  private:
    const_reference( void )
    : m_p( 0 )
    , m_b( 0 )
    {}

    const_reference( const elements_t &elements, index_t i )
    : m_p( &elements[ i >> 3 ] )
    , m_b( i & 0x7 )
    {}

    const_reference( const uint8_t *p, int8_t b )
    : m_p( p )
    , m_b( b )
    {}

  public:
    const_reference( const reference &r )
    : m_p( r.getP() )
    , m_b( r.getB() )
    {}

    operator bool( void ) const { return ( (*m_p) >> m_b ) & 1; }

    const_pointer operator&( void ) const { return const_pointer( getP(), getB() ); }

    const uint8_t *getP( void ) const { return m_p; }
     int8_t  getB( void ) const { return m_b; }
  private:
    const uint8_t *m_p;
    int8_t  m_b;
  };

  class pointer
  {
    friend class ArrayContainer< bool >;
  public:
    pointer( void )
    : m_p( 0 )
    , m_b( 0 )
    {}

    ~pointer( void ) {}

    pointer( elements_t &elements, index_t i )
    : m_p( &elements[ i >> 3 ] )
    , m_b( i & 0x7 )
    {}

    pointer( uint8_t *p, int8_t b )
    : m_p( p )
    , m_b( b )
    {}

    void swap( pointer &other )
    {
      std::swap( m_p, other.m_p );
      std::swap( m_b, other.m_b );
    }

    typedef SaveBool::bool_type bool_type;
    operator bool_type( void ) const { return SaveBool()( m_p || m_b ); }

    reference operator*( void ) const { return reference( m_p, m_b ); }

    reference operator[]( ptrdiff_t i ) const { return *( (*this) + i ); }

    pointer &operator=( const pointer &p )
    {
      m_p = p.getP();
      m_b = p.getB();
      return (*this);
    }

    ptrdiff_t operator-( const pointer &other ) const
    {
      return ( m_p - other.m_p ) * 8 + ( m_b - other.m_b );
    }

    pointer &operator+=( ptrdiff_t i )
    {
      const ptrdiff_t b = ptrdiff_t( m_b ) + i;
      m_b  = ( b & 0x7 );
      m_p += ( b >> 3 );
      return (*this);
    }

    pointer &operator-=( ptrdiff_t i )
    {
      return operator+=( -i );
    }

    pointer operator+( ptrdiff_t i ) const
    {
      pointer ret( (*this) );
      return ( ret += i );
    }

    pointer operator-( ptrdiff_t i ) const
    {
      return operator+( -i );
    }

    pointer &operator++( void )
    {
      return (*this) += 1;
    }

    pointer operator++( int )
    {
      const pointer ret( (*this) );
      (*this) += 1;
      return ret;
    }

    pointer &operator--( void )
    {
      return (*this) -= 1;
    }

    pointer operator--( int )
    {
      const pointer ret( (*this) );
      (*this) -= 1;
      return ret;
    }

    bool operator==( const pointer &other ) const { return getP() == other.getP() && getB() == other.getB(); }
    bool operator!=( const pointer &other ) const { return getP() != other.getP() || getB() != other.getB(); }
    bool operator< ( const pointer &other ) const { return getP() <  other.getP() || ( getP() == other.getP() && getB() <  other.getB() ); }
    bool operator> ( const pointer &other ) const { return getP() >  other.getP() || ( getP() == other.getP() && getB() >  other.getB() ); }
    bool operator<=( const pointer &other ) const { return getP() <  other.getP() || ( getP() == other.getP() && getB() <= other.getB() ); }
    bool operator>=( const pointer &other ) const { return getP() >  other.getP() || ( getP() == other.getP() && getB() >= other.getB() ); }
    bool operator==( const const_pointer &other ) const { return getP() == other.getP() && getB() == other.getB(); }
    bool operator!=( const const_pointer &other ) const { return getP() != other.getP() || getB() != other.getB(); }
    bool operator< ( const const_pointer &other ) const { return getP() <  other.getP() || ( getP() == other.getP() && getB() <  other.getB() ); }
    bool operator> ( const const_pointer &other ) const { return getP() >  other.getP() || ( getP() == other.getP() && getB() >  other.getB() ); }
    bool operator<=( const const_pointer &other ) const { return getP() <  other.getP() || ( getP() == other.getP() && getB() <= other.getB() ); }
    bool operator>=( const const_pointer &other ) const { return getP() >  other.getP() || ( getP() == other.getP() && getB() >= other.getB() ); }

    uint8_t *getP( void ) const { return m_p; }
     int8_t  getB( void ) const { return m_b; }
  private:
    uint8_t *m_p;
     int8_t  m_b;
  };

  class const_pointer
  {
    friend class ArrayContainer< bool >;
  public:
    const_pointer( void )
    : m_p( 0 )
    , m_b( 0 )
    {}

    const_pointer( const pointer &p )
    : m_p( p.getP() )
    , m_b( p.getB() )
    {}

    ~const_pointer( void ) {}

    const_pointer( const elements_t &elements, index_t i )
    : m_p( &elements[ i >> 3 ] )
    , m_b( i & 0x7 )
    {}

    const_pointer( const uint8_t *p, int8_t b )
    : m_p( p )
    , m_b( b )
    {}

    void swap( const_pointer &other )
    {
      std::swap( m_p, other.m_p );
      std::swap( m_b, other.m_b );
    }

    typedef SaveBool::bool_type bool_type;
    operator bool_type( void ) const { return SaveBool()( bool( m_p || m_b ) ); }

    const_reference operator*( void ) const { return const_reference( m_p, m_b ); }

    const_reference operator[]( ptrdiff_t i ) const { return *( (*this) + i ); }

    const_pointer &operator=( const pointer &p )
    {
      m_p = p.getP();
      m_b = p.getB();
      return (*this);
    }

    ptrdiff_t operator-( const const_pointer &other ) const
    {
      return ( m_p - other.m_p ) * 8 + ( m_b - other.m_b );
    }

    const_pointer &operator+=( ptrdiff_t i )
    {
      const ptrdiff_t b = ptrdiff_t( m_b ) + i;
      m_b  = ( b & 0x7 );
      m_p += ( b >> 3 );
      return (*this);
    }

    const_pointer &operator-=( ptrdiff_t i )
    {
      return operator+=( -i );
    }

    const_pointer operator+( ptrdiff_t i ) const
    {
      const_pointer ret( (*this) );
      return ( ret += i );
    }

    const_pointer operator-( ptrdiff_t i ) const
    {
      return operator+( -i );
    }

    const_pointer &operator++( void )
    {
      return (*this) += 1;
    }

    const_pointer operator++( int )
    {
      const const_pointer ret( (*this) );
      (*this) += 1;
      return ret;
    }

    const_pointer &operator--( void )
    {
      return (*this) -= 1;
    }

    const_pointer operator--( int )
    {
      const const_pointer ret( (*this) );
      (*this) -= 1;
      return ret;
    }

    bool operator==( const pointer &other ) const { return getP() == other.getP() && getB() == other.getB(); }
    bool operator!=( const pointer &other ) const { return getP() != other.getP() || getB() != other.getB(); }
    bool operator< ( const pointer &other ) const { return getP() <  other.getP() || ( getP() == other.getP() && getB() <  other.getB() ); }
    bool operator> ( const pointer &other ) const { return getP() >  other.getP() || ( getP() == other.getP() && getB() >  other.getB() ); }
    bool operator<=( const pointer &other ) const { return getP() <  other.getP() || ( getP() == other.getP() && getB() <= other.getB() ); }
    bool operator>=( const pointer &other ) const { return getP() >  other.getP() || ( getP() == other.getP() && getB() >= other.getB() ); }
    bool operator==( const const_pointer &other ) const { return m_p == other.m_p && m_b == other.m_b; }
    bool operator!=( const const_pointer &other ) const { return m_p != other.m_p || m_b != other.m_b; }
    bool operator< ( const const_pointer &other ) const { return m_p <  other.m_p || ( m_p == other.m_p && m_b <  other.m_b ); }
    bool operator> ( const const_pointer &other ) const { return m_p >  other.m_p || ( m_p == other.m_p && m_b >  other.m_b ); }
    bool operator<=( const const_pointer &other ) const { return m_p <  other.m_p || ( m_p == other.m_p && m_b <= other.m_b ); }
    bool operator>=( const const_pointer &other ) const { return m_p >  other.m_p || ( m_p == other.m_p && m_b >= other.m_b ); }

    const uint8_t *getP( void ) const { return m_p; }
     int8_t  getB( void ) const { return m_b; }
  private:
    const uint8_t *m_p;
     int8_t  m_b;
  };

  typedef pointer       iterator;
  typedef const_pointer const_iterator;

  explicit
  ArrayContainer( index_t n )
  : m_elements( ( n + 7 ) / 8 )
  , m_size( n )
  {}
  ~ArrayContainer( void ) {}

  void clear( void ) { m_elements.clear(); m_size = 0; }
  index_t size( void ) const { return m_size; }
  bool empty( void ) const { return m_elements.empty(); }
  index_t capacity( void ) const { return index_t( m_elements.size() * 8 ); }

  iterator       begin( void )       { return iterator( m_elements, 0 ); }
  const_iterator begin( void ) const { return const_iterator( m_elements, 0 ); }
  iterator       end( void )       { return iterator( m_elements, m_size ); }
  const_iterator end( void ) const { return const_iterator( m_elements, m_size ); }

  reference       operator[]( index_t i )       { return reference      ( m_elements, i ); }
  const_reference operator[]( index_t i ) const { return const_reference( m_elements, i ); }

  pointer       first( void )       { return pointer      ( m_elements, 0 ); }
  const_pointer first( void ) const { return const_pointer( m_elements, 0 ); }

private:
  elements_t m_elements;
  index_t m_size;
};

static inline
ArrayContainer< bool >::pointer operator+( ptrdiff_t n, const ArrayContainer< bool >::pointer &i )
{
  return ( i + n );
}

static inline
ArrayContainer< bool >::const_pointer operator+( ptrdiff_t n, const ArrayContainer< bool >::const_pointer &i )
{
  return ( i + n );
}

template< class _Element >
class ArrayData : public Collectable
{
  typedef Collectable Base;
protected:
  typedef ArrayContainer< _Element > Container;
public:
  typedef _Element Element;
  typedef typename Container::pointer       pointer;
  typedef typename Container::const_pointer const_pointer;
  typedef typename Container::reference       reference;
  typedef typename Container::const_reference const_reference;
  typedef typename Container::iterator       iterator;
  typedef typename Container::const_iterator const_iterator;

  explicit
  ArrayData( GarbageCollector *_gc, int dims, const index_t *s )
  : Base( _gc )
  , m_elements( pointer() )
  , m_capacity( 0 )
  , m_dims( dims )
  {
    std::copy( s, s+dims, m_size );
  }

  explicit
  ArrayData( GarbageCollector *_gc, const ArrayBase::Size &s )
  : Base( _gc )
  , m_elements( pointer() )
  , m_capacity( 0 )
  , m_dims( s.getDims() )
  {
    std::copy( s.getCoords(), s.getCoords()+m_dims, m_size );
  }

  virtual ~ArrayData( void )
  {}

  virtual void gc_clear( void )
  {
    m_elements = pointer();
  }

  virtual void gc_getChildren( Children & ) const
  {}

  pointer first( void )
  {
    return m_elements;
  }

  pointer getElement( int dims, const index_t *x )
  {
  #ifndef NDEBUG
    if( m_dims != dims ) throw ArrayBase::DimensionsMismatch( dims, m_dims );
  #endif
    index_t pos = 0;
    if( dims > 0 )
    {
      for( int d=dims-1; d>0; --d )
      {
        pos += x[ d ];
        pos *= m_size[ d-1 ];
      }
      pos += x[ 0 ];
    }
    return m_elements + pos;
  }

  index_t capacity( void ) const
  {
    return m_capacity;
  }

  ArrayBase::Size getSize( void ) const
  {
    return ArrayBase::Size( m_dims, m_size );
  }

  bool getCoords( const_pointer e, int dims, index_t *x ) const
  {
  #ifndef NDEBUG
    if( m_dims != dims ) throw ArrayBase::DimensionsMismatch( dims, m_dims );
  #endif
    const const_pointer this_e = m_elements;
    if( e < this_e || e >= this_e + m_capacity )
    {
      return false;
    }
    index_t pos = index_t( e - this_e ); // checked above
    for( int d=0; d<dims; ++d )
    {
      x[ d ] = pos % m_size[ d ];
      pos /= m_size[ d ];
    }
    return true;
  }

protected:
  static
  index_t calc_size( int dims, const index_t *s )
  {
    index_t ret = 1;
    for( int d=0; d<dims; ++d )
    {
      ret *= s[ d ];
    }
    return ret;
  }

  void setElements( pointer _elements, index_t _capacity )
  {
    m_elements = _elements;
    m_capacity = _capacity;
  }

private:
  pointer m_elements;
  index_t m_capacity;
  index_t m_size[ 4 ];
  const int m_dims;
};

template< class _Element >
class ContainerArrayData : public ArrayData< _Element >
{
  typedef ArrayData< _Element > Base;
  typedef typename Base::Container Container;
public:
  typedef typename Base::Element Element;
  typedef typename Base::pointer       pointer;
  typedef typename Base::const_pointer const_pointer;
  typedef typename Base::reference       reference;
  typedef typename Base::const_reference const_reference;
  typedef typename Base::iterator       iterator;
  typedef typename Base::const_iterator const_iterator;

  explicit
  ContainerArrayData( GarbageCollector *_gc, int dims, const index_t *s )
  : Base( _gc, dims, s )
  , m_container( Base::calc_size( dims, s ) )
  {
    if( !m_container.empty() )
    {
      Base::setElements( &m_container[ 0 ], index_t( m_container.size() ) );
    }
  }

  explicit
  ContainerArrayData( GarbageCollector *_gc, const ArrayBase::Size &s )
  : Base( _gc, s )
  , m_container( Base::calc_size( s.getDims(), s.getCoords() ) )
  {
    if( !m_container.empty() )
    {
      Base::setElements( &m_container[ 0 ], index_t( m_container.size() ) );
    }
  }

  virtual ~ContainerArrayData( void )
  {}

  virtual void gc_clear( void )
  {
    Base::gc_clear();
    m_container.clear();
  }

  virtual void gc_getChildren( Collectable::Children &children ) const
  {
    Base::gc_getChildren( children );
    if( isCollectable( (Element*)0 ) )
    {
      for( const_iterator i( m_container.begin() ), end( m_container.end() ); i != end; ++i )
      {
        children << (*i);
      }
    }
  }

private:
  Container m_container;
};

} // namespace RPGML

namespace std {

static inline
std::ostream &operator<<( std::ostream &o, const RPGML::ArrayContainer< bool >::const_pointer &p )
{
  o << p.getP() << "." << int( p.getB() );
  return o;
}

static inline
std::ostream &operator<<( std::ostream &o, const RPGML::ArrayContainer< bool >::pointer &p )
{
  o << p.getP() << "." << int( p.getB() );
  return o;
}

template<>
struct iterator_traits< RPGML::ArrayContainer< bool >::pointer >
{
  typedef random_access_iterator_tag               iterator_category;
  typedef bool                                     value_type;
  typedef RPGML::ArrayContainer< bool >::pointer   pointer;
  typedef RPGML::ArrayContainer< bool >::reference reference;
  typedef ptrdiff_t                                difference_tpe;
};

template<>
struct iterator_traits< RPGML::ArrayContainer< bool >::const_pointer >
{
  typedef random_access_iterator_tag                     iterator_category;
  typedef bool                                           value_type;
  typedef RPGML::ArrayContainer< bool >::const_pointer   const_pointer;
  typedef RPGML::ArrayContainer< bool >::const_reference const_reference;
  typedef ptrdiff_t                                      difference_tpe;
};

static inline
void swap( RPGML::ArrayContainer< bool >::pointer &x1, RPGML::ArrayContainer< bool >::pointer &x2 )
{
  x1.swap( x2 );
}

static inline
void swap( RPGML::ArrayContainer< bool >::const_pointer &x1, RPGML::ArrayContainer< bool >::const_pointer &x2 )
{
  x1.swap( x2 );
}

} // namespace std

#endif

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
#ifndef RPGML_iterator_base_h
#define RPGML_iterator_base_h

#include "types.h"

#include <algorithm>
#include <iterator>

namespace RPGML {

template< class _T, class _reference = _T&, class _pointer = _T*>
class iterator_base : public std::iterator< std::random_access_iterator_tag, _T >
{
public:
  typedef _T T;
  typedef _T value_type;
  typedef _reference reference;
  typedef _pointer pointer;

  iterator_base( void )
  : m_size( 0 )
  , m_stride( 0 )
  , m_p( pointer() )
  , m_dims( 0 )
  {
    std::fill( m_pos, m_pos+4, 0 );
  }

  template< class T2, class reference2, class pointer2 >
  iterator_base( const iterator_base< T2, reference2, pointer2 > &other )
  : m_size  ( other.getSize()   )
  , m_stride( other.getStride() )
  , m_p     ( other.get()       )
  , m_dims  ( other.getDims()   )
  {
    std::copy( other.getPos(), other.getPos()+4, m_pos );
  }

  explicit
  iterator_base( const ArrayBase::Size &s, const stride_t *stride, pointer element0, const index_t *pos=0 )
  : m_size( s.getCoords() )
  , m_stride( stride )
  , m_p( element0 )
  , m_dims( s.getDims() )
  {
    if( pos )
    {
      const int dims = m_dims;
      std::copy( pos, pos+dims, m_pos );
      std::fill( m_pos+dims, m_pos+4, index_t( 0 ) );
      for( int d = 0; d < dims; ++d )
      {
        m_p += stride_t( m_pos[ d ] ) * m_stride[ d ];
      }
    }
    else
    {
      std::fill( m_pos, m_pos+4, 0 );
    }
  }

protected:
  void inc( void )
  {
    const int dims = m_dims;
    if( 0 == dims ) ++m_p;
    for( int d = 0; d < dims; ++d )
    {
      ++m_pos[ d ];
      m_p += m_stride[ d ];

      if( m_pos[ d ] < m_size[ d ] )
      {
        break;
      }
      else if( d < dims-1 )
      {
        m_p -= stride_t( m_pos[ d ] ) * m_stride[ d ];
        m_pos[ d ] = 0;
      }
    }
  }

  void dec( void )
  {
    const int dims = m_dims;
    if( 0 == dims ) --m_p;
    for( int d = 0; d < dims; ++d )
    {
      if( m_pos[ d ] > 0 )
      {
        --m_pos[ d ];
        m_p -= m_stride[ d ];
        break;
      }
      else if( d < dims-1 )
      {
        m_p += stride_t( m_size[ d ]-1 ) * m_stride[ d ];
        m_pos[ d ] = m_size[ d ]-1;
      }
    }
  }

  void inc( stride_t x )
  {
    if( x < 0 ) { dec( -x ); return; }
    const int dims = m_dims;
    if( 0 == dims ) { m_p += x; return; }
    for( int d = 0; d < dims; ++d )
    {
      const stride_t size_d = m_size[ d ];
      const stride_t old_pos_d = stride_t( m_pos[ d ] );
      const stride_t new_pos_d = ( old_pos_d + x ) % size_d;

      m_pos[ d ] = index_t( new_pos_d );
      m_p += ( new_pos_d - old_pos_d ) * m_stride[ d ];
      x -= ( new_pos_d - old_pos_d );

      if( 0 == x ) break;

      assert( 0 == x % size_d );
      x /= size_d;
    }
  }

  void dec( stride_t x )
  {
    if( x < 0 ) { inc( -x ); return; }
    const int dims = m_dims;
    if( 0 == dims ) m_p -= x;
    for( int d = 0; d < dims; ++d )
    {
      const stride_t size_d = m_size[ d ];
      const stride_t old_pos_d = stride_t( m_pos[ d ] );
      const stride_t new_pos_d =
        ( old_pos_d < x
        ? ( ( old_pos_d - x ) % size_d ) + size_d
        : old_pos_d - x
        );

      m_pos[ d ] = index_t( new_pos_d );
      m_p -= ( old_pos_d - new_pos_d ) * m_stride[ d ];
      x -= ( old_pos_d - new_pos_d );

      if( 0 == x ) break;

      assert( 0 == x % size_d );
      x /= size_d;
    }
  }

public:
  template< class T2, class reference2, class pointer2 >
  stride_t diff( const iterator_base< T2, reference2, pointer2 > &other ) const
  {
    if( sizeof( T ) != sizeof( T2 ) )
    {
      throw Exception()
        << "Trying to diff iterators of different type"
        << ": This is " << getTypeName< T >()
        << ", other is " << getTypeName< T2 >()
        ;
    }
    const int dims = getDims();
    const index_t *const size = getSize();
  #ifndef NDEBUG
    if( dims != other.getDims() )
    {
      throw Exception()
        << "Trying to diff iterators of different dimensions"
        << ": This is " << dims
        << ", other is " << other.getDims()
        ;
    }
    if( size != other.getSize() )
    {
      throw Exception() << "Trying to diff iterators of different Arrays";
    }
  #endif
    const index_t *const this_pos = getPos();
    const index_t *const other_pos = other.getPos();

    stride_t s = 1;
    stride_t ret = 0;
    for( int d=0; d<dims; ++d )
    {
      ret += ( stride_t( this_pos[ d ] ) - stride_t( other_pos[ d ] ) ) * s;
      s *= size[ d ];
    }

    return ret;
  }

  void swap( iterator_base &other )
  {
    std::swap( m_size  , other.m_size   );
    std::swap( m_stride, other.m_stride );
    std::swap( m_p     , other.m_p      );
    std::swap( m_dims  , other.m_dims   );
    for( int d = 0; d < 4; ++d )
    {
      std::swap( m_pos[ d ], other.m_pos[ d ] );
    }
  }

  iterator_base &operator++( void ) { this->inc(); return (*this); }
  iterator_base operator++( int ) { iterator_base ret( *this ); this->inc(); return ret; }
  iterator_base &operator--( void ) { this->dec(); return (*this); }
  iterator_base operator--( int ) { iterator_base ret( *this ); this->dec(); return ret; }
  reference operator*( void ) const { return *this->get(); }
  pointer operator->( void ) const { return this->get(); }
  template< class T2 >
  iterator_base &operator=( const iterator_base< T2 > &other )
  {
    iterator_base tmp( other );
    this->swap( tmp );
    return (*this);
  }
  iterator_base &operator+=( stride_t x ) { this->inc( x ); return (*this); }
  iterator_base &operator-=( stride_t x ) { this->dec( x ); return (*this); }
  reference operator[]( stride_t x ) const { return *( (*this) + x ); }

  const pointer  &get      ( void ) const { return m_p; }
  const index_t  *getPos   ( void ) const { return m_pos; }
  const index_t  &getPos   ( int d ) const { return m_pos[ d ]; }
  int             getDims  ( void ) const { return m_dims; }
  const index_t  *getSize  ( void ) const { return m_size; }
  const index_t  &getSize  ( int d ) const { return m_size[ d ]; }
  const stride_t *getStride( void ) const { return m_stride; }
  const stride_t &getStride( int d ) const { return m_stride[ d ]; }

private:
  index_t m_pos[ 4 ];
  const index_t *m_size;
  const stride_t *m_stride;
  pointer m_p;
  int m_dims;
};

template< class Integer, class Element, class _reference, class _pointer >
iterator_base< Element, _reference, _pointer > operator+( const iterator_base< Element, _reference, _pointer > &i, Integer x )
{
  iterator_base< Element, _reference, _pointer > ret( i );
  return ( ret += stride_t( x ) );
}

template< class Integer, class Element, class _reference, class _pointer >
iterator_base< Element, _reference, _pointer > operator-( const iterator_base< Element, _reference, _pointer > &i, Integer x )
{
  iterator_base< Element, _reference, _pointer > ret( i );
  return ( ret -= stride_t( x ) );
}

template< class Integer, class Element, class _reference, class _pointer >
iterator_base< Element, _reference, _pointer > operator+( Integer x , const iterator_base< Element, _reference, _pointer > &i )
{
  iterator_base< Element, _reference, _pointer > ret( i );
  return ( ret += stride_t( x ) );
}

template< class Element1, class _reference1, class _pointer1, class Element2, class _reference2, class _pointer2 >
stride_t operator-(
    const iterator_base< Element1, _reference1, _pointer1 > &left
  , const iterator_base< Element2, _reference2, _pointer2 > &right
  )
{
  return left.diff( right );
}

template< class Element1, class _reference1, class _pointer1, class Element2, class _reference2, class _pointer2 >
bool operator==(
    const iterator_base< Element1, _reference1, _pointer1 > &left
  , const iterator_base< Element2, _reference2, _pointer2 > &right
  )
{
  return ( left.get() == right.get() );
}

template< class Element1, class _reference1, class _pointer1, class Element2, class _reference2, class _pointer2 >
bool operator!=(
    const iterator_base< Element1, _reference1, _pointer1 > &left
  , const iterator_base< Element2, _reference2, _pointer2 > &right
  )
{
  return ( left.get() != right.get() );
}

template< class Element1, class _reference1, class _pointer1, class Element2, class _reference2, class _pointer2 >
bool operator<(
    const iterator_base< Element1, _reference1, _pointer1 > &left
  , const iterator_base< Element2, _reference2, _pointer2 > &right
  )
{
  return ( left.get() < right.get() );
}

template< class Element1, class _reference1, class _pointer1, class Element2, class _reference2, class _pointer2 >
bool operator<=(
    const iterator_base< Element1, _reference1, _pointer1 > &left
  , const iterator_base< Element2, _reference2, _pointer2 > &right
  )
{
  return ( left.get() <= right.get() );
}

template< class Element1, class _reference1, class _pointer1, class Element2, class _reference2, class _pointer2 >
bool operator>(
    const iterator_base< Element1, _reference1, _pointer1 > &left
  , const iterator_base< Element2, _reference2, _pointer2 > &right
  )
{
  return ( left.get() > right.get() );
}

template< class Element1, class _reference1, class _pointer1, class Element2, class _reference2, class _pointer2 >
bool operator>=(
    const iterator_base< Element1, _reference1, _pointer1 > &left
  , const iterator_base< Element2, _reference2, _pointer2 > &right
  )
{
  return ( left.get() >= right.get() );
}

} // namespace RPGML

#endif


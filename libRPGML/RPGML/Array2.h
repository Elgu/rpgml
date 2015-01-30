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
#ifndef RPGML_Array2_h
#define RPGML_Array2_h

#include "ArrayBase.h"
#include "ArrayData.h"
#include "Type.h"
#include "Exception.h"
#include "GarbageCollector.h"

#include <cassert>
#include <vector>

namespace RPGML {

template< class _Element >
class Array2;

template< class _Element >
class Array2 : public ArrayBase
{
  typedef ArrayBase Base;
public:
  typedef _Element Element;

  typedef int32_t stride_t;
  typedef Base::ConstValueIterator     ConstValueIterator;
  typedef Base::CoordinatesIterator    CoordinatesIterator;
  typedef Base::Children               Children;
  typedef Base::Coordinates            Coordinates;
  typedef Base::Size                   Size;
  typedef Base::OutOfRange             OutOfRange;
  typedef Base::DimensionsMismatch     DimensionsMismatch;
  typedef typename ArrayData< Element >::      pointer       pointer;
  typedef typename ArrayData< Element >::const_pointer const_pointer;
  typedef typename ArrayData< Element >::      reference       reference;
  typedef typename ArrayData< Element >::const_reference const_reference;
  typedef RPGML::Iterator<       reference >      Iterator;
  typedef RPGML::Iterator< const_reference > ConstIterator;

  explicit
  Array2( GarbageCollector *_gc, int dims )
  : Base( _gc )
  , m_element0( pointer() )
  , m_dims( dims )
  {
    _check_dims();
    std::fill( m_size, m_size+dims, 0 );
    std::fill( m_size+dims, m_size+4, 1 );
    if( dims == 0 ) resize_v( 0, 0 );
  }

  explicit
  Array2( GarbageCollector *_gc, int dims, index_t sx )
  : Base( _gc )
  , m_element0( pointer() )
  , m_dims( dims )
  {
    _check_dims( 1 );
    std::fill( m_size, m_size+1, 0 );
    std::fill( m_size+1, m_size+4, 1 );
    resize( sx );
  }

  explicit
  Array2( GarbageCollector *_gc, int dims, index_t sx, index_t sy )
  : Base( _gc )
  , m_element0( pointer() )
  , m_dims( dims )
  {
    _check_dims( 2 );
    std::fill( m_size, m_size+2, 0 );
    std::fill( m_size+2, m_size+4, 1 );
    resize( sx, sy );
  }

  explicit
  Array2( GarbageCollector *_gc, int dims, index_t sx, index_t sy, index_t sz )
  : Base( _gc )
  , m_element0( pointer() )
  , m_dims( dims )
  {
    _check_dims( 3 );
    std::fill( m_size, m_size+3, 0 );
    std::fill( m_size+3, m_size+4, 1 );
    resize( sx, sy, sz );
  }

  explicit
  Array2( GarbageCollector *_gc, int dims, index_t sx, index_t sy, index_t sz, index_t st )
  : Base( _gc )
  , m_element0( pointer() )
  , m_dims( dims )
  {
    _check_dims( 4 );
    std::fill( m_size, m_size+4, 0 );
    resize( sx, sy, sz, st );
  }

  explicit
  Array2( GarbageCollector *_gc, int dims, const index_t *s )
  : Base( _gc )
  , m_element0( pointer() )
  , m_dims( dims )
  {
    _check_dims();
    std::fill( m_size, m_size+dims, 0 );
    std::fill( m_size+dims, m_size+4, 1 );
    resize_v( dims, s );
  }

  explicit
  Array2( GarbageCollector *_gc, const Size &s )
  : Base( _gc )
  , m_element0( pointer() )
  , m_dims( s.getDims() )
  {
    _check_dims();
    const int dims = s.getDims();
    std::fill( m_size, m_size+dims, 0 );
    std::fill( m_size+dims, m_size+4, 1 );
    resize( s );
  }

  virtual ~Array2( void )
  {}

  virtual void gc_clear( void )
  {
    m_element0 = pointer();
    m_elements_data.reset();
  }

  virtual void gc_getChildren( Children &children ) const
  {
    children << m_elements_data;
  }

  template< class _T >
  class iterator_base : public std::iterator< std::bidirectional_iterator_tag, _T >
  {
  public:
    typedef _T T;
    typedef ArrayData< typename RemoveConst< T >::T > ArrayData_T;
    static const bool is_const = IsConst< T >::B;
    typedef typename ArrayData_T::const_pointer const_T_pointer;
    typedef typename ArrayData_T::      pointer nonconst_T_pointer;
    typedef typename SelectType< int( is_const ), nonconst_T_pointer, const_T_pointer >::T T_pointer;

    iterator_base( void )
    : m_size( 0 )
    , m_stride( 0 )
    , m_p( 0 )
    , m_dims( 0 )
    {
      std::fill( m_pos, m_pos+4, 0 );
    }

    template< class T2 >
    iterator_base( const iterator_base< T2 > &other )
    : m_size  ( other.getSize()   )
    , m_stride( other.getStride() )
    , m_p     ( other.get()       )
    , m_dims  ( other.getDims()   )
    {
      std::copy( other.getPos(), other.getPos()+4, m_pos );
    }

  protected:
    explicit
    iterator_base( const Size &s, const stride_t *stride, T_pointer element0, const index_t *pos=0 )
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

  public:
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

    T_pointer       get      ( void ) const { return m_p; }
    const index_t  *getPos   ( void ) const { return m_pos; }
    int             getDims  ( void ) const { return m_dims; }
    const index_t  *getSize  ( void ) const { return m_size; }
    const stride_t *getStride( void ) const { return m_stride; }

  private:
    index_t m_pos[ 4 ];
    const index_t *m_size;
    const stride_t *m_stride;
    T_pointer m_p;
    int m_dims;
  };

  class iterator : public iterator_base< Element >
  {
    friend class Array2;
    typedef iterator_base< Element > Base;
  public:
    typedef typename Base::      T_pointer       pointer;
    typedef typename Base::const_T_pointer const_pointer;
    iterator( void ) {}
  protected:
    iterator( const Size &s, const stride_t *stride, pointer element0, const index_t *pos=0 )
    : Base( s, stride, element0, pos )
    {}
  public:
    bool operator==( const iterator_base<       Element > &other ) const { return ( this->get() == other.get() ); }
    bool operator==( const iterator_base< const Element > &other ) const { return ( this->get() == other.get() ); }
    bool operator!=( const iterator_base<       Element > &other ) const { return ( this->get() != other.get() ); }
    bool operator!=( const iterator_base< const Element > &other ) const { return ( this->get() != other.get() ); }
    iterator &operator++( void ) { this->inc(); return (*this); }
    iterator operator++( int ) { iterator ret( *this ); this->inc(); return ret; }
    iterator &operator--( void ) { this->dec(); return (*this); }
    iterator operator--( int ) { iterator ret( *this ); this->dec(); return ret; }
    reference operator*( void ) const { return *this->get(); }
    pointer operator->( void ) const { return this->get(); }
    iterator &operator=( const iterator &other )
    {
      iterator tmp( other );
      this->swap( tmp );
      return (*this);
    }
  };

  class const_iterator : public iterator_base< const Element >
  {
    friend class Array2;
    typedef iterator_base< const Element > Base;
  public:
    typedef typename Base::      T_pointer       pointer;
    typedef typename Base::const_T_pointer const_pointer;
    const_iterator( void ) {}
    const_iterator( const iterator &other ) : Base( other ) {}
  protected:
    const_iterator( const Size &s, const stride_t *stride, const_pointer element0, const index_t *pos=0 )
    : Base( s, stride, element0, pos )
    {}
  public:
    bool operator==( const iterator_base<       Element > &other ) const { return ( this->get() == other.get() ); }
    bool operator==( const iterator_base< const Element > &other ) const { return ( this->get() == other.get() ); }
    bool operator!=( const iterator_base<       Element > &other ) const { return ( this->get() != other.get() ); }
    bool operator!=( const iterator_base< const Element > &other ) const { return ( this->get() != other.get() ); }
    const_iterator &operator++( void ) { this->inc(); return (*this); }
    const_iterator operator++( int ) { const_iterator ret( *this ); this->inc(); return ret; }
    const_iterator &operator--( void ) { this->dec(); return (*this); }
    const_iterator operator--( int ) { const_iterator ret( *this ); this->dec(); return ret; }
    const_reference operator*( void ) const { return *this->get(); }
    const_pointer operator->( void ) const { return this->get(); }
    const_iterator &operator=( const const_iterator &other )
    {
      const_iterator tmp( other );
      this->swap( tmp );
      return (*this);
    }
    const_iterator &operator=( const iterator &other )
    {
      const_iterator tmp( other );
      swap( tmp );
      return (*this);
    }
  };

  iterator begin( void )
  {
    return iterator( getSize(), getStride(), elements() );
  }

  const_iterator begin( void ) const
  {
    return const_iterator( getSize(), getStride(), elements() );
  }

  iterator end( void )
  {
    const int dims = m_dims;
    if( dims > 0 )
    {
      index_t end_pos[ dims ];
      if( dims > 1 ) std::fill( end_pos, end_pos+(dims-1), 0 );
      end_pos[ dims-1 ] = m_size[ dims-1 ];
      return iterator( getSize(), getStride(), elements(), end_pos );
    }
    else
    {
      return iterator( getSize(), getStride(), elements()+1 );
    }
  }

  const_iterator end( void ) const
  {
    const int dims = m_dims;
    if( dims > 0 )
    {
      index_t end_pos[ dims ];
      if( dims > 1 ) std::fill( end_pos, end_pos+(dims-1), 0 );
      end_pos[ dims-1 ] = m_size[ dims-1 ];
      return const_iterator( getSize(), getStride(), elements(), end_pos );
    }
    else
    {
      return const_iterator( getSize(), getStride(), elements()+1 );
    }
  }

  class _ConstValueIterator : public ConstValueIterator
  {
  public:
    explicit
    _ConstValueIterator( const Array2 *array ) : m_i( array->begin() ), m_end( array->end() ) {}
    virtual ~_ConstValueIterator( void ) {}
    virtual bool done( void ) { return ( m_i == m_end ); }
    virtual void next( void ) { ++m_i; }
    virtual Value get( void ) { return CreateValue< Element >()( *m_i ); }
    virtual CountPtr< ConstValueIterator > clone( void ) const { return new _ConstValueIterator( *this ); }
  private:
    const_iterator m_i;
    const_iterator m_end;
  };

  virtual CountPtr< ConstValueIterator > getConstValueIterator( void ) const
  {
    return new _ConstValueIterator( this );
  }

  class _CoordinatesIterator : public CoordinatesIterator
  {
  public:
    explicit
    _CoordinatesIterator( const Array2 *array ) : m_i( array->begin() ), m_end( array->end() ) {}
    virtual ~_CoordinatesIterator( void ) {}
    virtual bool done( void ) { return ( m_i == m_end ); }
    virtual void next( void ) { ++m_i; }
    virtual Coordinates get( void ) { return Coordinates( m_i.getDims(), m_i.getPos() ); }
    virtual CountPtr< CoordinatesIterator > clone( void ) const { return new _CoordinatesIterator( *this ); }
  private:
    const_iterator m_i;
    const_iterator m_end;
  };

  virtual CountPtr< CoordinatesIterator > getCoordinatesIterator( void ) const
  {
    return new _CoordinatesIterator( this );
  }

  class _Iterator : public Iterator
  {
  public:
    explicit
    _Iterator( Array2 *array ) : m_i( array->begin() ), m_end( array->end() ) {}
    virtual ~_Iterator( void ) {}
    virtual bool done( void ) { return ( m_i == m_end ); }
    virtual void next( void ) { ++m_i; }
    virtual reference get( void ) { return (*m_i); }
    virtual CountPtr< Iterator > clone( void ) const { return new _Iterator( *this ); }
  private:
    iterator m_i;
    iterator m_end;
  };

  class _ConstIterator : public ConstIterator
  {
  public:
    explicit
    _ConstIterator( const Array2 *array ) : m_i( array->begin() ), m_end( array->end() ) {}
    virtual ~_ConstIterator( void ) {}
    virtual bool done( void ) { return ( m_i == m_end ); }
    virtual void next( void ) { ++m_i; }
    virtual const_reference get( void ) { return (*m_i); }
    virtual CountPtr< ConstIterator > clone( void ) const { return new _ConstIterator( *this ); }
  private:
    const_iterator m_i;
    const_iterator m_end;
  };

  virtual CountPtr< Iterator > getIterator( void )
  {
    return new _Iterator( this );
  }

  virtual CountPtr< ConstIterator > getIterator( void ) const
  {
    return new _ConstIterator( this );
  }

  virtual CountPtr< ConstIterator > getConstIterator( void ) const
  {
    return new _ConstIterator( this );
  }

  virtual const std::type_info &getTypeInfo( void ) const { return typeid( *this ); }

  virtual ArrayBase &clear( void )
  {
    m_element0 = pointer();
    m_elements_data.reset();
    std::fill( m_size, m_size+m_dims, 0 );
    std::fill( m_stride, m_stride+m_dims, 0 );
    return (*this);
  }

  bool empty( void ) const
  {
    if( m_element0 == pointer() ) return true;
    for( int d( 0 ), dims( m_dims ); d<dims; ++d )
    {
      if( 0 == m_size[ d ] ) return true;
    }
    return false;
  }

  virtual index_t        getDims( void ) const
  {
    return m_dims;
  }

  virtual Type           getType( void ) const
  {
    return Type( TypeOf< Element >::E );
  }

  virtual Size getSize( void ) const
  {
    return Size( m_dims, m_size );
  }

  virtual index_t size( void ) const
  {
    index_t ret = 1;
    for( int d( 0 ), dims( m_dims ); d<dims; ++d )
    {
      ret *= m_size[ d ];
    }
    return ret;
  }

  bool isDense( void ) const
  {
    if( m_element0 == pointer() ) return true;
    stride_t dense_stride = 1;
    for( int d( 0 ), dims( m_dims ); d<dims; ++d )
    {
      if( 0 == m_size[ d ] ) return true;
      if( m_stride[ d ] != dense_stride ) return false;
      dense_stride *= m_size[ d ];
    }
    return true;
  }

  const stride_t *getStride( void ) const
  {
    return m_stride;
  }

  const_pointer elements( void ) const
  {
    return m_element0;
  }

  pointer elements( void )
  {
    return m_element0;
  }

  void swap( Array2 &other )
  {
    if( m_dims != other.m_dims )
    {
      throw Exception()
        << "Cannot swap Arrays of different dimensions"
        << ": This is " << m_dims
        << ", other is " << other.m_dims
        ;
    }
    m_elements_data.swap( other.m_elements_data );
    std::swap( m_element0, other.m_element0 );
    for( int d( 0 ), dims( m_dims ); d<dims; ++d )
    {
      std::swap( m_size[ d ], other.m_size[ d ] );
      std::swap( m_stride[ d ], other.m_stride[ d ] );
    }
  }

  ArrayBase &resize( index_t sx=1, index_t sy=1, index_t sz=1, index_t st=1 )
  {
    assert( m_dims >= 0 );
    assert( m_dims <= 4 );
    const int dims = m_dims;
    switch( dims )
    {
      case 0: if( sx != 1 ) throw Exception() << "If dims is less than 1 (is " << m_dims << "), sx must be 1 (is " << sx << ")"; // fallthrough
      case 1: if( sy != 1 ) throw Exception() << "If dims is less than 2 (is " << m_dims << "), sy must be 1 (is " << sy << ")"; // fallthrough
      case 2: if( sz != 1 ) throw Exception() << "If dims is less than 3 (is " << m_dims << "), sz must be 1 (is " << sz << ")"; // fallthrough
      case 3: if( st != 1 ) throw Exception() << "If dims is less than 4 (is " << m_dims << "), st must be 1 (is " << st << ")"; // fallthrough
      case 4: {}
    }
    const index_t s[ 4 ] = { sx, sy, sz, st };
    return resize_v( dims, s );
  }

  virtual ArrayBase &resize_v( int dims, const index_t *s )
  {
    if( dims != m_dims )
    {
      throw Exception()
        << "Dimensions specified at resize() (" << dims << ")"
        << " must match dimensions specified at construction (" << m_dims << ")"
        ;
    }

    reserve_v( dims, s );
    std::copy( s, s+dims, m_size );
    return (*this);
  }

  virtual ArrayBase &resize( const Size &s )
  {
    return resize_v( s.getDims(), s.getCoords() );
  }

  virtual CountPtr< ArrayBase > clone( void ) const
  {
    CountPtr< Array2 > ret = new Array2( getGC(), getSize() );
    std::copy( begin(), end(), ret->begin() );
    return ret;
  }

  virtual Value getValue( index_t x=0, index_t y=0, index_t z=0, index_t t=0 ) const
  {
    const int dims = m_dims;
    const index_t xyzt[ 4 ] = { x, y, z, t };
    if( inRange_v( dims, xyzt ) )
    {
      return CreateValue< Element >()( at_v( dims, xyzt ) );
    }
    else
    {
      throw OutOfRange( getSize(), Coordinates( dims, xyzt ) );
    }
  }

  virtual Value getValue_v( int dims, const index_t *x ) const
  {
    if( inRange_v( dims, x ) )
    {
      return CreateValue< Element >()( at_v( dims, x ) );
    }
    else
    {
      throw OutOfRange( getSize(), Coordinates( dims, x ) );
    }
  }

  virtual Value getValue( const Coordinates &x ) const
  {
    return getValue_v( x.getDims(), x );
  }

  virtual void setValue( const Value &value, index_t x=0, index_t y=0, index_t z=0, index_t t=0 )
  {
    const int dims = m_dims;
    const index_t xyzt[ 4 ] = { x, y, z, t };
    if( inRange_v( dims, xyzt ) )
    {
      at_v( dims, xyzt ) = value.get< Element >();
    }
    else
    {
      throw OutOfRange( getSize(), Coordinates( dims, xyzt ) );
    }
  }

  virtual void setValue_v( const Value &value, int dims, const index_t *x )
  {
    if( inRange_v( dims, x ) )
    {
      at_v( dims, x ) = value.get< Element >();
    }
    else
    {
      throw OutOfRange( getSize(), Coordinates( dims, x ) );
    }
  }

  virtual void setValue( const Value &value, const Coordinates &x )
  {
    setValue_v( value, x.getDims(), x );
  }

  void fill( const Element &e )
  {
    std::fill( begin(), end(), e );
  }

  virtual void fillValue( const Value &value )
  {
    fill( value.get< Element >() );
  }

  virtual bool isValue( void ) const
  {
    return _isValue( (const Element*)0 );
  }

  stride_t position( index_t x ) const
  {
    assert( m_dims == 1 );
    return position_v( 1, &x );
  }

  stride_t position( index_t x, index_t y ) const
  {
    assert( m_dims == 2 );
    const index_t X[ 2 ] = { x, y };
    return position_v( 2, X );
  }

  stride_t position( index_t x, index_t y, index_t z ) const
  {
    assert( m_dims == 3 );
    const index_t X[ 3 ] = { x, y, z };
    return position_v( 3, X );
  }

  stride_t position( index_t x, index_t y, index_t z, index_t t ) const
  {
    assert( m_dims == 4 );
    const index_t X[ 4 ] = { x, y, z, t };
    return position_v( 4, X );
  }

  stride_t position_v( int dims, const index_t *x ) const
  {
    // Note: Use dims instead of m_dims here, so inlining will work
    assert( dims == m_dims );
    if( dims > 0 )
    {
      stride_t pos = 0;
//      for( int d = dims-1; d >= 0; --d )
      for( int d = 0; d < dims; ++d ) // faster
      {
        assert( x[ d ] < m_size[ d ] );
        pos += stride_t( x[ d ] ) * m_stride[ d ];
      }
      return pos;
    }
    else
    {
      return 0;
    }
  }

  reference at( index_t x )
  {
    return m_element0[ position( x ) ];
  }

  reference at( index_t x, index_t y )
  {
    return m_element0[ position( x, y ) ];
  }

  reference at( index_t x, index_t y, index_t z )
  {
    return m_element0[ position( x, y, z ) ];
  }

  reference at( index_t x, index_t y, index_t z, index_t t )
  {
    return m_element0[ position( x, y, z, t ) ];
  }

  const_reference at( index_t x ) const
  {
    return m_element0[ position( x ) ];
  }

  const_reference at( index_t x, index_t y ) const
  {
    return m_element0[ position( x, y ) ];
  }

  const_reference at( index_t x, index_t y, index_t z ) const
  {
    return m_element0[ position( x, y, z ) ];
  }

  const_reference at( index_t x, index_t y, index_t z, index_t t ) const
  {
    return m_element0[ position( x, y, z, t ) ];
  }

  reference at_v( int dims, const index_t *x )
  {
    return m_element0[ position_v( dims, x ) ];
  }

  const_reference at_v( int dims, const index_t *x ) const
  {
    return m_element0[ position_v( dims, x ) ];
  }

  const_reference operator[]( index_t x ) const
  {
    return at( x );
  }

  reference operator[]( index_t x )
  {
    return at( x );
  }

  const_reference operator*( void ) const
  {
    assert( m_dims == 0 );
    return *m_element0;
  }

  reference operator*( void )
  {
    assert( m_dims == 0 );
    return *m_element0;
  }

  const_pointer operator->( void ) const
  {
    assert( m_dims == 0 );
    return m_element0;
  }

  pointer operator->( void )
  {
    assert( m_dims == 0 );
    return m_element0;
  }

  bool inRange_v( int dims, const index_t *x ) const
  {
    assert( dims == m_dims );
    for( int d=0; d<dims; ++d )
    {
      if( x[ d ] > m_size[ d ] )
      {
        return false;
      }
    }
    return true;
  }

  CountPtr< const Array2 > getROI( int dims, const index_t *x, const index_t *s ) const
  {
    CountPtr< Array2 > ret = new Array2( (*this) );
    ret->setROI( dims, x, s );
    return ret;
  }

  CountPtr< Array2 > getROI( int dims, const index_t *x, const index_t *s )
  {
    CountPtr< Array2 > ret = new Array2( (*this) );
    ret->setROI( dims, x, s );
    return ret;
  }

  CountPtr< const Array2 > getROI( const Coordinates &x, const Size &s ) const
  {
    CountPtr< Array2 > ret = new Array2( (*this) );
    ret->setROI( x, s );
    return ret;
  }

  CountPtr< Array2 > getROI( const Coordinates &x, const Size &s )
  {
    CountPtr< Array2 > ret = new Array2( (*this) );
    ret->setROI( x, s );
    return ret;
  }

  CountPtr< Array2 > getROI( index_t x, index_t w ) const
  {
    if( 1 != m_dims )
    {
      throw Exception()
        << "Dimensions must be 2 for getROI( x, w ), is " << m_dims
        ;
    }
    CountPtr< Array2 > ret = new Array2( (*this) );
    ret->setROI( 1, &x, &w );
    return ret;
  }

  CountPtr< Array2 > getROI( index_t x, index_t y, index_t sx, index_t sy ) const
  {
    if( 2 != m_dims )
    {
      throw Exception()
        << "Dimensions must be 2 for getROI( x, y, sx, sy ), is " << m_dims
        ;
    }
    CountPtr< Array2 > ret = new Array2( (*this) );
    const index_t X[ 2 ] = { x, y };
    const index_t S[ 2 ] = { sx, sy };
    ret->setROI( 2, X, S );
    return ret;
  }

  CountPtr< Array2 > getROI( index_t x, index_t y, index_t z, index_t sx, index_t sy, index_t sz ) const
  {
    if( 3 != m_dims )
    {
      throw Exception()
        << "Dimensions must be 3 for getROI( x, y, z, sx, sy, sz ), is " << m_dims
        ;
    }
    CountPtr< Array2 > ret = new Array2( (*this) );
    const index_t X[ 3 ] = { x, y, z };
    const index_t S[ 3 ] = { sx, sy, sz };
    ret->setROI( 3, X, S );
    return ret;
  }

  CountPtr< Array2 > getROI(
      index_t x, index_t y, index_t z, index_t t
    , index_t sx, index_t sy, index_t sz, index_t st
    ) const
  {
    if( 4 != m_dims )
    {
      throw Exception()
        << "Dimensions must be 4 for getROI( x, y, z, t, sx, sy, sz, st ), is " << m_dims
        ;
    }
    CountPtr< Array2 > ret = new Array2( (*this) );
    const index_t X[ 4 ] = { x, y, z, t };
    const index_t S[ 4 ] = { sx, sy, sz, st };
    ret->setROI( 4, X, S );
    return ret;
  }

  Array2 &setROI( int dims, const index_t *x, const index_t *s )
  {
    if( dims != m_dims )
    {
      throw Exception()
        << "Dimensions specified for ROI (" << dims << ")"
        << " must match dimensions specified at construction (" << m_dims << ")"
        ;
    }

    for( int d=0; d<dims; ++d )
    {
      if( x[ d ] + s[ d ] > m_size[ d ] )
      {
        throw Exception() << "ROI is out of range";
      }
    }

    m_element0 += position_v( dims, x );
    std::copy( s, s + dims, m_size );

    return (*this);
  }

  Array2 &setROI( const Coordinates &x, const Size &s )
  {
    if( x.getDims() != s.getDims() )
    {
      throw Exception()
        << "ROI position x and size s have different dimensions"
        << ": x is " << x
        << ", s is " << s
        ;
    }
    return setROI( x.getDims(), x.getCoords(), s.getCoords() );
  }

  Array2 &setMirrored( int d )
  {
    if( d < 0 || d >= m_dims )
    {
      throw Exception()
        << "Dimension d out of range in setMirrored(),"
        << " must be at least 0 and less than " << m_dims << ", is " << d
        ;
    }

    m_element0 += stride_t( m_size[ d ]-1 ) * m_stride[ d ];
    m_stride[ d ] = -m_stride[ d ];

    return (*this);
  }

  Array2 &setSparse( int d, stride_t nth, index_t offset=0 )
  {
    if( d < 0 || d >= m_dims )
    {
      throw Exception()
        << "Dimension d out of range in setSparse(),"
        << " must be at least 0 and less than " << m_dims << ", is " << d
        ;
    }

    if( nth < 1 )
    {
      throw Exception() << "Argument 'nth' must be greater than 0 in sparse(), is " << nth;
    }

    if( offset > 0 )
    {
      if( offset >= index_t( nth ) )
      {
        throw Exception() << "Argument 'offset' must be less than 'nth' (" << nth << "), is " << offset;
      }
      m_element0 += stride_t( offset ) * m_stride[ d ];
    }

    m_size[ d ] /= nth;
    m_stride[ d ] *= nth;

    return (*this);
  }

  //! Rotates in screen-coordinates (d1=0("x"), d2=1("y")) clock-wise, cartesian anti-clockwise
  Array2 &setRotated( int times90deg, int d1=0, int d2=1 )
  {
    const int dims = m_dims;
    if( d1 < 0 || d1 >= dims )
    {
      throw Exception()
        << "Dimension d1 out of range in setRotated(),"
        << " must be at least 0 and less than " << dims << ", is " << d1
        ;
    }
    if( d2 < 0 || d2 >= dims )
    {
      throw Exception()
        << "Dimension d2 out of range in setRotated(),"
        << " must be at least 0 and less than " << dims << ", is " << d2
        ;
    }
    if( d1 == d2 )
    {
      throw Exception()
        << "Dimensions d1 and d2 must be different in setRotated(), are " << d1
        ;
    }

    times90deg %= 4;
    if( times90deg < 0 ) times90deg += 4;

    switch( times90deg )
    {
      case 0: return (*this);

      case 1:
        // Move start corner from "top-left" to "top-right"
        m_element0 += stride_t( m_size[ d1 ]-1 ) * m_stride[ d1 ];
        // Swap strides, negate one
        std::swap( m_stride[ d1 ], m_stride[ d2 ] );
        m_stride[ d2 ] = -m_stride[ d2 ];
        // Swap sizes
        std::swap( m_size[ d1 ], m_size[ d2 ] );
        break;

      case 2:
        // Move start corner from "top-left" to "bottom-right"
        m_element0 += stride_t( m_size[ d1 ]-1 ) * m_stride[ d1 ];
        m_element0 += stride_t( m_size[ d2 ]-1 ) * m_stride[ d2 ];
        // Both axes point into opposite directions
        m_stride[ d1 ] = -m_stride[ d1 ];
        m_stride[ d2 ] = -m_stride[ d2 ];
        // Sizes stay the same
        break;

      case 3:
        // Move start corner from "top-left" to "bottom-left"
        m_element0 += stride_t( m_size[ d2 ]-1 ) * m_stride[ d2 ];
        // Swap strides, negate one
        std::swap( m_stride[ d1 ], m_stride[ d2 ] );
        m_stride[ d1 ] = -m_stride[ d1 ];
        // Swap sizes
        std::swap( m_size[ d1 ], m_size[ d2 ] );
        break;

      default:
        throw Exception() << "Internal: Should never be reached";
    }

    return (*this);
  }

  ArrayBase &reserve( index_t sx=1, index_t sy=1, index_t sz=1, index_t st=1 )
  {
    assert( m_dims >= 0 );
    assert( m_dims <= 4 );
    const int dims = m_dims;
    switch( dims )
    {
      case 0: if( sx != 1 ) throw Exception() << "If dims is less than 1 (is " << m_dims << "), sx must be 1 (is " << sx << ")"; // fallthrough
      case 1: if( sy != 1 ) throw Exception() << "If dims is less than 2 (is " << m_dims << "), sy must be 1 (is " << sy << ")"; // fallthrough
      case 2: if( sz != 1 ) throw Exception() << "If dims is less than 3 (is " << m_dims << "), sz must be 1 (is " << sz << ")"; // fallthrough
      case 3: if( st != 1 ) throw Exception() << "If dims is less than 4 (is " << m_dims << "), st must be 1 (is " << st << ")"; // fallthrough
      case 4: {}
    }
    const index_t s[ 4 ] = { sx, sy, sz, st };
    return reserve_v( dims, s );
  }

  ArrayBase &reserve_v( int dims, const index_t *_res )
  {
    if( dims != m_dims )
    {
      throw Exception()
        << "Dimensions specified at reserve_v() (" << dims << ")"
        << " must match dimensions specified at construction (" << m_dims << ")"
        ;
    }

    // Array cannot shrink with reserve()
    bool grown = false;
    index_t res[ dims ];
    for( int d=0; d<dims; ++d )
    {
      if( _res[ d ] > m_size[ d ] )
      {
        grown = true;
        res[ d ] = _res[ d ];
      }
      else
      {
        res[ d ] = m_size[ d ];
      }
    }

    // Nothing to do
    if( dims > 0 && !grown ) return (*this);

    // Check, whether old data will suffice
    while( m_element0 ) // not a while
    {
      if( 0 == dims ) return (*this);

      // Get current start position in data
      index_t data_offset[ dims ];
      if( !m_elements_data->getCoords( m_element0, dims, data_offset ) ) break;

      const index_t *const data_size = m_elements_data->getSize();

      bool in_range = true;
      for( int d=0; d<dims; ++d )
      {
        // The array might be rotated or only access every nth element,
        // so search for the corresponding dimension in the data
        const stride_t stride_d = m_stride[ d ];
        const stride_t stride_d_abs = std::abs( stride_d );

        int data_d = -1;
        stride_t step_d = 0;
        stride_t data_stride_dd = 1;
        for( int dd=0; dd<dims; ++dd )
        {
          const stride_t next_data_stride_dd = data_stride_dd * data_size[ dd ];
          // If the stride is larger, the step cannot be in that direction
          if( stride_d_abs < next_data_stride_dd )
          {
            step_d = stride_d / data_stride_dd; // Assuming step is only along one dimension (not skewed)
            data_d = dd;
            break;
          }
          data_stride_dd = next_data_stride_dd;
        }

        if( -1 == data_d )
        {
          in_range = false;
          break;
        }

        /*
        std::cerr << std::endl;
        std::cerr
          << "d = " << d
          << ", data_d = " << data_d
          << ", step_d = " << step_d
          << ", data_size[ data_d ] = " << data_size[ data_d ]
          << ", data_offset[ data_d ] = " << data_offset[ data_d ]
          << ", res[ d ] = " << res[ d ]
          << std::endl
          ;
          */

        const stride_t last_d = stride_t( data_offset[ data_d ] ) + stride_t( res[ d ] - 1 ) * step_d;
        if(
            last_d >= stride_t( data_size[ data_d ] )
         || last_d < 0
         )
        {
          in_range = false;
          break;
        }
      }

      if( !in_range ) break;

      // already reserved enough and m_stride is already for this array
      return (*this);
    }

    // Information for copying the old data
    CountPtr< ArrayData< Element > > old_data;
    stride_t old_stride[ dims ];
    index_t common_size[ dims ];
    index_t common_end [ dims ];
    const pointer old_elements = m_element0;
    if( old_elements != pointer() )
    {
      old_data.swap( m_elements_data );
      std::copy( m_stride, m_stride + dims, old_stride );
      for( int d = 0; d < dims; ++d )
      {
        common_size[ d ] = std::min( res[ d ], m_size[ d ] );
      }
      if( dims > 0 )
      {
        std::fill( common_end, common_end + (dims-1), 0 );
        common_end[ dims-1 ] = common_size[ dims-1 ];
      }
    }

    // Calculate new stride, reserved region will be dense
    stride_t stride_d = 1;
    for( int d = 0; d < dims; ++d )
    {
      m_stride[ d ] = stride_d;
      stride_d *= res[ d ];
    }
    std::fill( m_stride + dims, m_stride + 4, stride_t( 0 ) );

    // Allocate new data
    m_elements_data = new ArrayData< Element >( getGC(), dims, res );
    m_element0 = m_elements_data->first();

    // Copy old data
    if( old_elements != pointer() )
    {
      const Size common( dims, common_size );
      const_iterator o = const_iterator( common, old_stride, old_elements );
      iterator       n = iterator      ( common, m_stride  , m_element0   );
      iterator       n_end = iterator  ( common, m_stride  , m_element0   , common_end );

      for( ; n != n_end; ++n, ++o )
      {
        (*n) = (*o);
      }
    }

    return (*this);
  }

private:
  void _check_dims( void )
  {
    if( m_dims < 0 || m_dims > 4 )
    {
      throw Exception() << "Dimensions must be positive and at most 4, is " << m_dims;
    }
  }

  void _check_dims( int expected )
  {
    if( m_dims != expected )
    {
      throw Exception() << "Expected dimensions to be " << expected << ", is " << m_dims;
    }
  }

  static inline bool _isValue( const Value* ) { return true; }
  template< class T >
  static inline bool _isValue( const T* ) { return false; }

  CountPtr< ArrayData< Element > > m_elements_data;
  pointer m_element0;
  index_t m_size[ 4 ];
  stride_t m_stride[ 4 ];
  const int m_dims;
};


} // namespace RPGML

#endif

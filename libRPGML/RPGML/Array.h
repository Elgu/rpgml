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
#ifndef RPGML_Array_h
#define RPGML_Array_h

#include "ArrayBase.h"
#include "ArrayData.h"
#include "Type.h"
#include "Exception.h"
#include "GarbageCollector.h"
#include "iterator_base.h"

#include <cassert>
#include <vector>

namespace RPGML {

template< class _Element >
class Array;

template< class _Element >
class Array : public ArrayBase
{
  typedef ArrayBase Base;
public:
  typedef _Element Element;

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
  typedef iterator_base<       Element,       reference,       pointer >       iterator;
  typedef iterator_base< const Element, const_reference, const_pointer > const_iterator;

  //! Use a custom elements_data, can be resized within its limits, takes its size by default
  explicit
  Array( GarbageCollector *_gc, const CountPtr< ArrayData< Element > > &elements_data )
  : Base( _gc, elements_data->getDims() )
  , m_elements_data( elements_data )
  , m_element0( elements_data->first() )
  {
    _check_dims();
    const index_t *s = elements_data->getSize();
    std::copy( s, s+m_dims, m_size );
    _setDenseStride();
  }

  explicit
  Array( GarbageCollector *_gc, int dims, const index_t *s=0 )
  : Base( _gc, dims )
  , m_element0( pointer() )
  {
    _check_dims();
    if( dims == 0 ) resize_v( 0, 0 );
    else if( s ) resize_v( dims, s );
  }

  explicit
  Array( GarbageCollector *_gc, int dims, index_t sx )
  : Base( _gc, dims )
  , m_element0( pointer() )
  {
    _check_dims( 1 );
    resize( sx );
  }

  explicit
  Array( GarbageCollector *_gc, int dims, index_t sx, index_t sy )
  : Base( _gc, dims )
  , m_element0( pointer() )
  {
    _check_dims( 2 );
    resize( sx, sy );
  }

  explicit
  Array( GarbageCollector *_gc, int dims, index_t sx, index_t sy, index_t sz )
  : Base( _gc, dims )
  , m_element0( pointer() )
  {
    _check_dims( 3 );
    resize( sx, sy, sz );
  }

  explicit
  Array( GarbageCollector *_gc, int dims, index_t sx, index_t sy, index_t sz, index_t st )
  : Base( _gc, dims )
  , m_element0( pointer() )
  {
    _check_dims( 4 );
    resize( sx, sy, sz, st );
  }

  explicit
  Array( GarbageCollector *_gc, const Size &s )
  : Base( _gc, s.getDims() )
  , m_element0( pointer() )
  {
    _check_dims();
    resize( s );
  }

  //! Wrapper
  Array( GarbageCollector *_gc, pointer element0, int dims, const index_t *_size, const stride_t *_stride = 0 )
  : Base( _gc, dims )
  , m_element0( pointer() )
  {
    _check_dims();
    wrap( element0, dims, _size, _stride );
  }

  //! Wrapper
  Array( GarbageCollector *_gc, pointer element0, const Size &_size, const stride_t *_stride = 0 )
  : Base( _gc, _size.getDims() )
  , m_element0( pointer() )
  {
    _check_dims();
    wrap( element0, _size, _stride );
  }

  virtual ~Array( void )
  {}

  Array &operator=( const Array &other )
  {
    Base::operator=( other );
    m_elements_data = other.m_elements_data;
    m_element0 = other.m_element0;
    return (*this);
  }

  template< class OtherElement >
  Array &assign( const Array< OtherElement > *other )
  {
    if( getDims() != other->getDims() )
    {
      throw DimensionsMismatch( getDims(), other->getDims() )
        << "Number of dimensions must match for Array<>::assign()"
        ;
    }

    resize( other->getSize() );
    std::copy( other->begin(), other->end(), begin() );

    return (*this);
  }

  template< class OtherElement >
  Array &assign( const CountPtr< Array< OtherElement > > &other )
  {
    return assign( other.get() );
  }

  template< class OtherElement >
  Array &assign( const CountPtr< const Array< OtherElement > > &other )
  {
    return assign( other.get() );
  }

  void wrap( pointer element0, int dims, const index_t *_size, const stride_t *_stride = 0 )
  {
    if( dims != m_dims )
    {
      throw Exception()
        << "Dimensions specified at resize() (" << dims << ")"
        << " must match dimensions specified at construction (" << m_dims << ")"
        ;
    }
    m_element0 = element0;
    m_elements_data.reset();
    std::copy( _size, _size+dims, m_size );
    if( _stride )
    {
      std::copy( _stride, _stride+dims, m_stride );
    }
    else
    {
      _setDenseStride();
    }
  }

  void wrap( pointer element0, const Size &_size, const stride_t *_stride = 0 )
  {
    wrap( element0, _size.getDims(), _size.getCoords(), _stride );
  }

  void swap( Array &other )
  {
    Base::swap( other );
    m_elements_data.swap( other.m_elements_data );
    std::swap( m_element0, other.m_element0 );
  }

  void gc_clear( void )
  {
    Base::gc_clear();
    clear();
  }

  virtual void gc_getChildren( Children &children ) const
  {
    Base::gc_getChildren( children );
    children << m_elements_data;
  }

  iterator begin( void )
  {
    return iterator( m_dims, m_size, getStride(), elements() );
  }

  const_iterator begin( void ) const
  {
    return const_iterator( m_dims, m_size, getStride(), elements() );
  }

  iterator end( void )
  {
    const int dims = m_dims;
    if( dims > 0 )
    {
      index_t end_pos[ dims ];
      if( dims > 1 ) std::fill( end_pos, end_pos+(dims-1), 0 );
      end_pos[ dims-1 ] = m_size[ dims-1 ];
      return iterator( m_dims, m_size, getStride(), elements(), end_pos );
    }
    else
    {
      return iterator( m_dims, m_size, getStride(), elements()+1 );
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
      return const_iterator( m_dims, m_size, getStride(), elements(), end_pos );
    }
    else
    {
      return const_iterator( m_dims, m_size, getStride(), elements()+1 );
    }
  }

  reference front( void )
  {
    return *elements();
  }

  const_reference front( void ) const
  {
    return *elements();
  }

  reference back( void )
  {
    const int dims = m_dims;
    if( dims > 0 )
    {
      index_t pos[ dims ];
      for( int d=0; d<dims; ++d )
      {
        pos[ d ] = m_size[ d ]-1;
      }
      return at_v( dims, pos );
    }
    else
    {
      return *elements();
    }
  }

  const_reference back( void ) const
  {
    const int dims = m_dims;
    if( dims > 0 )
    {
      index_t pos[ dims ];
      for( int d=0; d<dims; ++d )
      {
        pos[ d ] = m_size[ d ]-1;
      }
      return at_v( dims, pos );
    }
    else
    {
      return *elements();
    }
  }

  class _ConstValueIterator : public ConstValueIterator
  {
  public:
    explicit
    _ConstValueIterator( const Array *array ) : m_i( array->begin() ), m_end( array->end() ) {}
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
    _CoordinatesIterator( const Array *array ) : m_i( array->begin() ), m_end( array->end() ) {}
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
    _Iterator( Array *array ) : m_i( array->begin() ), m_end( array->end() ) {}
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
    _ConstIterator( const Array *array ) : m_i( array->begin() ), m_end( array->end() ) {}
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

  virtual Type           getType( void ) const
  {
    return Type( TypeOf< Element >::E );
  }

  index_t capacity( void ) const
  {
    if( !m_elements_data.isNull() )
    {
      return m_elements_data->capacity();
    }
    else
    {
      return 0;
    }
  }

  void push_back( const Element &x )
  {
    if( m_dims != 1 ) throw Exception() << "push_back() only makes sense on 1D-Arrays, is " << m_dims;
    const index_t curr_size = m_size[ 0 ];
    if( capacity() < curr_size+1 )
    {
      reserve( std::max( curr_size+1, curr_size*2 ) );
    }
    ++m_size[ 0 ];
    at( curr_size ) = x;
  }

  void pop_back( void )
  {
    if( m_dims != 1 ) throw Exception() << "pop_back() only makes sense on 1D-Arrays, is " << m_dims;
    if( m_size[ 0 ] > 0 )
    {
      --m_size[ 0 ];
    }
  }

  bool isDense( void ) const
  {
    if( m_element0 == pointer() ) return true;
    return Base::isDense();
  }

  const_pointer elements( void ) const
  {
    return m_element0;
  }

  pointer elements( void )
  {
    return m_element0;
  }

  ArrayBase &resize( index_t sx=1, index_t sy=1, index_t sz=1, index_t st=1 )
  {
    assert( m_dims >= 0 );
    assert( m_dims <= 4 );
    const int dims = m_dims;
    const index_t s[ 4 ] = { sx, sy, sz, st };
    for( int d=dims; d<4; ++d )
    {
      if( s[ d ] != 1 ) throw Exception()
        << "If dims is less than " << d << " (is " << m_dims << ")"
        << ", s" << ("xyzt"[ d ]) << " must be 1 (is " << s[ d ] << ")"
        ;
    }
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
    CountPtr< Array > ret = new Array( getGC(), getSize() );
    std::copy( begin(), end(), ret->begin() );
    return ret;
  }

  template< class Element >
  CountPtr< Array< Element > > cloneAs( void ) const;

  template< class Element >
  CountPtr< Array< Element > > cloneTo( void ) const;

  virtual CountPtr< ArrayBase > copy( void ) const
  {
    return new Array( *this );
  }

  CountPtr< Array > copy( Array *target = 0 ) const
  {
    if( target )
    {
      (*target) = (*this);
      return target;
    }
    else
    {
      return new Array( (*this) );
    }
  }

  virtual Value getValue_v( int dims, const index_t *x ) const
  {
    checkRange_v( dims, x );
    return CreateValue< Element >()( at_v( dims, x ) );
  }

  virtual void setValue_v( const Value &value, int dims, const index_t *x )
  {
    checkRange_v( dims, x );
    at_v( dims, x ) = value.get< Element >();
  }

  virtual Reference getReference_v( int dims, const index_t *x )
  {
    checkRange_v( dims, x );
    return createReference( x );
  }

  virtual ConstReference getReference_v( int dims, const index_t *x ) const
  {
    checkRange_v( dims, x );
    return createReference( x );
  }

  void fill( const Element &e )
  {
    std::fill( begin(), end(), e );
  }

  virtual void fillValue( const Value &value )
  {
    fill( value.get< Element >() );
  }

  virtual bool isValueArray( void ) const
  {
    return _isValue( (const Element*)0 );
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
  #ifndef NDEBUG
    if( m_dims != 0 ) throw DimensionsMismatch( 0, m_dims );
  #endif
    return *m_element0;
  }

  reference operator*( void )
  {
  #ifndef NDEBUG
    if( m_dims != 0 ) throw DimensionsMismatch( 0, m_dims );
  #endif
    return *m_element0;
  }

  const_pointer operator->( void ) const
  {
  #ifndef NDEBUG
    if( m_dims != 0 ) throw DimensionsMismatch( 0, m_dims );
  #endif
    return m_element0;
  }

  pointer operator->( void )
  {
  #ifndef NDEBUG
    if( m_dims != 0 ) throw DimensionsMismatch( 0, m_dims );
  #endif
    return m_element0;
  }

  CountPtr< const Array > getROI( int dims, const index_t *x, const index_t *s, Array *_ret = 0 ) const
  {
    CountPtr< Array > ret = copy( _ret );
    ret->setROI( dims, x, s );
    return ret;
  }

  CountPtr< Array > getROI( int dims, const index_t *x, const index_t *s, Array *_ret = 0 )
  {
    CountPtr< Array > ret = copy( _ret );
    ret->setROI( dims, x, s );
    return ret;
  }

  CountPtr< const Array > getROI( const Coordinates &x, const Size &s, Array *_ret = 0 ) const
  {
    CountPtr< Array > ret = copy( _ret );
    ret->setROI( x, s );
    return ret;
  }

  CountPtr< Array > getROI( const Coordinates &x, const Size &s, Array *_ret = 0 )
  {
    CountPtr< Array > ret = copy( _ret );
    ret->setROI( x, s );
    return ret;
  }

  CountPtr< Array > getROI( index_t x, index_t w, Array *_ret = 0 ) const
  {
    if( 1 != m_dims )
    {
      throw Exception()
        << "Dimensions must be 2 for getROI( x, w ), is " << m_dims
        ;
    }
    CountPtr< Array > ret = copy( _ret );
    ret->setROI( 1, &x, &w );
    return ret;
  }

  CountPtr< Array > getROI( index_t x, index_t y, index_t sx, index_t sy, Array *_ret = 0 ) const
  {
    if( 2 != m_dims )
    {
      throw Exception()
        << "Dimensions must be 2 for getROI( x, y, sx, sy ), is " << m_dims
        ;
    }
    CountPtr< Array > ret = copy( _ret );
    const index_t X[ 2 ] = { x, y };
    const index_t S[ 2 ] = { sx, sy };
    ret->setROI( 2, X, S );
    return ret;
  }

  CountPtr< Array > getROI( index_t x, index_t y, index_t z, index_t sx, index_t sy, index_t sz, Array *_ret = 0 ) const
  {
    if( 3 != m_dims )
    {
      throw Exception()
        << "Dimensions must be 3 for getROI( x, y, z, sx, sy, sz ), is " << m_dims
        ;
    }
    CountPtr< Array > ret = copy( _ret );
    const index_t X[ 3 ] = { x, y, z };
    const index_t S[ 3 ] = { sx, sy, sz };
    ret->setROI( 3, X, S );
    return ret;
  }

  CountPtr< Array > getROI(
      index_t x, index_t y, index_t z, index_t t
    , index_t sx, index_t sy, index_t sz, index_t st
    , Array *_ret = 0
    ) const
  {
    if( 4 != m_dims )
    {
      throw Exception()
        << "Dimensions must be 4 for getROI( x, y, z, t, sx, sy, sz, st ), is " << m_dims
        ;
    }
    CountPtr< Array > ret = copy( _ret );
    const index_t X[ 4 ] = { x, y, z, t };
    const index_t S[ 4 ] = { sx, sy, sz, st };
    ret->setROI( 4, X, S );
    return ret;
  }

  virtual ArrayBase &setROI( int dims, const index_t *x, const index_t *s )
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
        throw Exception()
          << "ROI is out of range"
          << ": Array size is " << getSize()
          << ", x is " << Coordinates( dims, x )
          << ", s is " << Size( dims, s )
          ;
      }
    }

    m_element0 += position_v( dims, x );
    std::copy( s, s + dims, m_size );

    return (*this);
  }

  ArrayBase &setROI( const Coordinates &x, const Size &s )
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

  ArrayBase &mirror( int d ) { return setMirrored( d ); }

  ArrayBase &setMirrored( int d )
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

  ArrayBase &sparse( int d, stride_t nth, index_t offset=0 ) { return setSparse( d, nth, offset ); }

  virtual ArrayBase &setSparse( int d, stride_t nth, index_t offset=0 )
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
  Array &setRotated( int times90deg, int d1=0, int d2=1 )
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

  virtual ArrayBase &setTransposed( int d1=0, int d2=1 )
  {
    const int dims = m_dims;
    if( d1 < 0 || d1 >= dims )
    {
      throw Exception()
        << "Dimension d1 out of range in setTransposed(),"
        << " must be at least 0 and less than " << dims << ", is " << d1
        ;
    }
    if( d2 < 0 || d2 >= dims )
    {
      throw Exception()
        << "Dimension d2 out of range in setTransposed(),"
        << " must be at least 0 and less than " << dims << ", is " << d2
        ;
    }
    if( d1 == d2 )
    {
      throw Exception()
        << "Dimensions d1 and d2 must be different in setTransposed(), are " << d1
        ;
    }

    std::swap( m_stride[ d1 ], m_stride[ d2 ] );
    std::swap( m_size[ d1 ], m_size[ d2 ] );
    return (*this);
  }

  CountPtr< Array > getTransposed( int d1=0, int d2=1, Array *_ret = 0 ) const
  {
    CountPtr< Array > ret = copy( _ret );
    ret->setTransposed( d1, d2 );
    return ret;
  }

  CountPtr< Array > T( int d1=0, int d2=1, Array *_ret = 0 ) const
  {
    return getTransposed( d1, d2, _ret );
  }

  CountPtr< Array > getLine( int dims, const index_t *pos, int direction = 0, Array *_ret = 0 ) const
  {
    if( dims != m_dims )
    {
      throw Exception()
        << "'dims' specified at getLine() (" << dims << ")"
        << " must match dimensions specified at construction (" << m_dims << ")"
        ;
    }
    if( direction < 0 || direction >= dims )
    {
      throw Exception()
        << "'direction' out of range, must be less than 'dims' (" << dims << "), is " << direction
        ;
    }
    if( _ret && _ret->getDims() != 1 )
    {
      throw Exception()
        << "If '_ret' is specified at getLine(), it must be 1-dimensional, is " << _ret->getDims()
        ;
    }
    CountPtr< Array > ret( _ret ? _ret : new Array( getGC(), 1 ) );

    ret->m_elements_data = m_elements_data;
    ret->m_element0 = &const_cast< Array* >( this )->at_v( dims, pos );

    ret->m_stride[ 0 ] = m_stride[ direction ];
    ret->m_size[ 0 ] = m_size[ direction ] - pos[ direction ];

    return ret;
  }

  CountPtr< Array > line( int direction, index_t x, Array *_ret = 0 ) const
  {
    const index_t pos[ 1 ] = { x };
    return getLine( 1, pos, direction, _ret );
  }

  CountPtr< Array > line( int direction, index_t x, index_t y, Array *_ret = 0 ) const
  {
    const index_t pos[ 2 ] = { x, y };
    return getLine( 2, pos, direction, _ret );
  }

  CountPtr< Array > line( int direction, index_t x, index_t y, index_t z, Array *_ret = 0 ) const
  {
    const index_t pos[ 3 ] = { x, y, z };
    return getLine( 3, pos, direction, _ret );
  }

  CountPtr< Array > line( int direction, index_t x, index_t y, index_t z, index_t t, Array *_ret = 0 ) const
  {
    const index_t pos[ 4 ] = { x, y, z, t };
    return getLine( 4, pos, direction, _ret );
  }

  //! Adds new dimension before position 'dim' (is that position in the result), this dimension has size 1
  CountPtr< Array > addDim( int d, Array *_ret = 0 ) const
  {
    if( getDims() >= 4 )
    {
      throw Exception()
        << "Cannot addDim(), getDims() is already " << getDims()
        ;
    }

    const int dims = getDims();
    const int new_dims = dims+1;

    if( _ret && _ret->getDims() != new_dims )
    {
      throw Exception()
        << "Dimension of '_ret' must match dimension of Array plus one"
        << ": is " << _ret->getDims()
        << ", should be " << new_dims
        ;
    }

    if( d < 0 || d > getDims() )
    {
      throw Exception()
        << "'dim' must not be negative and at most getDims()"
        << ": getDims() is " << dims
        << ", 'd' is " << d
        ;
    }

    CountPtr< Array > ret( _ret ? _ret : new Array( getGC(), new_dims ) );

    index_t new_size[ new_dims ];
    stride_t new_stride[ new_dims ];

    std::copy( m_size, m_size+d, new_size );
    new_size[ d ] = 1;
    std::copy( m_size+d, m_size+dims, new_size+d+1 );

    std::copy( m_stride, m_stride+d, new_stride );
    if( d < dims )
    {
      new_stride[ d ] = m_stride[ d ];
    }
    else
    {
      new_stride[ d ] = m_stride[ dims-1 ] * m_size[ dims-1 ];
    }
    std::copy( m_stride+d, m_stride+dims, new_stride+d+1 );

    ret->wrap( m_element0, new_dims, new_size, new_stride );

    return ret;
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

    // Allocate new data
    m_elements_data = new ContainerArrayData< Element >( getGC(), dims, res );
    m_element0 = m_elements_data->first();

    // Copy old data
    if( old_elements != pointer() )
    {
      const_iterator o = const_iterator( dims, common_size, old_stride, old_elements );
      iterator       n = iterator      ( dims, common_size, m_stride  , m_element0   );
      iterator       n_end = iterator  ( dims, common_size, m_stride  , m_element0   , common_end );

      for( ; n != n_end; ++n, ++o )
      {
        (*n) = (*o);
      }
    }

    return (*this);
  }

private:
  CountPtr< ArrayData< Element > > m_elements_data;
  pointer m_element0;
};

extern template class Array< bool                       >;
extern template class Array< uint8_t                    >;
extern template class Array< int8_t                     >;
extern template class Array< uint16_t                   >;
extern template class Array< int16_t                    >;
extern template class Array< uint32_t                   >;
extern template class Array< int32_t                    >;
extern template class Array< uint64_t                   >;
extern template class Array< int64_t                    >;
extern template class Array< float                      >;
extern template class Array< double                     >;
extern template class Array< String                     >;
extern template class Array< CountPtr< Frame          > >;
extern template class Array< CountPtr< Function       > >;
extern template class Array< CountPtr< Node           > >;
extern template class Array< CountPtr< Output         > >;
extern template class Array< CountPtr< Input          > >;
extern template class Array< CountPtr< Param          > >;
extern template class Array< CountPtr< const Sequence > >;
extern template class Array< CountPtr< ArrayBase      > >;

typedef Array< bool                       >     BoolArray;
typedef Array< uint8_t                    >    UInt8Array;
typedef Array< int8_t                     >     Int8Array;
typedef Array< uint16_t                   >   UInt16Array;
typedef Array< int16_t                    >    Int16Array;
typedef Array< uint32_t                   >   UInt32Array;
typedef Array< int32_t                    >    Int32Array;
typedef Array< uint64_t                   >   UInt64Array;
typedef Array< int64_t                    >    Int64Array;
typedef Array< float                      >    FloatArray;
typedef Array< double                     >   DoubleArray;
typedef Array< String                     >   StringArray;
typedef Array< CountPtr< Frame          > >    FrameArray;
typedef Array< CountPtr< Function       > > FunctionArray;
typedef Array< CountPtr< Node           > >     NodeArray;
typedef Array< CountPtr< Output         > >   OutputArray;
typedef Array< CountPtr< Input          > >    InputArray;
typedef Array< CountPtr< Param          > >    ParamArray;
typedef Array< CountPtr< const Sequence > > SequenceArray;
typedef Array< CountPtr< ArrayBase      > >    ArrayArray;
typedef Array< int                        >      IntArray;
typedef Array< index_t                    >    IndexArray;
typedef Array< stride_t                   >   StrideArray;

template< class Element >
CountPtr< Array< Element > > new_Array( GarbageCollector *gc, int dims, const Element *fill_value=0 )
{
  CountPtr< Array< Element > > ret = new Array< Element >( gc, dims );
  if( fill_value ) ret->fill( *fill_value );
  return ret;
}

template< class Element >
CountPtr< Array< Element > > new_Array( GarbageCollector *gc, int dims, const Value &fill_value )
{
  const Type element_type( TypeOf< Element >::E );
  if( element_type.isOther() )
  {
    throw ArrayBase::Exception() << "Cannot create an Array of 'other' custom type";
  }

  if( !fill_value.isNil() )
  {
    const Value fill_value_cast = fill_value.save_cast( element_type );
    const Element fill_element = fill_value_cast.get< Element >();
    return new_Array< Element >( gc, dims, &fill_element );
  }
  return new_Array< Element >( gc, dims );
}

template< class Element >
CountPtr< Array< Element > > cloneAs( const ArrayBase *base )
{
  CountPtr< Array< Element > > ret = new Array< Element >( base->getGC(), base->getSize() );

  CountPtr< ArrayBase::ConstValueIterator > i = base->getConstValueIterator();

  for(
      typename Array< Element >::iterator o( ret->begin() ), e( ret->end() )
    ; !i->done() && ( o != e )
    ; i->next(), ++o
    )
  {
    (*o) = i->get().save_cast< Element >();
  }

  return ret;
}

template< class Element >
CountPtr< Array< Element > > cloneTo( const ArrayBase *base )
{
  CountPtr< Array< Element > > ret = new Array< Element >( base->getGC(), base->getSize() );

  CountPtr< ArrayBase::ConstValueIterator > i = base->getConstValueIterator();

  for(
      typename Array< Element >::iterator o( ret->begin() ), e( ret->end() )
    ; !i->done() && ( o != e )
    ; i->next(), ++o
    )
  {
    (*o) = i->get().to< Element >();
  }

  return ret;
}

template< class Element >
template< class As >
CountPtr< Array< As > > Array< Element >::cloneAs( void ) const
{
  return RPGML::cloneAs< As >( this );
}

template< class Element >
template< class To >
CountPtr< Array< To > > Array< Element >::cloneTo( void ) const
{
  return RPGML::cloneTo< To >( this );
}

template< class Element >
struct MakeSigned< Array< Element > >
{
  typedef typename MakeSigned< Element >::T SignedElement;
  typedef Array< SignedElement > T;
  static const Type::Enum E = Type::OTHER;
  static const bool B = true;
};

template< class Element >
struct MakeSigned< CountPtr< Array< Element > > >
{
  typedef typename MakeSigned< Element >::T SignedElement;
  typedef CountPtr< Array< SignedElement > > T;
  static const Type::Enum E = Type::ARRAY;
  static const bool B = true;
};

} // namespace RPGML

namespace std {

template< class Element >
inline
void swap( RPGML::Array< Element > &x1, RPGML::Array< Element > &x2 )
{
  x1.swap( x2 );
}

} // namespace std

#endif


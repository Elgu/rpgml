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
#ifndef RPGML_ArrayBase_h
#define RPGML_ArrayBase_h

#include "Exception.h"
#include "GarbageCollector.h"
#include "Type.h"
#include "Value.h"
#include "Refcounted.h"
#include "Iterator.h"

#include <typeinfo>

namespace RPGML {

class ArrayBase;

class Reference : public Refcounted
{
  friend class ArrayBase;
public:
  ~Reference() {}
  ArrayBase     *getArray( void ) const { return m_array; }
  const index_t *getPos  ( void ) const { return m_pos; }
  int            getDims ( void ) const { return m_dims; }
  Value          get     ( void ) const;
  void           set     ( const Value &x );
  Reference &operator=( const Value &x ) { set( x ); return (*this); }
  operator Value( void ) const { return get(); }
private:
  Reference( void );
  Reference( ArrayBase *array, const index_t *pos );

  const CountPtr< ArrayBase > m_array;
  index_t m_pos[ 4 ];
  const int m_dims;
};

class ConstReference : public Refcounted
{
  friend class ArrayBase;
public:
  ~ConstReference() {}
  ConstReference( const Reference &_ref );
  const ArrayBase *getArray( void ) const { return m_array; }
  const index_t   *getPos  ( void ) const { return m_pos; }
  int              getDims ( void ) const { return m_dims; }
  Value            get     ( void ) const;
  operator Value( void ) const { return get(); }
private:
  ConstReference( void );
  ConstReference( const ArrayBase *array, const index_t *pos );

  const CountPtr< const ArrayBase > m_array;
  index_t m_pos[ 4 ];
  const int m_dims;
};

class ArrayBase : public Collectable
{
  typedef Collectable Base;
public:
  //! @brief Array Exception base class
  EXCEPTION_BASE( Exception );

  //! @brief Exception for when the specified dimensions do not match the Array's
  class DimensionsMismatch : public Exception
  {
  public:
    typedef Exception Base;
    DimensionsMismatch( int _specified, int _expected );
    EXCEPTION_BODY( DimensionsMismatch );
    int specified;
    int expected;
  };

  class Coordinates
  {
  public:
    Coordinates( int dims, const index_t *coords )
    : m_coords( dims > 0 ? coords : 0 )
    , m_dims( dims )
    {}

    int            getDims  ( void ) const { return m_dims; }
    const index_t *getCoords( void ) const { return m_coords; }

    operator const index_t *( void ) const { return getCoords(); }
    const index_t &operator[]( int d ) const { return getCoords()[ d ]; }

    bool operator==( const Coordinates &other ) const;
    bool operator!=( const Coordinates &other ) const { return !( (*this) == other ); }

  private:
    const index_t *const m_coords;
    const int m_dims;
  };

  typedef Coordinates Size;

  class OutOfRange : public Exception
  {
  public:
    typedef Exception Base;
    OutOfRange( const Size &size, const Coordinates &coords );
    EXCEPTION_BODY( OutOfRange );
  };

  ArrayBase( GarbageCollector *_gc, int dims );
  virtual ~ArrayBase( void );

  template< class DataType >
  DataType *getAs( DataType* &as )
  {
    as = dynamic_cast< DataType* >( this );
    return as;
  }

  template< class DataType >
  const DataType *getAs( const DataType* &as ) const
  {
    as = dynamic_cast< const DataType* >( this );
    return as;
  }

  template< class DataType >
  CountPtr< DataType > getAs( void )
  {
    DataType *as = 0;
    return getAs( as );
  }

  template< class DataType >
  CountPtr< const DataType > getAs( void ) const
  {
    const DataType *as = 0;
    return getAs( as );
  }

  virtual const std::type_info &getTypeInfo( void ) const = 0;
  virtual ArrayBase &clear( void ) = 0;

  bool isDense( void ) const;
  index_t size( void ) const;

  bool empty( void ) const { return 0 == size(); }

  int             getDims  ( void ) const { return m_dims; }
  Size            getSize  ( void ) const { return Size( m_dims, m_size ); }
  const stride_t *getStride( void ) const { return m_stride; }
  virtual Type    getType  ( void ) const = 0;

  virtual ArrayBase &resize_v( int dims, const index_t *new_size ) = 0;
  virtual ArrayBase &resize( index_t x, index_t y=1, index_t z=1, index_t t=1 ) = 0;
  virtual ArrayBase &resize( const Size &size ) = 0;

  //! Deep copy, own copy own data, isDense()
  virtual CountPtr< ArrayBase > clone( void ) const = 0;
  //! Bare copy of the Array structure, same data, stride etc.
  virtual CountPtr< ArrayBase > copy ( void ) const = 0;

  typedef Iterator< Value > ConstValueIterator;
  virtual CountPtr< ConstValueIterator > getConstValueIterator( void ) const = 0;

  typedef Iterator< Coordinates > CoordinatesIterator;
  virtual CountPtr< CoordinatesIterator > getCoordinatesIterator( void ) const = 0;

  virtual Value getValue_v( int dims, const index_t *x ) const = 0;

  Value getValue( index_t x=0, index_t y=0, index_t z=0, index_t t=0 ) const;
  Value getValue( const Coordinates &x ) const { return getValue_v( x.getDims(), x ); }

  virtual void setValue_v( const Value &value, int dims, const index_t *x ) = 0;

  void setValue( const Value &value, index_t x=0, index_t y=0, index_t z=0, index_t t=0 );
  void setValue( const Value &value, const Coordinates &x ) { setValue_v( value, x.getDims(), x ); }

  virtual Reference getReference_v( int dims, const index_t *x ) = 0;

  Reference getReference( index_t x=0, index_t y=0, index_t z=0, index_t t=0 );
  Reference getReference( const Coordinates &x ) { return getReference_v( x.getDims(), x ); }

  virtual ConstReference getReference_v( int dims, const index_t *x ) const = 0;

  ConstReference getReference( index_t x=0, index_t y=0, index_t z=0, index_t t=0 ) const;
  ConstReference getReference( const Coordinates &x ) const { return getReference_v( x.getDims(), x ); }

  virtual void fillValue( const Value &value ) = 0;
  virtual bool isValueArray( void ) const = 0;

  const char *getTypeName( void ) const { return getType().getTypeName(); }

  virtual ArrayBase &setROI( int dims, const index_t *x, const index_t *s ) = 0;
  virtual ArrayBase &setMirrored( int d ) = 0;
  virtual ArrayBase &setSparse( int d, stride_t nth, index_t offset=0 ) = 0;
  //! Rotates in screen-coordinates (d1=0("x"), d2=1("y")) clock-wise, cartesian anti-clockwise
  virtual ArrayBase &setRotated( int times90deg, int d1=0, int d2=1 ) = 0;
  virtual ArrayBase &setTransposed( int d1=0, int d2=1 ) = 0;

  ArrayBase &sparse( int d, stride_t nth, index_t offset=0 ) { return setSparse( d, nth, offset ); }
  ArrayBase &rotate( int times90deg, int d1=0, int d2=1 ) { return setRotated( times90deg, d1, d2 ); }
  ArrayBase &transpose( int d1=0, int d2=1 ) { return setTransposed( d1, d2 ); }

  inline
  index_t getSizeX( void ) const
  {
  #ifndef NDEBUG
    if( m_dims < 1 ) throw Exception() << "Dimensions must be at least 1 for getSizeX(), is " << m_dims;
  #endif
    return m_size[ 0 ];
  }

  inline
  index_t getSizeY( void ) const
  {
  #ifndef NDEBUG
    if( m_dims < 2 ) throw Exception() << "Dimensions must be at least 2 for getSizeY(), is " << m_dims;
  #endif
    return m_size[ 1 ];
  }

  inline
  index_t getSizeZ( void ) const
  {
  #ifndef NDEBUG
    if( m_dims < 3 ) throw Exception() << "Dimensions must be at least 3 for getSizeZ(), is " << m_dims;
  #endif
    return m_size[ 2 ];
  }

  inline
  index_t getSizeT( void ) const
  {
  #ifndef NDEBUG
    if( m_dims < 4 ) throw Exception() << "Dimensions must be at least 4 for getSizeT(), is " << m_dims;
  #endif
    return m_size[ 3 ];
  }

  inline
  stride_t position( index_t x ) const
  {
  #ifndef NDEBUG
    if( m_dims != 1 ) throw DimensionsMismatch( 1, m_dims );
  #endif
    return position_v( 1, &x );
  }

  inline
  stride_t position( index_t x, index_t y ) const
  {
  #ifndef NDEBUG
    if( m_dims != 2 ) throw DimensionsMismatch( 1, m_dims );
  #endif
    const index_t X[ 2 ] = { x, y };
    return position_v( 2, X );
  }

  inline
  stride_t position( index_t x, index_t y, index_t z ) const
  {
  #ifndef NDEBUG
    if( m_dims != 3 ) throw DimensionsMismatch( 1, m_dims );
  #endif
    const index_t X[ 3 ] = { x, y, z };
    return position_v( 3, X );
  }

  inline
  stride_t position( index_t x, index_t y, index_t z, index_t t ) const
  {
  #ifndef NDEBUG
    if( m_dims != 4 ) throw DimensionsMismatch( 1, m_dims );
  #endif
    const index_t X[ 4 ] = { x, y, z, t };
    return position_v( 4, X );
  }

  inline
  stride_t position_v( int dims, const index_t *x ) const
  {
  #ifndef NDEBUG
    if( m_dims != dims ) throw DimensionsMismatch( dims, m_dims );
  #endif
    // Note: Use dims instead of m_dims here, so inlining will work
    if( dims > 0 )
    {
      stride_t pos = 0;
//      for( int d = dims-1; d >= 0; --d )
      for( int d = 0; d < dims; ++d ) // faster
      {
#ifndef NDEBUG
        if( x[ d ] >= m_size[ d ] )
        {
          throw OutOfRange( getSize(), Coordinates( dims, x ) );
        }
#endif
        pos += stride_t( x[ d ] ) * m_stride[ d ];
      }
      return pos;
    }
    else
    {
      return 0;
    }
  }

protected:
  Reference      createReference( const index_t *x );
  ConstReference createReference( const index_t *x ) const;

  void _check_dims( void );
  void _check_dims( int expected );

  static inline bool _isValue( const Value* ) { return true; }
  template< class T >
  static inline bool _isValue( const T* ) { return false; }

  ArrayBase &operator=( const ArrayBase &other );
  void swap( ArrayBase &other );

  index_t m_size[ 4 ];
  stride_t m_stride[ 4 ];
  const int m_dims;
};

} // namespace RPGML

namespace std {

inline
std::ostream &operator<<( std::ostream &o, const RPGML::ArrayBase::Coordinates &x )
{
  const int dims = x.getDims();
  if( dims < 1 ) return o;
  o << "[ ";
  for( int i=0; i<dims; ++i )
  {
    if( i > 0 ) o << ", ";
    o << x[ i ];
  }
  o << " ]";
  return o;
}

} // namespace std

#endif

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

#include <typeinfo>

namespace RPGML {

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
    DimensionsMismatch( int _specified, int _expected )
    : specified( _specified )
    , expected( _expected )
    {
      (*this)
        << "Specified dims (" << specified << ")"
        << " does not match Array Dims (" << expected << ")"
        ;
    }
    EXCEPTION_BODY( DimensionsMismatch );
    int specified;
    int expected;
  };

  class Coordinates
  {
  public:
    explicit
    Coordinates( int dims, const index_t *coords )
    : m_coords( dims > 0 ? coords : 0 )
    , m_dims( dims )
    {}

    int            getDims  ( void ) const { return m_dims; }
    const index_t *getCoords( void ) const { return m_coords; }

    operator const index_t *( void ) const { return getCoords(); }
    const index_t &operator[]( int d ) const { return getCoords()[ d ]; }

    bool operator==( const Coordinates &other ) const
    {
      return
           getDims() == other.getDims()
        && std::equal( getCoords(), getCoords() + getDims(), other.getCoords() )
        ;
    }

    bool operator!=( const Coordinates &other ) const
    {
      return !( (*this) == other );
    }

  private:
    const index_t *const m_coords;
    int m_dims;
  };

  typedef Coordinates Size;

  class OutOfRange : public Exception
  {
  public:
    typedef Exception Base;
    explicit
    OutOfRange( const Size &size, const Coordinates &coords )
    {
      (*this)
        << "Out of range: Size is " << size
        << ", coordinates are " << coords
        ;
    }
    EXCEPTION_BODY( OutOfRange );
  };

  explicit
  ArrayBase( GarbageCollector *_gc, int dims )
  : Base( _gc )
  , m_dims( dims )
  {
    std::fill( m_size, m_size+dims, 0 );
    std::fill( m_size+dims, m_size+4, 1 );
  }

  virtual ~ArrayBase( void ) {}

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

  virtual const std::type_info &getTypeInfo( void ) const = 0;

  virtual ArrayBase &clear( void ) = 0;

  bool isDense( void ) const
  {
    stride_t dense_stride = 1;
    for( int d( 0 ), dims( m_dims ); d<dims; ++d )
    {
      if( 0 == m_size[ d ] ) return true;
      if( m_stride[ d ] != dense_stride ) return false;
      dense_stride *= m_size[ d ];
    }
    return true;
  }

  index_t size( void ) const
  {
    index_t ret = 1;
    for( int d( 0 ), dims( m_dims ); d<dims; ++d )
    {
      ret *= m_size[ d ];
    }
    return ret;
  }

  index_t         getDims  ( void ) const { return m_dims; }
  Size            getSize  ( void ) const { return Size( m_dims, m_size ); }
  const stride_t *getStride( void ) const { return m_stride; }
  virtual Type    getType  ( void ) const = 0;

  virtual ArrayBase &resize_v( int dims, const index_t *new_size ) = 0;
  virtual ArrayBase &resize( index_t x, index_t y=1, index_t z=1, index_t t=1 ) = 0;
  virtual ArrayBase &resize( const Size &size ) = 0;

  virtual CountPtr< ArrayBase > clone( void ) const = 0;

  typedef Iterator< Value > ConstValueIterator;
  virtual CountPtr< ConstValueIterator > getConstValueIterator( void ) const = 0;

  typedef Iterator< Coordinates > CoordinatesIterator;
  virtual CountPtr< CoordinatesIterator > getCoordinatesIterator( void ) const = 0;

  class Reference
  {
    friend class ArrayBase;
  public:
    ~Reference()
    {}

    ArrayBase     *getArray( void ) const { return m_array; }
    const index_t *getPos  ( void ) const { return m_pos; }
    int            getDims ( void ) const { return m_dims; }

    Reference &operator=( const Value &x )
    {
      m_array->setValue_v( x, m_dims, m_pos );
      return (*this);
    }

    operator Value( void ) const
    {
      return m_array->getValue_v( m_dims, m_pos );
    }

  private:
    Reference( void );

    Reference( ArrayBase *array, const index_t *pos )
    : m_array( array )
    , m_dims( array->getDims() )
    {
      std::copy( pos, pos+m_dims, m_pos );
      std::fill( m_pos+m_dims, m_pos+4, index_t( 0 ) );
    }

    const CountPtr< ArrayBase > m_array;
    index_t m_pos[ 4 ];
    const int m_dims;
  };

  class ConstReference
  {
    friend class ArrayBase;
  public:
    ~ConstReference()
    {}

    ConstReference( const Reference &ref )
    : m_array( ref.getArray() )
    , m_dims( ref.getDims() )
    {
      const index_t *const pos = ref.getPos();
      std::copy( pos, pos+m_dims, m_pos );
      std::fill( m_pos+m_dims, m_pos+4, index_t( 0 ) );
    }

    const ArrayBase *getArray( void ) const
    {
      return m_array;
    }

    operator Value( void ) const
    {
      return m_array->getValue_v( m_dims, m_pos );
    }

  private:
    ConstReference( void );

    ConstReference( const ArrayBase *array, const index_t *pos )
    : m_array( array )
    , m_dims( array->getDims() )
    {
      std::copy( pos, pos+m_dims, m_pos );
      std::fill( m_pos+m_dims, m_pos+4, index_t( 0 ) );
    }

    const CountPtr< const ArrayBase > m_array;
    index_t m_pos[ 4 ];
    const int m_dims;
  };

  virtual Value getValue_v( int dims, const index_t *x ) const = 0;

  Value getValue( index_t x=0, index_t y=0, index_t z=0, index_t t=0 ) const
  {
    const int dims = getDims();
    const index_t xyzt[ 4 ] = { x, y, z, t };
    return getValue_v( dims, xyzt );
  }

  Value getValue( const Coordinates &x ) const
  {
    return getValue_v( x.getDims(), x );
  }

  virtual void setValue_v( const Value &value, int dims, const index_t *x ) = 0;

  void setValue( const Value &value, index_t x=0, index_t y=0, index_t z=0, index_t t=0 )
  {
    const int dims = getDims();
    const index_t xyzt[ 4 ] = { x, y, z, t };
    return setValue_v( value, dims, xyzt );
  }

  void setValue( const Value &value, const Coordinates &x )
  {
    setValue_v( value, x.getDims(), x );
  }

  virtual Reference getReference_v( int dims, const index_t *x ) = 0;

  Reference getReference( index_t x=0, index_t y=0, index_t z=0, index_t t=0 )
  {
    const int dims = getDims();
    const index_t xyzt[ 4 ] = { x, y, z, t };
    return getReference_v( dims, xyzt );
  }

  Reference getReference( const Coordinates &x )
  {
    return getReference_v( x.getDims(), x );
  }

  virtual ConstReference getReference_v( int dims, const index_t *x ) const = 0;

  ConstReference getReference( index_t x=0, index_t y=0, index_t z=0, index_t t=0 ) const
  {
    const int dims = getDims();
    const index_t xyzt[ 4 ] = { x, y, z, t };
    return getReference_v( dims, xyzt );
  }

  ConstReference getReference( const Coordinates &x ) const
  {
    return getReference_v( x.getDims(), x );
  }

  virtual void fillValue( const Value &value ) = 0;
  virtual bool isValueArray( void ) const = 0;

  const char *getTypeName( void ) const
  {
    return getType().getTypeName();
  }

  stride_t position( index_t x ) const
  {
  #ifndef NDEBUG
    if( m_dims != 1 ) throw DimensionsMismatch( 1, m_dims );
  #endif
    return position_v( 1, &x );
  }

  stride_t position( index_t x, index_t y ) const
  {
  #ifndef NDEBUG
    if( m_dims != 2 ) throw DimensionsMismatch( 1, m_dims );
  #endif
    const index_t X[ 2 ] = { x, y };
    return position_v( 2, X );
  }

  stride_t position( index_t x, index_t y, index_t z ) const
  {
  #ifndef NDEBUG
    if( m_dims != 3 ) throw DimensionsMismatch( 1, m_dims );
  #endif
    const index_t X[ 3 ] = { x, y, z };
    return position_v( 3, X );
  }

  stride_t position( index_t x, index_t y, index_t z, index_t t ) const
  {
  #ifndef NDEBUG
    if( m_dims != 4 ) throw DimensionsMismatch( 1, m_dims );
  #endif
    const index_t X[ 4 ] = { x, y, z, t };
    return position_v( 4, X );
  }

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
  Reference createReference( const index_t *x )
  {
    return Reference( this, x );
  }

  ConstReference createReference( const index_t *x ) const
  {
    return ConstReference( this, x );
  }

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

  void swap( ArrayBase &other )
  {
    const int dims = m_dims;
    if( dims != other.m_dims )
    {
      throw Exception()
        << "Cannot swap an Array with another one with different dimensions"
        << ", this has " << dims
        << ", the other has " << other.m_dims
        ;
    }
    for( int d=0; d<dims; ++d )
    {
      std::swap( m_size[ d ], other.m_size[ d ] );
      std::swap( m_stride[ d ], other.m_stride[ d ] );
    }
  }

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

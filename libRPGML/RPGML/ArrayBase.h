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
  ArrayBase( GarbageCollector *_gc )
  : Base( _gc )
  {}

  ArrayBase( const ArrayBase &other )
  : Base( other )
  {}

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

  virtual index_t        getDims( void ) const = 0;
  virtual Type           getType( void ) const = 0;
  virtual Size           getSize( void ) const = 0;
  virtual index_t        size   ( void ) const = 0;

  virtual ArrayBase &resize_v( int dims, const index_t *new_size ) = 0;
  virtual ArrayBase &resize( index_t x, index_t y=1, index_t z=1, index_t t=1 ) = 0;
  virtual ArrayBase &resize( const Size &size ) = 0;

  virtual CountPtr< ArrayBase > clone( void ) const = 0;

  typedef Iterator< Value > ConstValueIterator;
  virtual CountPtr< ConstValueIterator > getConstValueIterator( void ) const = 0;

  typedef Iterator< Coordinates > CoordinatesIterator;
  virtual CountPtr< CoordinatesIterator > getCoordinatesIterator( void ) const = 0;

  virtual Value getValue( index_t x=0, index_t y=0, index_t z=0, index_t t=0 ) const = 0;
  virtual Value getValue_v( int dims, const index_t *x ) const = 0;
  virtual Value getValue( const Coordinates &x ) const = 0;
  virtual void setValue( const Value &value, index_t x=0, index_t y=0, index_t z=0, index_t t=0 ) = 0;
  virtual void setValue_v( const Value &value, int dims, const index_t *x ) = 0;
  virtual void setValue( const Value &value, const Coordinates &x ) = 0;
  virtual void fillValue( const Value &value ) = 0;
  virtual bool isValue( void ) const = 0;

  const char *getTypeName( void ) const
  {
    return getType().getTypeName();
  }
};

} // namespace RPGML

#endif

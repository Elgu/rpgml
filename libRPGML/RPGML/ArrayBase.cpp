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
#include "ArrayBase.h"

namespace RPGML {

Reference::Reference( ArrayBase *array, const index_t *pos )
: m_array( array )
, m_dims( array->getDims() )
{
  std::copy( pos, pos+m_dims, m_pos );
  std::fill( m_pos+m_dims, m_pos+4, index_t( 0 ) );
}

Value Reference::get( void ) const
{
  return m_array->getValue_v( m_dims, m_pos );
}

void Reference::set( const Value &x )
{
  m_array->setValue_v( x, m_dims, m_pos );
}

ConstReference::ConstReference( const Reference &_ref )
: m_array( _ref.getArray() )
, m_dims( _ref.getDims() )
{
  const index_t *const pos = _ref.getPos();
  std::copy( pos, pos+m_dims, m_pos );
  std::fill( m_pos+m_dims, m_pos+4, index_t( 0 ) );
}

ConstReference::ConstReference( const ArrayBase *array, const index_t *pos )
: m_array( array )
, m_dims( array->getDims() )
{
  std::copy( pos, pos+m_dims, m_pos );
  std::fill( m_pos+m_dims, m_pos+4, index_t( 0 ) );
}

Value ConstReference::get( void ) const
{
  return m_array->getValue_v( m_dims, m_pos );
}

ArrayBase::DimensionsMismatch::DimensionsMismatch( int _specified, int _expected )
: specified( _specified )
, expected( _expected )
{
  (*this)
    << "Specified dims (" << specified << ")"
    << " does not match Array Dims (" << expected << ")"
    ;
}

bool ArrayBase::Coordinates::operator==( const Coordinates &other ) const
{
  return
       getDims() == other.getDims()
    && std::equal( getCoords(), getCoords() + getDims(), other.getCoords() )
    ;
}

ArrayBase::OutOfRange::OutOfRange( const Size &size, const Coordinates &coords )
{
  (*this)
    << "Out of range: Size is " << size
    << ", coordinates are " << coords
    ;
}

ArrayBase::ArrayBase( GarbageCollector *_gc, int dims )
: Base( _gc )
, m_dims( dims )
{
  std::fill( m_size, m_size+dims, 0 );
  std::fill( m_size+dims, m_size+4, 1 );
}

ArrayBase::~ArrayBase( void )
{}

bool ArrayBase::isDense( void ) const
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

index_t ArrayBase::size( void ) const
{
  index_t ret = 1;
  for( int d( 0 ), dims( m_dims ); d<dims; ++d )
  {
    ret *= m_size[ d ];
  }
  return ret;
}

void ArrayBase::checkRange_v( int dims, const index_t *x ) const
{
  if( inRange_v( dims, x ) ) return;
  throw OutOfRange( getSize(), Coordinates( dims, x ) );
}

void ArrayBase::checkRange( const Coordinates &x ) const
{
  checkRange_v( x.getDims(), x.getCoords() );
}

bool ArrayBase::inRange_v( int dims, const index_t *x ) const
{
  if( m_dims != dims ) throw DimensionsMismatch( dims, m_dims );
  for( int d=0; d<dims; ++d )
  {
    if( x[ d ] >= m_size[ d ] )
    {
      return false;
    }
  }
  return true;
}

bool ArrayBase::inRange( const Coordinates &x ) const
{
  return inRange_v( x.getDims(), x.getCoords() );
}

Value ArrayBase::getValue( index_t x, index_t y, index_t z, index_t t ) const
{
  const int dims = getDims();
  const index_t xyzt[ 4 ] = { x, y, z, t };
  return getValue_v( dims, xyzt );
}

void ArrayBase::setValue( const Value &value, index_t x, index_t y, index_t z, index_t t )
{
  const int dims = getDims();
  const index_t xyzt[ 4 ] = { x, y, z, t };
  return setValue_v( value, dims, xyzt );
}

Reference ArrayBase::getReference( index_t x, index_t y, index_t z, index_t t )
{
  const int dims = getDims();
  const index_t xyzt[ 4 ] = { x, y, z, t };
  return getReference_v( dims, xyzt );
}

ConstReference ArrayBase::getReference( index_t x, index_t y, index_t z, index_t t ) const
{
  const int dims = getDims();
  const index_t xyzt[ 4 ] = { x, y, z, t };
  return getReference_v( dims, xyzt );
}

Reference ArrayBase::createReference( const index_t *x )
{
  return Reference( this, x );
}

ConstReference ArrayBase::createReference( const index_t *x ) const
{
  return ConstReference( this, x );
}

void ArrayBase::_check_dims( void )
{
  if( m_dims < 0 || m_dims > 4 )
  {
    throw Exception() << "Dimensions must be positive and at most 4, is " << m_dims;
  }
}

void ArrayBase::_check_dims( int expected )
{
  if( m_dims != expected )
  {
    throw Exception() << "Expected dimensions to be " << expected << ", is " << m_dims;
  }
}

ArrayBase &ArrayBase::operator=( const ArrayBase &other )
{
  if( getDims() != other.getDims() )
  {
    throw DimensionsMismatch( other.getDims(), getDims() )
      << "When assigning Arrays, the dimensions must match."
      ;
  }
  const int dims = getDims();
  for( int d=0; d<dims; ++d )
  {
    m_size[ d ]   = other.m_size[ d ];
    m_stride[ d ] = other.m_stride[ d ];
  }
  return (*this);
}

void ArrayBase::swap( ArrayBase &other )
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

std::ostream &ArrayBase::print( std::ostream &o ) const
{
  const int dims = getDims();

  int newline_dim = 1;
  if( dims > 0 )
  {
    index_t s = m_size[ 0 ];
    for( ; newline_dim < 4 && s < 16; ++newline_dim )
    {
      s *= m_size[ newline_dim ];
    }
  }

  o << "[ ";
  if( dims > newline_dim ) o << std::endl;

  print( o, newline_dim );

  if( dims > newline_dim ) o << std::endl;
  o << " ]";

  return o;
}

std::ostream &ArrayBase::print( std::ostream &o, int newline_dim ) const
{
  const index_t dims = getDims();

  typedef ArrayBase::Coordinates X;
  typedef ArrayBase::CoordinatesIterator C;
  typedef ArrayBase::ConstValueIterator I;

  CountPtr< I > i( getConstValueIterator() );
  CountPtr< C > c( getCoordinatesIterator() );

  index_t last_p[ dims ];
  X last( dims, last_p );
  std::fill( last_p, last_p + dims, 0 );

  const bool is_string = getType().isString();

  for( ; !i->done() && !c->done(); i->next(), c->next() )
  {
    const X x = c->get();

    // Check, which coordinate changed (highest)
    // print separator accordingly
    for( int d = dims-1; d >= 0; --d )
    {
      if( x[ d ] != last[ d ] )
      {
        if( 0 == d )
        {
          o << ", ";
        }
        else
        {
          if( d > 1 && d >= newline_dim ) o << std::endl;
          for( int s=0; s<d; ++s )
          {
            o << ";";
          }
          if( d >= newline_dim ) o << std::endl; else o << " ";
        }
        break;
      }
    }

    const Value v = i->get();
    if( is_string ) o << "\"";
    v.print( o );
    if( is_string ) o << "\"";

    std::copy( &x[ 0 ], &x[ 0 ] + dims, last_p );
  }

  return o;
}

} // namespace RPGML


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
#ifndef RPGML_Atomic_h
#define RPGML_Atomic_h

namespace RPGML {

template< class T >
class Atomic
{
public:
  Atomic( void )
  : m_value( T() )
  {}

  Atomic( const Atomic &other )
  : m_value( other.m_value )
  {}

  Atomic( T value )
  : m_value( value )
  {}

  ~Atomic( void )
  {}

  Atomic &operator=( T value )
  {
    while( !compare_and_swap( m_value, value ) ) {}
    return (*this);
  }

  T get( void ) const { return m_value; }
  operator T( void ) const { return get(); }

  // Try to decrement by an amount so value does not fall below 0
  bool trydec( T amount = T(1) )
  {
    for(;;)
    {
      const T old_value = m_value;
      if( old_value < amount )
      {
        return false;
      }

      const T new_value = old_value - amount;
      if( compare_and_swap( old_value, new_value ) )
      {
        return true;
      }
      // Try again
    }
  }

  //! prefix
  T operator++( void )
  {
    const T ret = __sync_fetch_and_add( &m_value, 1 );
    return ret + 1;
  }

  //! suffix
  T operator++( int )
  {
    const T ret = __sync_fetch_and_add( &m_value, 1 );
    return ret;
  }

  //! prefix
  T operator--( void )
  {
    const T ret = __sync_fetch_and_sub( &m_value, 1 );
    return ret - 1;
  }

  //! suffix
  T operator--( int )
  {
    const T ret = __sync_fetch_and_sub( &m_value, 1 );
    return ret;
  }

  T operator+=( T x )
  {
    const T ret = __sync_fetch_and_add( &m_value, x );
    return ret + x;
  }

  T operator-=( T x )
  {
    const T ret = __sync_fetch_and_sub( &m_value, x );
    return ret - x;
  }

  T operator|=( T x )
  {
    const T ret = __sync_fetch_and_or( &m_value, x );
    return ret | x;
  }

  T operator&=( T x )
  {
    const T ret = __sync_fetch_and_and( &m_value, x );
    return ret & x;
  }

  T operator^=( T x )
  {
    const T ret = __sync_fetch_and_xor( &m_value, x );
    return ret ^ x;
  }

  //! Returns whether write was successful
  bool compare_and_swap( T oldval, T newval )
  {
    return __sync_bool_compare_and_swap( &m_value, oldval, newval );
  }

private:
  T m_value;
};


} // namespace RPGML

#endif


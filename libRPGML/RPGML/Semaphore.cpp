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
/*! @file Semaphore.cpp
 * @brief Semaphore class implementation
 * @author Gunnar Payer
 * @date November 27th 2014
 */
#include "Semaphore.h"

#include <cerrno>

namespace RPGML {

EXCEPTION_DERIVED_DEFINE_FIXED_TEXT( Semaphore, ValueExceedsMax, "Semaphore value exceeds SEM_VALUE_MAX" );

Semaphore::Semaphore( value_t initial_value )
{
  if( -1 == sem_init( &m_sem, 0, initial_value ) )
  {
    if( EINVAL == errno ) throw ValueExceedsMax();
    throw Exception( "Internal: Unknown sem_init() error" );
  }
}

Semaphore::~Semaphore( void )
{
  // might be invalid, but we do not care at this point
  sem_destroy( &m_sem );
}

bool Semaphore::trylock( void )
{
  int ret = 0;

  // Retry if it was interrupted by a signal
  while( -1 == ( ret = sem_trywait( &m_sem ) ) && EINTR == errno ) {}

  if( 0 == ret ) return true;

  return false;
}

void Semaphore::lock( void )
{
  // Retry if it was interrupted by a signal
  while( -1 == sem_wait( &m_sem ) && EINTR == errno ) {}
}

void Semaphore::unlock( void )
{
  if( -1 == sem_post( &m_sem ) )
  {
    if( EOVERFLOW == errno ) throw ValueExceedsMax();
    throw Exception( "Internal: Unknown sem_post() error" );
  }
}

} // namespace RPGML


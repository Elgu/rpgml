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
#include "Mutex.h"

#include <iostream>
#include <cerrno>

namespace RPGML {

EXCEPTION_DERIVED_DEFINE_FIXED_TEXT( Mutex, AlreadyLocked, "the mutex is already locked by the calling thread" );
EXCEPTION_DERIVED_DEFINE_FIXED_TEXT( Mutex, NotOwned     , "the calling thread does not own the mutex" );

Mutex::Attr::Attr( int kind )
{
  pthread_mutexattr_init( &m_attr );
  pthread_mutexattr_settype( &m_attr, kind );
}

Mutex::Attr::~Attr( void )
{
  pthread_mutexattr_destroy( &m_attr );
}

Mutex::Attr::operator const pthread_mutexattr_t *( void ) const
{
  return &m_attr;
}

Mutex::Fast::Fast( void )
: Attr( PTHREAD_MUTEX_FAST_NP )
{}

Mutex::Recursive::Recursive( void )
: Attr( PTHREAD_MUTEX_RECURSIVE_NP )
{}


Mutex::ErrorCheck::ErrorCheck( void )
: Attr( PTHREAD_MUTEX_ERRORCHECK_NP )
{}

Mutex::Mutex( const Attr &attr )
{
  pthread_mutex_init( &m_mutex, attr );
}

Mutex::Mutex( void )
{
  pthread_mutex_init( &m_mutex, NULL );
}

Mutex::~Mutex( void )
{
  const int ret = pthread_mutex_destroy( &m_mutex );
  if( EBUSY == ret ) std::cerr << "warning: Mutex was still locked" << std::endl;
}

void Mutex::lock( void )
{
  const int ret = pthread_mutex_lock( &m_mutex );
  switch( ret )
  {
    case EINVAL: throw Exception( "Internal: the mutex has not been properly initialized." );
    case EDEADLK: throw AlreadyLocked();
    default: {}
  }
}

bool Mutex::trylock( void )
{
  const int ret = pthread_mutex_trylock( &m_mutex );
  switch( ret )
  {
    case EINVAL: throw Exception( "Internal: the mutex has not been properly initialized." );
    case EDEADLK: throw AlreadyLocked();
    default: {}
  }
  return ( ret != EBUSY );
}

void Mutex::unlock( void )
{
  const int ret = pthread_mutex_unlock( &m_mutex );
  switch( ret )
  {
    case EINVAL: throw Exception( "Internal: the mutex has not been properly initialized." );
    case EPERM: throw NotOwned();
    default: {}
  }
}

} // namespace RPGML


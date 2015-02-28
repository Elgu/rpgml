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
/*! @file Thread.cpp
 * @brief Thread base class implementation
 * @author Gunnar Payer
 * @date November 27th 2014
 */
#include "Thread.h"

#include <cerrno>

namespace RPGML {

EXCEPTION_DERIVED_DEFINE_FIXED_TEXT( Thread, AlreadyRunning, "Thread already running" );
EXCEPTION_DERIVED_DEFINE_FIXED_TEXT( Thread, NotRunning, "Thread not running" );
EXCEPTION_DERIVED_DEFINE_FIXED_TEXT( Thread, InsufficientResources, "Insufficient resources to create another thread, or a system-imposed limit on the number of threads was encountered" );
EXCEPTION_DERIVED_DEFINE_FIXED_TEXT( Thread, DeadLock, "A deadlock was detected" );
EXCEPTION_DERIVED_DEFINE_FIXED_TEXT( Thread, NotJoinable, "thread is not a joinable thread or Another thread is already waiting to join with this thread" );

Thread::Thread( GarbageCollector *_gc, bool start_immediately )
: Collectable( _gc )
, m_running( false )
{
  if( start_immediately ) start();
}

Thread::~Thread( void )
{
  if( isRunning() )
  {
    cancel();
    join();
  }
}

void Thread::gc_clear( void )
{}

void Thread::gc_getChildren( Children & ) const
{}

bool Thread::isRunning( void ) const
{
  return m_running;
}

void Thread::start( void )
{
  return start( &dispatch_run, this );
}

void Thread::start( void *(*start_routine)(void*), void *arg )
{
  if( isRunning() ) throw AlreadyRunning();

  const int ret = pthread_create( &m_thread, 0, start_routine, arg );

  switch( ret )
  {
    case EAGAIN: throw InsufficientResources();
    case EINVAL: throw StartException() << "Internal: Invalid settings in attr";
    case EPERM : throw StartException() << "Internal: No permission to set the scheduling policy and parameters specified in attr";
    default: {}
  }

  m_running = true;
}

void Thread::join( size_t *exit_status )
{
  if( !isRunning() ) throw NotRunning();

  void *exit_status_v = 0;

  const int ret = pthread_join( m_thread, &exit_status_v );

  switch( ret )
  {
    case EDEADLK: throw DeadLock();
    case EINVAL : throw NotJoinable();
    case ESRCH  : throw JoinException() << "Internal: No thread with the ID thread could be found";
    default: {}
  }

  m_arg.reset();
  m_running = false;
  if( exit_status ) (*exit_status) = reinterpret_cast< size_t >( exit_status_v );
}

void Thread::cancel( void )
{
  if( !isRunning() ) throw NotRunning();

  const int ret = pthread_cancel( m_thread );

  switch( ret )
  {
    case ESRCH: throw CancelException() << "Internal: No thread with the ID thread could be found";
    default: {}
  }

  // nothing else to do, join() must be called
}

void Thread::yield( void )
{
  pthread_yield();
}

size_t Thread::run( void )
{
  // Was not overloaded
  return size_t(-1);
}

void Thread::start( DispatchArg *arg )
{
  start( &dispatch_arg, arg );
}

void *Thread::dispatch_arg( void *arg )
{
  DispatchArg *const dispatch_arg = (DispatchArg*)arg;
  return (void*)dispatch_arg->run();
}

void *Thread::dispatch_run( void *arg )
{
  Thread *const thread = (Thread*)arg;
  return (void*)thread->run();
}

} // namespace RPGML


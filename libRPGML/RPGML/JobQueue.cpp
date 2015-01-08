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
#include "JobQueue.h"

#include "Thread.h"

namespace RPGML {

JobQueue::JobQueue( GarbageCollector *_gc )
: Collectable( _gc )
{}

JobQueue::~JobQueue( void )
{
  clear();
}

void JobQueue::clear( void )
{
  size_t n = 0;
  {
    Mutex::ScopedLock lock( &m_lock );
    n = m_queue.size();
    m_queue.clear();
    while( m_fill.trylock() ) --n;
  }
  for( size_t i=0; i<n; ++i )
  {
    m_fill.lock();
  }
}

void JobQueue::addJob( Job *job )
{
  Mutex::ScopedLock lock( &m_lock );
  m_queue.push( job );
  ++m_fill;
}

CountPtr< JobQueue::Job > JobQueue::getJob( void )
{
  for(;;)
  {
    --m_fill;
    Mutex::ScopedLock lock( &m_lock );
    if( !m_queue.empty() )
    {
      CountPtr< Job > ret( m_queue.top() );
      m_queue.pop();
      return ret;
    }
    // was deleted
    ++m_fill;
    Thread::yield();
  }
  // never reached
  return (Job*)0;
}

size_t JobQueue::doJob( Job *_job )
{
  CountPtr< Job > job( _job );
  CountPtr< Job::Token > token( job->getToken() );
  addJob( job );
  return token->wait();
}

void JobQueue::gc_clear( void )
{
  clear();
}

void JobQueue::gc_getChildren( Children &children ) const
{
  m_queue.gc_getChildren( children );
}

bool JobQueue::cmp_priority_less( const CountPtr< Job > &x, const CountPtr< Job > &y )
{
  return x->getPriority() < y->getPriority();
}

JobQueue::Job::Job( GarbageCollector *_gc, size_t priority )
: Collectable( _gc )
, m_priority( priority )
{}

JobQueue::Job::~Job( void )
{}

size_t JobQueue::Job::getPriority( void ) const
{
  return m_priority;
}

void JobQueue::Job::setPriority( size_t priority )
{
  m_priority = priority;
}

size_t JobQueue::Job::getReturnValue( void ) const
{
  return m_return_value;
}

size_t JobQueue::Job::work( CountPtr< JobQueue > queue )
{
  return done( doit( queue ) );
}

size_t JobQueue::Job::wait( void )
{
  m_wait_lock.wait();
  return m_return_value;
}

size_t JobQueue::Job::done( size_t return_value )
{
  m_return_value = return_value;
  m_wait_lock.done();
  return m_return_value;
}

void JobQueue::Job::gc_clear( void )
{}

void JobQueue::Job::gc_getChildren( Children &children ) const
{
  (void)children;
}

CountPtr< JobQueue::Job::Token > JobQueue::Job::getToken( void )
{
  return new Token( this );
}

JobQueue::Job::Token::Token( Job *job )
: m_job( job )
, m_return_value( 0 )
{
  if( !m_job.isNull() )
  {
    m_token = m_job->m_wait_lock.getToken();
  }
}

JobQueue::Job::Token::~Token( void )
{}

size_t JobQueue::Job::Token::wait( void )
{
  if( !m_token.isNull() )
  {
    m_token->wait();
    m_token.reset();
    m_return_value = m_job->getReturnValue();
  }
  return m_return_value;
}

JobQueue::EndJob::EndJob( GarbageCollector *_gc )
: Job( _gc )
{}

JobQueue::EndJob::~EndJob( void )
{}

size_t JobQueue::EndJob::doit( CountPtr< JobQueue > )
{
  return End;
}

JobQueue::Queue::Queue( GarbageCollector *_gc )
: Collectable( _gc )
, m_heap( 0 )
{}

JobQueue::Queue::~Queue( void )
{}

bool JobQueue::Queue::empty( void ) const
{
  return m_heap.empty();
}

void JobQueue::Queue::push( Job *job )
{
  m_heap.push_back( job );
  std::push_heap( m_heap.begin(), m_heap.end(), cmp_priority_less );
}

void JobQueue::Queue::pop( void )
{
  std::pop_heap( m_heap.begin(), m_heap.end(), cmp_priority_less );
  m_heap.pop_back();
}

CountPtr< JobQueue::Job > JobQueue::Queue::top( void ) const
{
  return m_heap.front();
}

void JobQueue::Queue::clear( void )
{
  m_heap.clear();
}

size_t JobQueue::Queue::size( void ) const
{
  return m_heap.size();
}

void JobQueue::Queue::gc_clear( void )
{
  clear();
}

void JobQueue::Queue::gc_getChildren( Children &children ) const
{
  m_heap.gc_getChildren( children );
}

} // namespace RPGML


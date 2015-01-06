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
#include "ThreadPool.h"

namespace RPGML {

ThreadPool::ThreadPool( GarbageCollector *_gc, index_t num_threads )
: Base( _gc )
, m_workers( new WorkersArray( _gc, num_threads ) )
, m_queue( new JobQueue( _gc ) )
{
  for( index_t i=0; i<num_threads; ++i )
  {
    (*m_workers)[ i ] = new Worker( _gc, m_queue );
  }
}

ThreadPool::~ThreadPool( void )
{
  clear();
}

CountPtr< JobQueue > ThreadPool::getQueue( void ) const
{
  return m_queue;
}

void ThreadPool::gc_clear( void )
{
  clear();
}

void ThreadPool::gc_getChildren( Children &children ) const
{
  children.add( m_workers );
  children.add( m_queue );
}

void ThreadPool::clear( void )
{
  const size_t num_threads = m_workers->size();

  CountPtr< EndWorker > end = new EndWorker( 0 );
  for( index_t i=0; i<num_threads; ++i )
  {
    m_queue->addJob( end );
  }

  for( index_t i=0; i<num_threads; ++i )
  {
    (*m_workers)[ i ]->join();
  }
}

ThreadPool::EndWorker::EndWorker( GarbageCollector *_gc )
: JobQueue::Job( _gc )
{
  setPriority( 0 );
}

ThreadPool::EndWorker::~EndWorker( void )
{}

size_t ThreadPool::EndWorker::doit( CountPtr< JobQueue > )
{
  return JobQueue::End;
}

ThreadPool::Worker::Worker( GarbageCollector *_gc, JobQueue *queue )
: Thread( _gc, false )
, m_queue( queue )
{
  start();
}

ThreadPool::Worker::~Worker( void )
{}

size_t ThreadPool::Worker::run( void )
{
  for(;;)
  {
    CountPtr< JobQueue::Job > job = m_queue->getJob();
    const size_t ret = job->done( job->doit( m_queue ) );
    if( JobQueue::End == ret ) return 0;
  }
}

void ThreadPool::Worker::gc_clear( void )
{}

void ThreadPool::Worker::gc_getChildren( Children &children ) const
{
  children.add( m_queue );
}

} // namespace RPGML


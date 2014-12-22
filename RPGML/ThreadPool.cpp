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
  return size_t( -1 );
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
    if( size_t(-1) == job->doit( m_queue ) ) return 0;
  }
}

void ThreadPool::Worker::gc_clear( void )
{}

void ThreadPool::Worker::gc_getChildren( Children &children ) const
{
  children.add( m_queue );
}

} // namespace RPGML


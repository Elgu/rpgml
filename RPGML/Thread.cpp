#include "Thread.h"

namespace RPGML {

Thread::Thread( GarbageCollector *_gc )
: Collectable( _gc )
, m_running( false )
{}

Thread::~Thread( void )
{
  cancel();
  join();
}

void Thread::gc_clear( void )
{}

void Thread::gc_getChildren( Children &children ) const
{}

bool Thread::isRunning( void ) const
{
  return m_running;
}

bool Thread::start( void )
{
  return start( &dispatch_run, this );
}

bool Thread::start( void *(*start_routine)(void*), void *arg )
{
  if( isRunning() ) return false;

  if( 0 != pthread_create( &m_thread, 0, start_routine, arg ) )
  {
    return false;
  }

  m_running = true;
  return true;
}

bool Thread::join( void )
{
  if( !isRunning() ) return false;
  void *ret = 0;
  if( 0 != pthread_join( m_thread, &ret ) ) return false;
  m_arg.reset();
  m_running = false;
  return true;
}

bool Thread::cancel( void )
{
  if( !isRunning() ) return false;
  if( 0 != pthread_cancel( m_thread ) ) return false;
  return true;
}

bool Thread::start( DispatchArg *arg )
{
  return start( &dispatch_arg, arg );
}

void *Thread::dispatch_arg( void *arg )
{
  DispatchArg *const dispatch_arg = (DispatchArg*)arg;
  dispatch_arg->run();
  return 0;
}

void *Thread::dispatch_run( void *arg )
{
  Thread *const thread = (Thread*)arg;
  thread->run();
  return 0;
}

} // namespace RPGML


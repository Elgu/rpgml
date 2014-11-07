#ifndef RPGML_Thread_h
#define RPGML_Thread_h

#include "GarbageCollector.h"

#include <memory>
#include <pthread.h>

namespace RPGML {

class Thread : public Collectable
{
public:
  explicit
  Thread( GarbageCollector *_gc );

  virtual ~Thread( void );

  virtual void gc_clear( void );
  virtual void gc_getChildren( Children &children ) const;

  bool isRunning( void ) const;

  // starts in run()
  bool start( void );

  // starts in function, starts with arg
  bool start( void *(*start_routine)(void*), void *arg );

  // starts in method
  template< class ParentClass >
  bool start( ParentClass *parent, void (ParentClass::*method)( void ) )
  {
    if( isRunning() ) return false;
    m_arg.reset( new DispatchMethod< ParentClass >( parent, method ) );
    return start( m_arg.get() );
  }

  // Waits for thread to finish, returns whether it was running
  bool join( void );

  // Cancels thread if running, join() should be called if one wants to wait
  // for the cancel to have taken effect returns whether it was running
  bool cancel( void );

protected:
  virtual void run( void ) {}

private:
  struct DispatchArg
  {
    DispatchArg( void ) {}
    virtual ~DispatchArg( void ) {}
    virtual void run( void ) = 0;
  };

  template< class ParentClass >
  struct DispatchMethod : public DispatchArg
  {
    DispatchMethod( ParentClass *_parent, void (ParentClass::*_method)( void ) )
    : parent( _parent )
    , method( _method )
    {}

    virtual ~DispatchMethod( void ) {}

    ParentClass *parent;
    void (ParentClass::*method)( void );

    virtual void run( void )
    {
      (parent->*method)();
    }
  };

  bool start( DispatchArg *arg );

  static void *dispatch_arg( void *arg );
  static void *dispatch_run( void *arg );

  std::auto_ptr< DispatchArg > m_arg;
  pthread_t m_thread;
  bool m_running;
};


} // namespace RPGML

#endif


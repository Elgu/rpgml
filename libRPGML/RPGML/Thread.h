#ifndef RPGML_Thread_h
#define RPGML_Thread_h

#include "GarbageCollector.h"
#include "Exception.h"

#include <memory>
#include <pthread.h>

namespace RPGML {

//! @brief Thread base class, overload run() or start in an object's method
class Thread : public Collectable
{
public:
  //! @brief Thread exception base class
  EXCEPTION_BASE( Exception );
  //! @brief Exception for when the thread is already running, but start() is called
  EXCEPTION_DERIVED_FIXED_TEXT( AlreadyRunning, Exception );
  //! @brief Exception for when the thread is not running, but is required
  EXCEPTION_DERIVED_FIXED_TEXT( NotRunning, Exception );
  //! @brief base Exception for errors in start()
  EXCEPTION_DERIVED( StartException, Exception );
  //! @brief Exception for when the thread could not be created for lack of resources
  EXCEPTION_DERIVED_FIXED_TEXT( InsufficientResources, StartException );
  //! @brief Exception for errors in cancel()
  EXCEPTION_DERIVED( CancelException, Exception );
  //! @brief Exception for errors in join()
  EXCEPTION_DERIVED( JoinException, Exception );
  //! @brief Exception for when threads try to wait on each other, i.e. two threads calling join() on each other
  EXCEPTION_DERIVED_FIXED_TEXT( DeadLock, JoinException );
  //! @brief Exception for dupplicate join() or just not joinable
  EXCEPTION_DERIVED_FIXED_TEXT( NotJoinable, JoinException );

  /*! @brief constructor: starts thread in run(), if requested
   *
   * @param _gc               [in] The GarbageCollector at which this object registers itself
   * @param start_immediately [in] Whether the thread should be started immediately
   * @throws StartException Only if the thread should be started immediately, see exception text for details
   */
  explicit
  Thread( GarbageCollector *_gc, bool start_immediately = false );

  //! @brief destructor, calls cancel(), then join(), if neccessary
  virtual ~Thread( void );

  //! @brief Overload for Collectable
  virtual void gc_clear( void );
  //! @brief Overload for Collectable
  virtual void gc_getChildren( Children &children ) const;

  //! @brief Returns whether the thread is running (start() was called and not yet join())
  bool isRunning( void ) const;

  /*! @brief Starts the thread in run(), will be marked as running
   *
   * @throws AlreadyRunning When the thread is already running i.e. join() was not called yet
   * @throws StartException For other errors like lack of resources, see exception text for details
   * @throws InsufficientResources i.e. thread limit reached or out of memory
   */
  void start( void );

  /*! @brief Starts the thread in the function specified, will be marked as running
   *
   * @param  start_routine [in] The thread will be started in that function
   * @param  arg           [in] This value will be supplied to the thread as argument to the function
   * @throws AlreadyRunning When the thread is already running i.e. join() was not called yet
   * @throws StartException For other errors like lack of resources, see exception text for details
   * @throws InsufficientResources i.e. thread limit reached or out of memory
   */
  void start( void *(*start_routine)(void*), void *arg = 0 );

  /*! @brief Starts the thread in the specified method of a parent object, will be marked as running
   *
   * Example: Obj obj; Thread thread; thread.start( &obj, &Obj::method );
   * @param parent [in] The object of which the method will be called
   * @param method [in] Method-pointer of a method of the parent's class, not base-class
   * @throws AlreadyRunning When the thread is already running i.e. join() was not called yet
   * @throws StartException For other errors like lack of resources, see exception text for details
   * @throws InsufficientResources i.e. thread limit reached or out of memory
   */
  template< class ParentClass >
  void start( ParentClass *parent, size_t (ParentClass::*method)( void ) )
  {
    if( isRunning() ) throw AlreadyRunning();
    m_arg.reset( new DispatchMethod< ParentClass >( parent, method ) );
    start( m_arg.get() );
  }

  /*! @brief Waits for the thread to exit
   *
   * optionaly returns the return value of run() in exit_status,
   * will be marked as not running afterwards
   * @param exit_status [out] If not null, the exit status of the thread will be written to that location
   * @throws NotRunning When the thread is not running i.e. start() was not called
   * @throws JoinException For other errors like lack of resources, see exception text for details
   * @throws DeadLock For circular join()
   * @throws NotJoinable For when another thread is already waiting or just not joinable
   */
  void join( size_t *exit_status = 0 );

  /*! @brief Requests the thread to exit
   *
   * The stack might not get cleaned up, is still marked as running until join() was called
   * @throws NotRunning When the thread is not running i.e. start() was not called
   * @throws CancelException For other errors like lack of resources, see exception text for details
   */
  void cancel( void );

  //! @brief Instructs scheduler to maybe let another thread run on the processor
  static void yield( void );

protected:
  /*! @brief Can be overloaded to implement the Thread
   *
   * does nothing by default
   * @return Returns the thread's application-defined exit status, size_t(-1) by default
   */
  virtual size_t run( void );

private:
  struct DispatchArg
  {
    DispatchArg( void ) {}
    virtual ~DispatchArg( void ) {}
    virtual size_t run( void ) = 0;
  };

  template< class ParentClass >
  struct DispatchMethod : public DispatchArg
  {
    DispatchMethod( ParentClass *_parent, size_t (ParentClass::*_method)( void ) )
    : parent( _parent )
    , method( _method )
    {}

    virtual ~DispatchMethod( void ) {}

    ParentClass *parent;
    size_t (ParentClass::*method)( void );

    virtual size_t run( void )
    {
      return (parent->*method)();
    }
  };

  void start( DispatchArg *arg );

  static void *dispatch_arg( void *arg );
  static void *dispatch_run( void *arg );

  std::auto_ptr< DispatchArg > m_arg;
  pthread_t m_thread;
  bool m_running;
};


} // namespace RPGML

#endif


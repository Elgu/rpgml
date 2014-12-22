#ifndef RPGML_ThreadPool_h
#define RPGML_ThreadPool_h

#include "Thread.h"
#include "JobQueue.h"
#include "GarbageCollector.h"
#include "Array.h"

namespace RPGML {

class ThreadPool : public Collectable
{
public:
  typedef Collectable Base;
  EXCEPTION_BASE( Exception );

  explicit
  ThreadPool( GarbageCollector *_gc, index_t num_threads );

  virtual ~ThreadPool( void );

  CountPtr< JobQueue > getQueue( void ) const;

  virtual void gc_clear( void );
  virtual void gc_getChildren( Children &children ) const;

private:
  void clear( void );

  class EndWorker : public JobQueue::Job
  {
  public:
    explicit
    EndWorker( GarbageCollector *_gc );
    virtual ~EndWorker( void );
    virtual size_t doit( CountPtr< JobQueue > queue );
  };

  class Worker : public Thread
  {
  public:
    explicit
    Worker( GarbageCollector *_gc, JobQueue *queue );

    virtual ~Worker( void );
    virtual size_t run( void );
    virtual void gc_clear( void );
    virtual void gc_getChildren( Children &children ) const;

  private:
    CountPtr< JobQueue > m_queue;
  };

  typedef Array< CountPtr< Worker >, 1 > WorkersArray;
  CountPtr< WorkersArray > m_workers;
  CountPtr< JobQueue > m_queue;

private:
  ThreadPool( const ThreadPool & );
  ThreadPool &operator=( const ThreadPool & );
};

} // namespace RPGML

#endif


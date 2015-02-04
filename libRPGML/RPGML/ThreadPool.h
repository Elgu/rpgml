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
#ifndef RPGML_ThreadPool_h
#define RPGML_ThreadPool_h

#include "Thread.h"
#include "JobQueue.h"
#include "GarbageCollector.h"
#include "Array.h"

namespace RPGML {

class ThreadPool : public Collectable
{
  typedef Collectable Base;
public:
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
  protected:
    virtual size_t doit( CountPtr< JobQueue > queue );
  };

  class Worker : public Thread
  {
    typedef Thread Base;
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

  typedef Array< CountPtr< Worker > > WorkersArray;
  CountPtr< WorkersArray > m_workers;
  CountPtr< JobQueue > m_queue;

private:
  ThreadPool( const ThreadPool & );
  ThreadPool &operator=( const ThreadPool & );
};

} // namespace RPGML

#endif


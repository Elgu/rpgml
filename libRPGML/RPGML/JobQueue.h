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
#ifndef RPGML_JobQueue_h
#define RPGML_JobQueue_h

#include "GarbageCollector.h"
#include "Semaphore.h"
#include "Mutex.h"
#include "Array.h"
#include "WaitLock.h"

#include <algorithm>

namespace RPGML {

class JobQueue : public Collectable
{
public:
  explicit
  JobQueue( GarbageCollector *_gc );
  virtual ~JobQueue( void );

  void clear( void );

  static const size_t End = size_t(-1)/2;

  class Job : public Collectable
  {
    friend class JobQueue;
  public:
    explicit
    Job( GarbageCollector *_gc, size_t priority = 0 );
    virtual ~Job( void );

    virtual size_t doit( CountPtr< JobQueue > queue ) = 0;

    size_t wait( void );
    //! Called by workers like size_t ret = done( doit( queue ) );
    size_t done( size_t return_value );

    size_t getPriority( void ) const;
    void setPriority( size_t priority );

    virtual void gc_clear( void );
    virtual void gc_getChildren( Children &children ) const;
  private:
    WaitLock m_wait_lock;
    size_t m_priority;
    size_t m_return_value;
  };

  class EndJob : public Job
  {
  public:
    EndJob( GarbageCollector *_gc );
    virtual ~EndJob( void );
    virtual size_t doit( CountPtr< JobQueue > queue );
  };

  void addJob( Job *job );
  CountPtr< Job > getJob( void );

  //! Blocks until job is done
  size_t doJob( Job *job );

  virtual void gc_clear( void );
  virtual void gc_getChildren( Children &children ) const;

private:
  static bool cmp_priority_less( const CountPtr< Job > &x, const CountPtr< Job > &y );

  class Queue : public Collectable
  {
  public:
    Queue( GarbageCollector *_gc = 0 );
    virtual ~Queue( void );
    bool empty( void ) const;
    size_t size( void ) const;
    void push( Job *job );
    void pop( void );
    CountPtr< Job > top( void ) const;
    void clear( void );
    virtual void gc_clear( void );
    virtual void gc_getChildren( Children &children ) const;
  private:
    Array< CountPtr< Job >, 1 > m_heap;
  };

  typedef Queue queue_t;
  queue_t m_queue;
  Semaphore m_fill;
  Mutex m_lock;
};

} // namespace RPGML

#endif


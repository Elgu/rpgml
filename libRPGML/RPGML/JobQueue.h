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
  typedef Collectable Base;
public:
  explicit
  JobQueue( GarbageCollector *_gc );
  virtual ~JobQueue( void );

  void clear( void );

  static const size_t End = size_t(-1)/2;

  class Job : public Collectable
  {
    typedef Collectable Base;
  public:
    explicit
    Job( GarbageCollector *_gc, size_t priority = 0 );
    virtual ~Job( void );

    virtual void gc_clear( void );
    virtual void gc_getChildren( Children &children ) const;

    //! Calls doit() and done()
    size_t work( CountPtr< JobQueue > queue );

    size_t getPriority( void ) const;
    void setPriority( size_t priority );

    //! Only valid after done() has been called
    size_t getReturnValue( void ) const;

    //! For waiting once on a Job, get one before addJob()
    class Token : public Collectable
    {
      typedef Collectable Base;
    private:
      explicit
      Token( GarbageCollector *_gc, Job *job );
    public:
      virtual ~Token( void );
      virtual void gc_clear( void );
      virtual void gc_getChildren( Children &children ) const;
      size_t wait( void );
      void destroy( void );
    private:
      friend class Job;
      Token( const Token & );
      Token &operator=( const Token & );
      CountPtr< Job > m_job;
      CountPtr< WaitLock::Token > m_token;
      size_t m_return_value;
    };

    CountPtr< Token > getToken( void );

  protected:
    virtual size_t doit( CountPtr< JobQueue > queue ) = 0;

  private:
    friend class JobQueue;
    friend class Token;
    //! Use a Token to wait for a Job, getToken() before addJob()
    size_t wait( void );
    //! Called by workers like size_t ret = done( doit( queue ) );
    size_t done( size_t return_value );
    WaitLock m_wait_lock;
    size_t m_priority;
    size_t m_return_value;
  };

  class EndJob : public Job
  {
    typedef Job Base;
  public:
    EndJob( GarbageCollector *_gc );
    virtual ~EndJob( void );
  protected:
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
    typedef Collectable Base;
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


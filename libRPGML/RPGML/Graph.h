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
#ifndef RPGML_Graph_h
#define RPGML_Graph_h

#include "Node.h"
#include "Atomic.h"
#include "GarbageCollector.h"
#include "JobQueue.h"
#include "Mutex.h"

#include <map>
#include <sstream>
#include <list>
#include <iostream>

namespace RPGML {

class JobQueue;

class Graph : public Collectable
{
public:
  typedef Collectable Base;

  explicit
  Graph( GarbageCollector *_gc );

  virtual ~Graph( void );

  void addNode( Node *node );
  bool alreadyAdded( const Node *node, index_t *index = 0 ) const;

  bool empty( void ) const;

  void merge( void );

  void setEverythingChanged( bool changed = true );

  //! Must be called by the main thread of a process
  void execute( JobQueue *queue );

  virtual void gc_clear( void );
  virtual void gc_getChildren( Children &children ) const;

  class Error
  {
    friend class Graph;
  private:
    explicit Error( Graph *graph );
  public:
    ~Error( void );
    template< class T > Error &operator<<( const T &x ) { m_str->str << x; return (*this); }
    std::string getText( void );
  private:
    class Stream : public Refcounted
    {
    public:
      std::ostringstream str;
    };
    CountPtr< Stream > m_str;
    CountPtr< Graph > m_graph;
  };

  Error error( void );
  void printErrors( std::ostream &o ) const;
  bool hasErrors( void ) const;
  void clearErrors( void );

  void setExitRequest( const std::string &request_text );
  bool hasExitRequest( void ) const;
  std::string getExitRequest( void ) const;
  void clearExitRequest( void );

private:
  friend class Error;
  class GraphNode;
  typedef Array< CountPtr< GraphNode >, 1 > GN_Array_t;

  class GraphNode : public JobQueue::Job
  {
  public:
    explicit
    GraphNode( GarbageCollector *_gc, Graph *_graph, Node *_node );

    void schedule( JobQueue *queue, JobQueue *main_thread_queue );
    void clear_order( void );
    void reset_predecessor_counter( void );

    virtual void gc_clear( void );
    virtual void gc_getChildren( Children &children ) const;

    int compare( const GraphNode &other ) const;
    bool equivalent( const GraphNode &other ) const;

    static
    bool compare_less( const GN_Array_t::Element &x1, const GN_Array_t::Element &x2 );

    GN_Array_t predecessors;
    GN_Array_t successors;
    CountPtr< Graph > graph;
    CountPtr< Node > node;
    CountPtr< JobQueue > main_thread;
    Atomic< size_t > predecessors_to_be_executed;
    int marker;

  protected:
    virtual size_t doit( CountPtr< JobQueue > queue );
  };

  class EndNode : public GraphNode
  {
  public:
    explicit
    EndNode( GarbageCollector *_gc, Graph *_graph );
  protected:
    virtual size_t doit( CountPtr< JobQueue > queue );
  };

  void determine_order( void );
  void report( const std::string &error_text );

  mutable Mutex m_errors_lock;
  mutable Mutex m_exit_request_lock;
  typedef std::list< std::string > errors_t;
  errors_t m_errors;
  std::string m_exit_request;
  GN_Array_t m_nodes;
  typedef std::map< const Node*, index_t > Node_to_index_t;
  Node_to_index_t m_Node_to_index;
  CountPtr< EndNode > m_end_node;
  bool m_order_determined;
};

} // namespace RPGML

#endif

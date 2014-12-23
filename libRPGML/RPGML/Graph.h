#ifndef RPGML_Graph_h
#define RPGML_Graph_h

#include "Node.h"
#include "Atomic.h"
#include "GarbageCollector.h"
#include "JobQueue.h"

#include <map>

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

  void execute( JobQueue *queue );

  virtual void gc_clear( void );
  virtual void gc_getChildren( Children &children ) const;

private:
  class GraphNode;
  typedef Array< CountPtr< GraphNode >, 1 > GN_Array_t;

  class GraphNode : public JobQueue::Job
  {
  public:
    explicit
    GraphNode( GarbageCollector *_gc, Node *_node );

    void schedule( JobQueue *queue );
    void clear_order( void );
    void reset_predecessor_counter( void );

    virtual void gc_clear( void );
    virtual void gc_getChildren( Children &children ) const;

    virtual size_t doit( CountPtr< JobQueue > queue );

    GN_Array_t predecessors;
    GN_Array_t successors;
    CountPtr< Node > node;
    Atomic< size_t > predecessors_to_be_executed;
    int marker;
  };

  class EndNode : public GraphNode
  {
  public:
    explicit
    EndNode( GarbageCollector *_gc );
    virtual size_t doit( CountPtr< JobQueue > queue );
    void wait( void );
  private:
    Semaphore m_wait_lock;
  };

  void determine_order( void );

  GN_Array_t m_nodes;
  typedef std::map< const Node*, index_t > Node_to_index_t;
  Node_to_index_t m_Node_to_index;
  CountPtr< EndNode > m_end_node;
  bool m_order_determined;
};

} // namespace RPGML

#endif

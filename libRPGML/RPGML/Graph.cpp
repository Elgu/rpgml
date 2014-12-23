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
#include "Graph.h"

#include <iostream>

namespace RPGML {

Graph::Graph( GarbageCollector *_gc )
: Base( _gc )
, m_nodes( 0 )
, m_order_determined( false )
{}

Graph::~Graph( void )
{}

void Graph::addNode( Node *node )
{
  if( !node ) return;

  std::vector< Node* > to_be_checked;
  to_be_checked.push_back( node );

  while( !to_be_checked.empty() )
  {
    Node *const curr = to_be_checked.back();
    to_be_checked.pop_back();

    if( !curr ) continue;

    if( m_Node_to_index.end() != m_Node_to_index.find( curr ) )
    {
      // Already added
      continue;
    }

    m_Node_to_index[ curr ] = m_nodes.size();
    m_nodes.push_back( new GraphNode( getGC(), curr ) );
    m_order_determined = false;

    // Inputs must also be added to the Graph
    for( index_t i( 0 ), end_i( curr->getNumInputs() ); i < end_i; ++i )
    {
      const Input *const input = curr->getInput( i );
      if( !input->isConnected() ) continue;

      to_be_checked.push_back( input->getOutput()->getParent() );
    }
  }
}

bool Graph::alreadyAdded( const Node *node, index_t *index ) const
{
  const Node_to_index_t::const_iterator i = m_Node_to_index.find( node );

  if( m_Node_to_index.end() != i )
  {
    // already added
    if( index ) (*index) = i->second;
    return true;
  }

  return false;
}

void Graph::execute( JobQueue *queue )
{
  if( !m_order_determined )
  {
    determine_order();
  }

  for( index_t gni( 0 ), end( m_nodes.size() ); gni < end; ++gni )
  {
    GraphNode *const gn = m_nodes[ gni ];
    gn->reset_predecessor_counter();
  }
  m_end_node->reset_predecessor_counter();

  for( index_t gni( 0 ), end( m_nodes.size() ); gni < end; ++gni )
  {
    GraphNode *const gn = m_nodes[ gni ];
    if( gn->predecessors.empty() )
    {
      gn->schedule( queue );
    }
  }

  m_end_node->wait();
}

void Graph::determine_order( void )
{
  // Reset order
  for( index_t gni( 0 ), end( m_nodes.size() ); gni < end; ++gni )
  {
    GraphNode *const gn = m_nodes[ gni ];
    gn->clear_order();
  }

  // Add predecessors and successors
  for( index_t gni( 0 ), end( m_nodes.size() ); gni < end; ++gni )
  {
    GraphNode *const gn = m_nodes[ gni ];
    const Node *const node = gn->node;

    // Inputs
    for( index_t i( 0 ), end_i( node->getNumInputs() ); i < end_i; ++i )
    {
      const Input *const input = node->getInput( i );
      if( !input ) continue;
      if( !input->isConnected() ) continue;

      const Node *const predecessor_node = input->getOutput()->getParent();

      index_t predecessor_node_index = index_t(-1);
      if( !alreadyAdded( predecessor_node, &predecessor_node_index ) )
      {
        throw "Input is connected to Node not belonging to this Graph";
      }

      GraphNode *const predecessor = m_nodes[ predecessor_node_index ];

      gn->predecessors.push_back( predecessor );
      predecessor->successors.push_back( gn );
    }
  }

  // Connect Sink-Nodes to End Node
  m_end_node.reset( new EndNode( getGC() ) );
  for( index_t gni( 0 ), end( m_nodes.size() ); gni < end; ++gni )
  {
    GraphNode *const gn = m_nodes[ gni ];

    if( gn->successors.empty() )
    {
      gn->successors.push_back( m_end_node );
      m_end_node->predecessors.push_back( gn );
    }
  }

  // Priority
  {
    std::vector< GraphNode* > to_be_checked;

    // Prepare
    for( index_t gni( 0 ), end( m_nodes.size() ); gni < end; ++gni )
    {
      GraphNode *const gn = m_nodes[ gni ];
      if( gn->successors.empty() )
      {
        // Sink Node
        to_be_checked.push_back( gn );
        gn->marker = 1;
        gn->setPriority( 0 );
      }
      else
      {
        gn->marker = 0;
        gn->setPriority( 0 );
      }
    }

    // Update prio
    while( !to_be_checked.empty() )
    {
      GraphNode *const gn = to_be_checked.back();
      to_be_checked.pop_back();
      gn->marker = 0;

      const size_t prio = gn->getPriority();
      const size_t pred_prio = prio+1;

      for( index_t i( 0 ), end( gn->predecessors.size() ); i < end; ++i )
      {
        GraphNode *const predecessor = gn->predecessors[ i ];
        if( predecessor->getPriority() < pred_prio )
        {
          predecessor->setPriority( pred_prio );
          if( !predecessor->marker )
          {
            to_be_checked.push_back( predecessor );
            predecessor->marker = 1;
          }
        }
      }
    } // to_be_checked
  }

  m_order_determined = true;
}

void Graph::gc_clear( void )
{
  m_nodes.clear();
  m_Node_to_index.clear();
  m_end_node.reset();
}

void Graph::gc_getChildren( Children &children ) const
{
  m_nodes.gc_getChildren( children );
  children.add( m_end_node );
}

Graph::GraphNode::GraphNode( GarbageCollector *_gc, Node *_node )
: JobQueue::Job( _gc )
, predecessors( 0 )
, successors( 0 )
, node( _node )
, marker( 0 )
{}

void Graph::GraphNode::schedule( JobQueue *queue )
{
  queue->addJob( this );
}

void Graph::GraphNode::clear_order( void )
{
  predecessors.clear();
  successors.clear();
}

void Graph::GraphNode::reset_predecessor_counter( void )
{
  predecessors_to_be_executed = predecessors.size();
}

void Graph::GraphNode::gc_clear( void )
{
  predecessors.clear();
  successors.clear();
  node.reset();
}

void Graph::GraphNode::gc_getChildren( Children &children ) const
{
  predecessors.gc_getChildren( children );
  successors.gc_getChildren( children );
  children.add( node );
}

size_t Graph::GraphNode::doit( CountPtr< JobQueue > queue )
{
  try
  {
    if( !node->tick() )
    {
      throw RPGML::Exception() << "Node execution failed";
    }
  }
  catch( const RPGML::Exception &e )
  {
    std::cerr << e.what() << std::endl;
    return 0;
  }
  catch( const std::exception &e )
  {
    std::cerr << e.what() << std::endl;
    return 0;
  }
  catch( const char *e )
  {
    std::cerr << e << std::endl;
    return 0;
  }
  catch( ... )
  {
    std::cerr << "Caught some exception" << std::endl;
    return 0;
  }

  // Try to schedule successors
  for( GN_Array_t::const_iterator succ( successors.begin() ), end( successors.end() ); succ != end; ++succ )
  {
    if( 0 == --(*succ)->predecessors_to_be_executed )
    {
      (*succ)->schedule( queue );
    }
  }

  return 1;
}

Graph::EndNode::EndNode( GarbageCollector *_gc )
: GraphNode( _gc, 0 )
{}

size_t Graph::EndNode::doit( CountPtr< JobQueue > )
{
  m_wait_lock.unlock();
  return 0;
}

void Graph::EndNode::wait( void )
{
  m_wait_lock.lock();
}

} // namespace RPGML


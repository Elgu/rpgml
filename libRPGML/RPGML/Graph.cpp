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
#include <cstring>
#include <algorithm>

using namespace std;

namespace RPGML {

Graph::Graph( GarbageCollector *_gc )
: Base( _gc )
, m_nodes( 0 )
, m_order_determined( false )
{}

Graph::~Graph( void )
{}

bool Graph::empty( void ) const
{
  return m_nodes.empty();
}

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
    m_nodes.push_back( new GraphNode( getGC(), this, curr ) );
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

void Graph::merge( void )
{
//  cerr << "merge: starting with " << m_nodes.size() << " nodes" << endl;

  bool merged_one;
  do
  {
    merged_one = false;

    GN_Array_t new_nodes( 0 );

    sort( m_nodes.begin(), m_nodes.end(), GraphNode::compare_less );

    const index_t num_nodes = m_nodes.size();

    // Start of first block of equivalent Nodes
    index_t equal_first = 0;
    index_t equal_last  = 0;
    // Run one time more to be able to merge the last block
    // Start comparing from 1, start first block at 0
    for( index_t i=1; i<=num_nodes; ++i )
    {
      if( i < num_nodes && m_nodes[ equal_first ]->equivalent( *m_nodes[ i ] ) )
      {
        equal_last = i;
      }
      else
      {
        // Merge block of equivalent Nodes into first of that block
        GN_Array_t::Element &gn_merge_into = m_nodes[ equal_first ];
        Node *const merge_into = gn_merge_into->node;

        for( index_t merge_i = equal_first+1; merge_i <= equal_last; ++merge_i )
        {
          GN_Array_t::Element &gn_merge_node = m_nodes[ merge_i ];
          Node *const merge_node = gn_merge_node->node;

          /*
          cerr
            << "Merging '" << merge_node->getIdentifier() << "'"
            << " into '" << merge_into->getIdentifier() << "'"
            << endl
            ;
            */

          bool merge_failed = false;

          const index_t num_outputs = merge_node->getNumOutputs();

          // Check, whether all Outputs needed by the to-be-merged Node are present in the merge-into Node
          // Remember the indeces, so the string lookup does not have to be done twice
          vector< index_t > into_output_index( num_outputs );
          for( index_t o=0; o<num_outputs; ++o )
          {
            Output *const merge_output = merge_node->getOutput( o );
            try
            {
              // throws, if not found
              (void)merge_into->getOutput( merge_output->getIdentifier(), &into_output_index[ o ] );
            }
            catch( const Node::NotFound & )
            {
              merge_failed = true;
              break;
            }
          }

          if( !merge_failed )
          {
            // Do the merge:
            // Connect all Inputs connected to the Outputs of the to-be-merged Node
            // to the corresponding Outputs of the merge-into Node
            for( index_t o=0; o<num_outputs; ++o )
            {
              Output *const merge_output = merge_node->getOutput( o );
              Output *const into_output  = merge_into->getOutput( into_output_index[ o ] );

              while( merge_output->isConnected() )
              {
                Input *const input = merge_output->inputs_begin()->get();
                if( input ) input->connect( into_output );
              }
            }
            merged_one = true;
          }
          else
          {
            // Not merged, so keep it
            new_nodes.push_back( gn_merge_node );
          }
        }

        // Add first to the new nodes array
        new_nodes.push_back( gn_merge_into );

        // Start of next block
        equal_first = equal_last = i;
      }
    }

    // Make merged Nodes current
    m_nodes.swap( new_nodes );
  }
  while( merged_one );

  // Update index
  m_Node_to_index.clear();
  for( index_t i( 0 ), end( m_nodes.size() ); i < end; ++i )
  {
    m_Node_to_index[ m_nodes[ i ]->node.get() ] = i;
  }

//  cerr << "merge: ending with " << m_nodes.size() << " nodes" << endl;
}

void Graph::setEverythingChanged( bool changed )
{
  for( index_t gni( 0 ), end( m_nodes.size() ); gni < end; ++gni )
  {
    GraphNode *const gn = m_nodes[ gni ];
    gn->node->setAllOutputChanged( changed );
  }
}

void Graph::execute( JobQueue *queue )
{
  if( m_nodes.empty() ) return;

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

  CountPtr< JobQueue > main_thread_queue = new JobQueue( getGC() );
  m_end_node->main_thread = main_thread_queue;

  for( index_t gni( 0 ), end( m_nodes.size() ); gni < end; ++gni )
  {
    GraphNode *const gn = m_nodes[ gni ];
    if( gn->predecessors.empty() )
    {
      gn->schedule( queue, main_thread_queue );
    }
  }

  for(;;)
  {
    CountPtr< JobQueue::Job > job = main_thread_queue->getJob();
    const size_t job_ret = job->work( main_thread_queue );
    if( JobQueue::End == job_ret ) break;
  }
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
  m_end_node.reset( new EndNode( getGC(), this ) );
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

Graph::Error Graph::error( void )
{
  return Error( this );
}

void Graph::report( const std::string &error_text )
{
  Mutex::ScopedLock lock( &m_errors_lock );
  m_errors.push_back( error_text );
}

void Graph::printErrors( std::ostream &o ) const
{
  Mutex::ScopedLock lock( &m_errors_lock );

  for( errors_t::const_iterator i( m_errors.begin() ), end( m_errors.end() ); i != end; ++i )
  {
    o << (*i) << std::endl;
  }
}

bool Graph::hasErrors( void ) const
{
  Mutex::ScopedLock lock( &m_errors_lock );
  return !m_errors.empty();
}

void Graph::clearErrors( void )
{
  Mutex::ScopedLock lock( &m_errors_lock );
  m_errors.clear();
}

void Graph::setExitRequest( const std::string &request_text )
{
  Mutex::ScopedLock lock( &m_exit_request_lock );
  if( !request_text.empty() )
  {
    m_exit_request = request_text;
  }
  else
  {
    m_exit_request = "exit";
  }
}

bool Graph::hasExitRequest( void ) const
{
  Mutex::ScopedLock lock( &m_exit_request_lock );
  return !m_exit_request.empty();
}

std::string Graph::getExitRequest( void ) const
{
  Mutex::ScopedLock lock( &m_exit_request_lock );
  return m_exit_request;
}

void Graph::clearExitRequest( void )
{
  Mutex::ScopedLock lock( &m_exit_request_lock );
  m_exit_request.clear();
}

Graph::Error::Error( Graph *graph )
: m_str( new Stream )
, m_graph( graph )
{}

Graph::Error::~Error( void )
{
  std::string text = getText();
  if( !text.empty() )
  {
//    std::cerr << "Graph::Error::~Error(): " << text << std::endl;
    m_graph->report( text );
  }
}

std::string Graph::Error::getText( void )
{
  return m_str->str.str();
}

Graph::GraphNode::GraphNode( GarbageCollector *_gc, Graph *_graph, Node *_node )
: JobQueue::Job( _gc )
, predecessors( 0 )
, successors( 0 )
, graph( _graph )
, node( _node )
, marker( 0 )
{}

void Graph::GraphNode::schedule( JobQueue *queue, JobQueue *main_thread_queue )
{
//  if( !node.isNull() ) std::cerr << "schedule " << node->getIdentifier() << std::endl;
  main_thread = main_thread_queue;
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

namespace Graph_impl {

  template< class T >
  static inline
  bool cmp( const T &x1, const T &x2 )
  {
    if( x1 == x2 ) return 0;
    if( x1 < x2 ) return -1;
    return 1;
  }

} // namespace Graph_impl

bool Graph::GraphNode::equivalent( const GraphNode &other ) const
{
  return ( 0 == compare( other ) );
}

bool Graph::GraphNode::compare_less( const GN_Array_t::Element &x1, const GN_Array_t::Element &x2 )
{
  return ( 0 > x1->compare( *x2 ) );
}

int Graph::GraphNode::compare( const GraphNode &other ) const
{
  using namespace Graph_impl;

  const char *const node_name = node->getName();
  const char *const other_name = other.node->getName();

  // Names are static strings defined in the .so files
  if( node_name != other_name ) return ::strcmp( node_name, other_name );

  const index_t node_num_inputs = node->getNumInputs();
  const index_t other_num_inputs = other.node->getNumInputs();

  const int cmp_num_inputs = cmp( node_num_inputs, other_num_inputs );
  if( 0 != cmp_num_inputs ) return cmp_num_inputs;

  const index_t node_num_params = node->getNumParams();
  const index_t other_num_params = other.node->getNumParams();

  const int cmp_num_params = cmp( node_num_params, other_num_params );
  if( 0 != cmp_num_params ) return cmp_num_params;

  for( index_t i=0; i<node_num_inputs; ++i )
  {
    const Input *const node_input = node->getInput( i );
    const String &node_input_identifier = node_input->getIdentifier();

    const Input *other_input = 0;
    try
    {
      other_input = other.node->getInput( node_input_identifier );
    }
    catch( const Node::NotFound & )
    {
      return 1;
    }
    if( !other_input ) return 1;

    const Output *const node_output = node_input->getOutput();
    const Output *const other_output = other_input->getOutput();

    const int cmp_output = cmp( node_output, other_output );
    if( 0 != cmp_output ) return cmp_output;
  }

  for( index_t i=0; i<node_num_params; ++i )
  {
    const Param *const node_param = node->getParam( i );
    const String &node_param_identifier = node_param->getIdentifier();

    const Param *other_param = 0;
    try
    {
      other_param = other.node->getParam( node_param_identifier );
    }
    catch( const Node::NotFound & )
    {
      return 1;
    }
    if( !other_param ) return 1;

    CountPtr< Param::SettingsIterator >
        node_settings = node_param->getSettings()
      , other_settings = other_param->getSettings()
      ;
    for(
      ;     !node_settings->done()
        && !other_settings->done()
      ;    node_settings->next()
        , other_settings->next()
      )
    {
      const int cmp_param = node_settings->get().compare( other_settings->get() );
      if( 0 != cmp_param ) return cmp_param;
    }

    if( !node_settings->done() ) return -1;
    if( !other_settings->done() ) return 1;
  }

  return 0;
}

void Graph::GraphNode::gc_clear( void )
{
  predecessors.clear();
  successors.clear();
  graph.reset();
  node.reset();
}

void Graph::GraphNode::gc_getChildren( Children &children ) const
{
  predecessors.gc_getChildren( children );
  successors.gc_getChildren( children );
  children.add( graph );
  children.add( node );
}

size_t Graph::GraphNode::doit( CountPtr< JobQueue > queue )
{
  size_t ret = 1;
  try
  {
//    std::cerr << "executing Node " << node->getIdentifier() << std::endl;
    node->tick( main_thread );
//    std::cerr << "executing Node " << node->getIdentifier() << " done" << std::endl;
    ret = 0;
  }
  catch( const RPGML::Node::ExitRequest &e )
  {
//    std::cerr << "RPGML::Node::ExitRequest" << std::endl;
    graph->setExitRequest( e.what() );
    ret = 1;
  }
  catch( const RPGML::Exception &e )
  {
    e.print_backtrace();
    graph->error()
      << node->getIdentifier() << ": " << e.what()
      ;
    ret = 0;
  }
  catch( const std::exception &e )
  {
    graph->error()
      << node->getIdentifier() << ": " << e.what()
      ;
    ret = 0;
  }
  catch( const char *e )
  {
    graph->error()
      << node->getIdentifier() << ": " << e
      ;
    ret = 0;
  }
  catch( ... )
  {
    graph->error()
      << node->getIdentifier()
      << ": Caught some exception"
      ;
    ret = 0;
  }

  // Try to schedule successors
  for( GN_Array_t::const_iterator succ( successors.begin() ), end( successors.end() ); succ != end; ++succ )
  {
    if( 0 == --(*succ)->predecessors_to_be_executed )
    {
      (*succ)->schedule( queue, main_thread );
    }
  }

  return ret;
}

Graph::EndNode::EndNode( GarbageCollector *_gc, Graph *_graph )
: GraphNode( _gc, _graph, 0 )
{}

size_t Graph::EndNode::doit( CountPtr< JobQueue > )
{
//  std::cerr << "executing EndNode " << std::endl;
  if( !main_thread.isNull() )
  {
//    std::cerr << "adding EndJob" << std::endl;
    main_thread->addJob( new JobQueue::EndJob( getGC() ) );
  }
  return 0;
}

} // namespace RPGML


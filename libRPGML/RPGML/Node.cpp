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
#include "Node.h"

#include "String.h"
#include "SharedObject.h"

namespace RPGML {

Port::Port( GarbageCollector *_gc, Node *parent )
: Collectable( _gc )
, m_parent( parent )
{}

Port::~Port( void )
{}

Node *Port::getParent( void ) const
{
  return m_parent;
}

const String &Port::getIdentifier( void ) const
{
  return m_identifier;
}

void Port::setIdentifier( const String &identifier )
{
  m_identifier = identifier;
}

void Port::gc_clear( void )
{
  m_parent.reset();
  m_identifier.clear();
}

void Port::gc_getChildren( Children &children ) const
{
  children.add( m_parent.get() );
}

Input::Input( GarbageCollector *_gc, Node *parent )
: Port( _gc, parent )
{}

Input::~Input( void )
{
  disconnect();
}

void Input::init( const String &identifier )
{
  setIdentifier( identifier );
}

void Input::gc_clear( void )
{
  Port::gc_clear();
  disconnect();
}

const Output *Input::getOutput( void ) const
{
  return m_output;
}

const ArrayBase *Input::getData( void ) const
{
  if( isConnected() )
  {
    return getOutput()->getData();
  }
  else
  {
    return 0;
  }
}

void Input::gc_getChildren( Children &children ) const
{
  Port::gc_getChildren( children );

  if( isConnected() )
  {
    children.add( m_output.get() );
  }
}

void Input::disconnect( void )
{
  Output *output = m_output;
  if( output )
  {
    m_output.reset();
    output->disconnect( this );
  }
}

void Input::connect( Output *output )
{
  if( output != m_output )
  {
    disconnect();
    m_output.reset( output );
    output->connect( this );
  }
}

bool Input::isConnected( void ) const
{
  return m_output.get() != 0;
}

Output::Output( GarbageCollector *_gc, Node *parent )
: Port( _gc, parent )
, m_inputs( 0 )
{}

Output::~Output( void )
{
  disconnect();
}

void Output::setData( ArrayBase *data )
{
  m_data = data;
}

ArrayBase *Output::getData( void )
{
  return m_data;
}

const ArrayBase *Output::getData( void ) const
{
  return m_data;
}

void Output::gc_clear( void )
{
  Port::gc_clear();
  disconnect();
}

void Output::gc_getChildren( Children &children ) const
{
  Port::gc_getChildren( children );
  m_inputs.gc_getChildren( children );
}

void Output::disconnect( void )
{
  inputs_t tmp( m_inputs.getGC() );
  tmp.swap( m_inputs );

  for( inputs_t::const_iterator i( tmp.begin() ), end( tmp.end() ); i != end; ++i )
  {
    if( (*i).get() ) (*i)->disconnect();
  }
}

void Output::disconnect( Input *input )
{
  for( inputs_t::iterator i( m_inputs.begin() ), end( m_inputs.end() ); i != end; ++i )
  {
    if( (*i).get() == input )
    {
      (*i).reset();
      input->disconnect();
      break;
    }
  }
}

void Output::connect( Input *input )
{
  CountPtr< Input > *free_pos = 0;
  for( inputs_t::reverse_iterator i( m_inputs.rbegin() ), end( m_inputs.rend() ); i != end; ++i )
  {
    Input *const input_i = (*i).get();
    if( input_i == input )
    {
      // already connected
      return;
    }
    else if( 0 == input_i )
    {
      // store there
      free_pos = &(*i);
    }
  }

  // Not connected yet
  if( free_pos )
  {
    free_pos->reset( input );
  }
  else
  {
    m_inputs.push_back( CountPtr< Input >( input ) );
  }

  input->connect( this );
}

bool Output::isConnected( void ) const
{
  for( inputs_t::const_iterator i( m_inputs.begin() ), end( m_inputs.end() ); i != end; ++i )
  {
    if( (*i).get() ) return true;
  }
  return false;
}

Node::Node(
    GarbageCollector *_gc
  , const String &identifier
  , const SharedObject *so
  , index_t num_inputs
  , index_t num_outputs
  , index_t num_params
  )
: Frame( _gc, 0 )
, m_inputs( _gc )
, m_outputs( _gc )
, m_params( _gc )
, m_identifier( identifier )
, m_so( so )
{
  if( num_inputs  ) setNumInputs ( num_inputs  );
  if( num_outputs ) setNumOutputs( num_outputs );
  if( num_params  ) setNumParams ( num_params  );
  reserve( num_inputs + num_outputs + num_params );
  setThis( true );
}

Node::~Node( void )
{}

const String &Node::getIdentifier( void ) const
{
  return m_identifier;
}

const SharedObject *Node::getSO( void ) const
{
  return m_so;
}

bool Node::tick( void )
{
  return true;
}

void Node::gc_clear( void )
{
  Frame::gc_clear();
  m_inputs.gc_clear();
  m_outputs.gc_clear();
}

void Node::gc_getChildren( Children &children ) const
{
  Frame::gc_getChildren( children );
  m_inputs.gc_getChildren( children );
  m_outputs.gc_getChildren( children );
}

Input *Node::getInput( index_t i ) const
{
  if( i < m_inputs.size() )
  {
    return m_inputs[ i ];
  }
  throw "Node::getInput( i ): Index out of bounds";
}

Input *Node::getInput( int i ) const
{
  return getInput( index_t( i ) );
}

Input *Node::getInput( const char *identifier ) const
{
  for( inputs_t::const_iterator i( m_inputs.begin() ), end( m_inputs.end() ); i != end; ++i )
  {
    if( (*i)->getIdentifier() == identifier ) return (*i);
  }
  throw "Node::getInput(): Identifier not found";
}

Output *Node::getOutput( index_t i ) const
{
  if( i < m_outputs.size() )
  {
    return m_outputs[ i ];
  }
  throw "Node::getOutput( i ): Index out of bounds";
}

Output *Node::getOutput( int i ) const
{
  return getOutput( index_t( i ) );
}

Output *Node::getOutput( const char *identifier ) const
{
  for( outputs_t::const_iterator i( m_outputs.begin() ), end( m_outputs.end() ); i != end; ++i )
  {
    if( (*i)->getIdentifier() == identifier ) return (*i);
  }
  throw "Node::getOutput(): Identifier not found";
}

Param *Node::getParam( index_t i ) const
{
  if( i < m_params.size() )
  {
    return m_params[ i ];
  }
  throw "Node::getParam( i ): Index out of bounds";
}

Param *Node::getParam( int i ) const
{
  return getParam( index_t( i ) );
}

Param *Node::getParam( const char *identifier ) const
{
  for( params_t::const_iterator i( m_params.begin() ), end( m_params.end() ); i != end; ++i )
  {
    if( !(*i).isNull() && (*i)->getIdentifier() == identifier ) return (*i);
  }
  throw "Node::getParam(): Identifier not found";
}

Param *Node::setParam( index_t i, Param *param )
{
  if( i < m_params.size() )
  {
    return ( m_params[ i ] = param );
  }
  else
  {
    return 0;
  }
}

void Node::setNumInputs( index_t n )
{
  index_t old_n = m_inputs.size();

  m_inputs.resize( n );
  for( index_t i = old_n; i < n; ++i )
  {
    m_inputs[ i ] = new Input( getGC(), this );
  }
}

void Node::setNumOutputs( index_t n )
{
  index_t old_n = m_outputs.size();

  m_outputs.resize( n );
  for( index_t i = old_n; i < n; ++i )
  {
    m_outputs[ i ] = new Output( getGC(), this );
  }
}

void Node::setNumParams( index_t n )
{
  m_params.resize( n );
}

index_t Node::getNumInputs( void ) const
{
  return m_inputs.size();
}

index_t Node::getNumOutputs( void ) const
{
  return m_outputs.size();
}

index_t Node::getNumParams( void ) const
{
  return m_params.size();
}

Node::NotConnected::NotConnected( const Input *_input )
: input( _input )
{
  (*this)
    << "Input '" << input->getIdentifier() << "' is not connected."
    ;
}

Node::InitFailed::InitFailed( const Output *_output )
: output( _output )
{
  (*this)
    << "Initializing Output '" << output->getIdentifier() << "' failed"
    ;
}

} // namespace RPGML


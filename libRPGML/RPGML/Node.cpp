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

#include <iostream>

using namespace std;

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
  children << m_parent;
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

void Input::gc_clear( void )
{
  Port::gc_clear();
  m_output.reset();
}

void Input::gc_getChildren( Children &children ) const
{
  Base::gc_getChildren( children );
  children.add( m_output );
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
    if( output )
    {
      m_output.reset( output );
      output->connect( this );
    }
  }
}

bool Input::isConnected( void ) const
{
  return !m_output.isNull();
}

bool Input::isReady( void ) const
{
  return isConnected() && m_output->isReady();
}

bool Input::hasChanged( void ) const
{
  return isConnected() && getOutput()->hasChanged();
}

Output::Output( GarbageCollector *_gc, Node *parent )
: Port( _gc, parent )
, m_inputs( new InputArray( _gc, 1 ) )
, m_hasChanged( false )
{}

Output::~Output( void )
{
  disconnect();
}

void Output::setData( CountPtr< ArrayBase > data )
{
  m_data.swap( data );
}

ArrayBase *Output::getData( void )
{
  return m_data;
}

const ArrayBase *Output::getData( void ) const
{
  return m_data;
}

void Output::resolve( void )
{
  if( !m_data.isNull() )
  {
    m_data = Node::resolve( m_data );
  }
}

void Output::gc_clear( void )
{
  Base::gc_clear();
  m_inputs.reset();
  m_data.reset();
}

void Output::gc_getChildren( Children &children ) const
{
  Base::gc_getChildren( children );
  children.add( m_inputs );
  children.add( m_data );
}

void Output::disconnect( void )
{
  if( m_inputs.isNull() ) return;

  CountPtr< InputArray > tmp = new InputArray( m_inputs->getGC(), 1 );
  std::swap( (*tmp), (*m_inputs) );

  for( InputArray::const_iterator i( tmp->begin() ), end( tmp->end() ); i != end; ++i )
  {
    if( !(*i).isNull() ) (*i)->disconnect();
  }
}

void Output::disconnect( Input *input )
{
  for( InputArray::iterator i( m_inputs->begin() ), end( m_inputs->end() ); i != end; ++i )
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
  if( !input ) return;
  if( !this )
  {
    input->disconnect();
    return;
  }

  CountPtr< Input > *free_pos = 0;
  if( !m_inputs->empty() )
  {
    for( InputArray::iterator i( m_inputs->begin() ), end( m_inputs->end() ); i != end; ++i )
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
        break;
      }
    }
  }

  // Not connected yet
  if( free_pos )
  {
    free_pos->reset( input );
  }
  else
  {
    m_inputs->push_back( CountPtr< Input >( input ) );
  }

  input->connect( this );
}

bool Output::isConnected( void ) const
{
  for( InputArray::const_iterator i( m_inputs->begin() ), end( m_inputs->end() ); i != end; ++i )
  {
    if( !i->isNull() ) return true;
  }
  return false;
}

bool Output::isReady( void ) const
{
  return !m_data.isNull();
}

Output::inputs_iterator Output::inputs_begin( void )
{
  inputs_iterator ret = m_inputs->begin();
  inputs_iterator end = m_inputs->end();

  while( ret != end && ret->isNull() ) ++ret;

  return ret;
}

Output::inputs_iterator Output::inputs_end  ( void )
{
  return m_inputs->end();
}

bool Output::hasChanged( void ) const
{
  return m_hasChanged;
}

void Output::setChanged( bool changed )
{
  m_hasChanged = changed;
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
, m_inputs( new InputArray( _gc , 1 ) )
, m_outputs( new OutputArray( _gc , 1 ) )
, m_params( new ParamArray( _gc , 1 ) )
, m_so( so )
{
  setIdentifier( identifier );
  if( num_inputs  ) setNumInputs ( num_inputs  );
  if( num_outputs ) setNumOutputs( num_outputs );
  if( num_params  ) setNumParams ( num_params  );
  reserve( num_inputs + num_outputs + num_params );
  setThis( true );
}

Node::~Node( void )
{}

const SharedObject *Node::getSO( void ) const
{
  return m_so;
}

bool Node::tick( void )
{
  throw Exception() << "tick() not implemented";
}

bool Node::tick( CountPtr< JobQueue > )
{
  return tick();
}

void Node::gc_clear( void )
{
  Base::gc_clear();
  m_inputs.reset();
  m_outputs.reset();
  m_params.reset();
}

void Node::gc_getChildren( Children &children ) const
{
  Base::gc_getChildren( children );
  children.add( m_inputs  );
  children.add( m_outputs );
  children.add( m_params  );
}

Input *Node::getInput( index_t i ) const
{
  if( i < m_inputs->size() )
  {
    return (*m_inputs)[ i ];
  }
  throw InputNotFound() << "Input index " << i << " out of bounds";
}

Input *Node::getInput( int i ) const
{
  return getInput( index_t( i ) );
}

Input *Node::getInput( const char *identifier, index_t *index ) const
{
  if( index ) (*index) = 0;
  for( InputArray::const_iterator i( m_inputs->begin() ), end( m_inputs->end() ); i != end; ++i )
  {
    if( (*i)->getIdentifier() == identifier ) return (*i);
    if( index ) ++(*index);
  }
  throw InputNotFound() << "Input '" << identifier << "' not found";
}

Output *Node::getOutput( index_t i ) const
{
  if( i < m_outputs->size() )
  {
    return (*m_outputs)[ i ];
  }
  throw OutputNotFound() << "Output index " << i << " out of bounds";
}

Output *Node::getOutput( int i ) const
{
  return getOutput( index_t( i ) );
}

Output *Node::getOutput( const char *identifier, index_t *index ) const
{
  if( index ) (*index) = 0;
  for( OutputArray::const_iterator i( m_outputs->begin() ), end( m_outputs->end() ); i != end; ++i )
  {
    if( (*i)->getIdentifier() == identifier ) return (*i);
    if( index ) ++(*index);
  }
  throw OutputNotFound() << "Output '" << identifier << "' not found";
}

Param *Node::getParam( index_t i ) const
{
  if( i < m_params->size() )
  {
    return (*m_params)[ i ];
  }
  throw ParamNotFound() << "Param index " << i << " out of bounds";
}

Param *Node::getParam( int i ) const
{
  return getParam( index_t( i ) );
}

Param *Node::getParam( const char *identifier, index_t *index ) const
{
  if( index ) (*index) = 0;
  for( ParamArray::const_iterator i( m_params->begin() ), end( m_params->end() ); i != end; ++i )
  {
    if( !(*i).isNull() && (*i)->getIdentifier() == identifier ) return (*i);
    if( index ) ++(*index);
  }
  throw ParamNotFound() << "Param '" << identifier << "' not found";
}

Param *Node::setParam( index_t i, CountPtr< Param > param )
{
  if( i < m_params->size() )
  {
    return ( (*m_params)[ i ] = param );
  }
  throw ParamNotFound() << "Param index " << i << " out of bounds";
}

void Node::setNumInputs( index_t n )
{
  index_t old_n = m_inputs->size();

  m_inputs->resize( n );
  for( index_t i = old_n; i < n; ++i )
  {
    (*m_inputs)[ i ] = new Input( getGC(), this );
  }
}

void Node::setNumOutputs( index_t n )
{
  index_t old_n = m_outputs->size();

  m_outputs->resize( n );
  for( index_t i = old_n; i < n; ++i )
  {
    (*m_outputs)[ i ] = new Output( getGC(), this );
  }
}

void Node::setNumParams( index_t n )
{
  m_params->resize( n );
}

index_t Node::getNumInputs( void ) const
{
  return m_inputs->size();
}

index_t Node::getNumOutputs( void ) const
{
  return m_outputs->size();
}

index_t Node::getNumParams( void ) const
{
  return m_params->size();
}

void Node::setAllOutputChanged( bool changed )
{
  for( index_t i( 0 ), end( m_outputs->size() ); i < end; ++i )
  {
    Output *const output = (*m_outputs)[ i ];
    if( output ) output->setChanged( changed );
  }
}

bool Node::hasAnyInputChanged( void ) const
{
  const index_t end = m_inputs->size();
  if( 0 == end ) return true;
  index_t num_connected = 0;
  for( index_t i=0; i < end; ++i )
  {
    Input *const input = (*m_inputs)[ i ];
    if( input )
    {
      if( input->isConnected() ) ++num_connected;
      if( input->hasChanged() ) return true;
    }
  }
  if( 0 == num_connected ) return true;
  return false;
}

CountPtr< const ArrayBase > Node::resolve( const CountPtr< const ArrayBase > &in_base )
{
  return resolve( in_base.get() );
}

CountPtr< const ArrayBase > Node::resolve( const ArrayBase *in_base )
{
  while( in_base && 0 == in_base->getDims()  )
  {
    const Type in_base_type = in_base->getType();
    if( in_base_type.isArray() )
    {
      const ArrayArray *e = 0;
      if( in_base->getAs( e ) && !e->empty() )
      {
        in_base = e->elements()->get();
      }
      else
      {
        break;
      }
    }
    else if( in_base_type.isOutput() )
    {
      const OutputArray *e = 0;
      if( in_base->getAs( e ) && !e->empty() )
      {
        in_base = e->elements()->get()->getData();
        break;
      }
      else
      {
        break;
      }
    }
    else
    {
      break;
    }
  }

  return in_base;
}

CountPtr< ArrayBase > Node::resolve( const CountPtr< ArrayBase > &in_base )
{
  return resolve( in_base.get() );
}

CountPtr< ArrayBase > Node::resolve( ArrayBase *in_base )
{
  while( in_base && 0 == in_base->getDims() )
  {
    const Type in_base_type = in_base->getType();
    if( in_base_type.isArray() )
    {
      ArrayArray *e = 0;
      if( in_base->getAs( e ) && !e->empty() )
      {
        in_base = e->elements()->get();
      }
      else
      {
        break;
      }
    }
    else if( in_base_type.isOutput() )
    {
      OutputArray *e = 0;
      if( in_base->getAs( e ) && !e->empty() )
      {
        in_base = e->elements()->get()->getData();
        break;
      }
      else
      {
        break;
      }
    }
    else
    {
      break;
    }
  }

  return in_base;
}

NotConnected::NotConnected( const Input *_input )
: input( _input )
{
  (*this)
    << "Input '" << input->getIdentifier() << "' is not connected."
    ;
}

NotReady::NotReady( const Port *_port )
: port( _port )
{
  (*this)
    << "Port '" << port->getIdentifier() << "' is not ready."
    ;
}

IncompatibleOutput::IncompatibleOutput( const Input *_input )
: input( _input )
{
  (*this)
    << "Input '" << input->getIdentifier() << "'"
    << " is connected to incompatibe Output"
    ;
  if( input->isConnected() )
  {
    const Output *const output = input->getOutput();
    (*this)
      << " '" << output->getIdentifier() << "." << output->getParent()->getIdentifier() << "'"
      << " of < " << output->getData()->getTypeName() << " >[ " << output->getData()->getDims() << " ]"
      ;
  }
}

Node::InitFailed::InitFailed( const Output *_output )
: output( _output )
{
  (*this)
    << "Initializing Output '" << output->getIdentifier() << "' failed"
    ;
}

bool     Node::getBool  ( int input_index ) const { return getScalar< bool     >( input_index ); }
uint8_t  Node::getUInt8 ( int input_index ) const { return getScalar< uint8_t  >( input_index ); }
int8_t   Node::getInt8  ( int input_index ) const { return getScalar< int8_t   >( input_index ); }
uint16_t Node::getUInt16( int input_index ) const { return getScalar< uint16_t >( input_index ); }
int16_t  Node::getInt16 ( int input_index ) const { return getScalar< int16_t  >( input_index ); }
uint32_t Node::getUInt32( int input_index ) const { return getScalar< uint32_t >( input_index ); }
int32_t  Node::getInt32 ( int input_index ) const { return getScalar< int32_t  >( input_index ); }
uint64_t Node::getUInt64( int input_index ) const { return getScalar< uint64_t >( input_index ); }
int64_t  Node::getInt64 ( int input_index ) const { return getScalar< int64_t  >( input_index ); }
float    Node::getFloat ( int input_index ) const { return getScalar< float    >( input_index ); }
double   Node::getDouble( int input_index ) const { return getScalar< double   >( input_index ); }
String   Node::getString( int input_index ) const { return getScalar< String   >( input_index ); }

Identity::Identity( GarbageCollector *_gc, const String &identifier )
: Node( _gc, identifier, 0, 1, 1 )
{
  DEFINE_INPUT( 0, "in" );
  DEFINE_OUTPUT( 0, "out" );
}

Identity::~Identity( void )
{}

const char *Identity::getName( void ) const
{
  return "Identity";
}

bool Identity::tick( CountPtr< JobQueue > )
{
  GET_INPUT_BASE( 0, in_base );
  getOutput( 0 )->setData( const_cast< ArrayBase* >( in_base ) );
  return true;
}

InOut::InOut( GarbageCollector *_gc, const String &identifier )
: Collectable( _gc )
, m_id( new Identity( _gc, identifier ) )
{}

InOut::~InOut( void )
{}

void InOut::gc_clear( void )
{
  m_id.reset();
}

void InOut::gc_getChildren( Children &children ) const
{
  children << m_id;
}

const String &InOut::getIdentifier( void ) const
{
  return m_id->getIdentifier();
}

Input *InOut::getInput( void ) const
{
  return m_id->getInput( 0 );
}

Output *InOut::getOutput( void ) const
{
  return m_id->getOutput( 0 );
}

void InOut::disconnect( void )
{
  getInput()->disconnect();
  getOutput()->disconnect();
}

void InOut::connect( Output *output )
{
  output->connect( getInput() );
}

void InOut::connect( Input *input )
{
  getOutput()->connect( input );
}

Param::Param( GarbageCollector *_gc , const String &identifier )
: Collectable( _gc )
, m_identifier( identifier )
{}

Param::~Param( void )
{}

void Param::gc_clear( void )
{}

void Param::gc_getChildren( Children & ) const
{}

void Param::set( const Setting &setting )
{
  const int n_coords = int( setting.coords.size() );
  const index_t *const coords = ( n_coords ? &setting.coords[ 0 ] : 0 );
  set( setting.value, n_coords, coords );
}

CountPtr< Param::SettingsIterator > Param::getSettings( void ) const
{
  return new SettingsIterator( m_settings.begin(), m_settings.end() );
}

void Param::setValue( const Value &accepted_value, int n_coords, const index_t *coords )
{
  m_settings.push_back( Setting( accepted_value, n_coords, coords ) );
}

Param::Setting::Setting( const Value &_value, int n_coords, const index_t *_coords )
: value( _value )
{
  if( n_coords && _coords ) coords.insert( coords.end(), _coords, _coords + n_coords );
}

int Param::Setting::compare( const Setting &other ) const
{
  const int cmp_value = value.compare_exactly( other.value );
  if( 0 != cmp_value ) return cmp_value;

  const size_t coords_size = coords.size();
  const size_t other_coords_size = other.coords.size();

  if( coords_size < other_coords_size ) return -1;
  if( other_coords_size < coords_size ) return  1;

  // other_coords_size == coords_size
  for( size_t i=0; i<other_coords_size; ++i )
  {
    const index_t coords_i = coords[ i ];
    const index_t other_coords_i = other.coords[ i ];
    if( coords_i < other_coords_i ) return -1;
    if( other_coords_i < coords_i ) return  1;
  }

  return 0;
}

bool Param::Setting::operator<( const Setting &other ) const
{
  return ( 0 > compare( other ) );
}

bool Param::Setting::operator==( const Setting &other ) const
{
  return ( 0 == compare( other ) );
}

bool Param::Setting::operator!=( const Setting &other ) const
{
  return ( 0 != compare( other ) );
}

} // namespace RPGML


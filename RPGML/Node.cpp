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
  children.push_back( m_parent );
}

Input::Input( GarbageCollector *_gc, Node *parent )
: Port( _gc, parent )
{}

Input::~Input( void )
{
  disconnect();
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

const Data *Input::getData( void ) const
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
    children.push_back( m_output.get() );
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

void Output::setData( Data *data )
{
  m_data = data;
}

Data *Output::getData( void )
{
  return m_data;
}

const Data *Output::getData( void ) const
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

Node::Node( GarbageCollector *_gc, const String &identifier, const SharedObject *so )
: Frame( _gc, 0 )
, m_inputs( 0 )
, m_outputs( 0 )
, m_identifier( identifier )
, m_so( so )
{}

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
  return 0;
}

Input *Node::getInput( const char *identifier ) const
{
  for( inputs_t::const_iterator i( m_inputs.begin() ), end( m_inputs.end() ); i != end; ++i )
  {
    if( (*i)->getIdentifier() == identifier ) return (*i);
  }
  return 0;
}

Output *Node::getOutput( index_t i ) const
{
  if( i < m_outputs.size() )
  {
    return m_outputs[ i ];
  }
  return 0;
}

Output *Node::getOutput( const char *identifier ) const
{
  for( outputs_t::const_iterator i( m_outputs.begin() ), end( m_outputs.end() ); i != end; ++i )
  {
    if( (*i)->getIdentifier() == identifier ) return (*i);
  }
  return 0;
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

} // namespace RPGML


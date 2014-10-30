#include "Node.h"

#include "String.h"
#include "SharedObject.h"

namespace RPGML {

Port::Port( GarbageCollector *_gc, Node *parent, const String &identifier )
: Collectable( _gc )
, m_parent( parent )
, m_identifier( identifier )
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

void Port::gc_clear( void )
{
  m_parent.reset();
  m_identifier.clear();
}

void Port::gc_getChildren( Children &children ) const
{
  children.push_back( m_parent );
}

Input::Input( GarbageCollector *_gc, Node *parent, const String &identifier )
: Port( _gc, parent, identifier )
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

Output::Output( GarbageCollector *_gc, Node *parent, const String &identifier )
: Port( _gc, parent, identifier )
{}

Output::~Output( void )
{
  disconnect();
}

void Output::gc_clear( void )
{
  Port::gc_clear();
  disconnect();
}

void Output::gc_getChildren( Children &children ) const
{
  Port::gc_getChildren( children );

  for( inputs_t::const_iterator i( m_inputs.begin() ), end( m_inputs.end() ); i != end; ++i )
  {
    const Input *const input = i->get();
    if( input )
    {
      children.push_back( input );
    }
  }
}

void Output::disconnect( void )
{
  inputs_t tmp;
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

Node::Node( GarbageCollector *_gc, const String &global_name, const SharedObject *so )
: Frame( _gc, 0 )
, m_global_name( global_name )
, m_so( so )
{
  setThis( true );
}

Node::~Node( void )
{}

const String &Node::getGlobalName( void ) const
{
  return m_global_name;
}

const SharedObject *Node::getSO( void ) const
{
  return m_so.get();
}

void Node::gc_clear( void )
{
  Frame::gc_clear();
}

void Node::gc_getChildren( Children &children ) const
{
  Frame::gc_getChildren( children );
}

void Node::execute( void )
{}

} // namespace RPGML


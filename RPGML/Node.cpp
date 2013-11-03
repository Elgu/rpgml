#include "Node.h"

#include "String.h"

namespace RPGML {

Port::Port( GarbageCollector *_gc, Node *parent, const String *identifier )
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

void Port::gc_clear( void )
{
  m_parent.reset();
  m_identifier.reset();
}

CountPtr< Collectable::Children > Port::getChildren( void ) const
{
  return new ItemIterator< Children >( m_parent.get() );
}

Input::Input( GarbageCollector *_gc, Node *parent, const String *identifier )
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

CountPtr< Collectable::Children > Input::getChildren( void ) const
{
  if( isConnected() )
  {
    return
      new MultiIterator< Children >(
          Port::getChildren()
        , new ItemIterator< Children >( m_output.get() )
        );
  }
  else
  {
    return Port::getChildren();
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

Output::Output( GarbageCollector *_gc, Node *parent, const String *identifier )
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

CountPtr< Collectable::Children > Output::getChildren( void ) const
{
  if( isConnected() )
  {
    return
      new MultiIterator< Children >(
          Port::getChildren()
        , new ItemIterator< Children >( m_inputs.begin(), m_inputs.end(), m_inputs.size() )
        );
  }
  else
  {
    return Port::getChildren();
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


} // namespace RPGML


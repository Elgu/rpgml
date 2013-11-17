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

void Port::getChildren( Children &children ) const
{
  children.push_back( m_parent );
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

void Input::getChildren( Children &children ) const
{
  Port::getChildren( children );

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

void Output::getChildren( Children &children ) const
{
  Port::getChildren( children );

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

Param::Param( GarbageCollector *_gc, Node *parent, const String *identifier, Type type )
: Collectable( _gc )
, m_parent( parent )
, m_identifier( identifier )
, m_type( type )
{}

Param::~Param( void )
{}

Node *Param::getParent( void ) const
{
  return m_parent;
}

bool Param::set( const Value &value )
{
  if( m_type == Type::Invalid() )
  {
    m_value == value;
  }
  else
  {
    try
    {
      m_value = value.to( m_type );
    }
    catch( const char *e )
    {
      throw "Type of value does not match type for parameter.";
    }
  }

  return m_parent->setParamRecord( this );
}

const Value &Param::get( void ) const
{
  return m_value;
}

void Param::gc_clear( void )
{
  m_parent.reset();
  m_value.clear();
}

void Param::getChildren( Children &children ) const
{
  children.push_back( m_parent );
  if( m_value.isCollectable() )
  {
    children.push_back( m_value.getCollectable() );
  }
}

Node::Node( GarbageCollector *_gc, Map *_parent )
: Map( _gc, _parent )
{}

Node::~Node( void )
{}

void Node::gc_clear( void )
{
  Map::gc_clear();
  m_setParameters.clear();
}

void Node::getChildren( Children &children ) const
{
  Map::getChildren( children );

  for( size_t i( 0 ), end( m_setParameters.size() ); i < end; ++i )
  {
    const SetParameter &s = m_setParameters[ i ];
    children.push_back( s.param.get() );
    if( s.value.isCollectable() )
    {
      children.push_back( s.value.getCollectable() );
    }
  }
}

bool Node::setParamRecord( const Param *param )
{
  m_setParameters.push_back( SetParameter( param, param->get() ) );
  return true;
}

} // namespace RPGML


#include "Scope.h"

#include "Context.h"
#include "Frame.h"
#include "Value.h"
#include "String.h"
#include "StringUnifier.h"
#include "Refcounted.h"

#include <iostream>

namespace RPGML {

Scope::Scope( Context *context )
: m_context( context )
, m_curr( context->getRoot() )
{}

Scope::~Scope( void )
{}

GarbageCollector *Scope::getGC( void ) const
{
  return m_context->getGC();
}

StringUnifier *Scope::getUnifier( void ) const
{
  return m_context->getUnifier();
}

Frame *Scope::getRoot( void ) const
{
  return m_context->getRoot();
}

Value *Scope::lookup( const String &identifier ) const
{
  for( Frame *curr = getCurr(); curr; curr = curr->getParent() )
  {
    Value *const ret = curr->load( identifier, this );
    if( ret ) return ret;
  }

  return 0;
}

Value *Scope::lookup( const char *identifier ) const
{
  return lookup( String::Static( identifier ) );
}

Value *Scope::lookup( const std::string &identifier ) const
{
  return lookup( String( new StdString( identifier ) ) );
}

Value *Scope::create_unified( const String &unified_identifier, const Value &value ) const
{
  Frame *const curr = getCurr();

  const index_t index = curr->getIndex( unified_identifier );

  if( index == Frame::unknown )
  {
    return curr->push_back( unified_identifier, value );
  }
  else
  {
    throw "Identifier already exists";
  }

  return 0;
}

Value *Scope::create( const String &identifier, const Value &value ) const
{
  return create_unified( unify( identifier ), value );
}

Value *Scope::create( const char *identifier       , const Value &value ) const
{
  return create_unified( unify( identifier ), value );
}

Value *Scope::create( const std::string &identifier, const Value &value ) const
{
  return create_unified( unify( identifier ), value );
}

CountPtr< Scope::EnterLeaveGuard > Scope::enter( const String &identifier )
{
  Value *const n = getCurr()->get( identifier );
  if( n && n->isFrame() )
  {
    return new EnterLeaveGuard( this, n->getFrame() );
  }
  else
  {
    return 0;
  }
}

CountPtr< Scope::EnterLeaveGuard > Scope::enter( const char *identifier )
{
  return enter( String::Static( identifier ) );
}

CountPtr< Scope::EnterLeaveGuard > Scope::enter( const std::string &identifier )
{
  return enter( identifier.c_str() );
}

const StringData *Scope::unify( const String &identifier ) const
{
  return m_context->getUnifier()->unify( identifier );
}

const StringData *Scope::unify( const char *identifier ) const
{
  return m_context->getUnifier()->unify( identifier );
}

const StringData *Scope::unify( const std::string &identifier ) const
{
  return m_context->getUnifier()->unify( identifier );
}

CountPtr< Frame > Scope::new_Frame( void ) const
{
  return new Frame( getGC(), getCurr() );
}

String Scope::genGlobalName( const String &identifier ) const
{
  String ret = identifier;

  for( const Frame *m = getCurr(); m; m = m->getParent() )
  {
    const String &id = m->getIdentifier();
    if( !id.empty() )
    {
      if( ret.empty() )
      {
        ret = id;
      }
      else
      {
        ret = id + "." + ret;
      }
    }
  }

  return ret;
}

size_t Scope::getNr( void ) const
{
  return m_context->getNr();
}

Scope &Scope::setCurr( Frame *curr )
{
  m_curr = curr;
  return (*this);
}

} // namespace RPGML


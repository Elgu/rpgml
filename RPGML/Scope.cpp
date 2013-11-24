#include "Scope.h"

#include "Context.h"
#include "Map.h"
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

Value *Scope::lookup( const String &identifier ) const
{
  const Value id( identifier );
  for( Map *curr = getCurr(); curr; curr = curr->getParent() )
  {
    Value *const ret = curr->get( id );
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
  return lookup( identifier.c_str() );
}

Value *Scope::create_unified( const String &unified_identifier, const Value &value ) const
{
  Map *const curr = getCurr();

  bool exists = false;
  const index_t index = curr->getCreateIndex( Value( unified_identifier ), &exists );

  if( !exists )
  {
    Value &ret = (*curr)[ index ];
    ret = value;
    return &ret;
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
  Value *const n = getCurr()->get( Value( identifier ) );
  if( n && n->isMap() )
  {
    return new EnterLeaveGuard( this, n->getMap() );
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

Scope &Scope::setCurr( Map *curr )
{
  m_curr = curr;
  return (*this);
}

} // namespace RPGML


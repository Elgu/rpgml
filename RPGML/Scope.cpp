#include "Scope.h"

#include "Context.h"
#include "Frame.h"
#include "Value.h"
#include "String.h"
#include "StringUnifier.h"
#include "Refcounted.h"
#include "Node.h"

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

  if( index == unknown )
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
  Value *const n = getCurr()->getVariable( identifier );
  if( n && n->isFrame() )
  {
    return new EnterLeaveGuard( this, n->getFrame() );
  }
  else
  {
    return (EnterLeaveGuard*)0;
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

bool Scope::call(
    const Location *loc
  , index_t recursion_depth
  , const String &function_identifier
  , Value &ret
  , const Function::Args *call_args
  )
{
  Value *const func = lookup( function_identifier );
  if( !func ) throw "Function not found";
  if( !func->isFunction() ) throw "is not a Function";

  if( !func->getFunction()->call( loc, recursion_depth, this, ret, call_args ) )
  {
    throw "Function call failed";
  }

  return true;
}

bool Scope::call(
    const Location *loc
  , index_t recursion_depth
  , const String &function_identifier
  , Value &ret
  , const Value &arg0
  , const Value &arg1
  , const Value &arg2
  , const Value &arg3
  )
{
  Function::Args args;
  args.reserve( 4 );

  do
  {
    if( !arg0.isInvalid() ) args[ 0 ] = Function::Arg( String(), arg0 ); else break;
    if( !arg1.isInvalid() ) args[ 1 ] = Function::Arg( String(), arg1 ); else break;
    if( !arg2.isInvalid() ) args[ 2 ] = Function::Arg( String(), arg2 ); else break;
    if( !arg3.isInvalid() ) args[ 3 ] = Function::Arg( String(), arg3 ); else break;
  }
  while( false );

  return call( loc, recursion_depth, function_identifier, ret, &args );
}

CountPtr< Output > Scope::toOutput(
    const Location *loc
  , index_t recursion_depth
  , const Value &x
  )
{

  if( x.isOutput() )
  {
    return x.getOutput();
  }
  else if( x.isInvalid() )
  {
    throw "Scope::toOutput(): invalid Value";
    return (Output*)0;
  }
  else if( x.isPrimitive() )
  {
    Value ret;
    if( !call( loc, recursion_depth, String::Static( "constant" ), ret, x ) ) return (Output*)0;
    if( !ret.isOutput() ) throw "return value of 'constant' should be Output";
    return ret.getOutput();
  }
  else
  {
    throw "Scope::toOutput(): Value must be Output or primitive.";
    return (Output*)0;
  }

  return (Output*)0;
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


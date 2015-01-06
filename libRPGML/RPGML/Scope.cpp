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
#include "Scope.h"

#include "Context.h"
#include "Frame.h"
#include "Value.h"
#include "String.h"
#include "StringUnifier.h"
#include "Refcounted.h"
#include "Node.h"
#include "ParseException.h"

#include <iostream>
using namespace std;

namespace RPGML {

Scope::Scope( Context *context )
: m_context( context )
, m_curr( context->getRoot() )
{}

Scope::Scope( const Scope *other, Frame *curr )
: m_context( other->getContext() )
, m_curr( m_context->getRoot() )
{
  Frame *root = getContext()->getRoot();
  for( Frame *c = curr; c != 0; c = c->getParent() )
  {
    if( c == root )
    {
      // valid frame as curr
      setCurr( curr );
      curr = 0;
      break;
    }
  }

  if( curr )
  {
    throw Exception()
      << "Frame to be used as current Frame for new Scope has different root as the Context used"
      ;
  }
}

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
    throw Exception() << "Identifier '" << unified_identifier << "' already exists";
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
  if( !func ) throw ParseException( loc ) << "Function '" << function_identifier << "' not found";
  if( !func->isFunction() ) throw ParseException( loc ) << "Variable '" << function_identifier << "' is not a Function";

  if( !func->getFunction()->call( loc, recursion_depth, this, ret, call_args ) )
  {
    throw ParseException( loc ) << "Calling Function '" << function_identifier << "' failed";
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
    if( !arg0.isNil() ) args.push_back( Function::Arg( String(), arg0 ) ); else break;
    if( !arg1.isNil() ) args.push_back( Function::Arg( String(), arg1 ) ); else break;
    if( !arg2.isNil() ) args.push_back( Function::Arg( String(), arg2 ) ); else break;
    if( !arg3.isNil() ) args.push_back( Function::Arg( String(), arg3 ) ); else break;
  }
  while( false );

  return call( loc, recursion_depth, function_identifier, ret, &args );
}

CountPtr< Node > Scope::create_Node(
    const Location *loc
  , index_t recursion_depth
  , const String &name
  )
{
  Value ret;
  if( !call( loc, recursion_depth+1, name, ret ) )
  {
    throw ParseException()
      << "Could not find/instantiate Node '" << name << "'"
      ;
  }

  if( !ret.isNode() )
  {
    throw ParseException()
      << "Object '" << name << "' did not create a Node, created a " << ret.getTypeName()
      ;
  }

  return ret.getNode();
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
  else if( x.isNil() )
  {
    // Not connected
    return (Output*)0;
  }
  else if( x.isPrimitive() )
  {
    Value ret;
    if( !call( loc, recursion_depth, String::Static( "constant" ), ret, x ) )
    {
      return (Output*)0;
    }

    if( !ret.isOutput() )
    {
      throw ParseException( loc )
        << "Return value of 'constant' should be Output, is " << ret.getTypeName()
        ;
    }

    return ret.getOutput();
  }
  else if( x.isArray() )
  {
    // TODO: use ConstantArray or something
    Value ret;
    if( !call( loc, recursion_depth, String::Static( "constant" ), ret, x ) ) return (Output*)0;
    if( !ret.isOutput() )
    {
      throw ParseException( loc )
        << "Return value of 'constant' should be Output, is " << ret.getTypeName()
        ;
    }
    return ret.getOutput();
  }
  else
  {
    throw ParseException( loc ) << "Cannot convert to Output, must be Output, Array or primitive.";
  }
}

CountPtr< Frame > Scope::new_Frame( void ) const
{
  return new Frame( getGC(), getCurr() );
}

String Scope::genGlobalName( const String &identifier ) const
{
  return getCurr()->genGlobalName( identifier );
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


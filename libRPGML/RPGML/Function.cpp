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
#include "Function.h"

#include "Scope.h"
#include "Frame.h"
#include "String.h"
#include "SharedObject.h"

#include <iostream>
using namespace std;

namespace RPGML {

Function::Function( GarbageCollector *_gc, Frame *parent, const Args *decl, bool is_method, const SharedObject *so )
: Collectable( _gc )
, m_parent( parent )
, m_decl( decl )
, m_so( so )
, m_is_method( is_method )
{
//  {
//    const Args &decl_args = *decl;
//
//    cerr << "Function::Function(): " << endl;
//    for( index_t pos = 0; pos < decl_args.size(); ++pos )
//    {
//      cerr << "  decl_args[ " << pos << " ] = { '" << decl_args[ pos ].identifier << "', '" << decl_args[ pos ].value << "' )" << endl;
//    }
//  }
}

Function::~Function( void )
{}

Frame *Function::getParent( void ) const
{
  return m_parent;
}

const Function::Args *Function::getDecl( void ) const
{
  return m_decl.get();
}

const SharedObject *Function::getSO( void ) const
{
  return m_so.get();
}

size_t Function::getNumArgs( void ) const
{
  return m_decl->size();
}

bool Function::isMethod( void ) const
{
  return m_is_method;
}

size_t Function::getFrameSize( void ) const
{
  return getNumArgs();
}

bool Function::call( const Location *loc, index_t recursion_depth, Scope *scope, Value &ret, const Args *call_args )
{
  Scope::EnterLeaveGuard guard1( scope, getParent() );
  CountPtr< Frame > function_frame = new Frame( scope->getGC(), scope->getCurr() );
  Scope::EnterLeaveGuard guard2( scope, function_frame );

  setupFunctionFrame( *function_frame, *call_args );
  const index_t n_args = index_t( function_frame->getSize() );
  return call_impl( loc, recursion_depth, scope, ret, n_args, ( n_args ? function_frame->getStack( 0 ) : (Value*)0 ) );
}

bool Function::call( const Location *loc, index_t recursion_depth, Scope *scope, Value &ret, index_t n_args, const Value *args )
{
  Scope::EnterLeaveGuard guard1( scope, getParent() );
  CountPtr< Frame > function_frame = new Frame( scope->getGC(), scope->getCurr() );
  Scope::EnterLeaveGuard guard2( scope, function_frame );

  setupFunctionFrame( *function_frame, n_args, args );
  return call_impl( loc, recursion_depth, scope, ret, n_args, ( n_args ? function_frame->getStack( 0 ) : (Value*)0 ) );
}

void Function::setupFunctionFrame( Frame &frame, const Args &call_args )
{
  const Args &decl_args = *(m_decl.get());

  const index_t n_decl_args = index_t( decl_args.size() );
  const index_t n_call_args = index_t( call_args.size() );

  if( n_call_args > n_decl_args )
  {
    throw "too many arguments";
  }

  frame.reserve( n_decl_args );
  std::vector< int8_t > used( call_args.size(), int8_t( false ) );

  index_t pos = 0;

  // positional arguments, no identifier
  for( ; pos < call_args.size(); ++pos )
  {
    const String &call_identifier = call_args[ pos ].getIdentifier();
    if( call_identifier.empty() )
    {
      frame.push_back( decl_args[ pos ].getIdentifier(), call_args[ pos ].getValue() );
      used[ pos ] = true;
    }
    else
    {
      break;
    }
  }

  const index_t first_init = pos;

  // identifier arguments
  for( ; pos < decl_args.size(); ++pos )
  {
    const String &decl_identifier = decl_args[ pos ].getIdentifier();
    bool found = false;
    for( index_t i=first_init; i < n_call_args; ++i )
    {
      const String &call_identifier = call_args[ i ].getIdentifier();
      if( !call_identifier.empty() )
      {
        if( !used[ i ] && decl_identifier == call_identifier )
        {
          found = true;
          // Was not yet initialized (even with invalid Value)
          frame.push_back( call_identifier, call_args[ i ].getValue() );
          used[ i ] = true;
        }
      }
      else
      {
        throw "no fixed position argument allowed after init argument";
      }
    }

    // No identifier argument specified, try default value
    if( !found )
    {
      if( decl_args[ pos ].hasValue() )
      {
        const Value &default_value = decl_args[ pos ].getValue();
        frame.push_back( decl_identifier, default_value );
      }
      else
      {
        throw "no default value for argument, must be specified";
      }
    }
  }

  for( index_t i=first_init; i < call_args.size(); ++i )
  {
    if( !used[ i ] )
    {
      throw "init argument identifier not found or already initialized by fixed argument or dupplicate";
    }
  }
}

void Function::setupFunctionFrame( Frame &frame, index_t n_args, const Value *args )
{
  const Args &decl_args = *(m_decl.get());

  const index_t n_decl_args = index_t( decl_args.size() );

  if( n_args > n_decl_args )
  {
    throw "too many arguments";
  }

  frame.reserve( n_decl_args );
  index_t pos = 0;

  // positional arguments, no identifier
  for( ; pos < n_args; ++pos )
  {
    frame.push_back( decl_args[ pos ].getIdentifier(), args[ pos ] );
  }

  // default value arguments
  for( ; pos < n_decl_args; ++pos )
  {
    const String &decl_identifier = decl_args[ pos ].getIdentifier();

    if( decl_args[ pos ].hasValue() )
    {
      const Value &default_value = decl_args[ pos ].getValue();
      frame.push_back( decl_identifier, default_value );
    }
    else
    {
      throw "no default value for argument, must be specified";
    }
  }
}

void Function::gc_clear( void )
{
  m_parent.reset();
  m_decl.reset();
}

void Function::gc_getChildren( Children &children ) const
{
  children.add( m_parent.get() );

  if( !m_decl.isNull() )
  {
    for( size_t i( 0 ), end( m_decl->size() ); i<end; ++i )
    {
      const Value &value = m_decl->at( i ).getValue();

      if( value.isCollectable() )
      {
        children.add( value.getCollectable() );
      }
    }
  }
}

Function::Arg::Arg( void )
: m_hasValue( false )
{}

Function::Arg::Arg( const String &identifier )
: m_identifier( identifier )
, m_hasValue( false )
{}

Function::Arg::Arg( const String &identifier, const Value &value )
: m_identifier( identifier )
, m_value( value )
, m_hasValue( true )
{}

Function::Arg &Function::Arg::setIdentfier( const String &identifier )
{
  m_identifier = identifier;
  return (*this);
}

Function::Arg &Function::Arg::setValue( const Value &value )
{
  m_value = value;
  m_hasValue = true;
  return (*this);
}

const String &Function::Arg::getIdentifier( void ) const
{
  return m_identifier;
}

const Value  &Function::Arg::getValue    ( void ) const
{
  return m_value;
}

bool Function::Arg::hasValue( void ) const
{
  return m_hasValue;
}

} // namespace RPGML


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
#include "Value.h"
#include "String.h"
#include "StringUnifier.h"
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

Frame::Ref Scope::lookup( const String &identifier ) const
{
  if( identifier.empty() ) return Frame::Ref();

  if( identifier[ 0 ] == '.' )
  {
    return getRoot()->load( identifier.mid( 1 ), this );
  }

  for( Frame *curr = getCurr(); curr; curr = curr->getParent() )
  {
    Frame::Ref ret = curr->load( identifier, this );
    if( !ret.isNull() ) return ret;
  }

  return Frame::Ref();
}

Frame::Ref Scope::lookup( const char *identifier ) const
{
  return lookup( String( unify( identifier ) ) );
}

Frame::Ref Scope::lookup( const std::string &identifier ) const
{
  return lookup( String( unify( identifier ) ) );
}

Frame::Ref Scope::create_unified( const String &unified_identifier, const Value &value ) const
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

  return Frame::Ref();
}

Frame::Ref Scope::create( const String &identifier, const Value &value ) const
{
  return create_unified( unify( identifier ), value );
}

Frame::Ref Scope::create( const char *identifier       , const Value &value ) const
{
  return create_unified( unify( identifier ), value );
}

Frame::Ref Scope::create( const std::string &identifier, const Value &value ) const
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
  return enter( unify( identifier ) );
}

CountPtr< Scope::EnterLeaveGuard > Scope::enter( const std::string &identifier )
{
  return enter( unify( identifier ) );
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
  , const Value &arg4
  )
{
  Value args[ 5 ];
  index_t n_args = 0;

  do
  {
    if( !arg0.isNil() ) args[ n_args++ ] = arg0; else break;
    if( !arg1.isNil() ) args[ n_args++ ] = arg1; else break;
    if( !arg2.isNil() ) args[ n_args++ ] = arg2; else break;
    if( !arg3.isNil() ) args[ n_args++ ] = arg3; else break;
    if( !arg4.isNil() ) args[ n_args++ ] = arg4; else break;
  }
  while( false );

  return call( loc, recursion_depth, function_identifier, ret, n_args, args );
}

bool Scope::call(
    const Location *loc
  , index_t recursion_depth
  , const String &function_identifier
  , Value &ret
  , index_t n_args
  , const Value *args
  )
{
  Value *const func = lookup( function_identifier );
  if( !func ) throw ParseException( loc ) << "Function '" << function_identifier << "' not found";
  if( !func->isFunction() ) throw ParseException( loc ) << "Variable '" << function_identifier << "' is not a Function";

  if( !func->getFunction()->call( loc, recursion_depth, this, ret, n_args, args ) )
  {
    throw ParseException( loc ) << "Calling Function '" << function_identifier << "' failed";
  }

  return true;
}

CountPtr< Node > Scope::createNode(
    const Location *loc
  , index_t recursion_depth
  , const String &name
  )
{
  Value ret;
  if( !call( loc, recursion_depth+1, name, ret, 0, (const Value*)0 ) )
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
    if( !call( loc, recursion_depth, String::Static( ".constant" ), ret, x ) )
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
    const ArrayBase *const array_base = x.getArray();
    const ArrayBase::Size  array_size = array_base->getSize();
    const Type             array_type = array_base->getType();
    const int              dims       = array_size.getDims();

    CountPtr< Node > array_node = createNode( loc, recursion_depth, String::Static( ".ConstantArray" ) );

    if( array_type.isPrimitive() )
    {
      const Type constant_array_type = array_type;

      array_node->getParam( "type" )->set( Value( String::Static( constant_array_type.getTypeName() ) ) );
      array_node->getParam( "dims" )->set( Value( dims ) );

      static const char *const size_names[] = { "sizeX", "sizeY", "sizeZ", "sizeT" };
      for( int d=0; d<dims; ++d )
      {
        array_node->getParam( size_names[ d ] )->set( Value( array_size[ d ] ) );
      }

      const Frame::Ref constant_array_v = array_node->getVariable( "in" );
      if( constant_array_v.isNull() )
      {
        throw ParseException( loc ) << "Could not get 'in' from ConstantArray";
      }
      if( !constant_array_v->isArray() )
      {
        throw ParseException( loc )
          << "Expected 'in' of ConstantArray to be an Array, is " << constant_array_v->getTypeName()
          ;
      }
      const ArrayBase *const constant_array_base = constant_array_v->getArray();

      const ArrayBase::Size constant_array_size = constant_array_base->getSize();
      if( constant_array_size != array_size )
      {
        throw ParseException( loc )
          << "Expected 'in[]' of ConstantArray to be the specified size."
          << " Specified was " << array_size
          << ", is " << constant_array_size
          ;
      }

      if( !constant_array_base->getType().isParam() )
      {
        throw ParseException( loc )
          << "Expected 'in[]' of ConstantArray to be a Param Array"
          << ", is " << constant_array_base->getType() << " Array"
          ;
      }

      const ParamArrayElements *constant_array = 0;
      if( !constant_array_base->getAs( constant_array ) )
      {
        throw ParseException( loc )
          << "Could not get 'in[]' of ConstantArray as ParamArrayElements"
          ;
      }

      ParamArrayElements::const_iterator         p    ( constant_array->begin()            );
      ParamArrayElements::const_iterator         p_end( constant_array->end()              );
      CountPtr< ArrayBase::ValueIterator       > a    ( array_base    ->getValueIterator() );

      for( ; !a->done() && p != p_end; a->next(), ++p )
      {
        (*p)->set( a->get() );
      }

      if( !a->done() || p != p_end )
      {
        throw ParseException( new Location( __FILE__, __LINE__ ) )
          << "Internal: Array Iterators did not match"
          << ": a->done() = " << a->done()
          << ", ( p == p_end ) = " << ( p == p_end )
          ;
      }
    }
    else
    {
      throw ParseException( loc )
        << "Unsupported Array type of " << array_base->getType()
        ;
    }

    return array_node->getOutput( "out" );
  }
  else
  {
    throw ParseException( loc ) << "Cannot convert to Output, must be Output, Array or primitive.";
  }
}

CountPtr< Frame > Scope::newFrame( void ) const
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


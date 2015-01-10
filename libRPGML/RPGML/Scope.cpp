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
#include <ctype.h>

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
    CountPtr< Node > ret = createNode( loc, recursion_depth, String::Static( ".Constant" ) );

    ret->getParam( "value" )->set( x );

    const String global_name = genGlobalName( "Constant " + make_printable( toString( x ).mid( 0, 32 ) ), loc );
    ret->setIdentifier( global_name );

    return ret->getOutput( "out" );
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
        throw ParseException( new Location( __FILE__, __LINE__ ) )
          << "Could not get 'in' from ConstantArray";
      }
      if( !constant_array_v->isParam() )
      {
        throw ParseException( new Location( __FILE__, __LINE__ ) )
          << "Expected 'in' of ConstantArray to be a Param, is " << constant_array_v->getTypeName()
          ;
      }
      Param *const constant_array = constant_array_v->getParam();

      for( CountPtr< ArrayBase::CoordinatesIterator > c( array_base->getCoordinatesIterator() ); !c->done(); c->next() )
      {
        const ArrayBase::Coordinates C = c->get();
        constant_array->set( array_base->getValue( C ), C.getDims(), C.getCoords() );
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

String Scope::genGlobalName( const String &node_name, const Location *loc ) const
{
  return
    genGlobalName(
        ":" + node_name
      + "@" + toString( loc->withoutFilename() )
      + "#" + toString( getNr() )
      );
}

String Scope::make_printable( const String &s )
{
  std::string ret;

  for( const char *c = s.c_str(); (*c) != '\0'; ++c )
  {
    switch( (*c) )
    {
      case '\a': ret += "\\a"; break;
      case '\b': ret += "\\b"; break;
      case '\f': ret += "\\f"; break;
      case '\n': ret += "\\n"; break;
      case '\r': ret += "\\r"; break;
      case '\t': ret += "\\t"; break;
      case '\v': ret += "\\v"; break;
      default:
        if( ::isprint( (*c) ) )
        {
          ret += (*c);
        }
        else
        {
          ret += "\\x";
          ret += char( '0' + char(((*c)&0xF0)>>4) );
          ret += char( '0' + char( (*c)&0x0F)   );
        }
        break;
    }
  }

  return String::MoveFrom( ret );
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


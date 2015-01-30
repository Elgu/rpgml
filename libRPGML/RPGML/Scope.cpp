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
#include "make_printable.h"

#include <iostream>

using namespace std;

namespace RPGML {

Scope::Scope( GarbageCollector *_gc, Context *context )
: Collectable( _gc )
, m_context( context )
, m_curr( context->getRoot() )
{}

Scope::Scope( GarbageCollector *_gc, const Scope *other, Frame *curr )
: Collectable( _gc )
, m_context( other->getContext() )
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

void Scope::gc_clear( void )
{
  m_context.reset();
  m_curr.reset();
}

void Scope::gc_getChildren( Children &children ) const
{
  children
    << m_context
    << m_curr
    ;
}

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

Value Scope::call(
    const Location *loc
  , index_t recursion_depth
  , const String &function_identifier
  , const Function::Args *call_args
  )
{
  Value *const func = lookup( function_identifier );
  if( !func ) throw ParseException( loc ) << "Function '" << function_identifier << "' not found";
  if( !func->isFunction() ) throw ParseException( loc ) << "Variable '" << function_identifier << "' is not a Function";

  try
  {
    return func->getFunction()->call( loc, recursion_depth, this, call_args );
  }
  catch( const ParseException & )
  {
    throw;
  }
  catch( const RPGML::Exception &e )
  {
    throw ParseException( loc, e );
  }

  // Never reached
  return Value();
}

Value Scope::call(
    const Location *loc
  , index_t recursion_depth
  , const String &function_identifier
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

  return call( loc, recursion_depth, function_identifier, n_args, args );
}

Value Scope::call(
    const Location *loc
  , index_t recursion_depth
  , const String &function_identifier
  , index_t n_args
  , const Value *args
  )
{
  Value *const func = lookup( function_identifier );
  if( !func ) throw ParseException( loc ) << "Function '" << function_identifier << "' not found";
  if( !func->isFunction() ) throw ParseException( loc ) << "Variable '" << function_identifier << "' is not a Function";

  try
  {
    return func->getFunction()->call( loc, recursion_depth, this, n_args, args );
  }
  catch( const ParseException & )
  {
    throw;
  }
  catch( const RPGML::Exception &e )
  {
    throw ParseException( loc, e );
  }

  // Never reached
  return Value();
}

CountPtr< Node > Scope::createNode(
    const Location *loc
  , index_t recursion_depth
  , const String &name
  )
{
  Value ret;
  try
  {
    ret = call( loc, recursion_depth+1, name, 0, (const Value*)0 );
  }
  catch( const ParseException & )
  {
    throw;
  }
  catch( const RPGML::Exception &e )
  {
    throw ParseException( loc, e );
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
    CountPtr< Node > ret = createNode( loc, recursion_depth+1, String::Static( ".core.Constant" ) );

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

    static const char *const size_names[] = { "sizeX", "sizeY", "sizeZ", "sizeT" };

    if( array_type.isPrimitive() )
    {
      CountPtr< Node > array_node = createNode( loc, recursion_depth+1, String::Static( ".core.ConstantArray" ) );
      const Type constant_array_type = array_type;

      // Set type and dims
      array_node->getParam( "type" )->set( Value( String::Static( constant_array_type.getTypeName() ) ) );
      array_node->getParam( "dims" )->set( Value( dims ) );

      // Set size
      for( int d=0; d<dims; ++d )
      {
        array_node->getParam( size_names[ d ] )->set( Value( array_size[ d ] ) );
      }

      const Frame::Ref constant_array_v = array_node->getVariable( "in" );
      if( constant_array_v.isNull() )
      {
        throw ParseException( new Location( __FILE__, __LINE__ ) )
          << "Could not get 'in' from ConstantArray"
          ;
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

      return array_node->getOutput( "out" );
    }
    else if( array_type.isOutput() || array_base->isValue() )
    {
      CountPtr< Node > array_node = createNode( loc, recursion_depth+1, String::Static( ".core.OutputArray" ) );

      // Set dims
      array_node->getParam( "dims" )->set( Value( dims ) );

      // Set size
      for( int d=0; d<dims; ++d )
      {
        array_node->getParam( size_names[ d ] )->set( Value( array_size[ d ] ) );
      }

      const Frame::Ref in_v = array_node->getVariable( "in" );
      if( in_v.isNull() )
      {
        throw ParseException( new Location( __FILE__, __LINE__ ) )
          << "Could not get 'in' from OutputArray";
      }
      if( !in_v->isArray() )
      {
        throw ParseException( new Location( __FILE__, __LINE__ ) )
          << "Expected 'in' of OutputArray to be a Array, is " << in_v->getTypeName()
          ;
      }
      ArrayBase *const in_base = in_v->getArray();
      InputArrayElements *in = 0;
      if( !in_base->getAs( in ) )
      {
        throw ParseException( new Location( __FILE__, __LINE__ ) )
          << "Expected 'in' of OutputArray to be a InputArray, Elements are " << in_base->getTypeName()
          ;
      }

      InputArrayElements::iterator in_iter = in->begin();
      for(
          CountPtr< ArrayBase::ConstValueIterator >
          array_iter = array_base->getConstValueIterator()
        ; !array_iter->done()
        ; array_iter->next(), ++in_iter
        )
      {
        CountPtr< Output > output = toOutput( loc, recursion_depth+1, array_iter->get() );
        (*in_iter)->connect( output );
      }

      return array_node->getOutput( "out" );
    }
    else
    {
      throw ParseException( loc )
        << "Cannot convert to Output: Unsupported Array Element type of " << array_base->getType()
        ;
    }
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


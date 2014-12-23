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
#include "Frame.h"

#include "String.h"
#include "Scope.h"
#include "SharedObject.h"
#include "Function.h"
#include "Node.h"
#include "InterpretingParser.h"
#include "FileSource.h"

#include <sys/types.h>
#include <dirent.h>

using namespace std;

namespace RPGML {

namespace Frame_impl {

class NodeCreator : public Function
{
public:
  typedef Function Base;
  EXCEPTION_BASE( Exception );

  NodeCreator( GarbageCollector *gc, Frame *parent, const String &name, create_Node_t create_Node, const SharedObject *so=0 );
  virtual ~NodeCreator( void );

  virtual bool call( const Location *loc, index_t recursion_depth, Scope *scope, Value &ret, const Args *call_args );
  virtual bool call_impl( const Location *loc, index_t recursion_depth, Scope *scope, Value &ret, index_t n_args, const Value *args );
  virtual void gc_clear( void );
  virtual void gc_getChildren( Children &children ) const;

  virtual const char *getName( void ) const;

private:
  String m_name;
  create_Node_t m_create_Node;
};

NodeCreator::NodeCreator( GarbageCollector *_gc, Frame *parent, const String &name, create_Node_t create_Node, const SharedObject *so )
: Function( _gc, parent, 0, so )
, m_name( name )
, m_create_Node( create_Node )
{}

NodeCreator::~NodeCreator( void )
{}

bool NodeCreator::call( const Location *loc, index_t recursion_depth, Scope *scope, Value &ret, const Args *call_args )
{
  (void)recursion_depth;
  (void)call_args;

  const String global_name =
    scope->genGlobalName(
         ":" + m_name + "@" + toString( loc->withoutFilename() )
       + "#" + toString( scope->getNr() )
       );
  CountPtr< Node > node( m_create_Node( getGC(), global_name, getSO() ) );

  if( call_args )
  {
    for( size_t i=0; i<call_args->size(); ++i )
    {
      const Arg &arg = call_args->at( i );
      if( arg.identifier.empty() )
      {
        throw Exception( "only identifier arguments allowed when instantiating a Node" );
      }

      Param *const param = node->getParam( arg.identifier );

      if( !param )
      {
        throw Exception( "Param not found." );
      }

      param->set( arg.value );
    }
  }

  ret = Value( node.get() );
  return true;
}

bool NodeCreator::call_impl( const Location *, index_t, Scope *, Value &, index_t, const Value * )
{
  throw Exception( "NodeCreator::call_impl() should newer be called" );
  return false;
}

void NodeCreator::gc_clear( void )
{
  Function::gc_clear();
  m_name.clear();
  m_create_Node = 0;
}

void NodeCreator::gc_getChildren( Children &children ) const
{
  Function::gc_getChildren( children );
}

const char *NodeCreator::getName( void ) const
{
  return m_name.get();
}

} // namespace Frame_impl

Frame::Frame( GarbageCollector *_gc, Frame *parent, const String &path )
: Collectable( _gc )
, m_parent( parent )
, m_path( path )
, m_depth( parent ? parent->getDepth()+1 : 0 )
, m_is_this( false )
{}

Frame::~Frame( void )
{}

Frame *Frame::getParent( void ) const
{
  return m_parent;
}

const String &Frame::getPath( void ) const
{
  return m_path;
}

Frame &Frame::setIdentifier( const String &identifier )
{
  m_identifier = identifier;
  return (*this);
}

const String &Frame::getIdentifier( void ) const
{
  return m_identifier;
}

Frame &Frame::setThis( bool is_this )
{
  m_is_this = is_this;
  return (*this);
}

bool Frame::isThis( void ) const
{
  return m_is_this;
}

index_t Frame::getDepth( void ) const
{
  return m_depth;
}

void Frame::reserve( index_t n )
{
  m_values.reserve( n );
}

index_t Frame::getIndex( const char *identifier ) const
{
  const int n = int( m_identifiers.size() );
  for( int i = n-1; i>=0; --i )
  {
//    cerr << "Frame::getIndex( '" << identifier << "' ): " << i << " = " << m_identifiers[ i ] << endl;
    if( m_identifiers[ i ] == identifier )
    {
      return index_t( i );
    }
  }
  return unknown;
}

index_t Frame::getCreateIndex( const String &identifier, bool *existed_p )
{
  index_t index = getIndex( identifier );
  if( unknown != index )
  {
    if( existed_p ) (*existed_p) = true;
    return index;
  }

  assert( m_values.size() == m_identifiers.size() );
  index = index_t( m_values.size() );
  m_values     .push_back( Value() );
  m_identifiers.push_back( identifier );

  if( existed_p ) (*existed_p) = false;
  return index;
}

index_t Frame::set( const String &identifier, const Value &value )
{
  const index_t index = getCreateIndex( identifier );
  m_values[ index ] = value;
  return index;
}

const Value *Frame::getVariable( const char *identifier ) const
{
  const index_t index = getIndex( identifier );

  if( index != unknown )
  {
    return &m_values[ index ];
  }
  else
  {
    return 0;
  }
}

Value *Frame::getVariable( const char *identifier )
{
  return const_cast< Value* >( ((const Frame*)this)->getVariable( identifier ) );
}

const Value *Frame::getStack( index_t index ) const
{
  if( size_t( index ) < m_values.size() )
  {
    return &m_values[ index ];
  }
  else
  {
    return 0;
  }
}

Value       *Frame::getStack( index_t index )
{
  return const_cast< Value* >( ((const Frame*)this)->getStack( index ) );
}

String Frame::genGlobalName( const String &identifier ) const
{
  String ret = identifier;

  for( const Frame *m = this; m; m = m->getParent() )
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

Value *Frame::push_back( const String &identifier, const Value &value )
{
  assert( m_values.size() == m_identifiers.size() );
  const index_t index = index_t( m_values.size() );

  m_identifiers.push_back( identifier );
  m_values.push_back( value );

  return &m_values[ index ];
}

void Frame::pop_back( void )
{
  assert( m_values.size() == m_identifiers.size() );
  if( m_values.empty() ) throw Exception( "Frame already empty" );

  const size_t new_size = m_values.size()-1;
  m_values.resize( new_size );
  m_identifiers.resize( new_size );
}

Frame::PushPopGuard::PushPopGuard( Frame *frame, const String &identifier, const Value &value )
: m_frame( frame )
, m_value( m_frame ? m_frame->push_back( identifier, value ) : 0 )
{}

Frame::PushPopGuard::~PushPopGuard( void )
{
  if( m_frame ) m_frame->pop_back();
}

Value *Frame::PushPopGuard::get( void ) const
{
  return m_value;
}

Value *Frame::load( const String &identifier, const Scope *scope )
{
  const index_t index = getIndex( identifier );

  if( index != unknown )
  {
    return &m_values[ index ];
  }

  const bool is_root = ( this == scope->getRoot() );

  if( !getPath().empty() || is_root )
  {
    if( !identifier.empty() )
    {
      if( is_root )
      {
        return load_global( identifier, scope );
      }
      else
      {
        return load_local( identifier, scope );
      }
    }
  }

  return 0;
}

Value *Frame::load_local ( const String &identifier, const Scope *scope )
{
  return load( getPath(), identifier, scope );
}

Value *Frame::load_global( const String &identifier, const Scope *scope )
{
  return load( ".", identifier, scope );
}

Value *Frame::load( const String &path, const String &identifier, const Scope *scope )
{
	// Check whether identifier refers to a directory, use it as namespace
  String dir = path + "/" + identifier;
  DIR *const dir_p = opendir( dir.c_str() );
  if( dir_p )
  {
    closedir( dir_p );
    return getStack( set( identifier, Value( new Frame( getGC(), this, dir ) ) ) );
  }

	// Check whether identifier refers to a Function-plugin
  {
    const String so = path + "/libRPGML_Function_" + identifier + ".so";
    FILE *so_file = fopen( so.c_str(), "rb" );
    if( so_file )
    {
  //    std::cerr << "Found shared object '" << so << "'" << std::endl;
      fclose( so_file );

      String err;
      CountPtr< SharedObject > so_p = new SharedObject( so, err );
      if( so_p->isValid() )
      {
  //      std::cerr << "Loaded shared object '" << so << "'" << std::endl;

        create_Function_t create_function;
        const String symbol = identifier + "_create_Function";
        if( so_p->getSymbol( symbol, create_function, err ) )
        {
  //        std::cerr << "Got symbol '" << symbol << " from shared object '" << so << "'" << std::endl;
          CountPtr< Function > function = create_function( getGC(), this, so_p.get() );
          return getStack( set( identifier, Value( function.get() ) ) );
        }
      }
      else
      {
        throw Exception() << "Opening shared object '" << so << "' failed: " << err;
      }
    }
  }

	// Check whether identifier refers to a Node-plugin
  {
    const String so = path + "/libRPGML_Node_" + identifier + ".so";
    FILE *so_file = fopen( so.c_str(), "rb" );
    if( so_file )
    {
  //    std::cerr << "Found shared object '" << so << "'" << std::endl;
      fclose( so_file );

      String err;
      CountPtr< SharedObject > so_p = new SharedObject( so, err );
      if( so_p->isValid() )
      {
  //      std::cerr << "Loaded shared object '" << so << "'" << std::endl;

        create_Node_t create_Node;
        const String symbol = identifier + "_create_Node";
        if( so_p->getSymbol( symbol, create_Node, err ) )
        {
  //        std::cerr << "Got symbol '" << symbol << " from shared object '" << so << "'" << std::endl;
          const String name = genGlobalName( identifier );
          CountPtr< Function > node_creator(
            new Frame_impl::NodeCreator(
              getGC(), this, name, create_Node, so_p.get()
              )
            );
          return getStack( set( identifier, Value( node_creator.get() ) ) );
        }
      }
      else
      {
        throw Exception() << "Opening so file '" << so << "' failed: " << err;
      }
    }
  }

	// Check whether identifier refers to a script
  {
    const String rpgml = path + "/" + identifier + ".rpgml";
    FILE *const rpgml_p = fopen( rpgml.c_str(), "r" );
    if( rpgml_p )
    {
      CountPtr< Source > source = new FileSource( rpgml_p );
      CountPtr< Scope > new_scope = new Scope( *scope );
      CountPtr< InterpretingParser > parser = new InterpretingParser( new_scope, source );
      try
      {
        parser->parse();
      }
      catch( const char *e )
      {
        throw Exception()
          << "Failed to parse '" << rpgml << "'"
          << ": " << e
          ;
      }

      Value *const ret = getVariable( identifier );
      if( !ret )
      {
        throw Exception()
          << "Variable '" << identifier << "'"
          << " was not defined in the rpgml file '" << rpgml << "'"
          ;
      }
      return ret;
    }
  }

  return 0;
}

void Frame::gc_clear( void )
{
  m_values.clear();
  m_identifiers.clear();
  m_identifier.clear();
  m_parent.reset();
}

void Frame::gc_getChildren( Children &children ) const
{
  children.add( m_parent.get() );

  for( size_t i( 0 ), end( m_values.size() ); i<end; ++i )
  {
    const Value &value = m_values[ i ];
    if( value.isCollectable() )
    {
      children.add( value.getCollectable() );
    }
  }
}

} // namespace RPGML


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
#include "ParseException.h"

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

  NodeCreator( GarbageCollector *gc, const Location *loc, Frame *parent, const String &name, create_Node_t create_Node, const SharedObject *so=0 );
  virtual ~NodeCreator( void );

  virtual Value call( const Location *loc, index_t recursion_depth, Scope *scope, const Args *call_args );
  virtual Value call( const Location *loc, index_t recursion_depth, Scope *scope, index_t n_args, const Value *args );
  virtual Value call_impl( const Location *loc, index_t recursion_depth, Scope *scope, index_t n_args, const Value *args );

  virtual void gc_clear( void );
  virtual void gc_getChildren( Children &children ) const;

  virtual const char *getName( void ) const;

private:
  String m_name;
  create_Node_t m_create_Node;
};

NodeCreator::NodeCreator( GarbageCollector *_gc, const Location *loc, Frame *parent, const String &name, create_Node_t create_Node, const SharedObject *so )
: Function( _gc, loc, parent, 0, so )
, m_name( name )
, m_create_Node( create_Node )
{}

NodeCreator::~NodeCreator( void )
{}

Value NodeCreator::call( const Location *loc, index_t recursion_depth, Scope *scope, const Args *call_args )
{
  (void)recursion_depth;
  (void)call_args;

  const String global_name = scope->genGlobalName( m_name, loc );
  CountPtr< Node > node( m_create_Node( getGC(), global_name, getSO() ) );

  if( call_args )
  {
    for( size_t i=0; i<call_args->size(); ++i )
    {
      const Arg &arg = call_args->at( i );
      const String &identifier = arg.getIdentifier();
      if( identifier.empty() )
      {
        throw Exception() << "Only identifier arguments allowed when instantiating a Node";
      }

      try
      {
        node->getParam( identifier )->set( arg.getValue() );
      }
      catch( const RPGML::Exception &e )
      {
        throw ParseException( loc, e );
      }
    }
  }

  return Value( node.get() );
}

Value NodeCreator::call( const Location *loc, index_t recursion_depth, Scope *scope, index_t n_args, const Value *args )
{
  if( 0 == n_args )
  {
    return call( loc, recursion_depth, scope, (const Args*)0 );
  }
  else
  {
    return call_impl( loc, recursion_depth, scope, n_args, args );
  }
}

Value NodeCreator::call_impl( const Location *, index_t, Scope *, index_t, const Value * )
{
  throw Exception() << "NodeCreator::call_impl() should never be called";
}

void NodeCreator::gc_clear( void )
{
  Base::gc_clear();
}

void NodeCreator::gc_getChildren( Children &children ) const
{
  Base::gc_getChildren( children );
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
{
//  cerr << "Frame " << (void*)this << " create: gc = " << (void*)getGC() << endl;
}

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
  m_identifiers.reserve( n );
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

Frame::ConstRef Frame::getVariable( const char *identifier ) const
{
  const index_t index = getIndex( identifier );

  if( index != unknown )
  {
    return ConstRef( this, index );
  }
  else
  {
    return ConstRef();
  }
}

Frame::Ref Frame::getVariable( const char *identifier )
{
  const index_t index = getIndex( identifier );

  if( index != unknown )
  {
    return Ref( this, index );
  }
  else
  {
    return Ref();
  }
}

Frame::ConstRef Frame::getStack( index_t index ) const
{
  if( size_t( index ) < m_values.size() )
  {
    return ConstRef( this, index );
  }
  else
  {
    return ConstRef();
  }
}

Frame::Ref Frame::getStack( index_t index )
{
  if( size_t( index ) < m_values.size() )
  {
    return Ref( this, index );
  }
  else
  {
    return Ref();
  }
}

Frame::Ref      Frame::operator[]( const char *identifier )
{
  return getVariable( identifier );
}

Frame::ConstRef Frame::operator[]( const char *identifier ) const
{
  return getVariable( identifier );
}

Frame::Ref      Frame::operator[]( index_t index )
{
  return getStack( index );
}

Frame::ConstRef Frame::operator[]( index_t index ) const
{
  return getStack( index );
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

Frame::Ref Frame::push_back( const String &identifier, const Value &value )
{
  assert( m_values.size() == m_identifiers.size() );
  const index_t index = index_t( m_values.size() );

  m_identifiers.push_back( identifier );
  m_values.push_back( value );

  return Ref( this, index );
}

void Frame::pop_back( void )
{
  assert( m_values.size() == m_identifiers.size() );
  if( m_values.empty() ) throw Exception() << "Frame already empty";

  const size_t new_size = m_values.size()-1;
  m_values.resize( new_size );
  m_identifiers.resize( new_size );
}

Frame::Ref::Ref( void )
: m_index( 0 )
{}

Frame::Ref::Ref( Frame *frame, index_t index )
: m_frame( frame ), m_index( index )
{}

Frame::Ref::~Ref( void )
{}

bool Frame::Ref::isNull( void ) const
{
  return m_frame.isNull();
}

Value *Frame::Ref::getValue( void ) const
{
  if( !isNull() )
  {
    return &m_frame->m_values[ m_index ];
  }
  else
  {
    return 0;
  }
}

index_t Frame::Ref::getIndex( void ) const
{
  return m_index;
}

Frame *Frame::Ref::getFrame( void ) const
{
  return m_frame;
}

Frame::Ref::operator Value*( void ) const
{
  return getValue();
}

Value &Frame::Ref::operator*( void ) const
{
  return *getValue();
}

Value *Frame::Ref::operator->( void ) const
{
  return getValue();
}

Frame::ConstRef::ConstRef( void )
: m_index( 0 )
{}

Frame::ConstRef::ConstRef( const Ref &ref )
: m_frame( ref.m_frame )
, m_index( ref.m_index )
{}

Frame::ConstRef::ConstRef( const Frame *frame, index_t index )
: m_frame( frame )
, m_index( index )
{}

Frame::ConstRef::~ConstRef( void )
{}

bool Frame::ConstRef::isNull( void ) const
{
  return m_frame.isNull();
}

const Value *Frame::ConstRef::getValue( void ) const
{
  if( !isNull() )
  {
    return &m_frame->m_values[ m_index ];
  }
  else
  {
    return 0;
  }
}

index_t Frame::ConstRef::getIndex( void ) const
{
  return m_index;
}

const Frame *Frame::ConstRef::getFrame( void ) const
{
  return m_frame;
}

Frame::ConstRef::operator const Value*( void ) const
{
  return getValue();
}

const Value &Frame::ConstRef::operator*( void ) const
{
  return *getValue();
}

const Value *Frame::ConstRef::operator->( void ) const
{
  return getValue();
}

Frame::PushPopGuard::PushPopGuard( Frame *frame, const String &identifier, const Value &value )
: m_ref( frame ? frame->push_back( identifier, value ) : Ref() )
{}

Frame::PushPopGuard::~PushPopGuard( void )
{
  if( !m_ref.isNull() ) m_ref.getFrame()->pop_back();
}

Frame::Ref Frame::PushPopGuard::get( void ) const
{
  return m_ref;
}

Frame::Ref Frame::load( const String &identifier, const Scope *scope )
{
  if( identifier.empty() )
  {
    throw Exception() << "Cannot load empty identifier";
  }

  if( identifier[ 0 ] == '.' )
  {
    throw Exception() << "Frame member identifiers cannot start with a '.'";
  }

//  cerr << "Frame::load( '" << identifier << "', scope ); Frame = '" << getIdentifier() << "'" << endl;

  Frame *curr_frame = this;
  size_t dot_pos = 0;
  size_t identifier_begin = 0;
  while( String::npos != ( dot_pos = identifier.find( '.', identifier_begin ) ) )
  {
    const String frame_identifier = identifier.mid( identifier_begin, dot_pos-1 );
    if( frame_identifier.empty() )
    {
      throw Exception()
        << "Identifier '" << identifier << "' contains an empty frame identifier between two '.'"
        << ": identifier_begin = " << identifier_begin
        << ", dot_pos = " << dot_pos
        ;
    }

    Frame::Ref v = curr_frame->load( frame_identifier, scope );
    if( v.isNull() )
    {
      throw Exception() << "Identifier '" << frame_identifier << "' not found";
    }
    else if( v->isFrame() )
    {
      curr_frame = v->getFrame();
    }
    else if( v->isNode() )
    {
      curr_frame = v->getNode();
    }
    else
    {
      throw Exception()
        << "Identifier '" << frame_identifier << "' is not a Frame or a Node, is " << v->getType()
        ;
    }

    identifier_begin = dot_pos+1;
  }

  String load_identifier( identifier_begin > 0 ? identifier.mid( identifier_begin ) : identifier );

  const index_t index = curr_frame->getIndex( load_identifier );
  if( index != unknown )
  {
    return Ref( curr_frame, index );
  }

  const bool is_root = ( curr_frame == scope->getRoot() );

  if( !curr_frame->getPath().empty() || is_root )
  {
    if( is_root )
    {
      return curr_frame->load_global( load_identifier, scope );
    }
    else
    {
      return curr_frame->load_local( load_identifier, scope );
    }
  }

  return Ref();
}

Frame::Ref Frame::load_local ( const String &identifier, const Scope *scope )
{
  return load( getPath(), identifier, scope );
}

Frame::Ref Frame::load_global( const String &identifier, const Scope *scope )
{
  const std::vector< String > &searchPaths = scope->getContext()->getSearchPaths();

  Frame::Ref ret;

  for( size_t i( 0 ), end( searchPaths.size() ); i < end; ++i )
  {
    const Ref ret_i = load( searchPaths[ i ].c_str(), identifier, scope );
    if( !ret_i.isNull() )
    {
      if( !ret.isNull() )
      {
        std::cerr
          << "warning: '" << identifier << "'"
          << " was found multiple times in different search paths"
          << std::endl
          ;
      }
      else
      {
        ret = ret_i;
      }
    }
  }

  return ret;
}

Frame::Ref Frame::load( const String &path, const String &identifier, const Scope *scope )
{
//  std::cerr << "load( '" << path << "', '" << identifier << "', )" << std::endl;

  // Check whether identifier refers to a directory, use it as namespace
  String dir = path + "/" + identifier;
  DIR *const dir_p = opendir( dir.c_str() );
  if( dir_p )
  {
    closedir( dir_p );
//    std::cerr << "Found directory '" << dir << "'" << std::endl;
    CountPtr< Frame > ret = new Frame( getGC(), this, dir );
    const String unified = scope->unify( identifier );
    ret->setIdentifier( unified );
    return getStack( set( unified, Value( ret ) ) );
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
          const String unified = scope->unify( identifier );
          return getStack( set( unified, Value( function.get() ) ) );
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
              getGC(), new Location( so ), this, name, create_Node, so_p.get()
              )
            );
          const String unified = scope->unify( identifier );
          return getStack( set( unified, Value( node_creator.get() ) ) );
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
      CountPtr< Scope > new_scope = new Scope( getGC(), scope, this );
      CountPtr< InterpretingParser > parser = new InterpretingParser( getGC(), new_scope, source );

      parser->setFilename( rpgml );
      parser->parse();
      parser.reset();
      source.reset();

      Frame::Ref ret = getVariable( identifier );
      if( ret.isNull() )
      {
        throw Exception()
          << "Frame(" << (void*)this << "): "
          << "Variable '" << identifier << "'"
          << " was not defined in the rpgml file '" << rpgml << "'"
          ;
      }
      return ret;
    }
  }

  return Ref();
}

void Frame::gc_clear( void )
{
  m_parent.reset();
  m_values.clear();
}

void Frame::gc_getChildren( Children &children ) const
{
  Base::gc_getChildren( children );
  children << m_parent;

  for( size_t i( 0 ), end( m_values.size() ); i<end; ++i )
  {
    children << m_values[ i ];
  }
}

} // namespace RPGML


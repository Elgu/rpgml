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
#ifndef RPGML_Node_h
#define RPGML_Node_h

#include "GarbageCollector.h"
#include "Frame.h"
#include "Array.h"
#include "Exception.h"

#include <vector>

#define DEFINE_INPUT( INPUT_ENUM, identifier ) \
  getInput( INPUT_ENUM )->setIdentifier( String::Static( identifier ) ); \
  push_back( String::Static( identifier  ), Value( getInput( INPUT_ENUM ) ) ); \

#define DEFINE_OUTPUT( OUTPUT_ENUM, identifier ) \
  getOutput( OUTPUT_ENUM )->setIdentifier( String::Static( identifier ) ); \
  push_back( String::Static( identifier  ), Value( getOutput( OUTPUT_ENUM ) ) ); \

#define DEFINE_OUTPUT_INIT( OUTPUT_ENUM, identifier, type, dims ) \
  getOutput( OUTPUT_ENUM )->init< type, dims >( String::Static( identifier ) ); \
  push_back( String::Static( identifier  ), Value( getOutput( OUTPUT_ENUM ) ) ); \

#define DEFINE_PARAM( PARAM_ENUM, identifier, method ) \
  setParam( PARAM_ENUM, new NParam( getGC(), this, String::Static( identifier ), &method ) ); \
  push_back( String::Static( identifier ), Value( getParam( PARAM_ENUM ) ) ); \

namespace RPGML {

class Port;
class Input;
class Output;
class Node;

class Port : public Collectable
{
public:
  Port( GarbageCollector *gc, Node *parent );
  virtual ~Port( void );

  Node *getParent( void ) const;
  const String &getIdentifier( void ) const;
  void setIdentifier( const String &identifier );

  virtual void gc_clear( void );
  virtual void gc_getChildren( Children &children ) const;

private:
  CountPtr< Node > m_parent;
  String m_identifier;
};

class Input : public Port
{
public:
  Input( GarbageCollector *gc, Node *parent );
  virtual ~Input( void );

  void init( const String &identifier );

  virtual void gc_clear( void );
  virtual void gc_getChildren( Children &children ) const;

  void disconnect( void );
  void connect( Output *output );

  bool isConnected( void ) const;

  const Output *getOutput( void ) const;
  const ArrayBase *getData( void ) const;

private:
  CountPtr< Output > m_output;
};

class Output : public Port
{
public:
  Output( GarbageCollector *gc, Node *parent );
  virtual ~Output( void );

  template< class T, int DIMS >
  void init( const String &identifier, const index_t *size = 0 );

  void disconnect( void );
  void disconnect( Input *input );

  void connect( Input *input );

  bool isConnected( void ) const;

  void setData( ArrayBase *data );
  ArrayBase *getData( void );
  const ArrayBase *getData( void ) const;

  template< class T >
  void initData( int dims, const index_t *size );

  template< class DataType >
  DataType *getAs( DataType* &as )
  {
    if( !m_data.isNull() ) return m_data->getAs( as );
    as = 0; return 0;
  }

  template< class DataType >
  const DataType *getAs( const DataType* &as ) const
  {
    if( !m_data.isNull() ) return m_data->getAs( as );
    as = 0; return 0;
  }

  virtual void gc_clear( void );
  virtual void gc_getChildren( Children &children ) const;

private:
  typedef Array< CountPtr< Input >, 1 > inputs_t;
  inputs_t m_inputs;
  CountPtr< ArrayBase > m_data;
};

class Param : public Collectable
{
public:
  explicit Param(
      GarbageCollector *_gc
    , const String &identifier
    )
  : Collectable( _gc )
  , m_identifier( identifier )
  {}

  virtual ~Param( void ) {}

  virtual void gc_clear( void ) {}
  virtual void gc_getChildren( Children & ) const {}

  virtual void set( const Value &value ) = 0;
  virtual Node *getParent( void ) const = 0;
  const String &getIdentifier( void ) const { return m_identifier; }
private:
  const String m_identifier;
};

class Node : public Frame
{
public:
  EXCEPTION_BASE( Exception );
  typedef Frame Base;

  class NotConnected : public Exception
  {
  public:
    typedef Exception Base;
    explicit NotConnected( const Input *_input );
    EXCEPTION_BODY( NotConnected )
    CountPtr< const Input > input;
  };

  class GetAsFailed : public Exception
  {
  public:
    typedef Exception Base;

    template< class T, int DIMS >
    explicit GetAsFailed( const Port *_port, const Array< T, DIMS > * )
    : port( _port )
    {
      (*this) << "Could not get '" << port->getIdentifier() << "' as 'Array< " << getTypeName< T >() << ", " << DIMS << " >'";
    }

    template< class T >
    explicit GetAsFailed( const Port *_port, const ArrayElements< T > * )
    : port( _port )
    {
      (*this) << "Could not get '" << port->getIdentifier() << "' as 'ArrayElements< " << getTypeName< T >() << " >'";
    }

    EXCEPTION_BODY( GetAsFailed )
    CountPtr< const Port > port;
  };

  class InitFailed : public Exception
  {
  public:
    typedef Exception Base;
    explicit
    InitFailed( const Output *_output );
    EXCEPTION_BODY( InitFailed )
    CountPtr< const Output > output;
  };

  Node(
      GarbageCollector *gc
    , const String &identifier
    , const SharedObject *so
    , index_t num_inputs = 0
    , index_t num_outputs = 0
    , index_t num_params = 0
    );

  virtual ~Node( void );

  virtual const char *getName( void ) const = 0;
  const String &getIdentifier( void ) const;
  const SharedObject *getSO( void ) const;

  virtual bool tick( void );

  virtual void gc_clear( void );
  virtual void gc_getChildren( Children &children ) const;

  Input *getInput( index_t i ) const;
  Input *getInput( int     i ) const;
  Input *getInput( const char *identifier ) const;
  Output *getOutput( index_t i ) const;
  Output *getOutput( int     i ) const;
  Output *getOutput( const char *identifier ) const;
  Param *getParam( index_t i ) const;
  Param *getParam( int     i ) const;
  Param *getParam( const char *identifier ) const;

  Param *setParam( index_t i, Param *param );

  void setNumInputs( index_t n );
  void setNumOutputs( index_t n );
  void setNumParams( index_t n );

  index_t getNumInputs( void ) const;
  index_t getNumOutputs( void ) const;
  index_t getNumParams( void ) const;

protected:
  template< class ParentNode >
  class NodeParam : public Param
  {
  public:
    typedef void (ParentNode::*callback_t)( const Value &value, index_t );

    explicit NodeParam(
        GarbageCollector *_gc
      , ParentNode *parent
      , const String &identifier
      , callback_t callback
      , index_t index=0
      )
    : Param( _gc, identifier )
    , m_parent( parent )
    , m_callback( callback )
    , m_index( index )
    {}
    virtual ~NodeParam( void ) {}

    virtual void gc_clear( void ) { Param::gc_clear(); m_parent.clear(); }
    virtual void gc_getChildren( Children &children ) const
    {
      Param::gc_getChildren( children );
      children.add( static_cast< Collectable* >( m_parent.get() ) );
    }

    virtual void set( const Value &value )
    {
      (m_parent->*m_callback)( value, m_index );
    }

    virtual Node *getParent( void ) const
    {
      return m_parent.get();
    }

  private:
    CountPtr< ParentNode > m_parent;
    callback_t m_callback;
    index_t m_index;
  };

private:
  typedef Array< CountPtr< Input  >, 1 > inputs_t;
  typedef Array< CountPtr< Output >, 1 > outputs_t;
  typedef Array< CountPtr< Param  >, 1 > params_t;
  inputs_t  m_inputs;
  outputs_t m_outputs;
  params_t  m_params;
  const String m_identifier;
  CountPtr< const SharedObject > m_so;
};

typedef CountPtr< Function > (*create_Function_t)( GarbageCollector *gc, Frame *parent, const SharedObject *so );
typedef CountPtr< Node > (*create_Node_t)( GarbageCollector *gc, const String &identifier, const SharedObject *so );

template< class T, int DIMS >
void Output::init( const String &identifier, const index_t *size )
{
  setIdentifier( identifier );

  CountPtr< ArrayBase > out = getData();

  if(
       out.isNull()
    || TypeOf< T >::E == Type::OTHER
    || out->getType() != TypeOf< T >::E
    || out->getDims() != DIMS
    )
  {
    setData( new Array< T, DIMS >( getGC(), DIMS, size ) );
  }
  else
  {
    if( size ) out->resize( DIMS, size );
  }
}

template< class T >
void Output::initData( int dims, const index_t *size )
{
  CountPtr< ArrayBase > out = getData();

  if(
       out.isNull()
    || TypeOf< T >::E == Type::OTHER
    || out->getType() != TypeOf< T >::E
    || out->getDims() != dims
    )
  {
    switch( dims )
    {
      case 0: out.reset( new Array< T, 0 >( getGC(), dims, size ) ); break;
      case 1: out.reset( new Array< T, 1 >( getGC(), dims, size ) ); break;
      case 2: out.reset( new Array< T, 2 >( getGC(), dims, size ) ); break;
      case 3: out.reset( new Array< T, 3 >( getGC(), dims, size ) ); break;
      case 4: out.reset( new Array< T, 4 >( getGC(), dims, size ) ); break;
      default:
        throw Node::InitFailed( this )
          << ": Number of dimensions greater than 4 not supported."
          ;
    }
    setData( out );
  }

  out->resize( dims, size );
}

} // namespace RPGML

#define RPGML_CREATE_FUNCTION( NAME ) \
  extern "C"\
  RPGML::CountPtr< RPGML::Function > NAME ## _create_Function( RPGML::GarbageCollector *gc, RPGML::Frame *parent, const RPGML::SharedObject *so )\
  {\
    return new RPGML::Function_ ## NAME( gc, parent, so );\
  }\

#define RPGML_CREATE_NODE( NAME ) \
  extern "C"\
  RPGML::CountPtr< RPGML::Node > NAME ## _create_Node( RPGML::GarbageCollector *gc, const RPGML::String &identifier, const RPGML::SharedObject *so )\
  {\
    return new RPGML::NAME( gc, identifier, so );\
  }\

#endif


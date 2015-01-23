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
#include "JobQueue.h"

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
  setParam( PARAM_ENUM, new NParam( getGC(), this, String::Static( identifier ), &method, unknown, false ) ); \
  push_back( String::Static( identifier ), Value( getParam( PARAM_ENUM ) ) ); \

#define DEFINE_PARAM_INDEX( PARAM_ENUM, identifier, method, index ) \
  setParam( PARAM_ENUM, new NParam( getGC(), this, String::Static( identifier ), &method, index, false ) ); \
  push_back( String::Static( identifier ), Value( getParam( PARAM_ENUM ) ) ); \

#define DEFINE_PARAM_ARRAY( PARAM_ENUM, identifier, method ) \
  setParam( PARAM_ENUM, new NParam( getGC(), this, String::Static( identifier ), &method, unknown, true ) ); \
  push_back( String::Static( identifier ), Value( getParam( PARAM_ENUM ) ) ); \

#define DEFINE_PARAM_ARRAY_INDEX( PARAM_ENUM, identifier, method, index ) \
  setParam( PARAM_ENUM, new NParam( getGC(), this, String::Static( identifier ), &method, index, true ) ); \
  push_back( String::Static( identifier ), Value( getParam( PARAM_ENUM ) ) ); \

#define GET_INPUT_DATA( INPUT_ENUM, var_identifier ) \
  if( !getInput( INPUT_ENUM )->isConnected() ) throw NotConnected( getInput( INPUT_ENUM ) ); \
  const ArrayBase *const var_identifier = getInput( INPUT_ENUM )->getData(); \
  if( !var_identifier ) throw NotReady( getInput( INPUT_ENUM ) ); \

#define GET_INPUT_MANDATORY( INPUT_ENUM, var_identifier, type, dims ) \
  const Array< type, dims > *var_identifier = 0; \
  if( !getInput( INPUT_ENUM )->isConnected() ) \
  { \
    throw NotConnected( getInput( INPUT_ENUM ) ); \
  } \
  if( !getInput( INPUT_ENUM )->getOutput()->getAs( var_identifier ) ) \
  { \
    throw GetAsFailed( getInput( INPUT_ENUM ), var_identifier ); \
  } \

#define GET_INPUT_ELEMENTS( INPUT_ENUM, var_identifier, type ) \
  const ArrayElements< type > *var_identifier = 0; \
  if( !getInput( INPUT_ENUM )->isConnected() ) \
  { \
    throw NotConnected( getInput( INPUT_ENUM ) ); \
  } \
  if( !getInput( INPUT_ENUM )->getOutput()->getAs( var_identifier ) ) \
  { \
    throw GetAsFailed( getInput( INPUT_ENUM ), var_identifier ); \
  } \

#define GET_INPUT_IF_CONNECTED( INPUT_ENUM, var_identifier, type, dims ) \
  const Array< type, dims > *var_identifier = 0; \
  if( getInput( INPUT_ENUM )->isConnected() ) \
  { \
    if( !getInput( INPUT_ENUM )->getOutput()->getAs( var_identifier ) ) \
    { \
      throw GetAsFailed( getInput( INPUT_ENUM ), var_identifier ); \
    } \
  } \

#define GET_INPUT_ELEMENTS_IF_CONNECTED( INPUT_ENUM, var_identifier, type ) \
  const ArrayElements< type > *var_identifier = 0; \
  if( getInput( INPUT_ENUM )->isConnected() ) \
  { \
    if( !getInput( INPUT_ENUM )->getOutput()->getAs( var_identifier ) ) \
    { \
      throw GetAsFailed( getInput( INPUT_ENUM ), var_identifier ); \
    } \
  } \

#define GET_OUTPUT_AS( OUTPUT_ENUM, var_identifier, type, dims ) \
  Array< type, dims > *var_identifier = 0; \
  if( !getOutput( OUTPUT_ENUM )->getAs( var_identifier ) ) \
  { \
    throw GetAsFailed( getOutput( OUTPUT_ENUM ), var_identifier ); \
  } \

#define GET_OUTPUT_INIT( OUTPUT_ENUM, var_identifier, type, dims, size ) \
  getOutput( OUTPUT_ENUM )->initData< type >( dims, size ); \
  Array< type, dims > *var_identifier = 0; \
  if( !getOutput( OUTPUT_ENUM )->getAs( var_identifier ) ) \
  { \
    throw GetAsFailed( getOutput( OUTPUT_ENUM ), var_identifier ); \
  } \

#define GET_OUTPUT_ELEMENTS_INIT( OUTPUT_ENUM, var_identifier, type, dims, size ) \
  getOutput( OUTPUT_ENUM )->initData< type >( dims, size ); \
  ArrayElements< type > *var_identifier = 0; \
  if( !getOutput( OUTPUT_ENUM )->getAs( var_identifier ) ) \
  { \
    throw GetAsFailed( getOutput( OUTPUT_ENUM ), var_identifier ); \
  } \
  if( size ) var_identifier->resize_v( dims, size );\

#define GET_OUTPUT_IF_CONNECTED( OUTPUT_ENUM, var_identifier, type, dims ) \
  Array< type, dims > *var_identifier = 0; \
  { \
    Output *const output = getOutput( OUTPUT_ENUM ); \
    if( output->isConnected() && !output->getAs( var_identifier ) ) \
    { \
      throw GetAsFailed( getOutput( OUTPUT_ENUM ), var_identifier ); \
    } \
  } \

#define GET_OUTPUT_ELEMENTS_IF_CONNECTED( OUTPUT_ENUM, var_identifier, type ) \
  ArrayElements< type > *var_identifier = 0; \
  { \
    Output *const output = getOutput( OUTPUT_ENUM ); \
    if( output->isConnected() && !output->getAs( var_identifier ) ) \
    { \
      throw GetAsFailed( getOutput( OUTPUT_ENUM ), var_identifier ); \
    } \
  } \

class utest_Node;

namespace RPGML {

class Port;
class Input;
class Output;
class Node;

class Port : public Collectable
{
public:
  typedef Collectable Base;
  Port( GarbageCollector *gc, Node *parent );
  virtual ~Port( void );

  Node *getParent( void ) const;
  const String &getIdentifier( void ) const;
  void setIdentifier( const String &identifier );

  virtual void gc_clear( void );
  virtual void gc_getChildren( Children &children ) const;

private:
  friend class ::utest_Node;
  CountPtr< Node > m_parent;
  String m_identifier;
};

class Input : public Port
{
public:
  typedef Port Base;
  Input( GarbageCollector *gc, Node *parent );
  virtual ~Input( void );

  void init( const String &identifier );

  virtual void gc_clear( void );
  virtual void gc_getChildren( Children &children ) const;

  void disconnect( void );
  void connect( Output *output );

  bool isConnected( void ) const;
  bool hasChanged( void ) const;

  const Output *getOutput( void ) const;
  const ArrayBase *getData( void ) const;

private:
  friend class ::utest_Node;
  CountPtr< Output > m_output;
};

class Output : public Port
{
public:
  typedef Port Base;
  Output( GarbageCollector *gc, Node *parent );
  virtual ~Output( void );

  template< class T, int DIMS >
  void init( const String &identifier, const index_t *size = 0 );

  void disconnect( void );
  void disconnect( Input *input );

  void connect( Input *input );

  bool isConnected( void ) const;
  bool hasChanged( void ) const;

  void setChanged( bool changed=true );

  void setData( CountPtr< ArrayBase > data );
  ArrayBase *getData( void );
  const ArrayBase *getData( void ) const;

  template< class T >
  void initData( int dims, const index_t *size );

  template< class T >
  void initData( const ArrayBase::Size &size ) { initData< T >( size.getDims(), size.getCoords() ); }

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

  void resolve( void );

  virtual void gc_clear( void );
  virtual void gc_getChildren( Children &children ) const;

  typedef Array< CountPtr< Input >, 1 > inputs_t;

  typedef inputs_t::iterator inputs_iterator;
  inputs_iterator inputs_begin( void );
  inputs_iterator inputs_end  ( void );

private:
  friend class ::utest_Node;
  CountPtr< inputs_t  > m_inputs;
  CountPtr< ArrayBase > m_data;
  bool m_hasChanged;
};

class Param : public Collectable
{
public:
  explicit
  Param( GarbageCollector *_gc , const String &identifier );

  virtual ~Param( void );

  virtual void gc_clear( void );
  virtual void gc_getChildren( Children & ) const;

  virtual void set( const Value &value, int n_coords=0, const index_t *coords=0 ) = 0;
  virtual Node *getParent( void ) const = 0;
  const String &getIdentifier( void ) const { return m_identifier; }

  struct Setting
  {
    Setting( void ) {}
    Setting( const Value &_value, int n_coords, const index_t *_coords );
    int compare( const Setting &other ) const;
    bool operator<( const Setting &other ) const;
    bool operator==( const Setting &other ) const;
    bool operator!=( const Setting &other ) const;
    Value value;
    std::vector< index_t > coords;
  };

private:
  typedef std::vector< Setting > settings_t;
public:
  typedef BeginEndIterator< Iterator< Setting >, settings_t::const_iterator > SettingsIterator;

  void set( const Setting &setting );

  CountPtr< SettingsIterator > getSettings( void ) const;

protected:
  void setValue( const Value &accepted_value, int n_coords, const index_t *coords );
private:
  friend class ::utest_Node;
  settings_t m_settings;
  const String m_identifier;
};

class Node : public Frame
{
public:
  typedef Frame Base;
  EXCEPTION_BASE( Exception );

  EXCEPTION_DERIVED( ExitRequest   , Exception );
  EXCEPTION_DERIVED( NotFound      , Exception );
  EXCEPTION_DERIVED( InputNotFound , NotFound  );
  EXCEPTION_DERIVED( OutputNotFound, NotFound  );
  EXCEPTION_DERIVED( ParamNotFound , NotFound  );

  class NotConnected;
  class NotReady;
  class IncompatibleOutput;
  class GetAsFailed;

  class NotConnected : public Exception
  {
    typedef Exception Base;
  public:
    explicit NotConnected( const Input *_input );
    EXCEPTION_BODY( NotConnected )
    CountPtr< const Input > input;
  };

  class NotReady : public Exception
  {
    typedef Exception Base;
  public:
    explicit NotReady( const Input *_input );
    EXCEPTION_BODY( NotReady)
    CountPtr< const Input > input;
  };

  class IncompatibleOutput : public Exception
  {
    typedef Exception Base;
  public:
    explicit IncompatibleOutput( const Input *_input );
    EXCEPTION_BODY( IncompatibleOutput )
    CountPtr< const Input > input;
  };

  class GetAsFailed : public Exception
  {
    typedef Exception Base;
  public:
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
  const SharedObject *getSO( void ) const;

  virtual bool tick( CountPtr< JobQueue > main_thread );
  virtual bool tick( void );

  virtual void gc_clear( void );
  virtual void gc_getChildren( Children &children ) const;

  Input *getInput( index_t i ) const;
  Input *getInput( int     i ) const;
  Input *getInput( const char *identifier, index_t *index=0 ) const;
  Output *getOutput( index_t i ) const;
  Output *getOutput( int     i ) const;
  Output *getOutput( const char *identifier, index_t *index=0 ) const;
  Param *getParam( index_t i ) const;
  Param *getParam( int     i ) const;
  Param *getParam( const char *identifier, index_t *index=0 ) const;

  Param *setParam( index_t i, CountPtr< Param > param );

  void setNumInputs( index_t n );
  void setNumOutputs( index_t n );
  void setNumParams( index_t n );

  index_t getNumInputs( void ) const;
  index_t getNumOutputs( void ) const;
  index_t getNumParams( void ) const;

  void clearHasChanged( void ) const;
  bool hasAnyInputChanged( void ) const;
  void setAllOutputChanged( bool changed = true );

  static CountPtr< const ArrayBase > resolve( const CountPtr< const ArrayBase > &in_base );
  static CountPtr< const ArrayBase > resolve( const ArrayBase *in_base );
  static CountPtr< ArrayBase > resolve( const CountPtr< ArrayBase > &in_base );
  static CountPtr< ArrayBase > resolve( ArrayBase *in_base );
protected:
  template< class ParentNode >
  class NodeParam : public Param
  {
  public:
    typedef Param Base;
    typedef void (ParentNode::*callback_t)( const Value &value, index_t, int, const index_t* );

    explicit NodeParam(
        GarbageCollector *_gc
      , ParentNode *parent
      , const String &identifier
      , callback_t callback
      , index_t index
      , bool is_array
      )
    : Param( _gc, identifier )
    , m_parent( parent )
    , m_callback( callback )
    , m_index( index )
    , m_is_array( is_array )
    {}
    virtual ~NodeParam( void ) {}

    virtual void gc_clear( void ) { Param::gc_clear(); m_parent.clear(); }
    virtual void gc_getChildren( Children &children ) const
    {
      Param::gc_getChildren( children );
      children.add( static_cast< Collectable* >( m_parent.get() ) );
    }

    virtual void set( const Value &value, int n_coords=0, const index_t *coords=0 )
    {
      if( !m_is_array && ( n_coords > 0 || coords != 0 ) )
      {
        throw Exception() << "Param '" << getIdentifier() << "' is not a Param Array";
      }
      (m_parent->*m_callback)( value, m_index, n_coords, coords );
      setValue( value, n_coords, coords );
    }

    virtual Node *getParent( void ) const
    {
      return m_parent.get();
    }

  private:
    CountPtr< ParentNode > m_parent;
    callback_t m_callback;
    index_t m_index;
    bool m_is_array;
  };

private:
  friend class ::utest_Node;
  typedef Array< CountPtr< Input  >, 1 > inputs_t;
  typedef Array< CountPtr< Output >, 1 > outputs_t;
  typedef Array< CountPtr< Param  >, 1 > params_t;
  CountPtr< inputs_t  > m_inputs;
  CountPtr< outputs_t > m_outputs;
  CountPtr< params_t  > m_params;
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
    if( size ) out->resize_v( DIMS, size );
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

  out->resize_v( dims, size );
}

} // namespace RPGML

#define RPGML_CREATE_FUNCTION( NAME, EXTRA_NAMESPACES ) \
  extern "C"\
  RPGML::CountPtr< RPGML::Function > NAME ## _create_Function( RPGML::GarbageCollector *gc, RPGML::Frame *parent, const RPGML::SharedObject *so )\
  {\
    return new RPGML:: EXTRA_NAMESPACES Function_ ## NAME( gc, parent, so );\
  }\

#define RPGML_CREATE_NODE( NAME, EXTRA_NAMESPACES ) \
  extern "C"\
  RPGML::CountPtr< RPGML::Node > NAME ## _create_Node( RPGML::GarbageCollector *gc, const RPGML::String &identifier, const RPGML::SharedObject *so )\
  {\
    return new RPGML:: EXTRA_NAMESPACES NAME( gc, identifier, so );\
  }\

#endif


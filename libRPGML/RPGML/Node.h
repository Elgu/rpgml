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
  getOutput( OUTPUT_ENUM )->init< type >( String::Static( identifier ), dims ); \
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

#define GET_INPUT_BASE( INPUT_ENUM, var_identifier ) \
  const ArrayBase *var_identifier = 0; \
  { \
    const Input *const input = getInput( INPUT_ENUM ); \
    if( !input->isConnected() ) throw NotConnected( input ); \
    var_identifier = input->getData(); \
    if( !var_identifier ) throw NotReady( getInput( INPUT_ENUM ) ); \
  } \

#define GET_INPUT_BASE_IF_CONNECTED( INPUT_ENUM, var_identifier ) \
  const ArrayBase *var_identifier = 0; \
  { \
    const Input *const input = getInput( INPUT_ENUM ); \
    if( input->isConnected() ) \
    { \
      var_identifier = input->getData(); \
      if( !var_identifier ) throw NotReady( getInput( INPUT_ENUM ) ); \
    } \
  } \

#define GET_INPUT_AS( INPUT_ENUM, var_identifier, type ) \
  const Array< type > *var_identifier = 0; \
  { \
    const Input *const input = getInput( INPUT_ENUM ); \
    if( !input->isConnected() ) throw NotConnected( input ); \
    if( !input->getAs( var_identifier ) ) \
    { \
      throw GetAsFailed( input, var_identifier ); \
    } \
  } \

#define GET_INPUT_AS_IF_CONNECTED( INPUT_ENUM, var_identifier, type ) \
  const Array< type > *var_identifier = 0; \
  { \
    const Input *const input = getInput( INPUT_ENUM ); \
    if( input->isConnected() && !input->getAs( var_identifier ) ) \
    { \
      throw GetAsFailed( input, var_identifier ); \
    } \
  } \

#define GET_INPUT_AS_DIMS( INPUT_ENUM, var_identifier, type, dims ) \
  const Array< type > *var_identifier = 0; \
  { \
    const Input *const input = getInput( INPUT_ENUM ); \
    if( !input->isConnected() ) throw NotConnected( input ); \
    if( !input->getAs( var_identifier ) ) \
    { \
      throw GetAsFailed( input, var_identifier ); \
    } \
    if( var_identifier->getDims() != dims ) \
    { \
      throw IncompatibleOutput( input ) \
        << ": Expected " << dims << " dimensions, has " << var_identifier->getDims() \
        ; \
    } \
  } \

#define GET_INPUT_AS_DIMS_IF_CONNECTED( INPUT_ENUM, var_identifier, type, dims ) \
  const Array< type > *var_identifier = 0; \
  { \
    const Input *const input = getInput( INPUT_ENUM ); \
    if( input->isConnected() ) \
    { \
      if( !input->getAs( var_identifier ) ) \
      { \
        throw GetAsFailed( input, var_identifier ); \
      } \
      if( var_identifier->getDims() != dims ) \
      { \
        throw IncompatibleOutput( input ) \
          << ": Expected " << dims << " dimensions, has " << var_identifier->getDims() \
          ; \
      } \
    } \
  } \

#define GET_OUTPUT_AS( OUTPUT_ENUM, var_identifier, type ) \
  Array< type > *var_identifier = 0; \
  { \
    Output *const output = getOutput( OUTPUT_ENUM ); \
    if( !output->getAs( var_identifier ) ) \
    { \
      throw GetAsFailed( output, var_identifier ); \
    } \
  } \

#define GET_OUTPUT_AS_IF_CONNECTED( OUTPUT_ENUM, var_identifier, type ) \
  Array< type > *var_identifier = 0; \
  { \
    Output *const output = getOutput( OUTPUT_ENUM ); \
    if( output->isConnected() && !output->getAs( var_identifier ) ) \
    { \
      throw GetAsFailed( output, var_identifier ); \
    } \
  } \

#define GET_OUTPUT_INIT( OUTPUT_ENUM, var_identifier, type, dims, size ) \
  Array< type > *var_identifier = 0; \
  { \
    Output *const output = getOutput( OUTPUT_ENUM ); \
    output->initData< type >( dims, size ); \
    if( !output->getAs( var_identifier ) ) \
    { \
      throw GetAsFailed( output, var_identifier ); \
    } \
  } \

#define GET_OUTPUT_INIT_IF_CONNECTED( OUTPUT_ENUM, var_identifier, type, dims, size ) \
  Array< type > *var_identifier = 0; \
  { \
    Output *const output = getOutput( OUTPUT_ENUM ); \
    output->initData< type >( dims, size ); \
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

EXCEPTION_DERIVED( ExitRequest   , Exception );

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
  explicit NotReady( const Port *_port );
  EXCEPTION_BODY( NotReady)
  CountPtr< const Port > port;
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
  template< class Element >
  explicit GetAsFailed( const Input *input, const Array< Element > *, int _dims = -1 );

  template< class Element >
  explicit GetAsFailed( const Output *output, const Array< Element > *, int _dims = -1 );

  EXCEPTION_BODY( GetAsFailed )
  CountPtr< const Port > port;
  int dims;

private:
  template< class Element >
  void _set( const char *t );
};

class Port : public Collectable
{
  typedef Collectable Base;
public:
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
  typedef Port Base;
public:
  Input( GarbageCollector *gc, Node *parent );
  virtual ~Input( void );

  void init( const String &identifier );

  virtual void gc_clear( void );
  virtual void gc_getChildren( Children &children ) const;

  void disconnect( void );
  void connect( Output *output );

  bool isReady( void ) const;
  bool isConnected( void ) const;
  bool hasChanged( void ) const;

  const Output *getOutput( void ) const;
  const ArrayBase *getData( void ) const;

  template< class DataType >
  const DataType *getAs( const DataType* &as ) const
  {
    if( !isConnected() ) throw NotConnected( this );
    const ArrayBase *const data = getData();
    if( !data ) throw NotReady( this );
    return data->getAs( as );
  }

private:
  friend class ::utest_Node;
  CountPtr< Output > m_output;
};

class Output : public Port
{
  typedef Port Base;
public:
  Output( GarbageCollector *gc, Node *parent );
  virtual ~Output( void );

  template< class Element >
  void init( const String &identifier, int dims, const index_t *size = 0 );

  void disconnect( void );
  void disconnect( Input *input );

  void connect( Input *input );

  bool isReady( void ) const;
  bool isConnected( void ) const;
  bool hasChanged( void ) const;

  void setChanged( bool changed=true );

  void setData( CountPtr< ArrayBase > data );
  ArrayBase *getData( void );
  const ArrayBase *getData( void ) const;

  template< class Element >
  void initData( int dims, const index_t *size );

  template< class Element >
  void initData( const ArrayBase::Size &size ) { initData< Element >( size.getDims(), size.getCoords() ); }

  template< class DataType >
  DataType *getAs( DataType* &as )
  {
    if( !isReady() ) throw NotReady( this );
    return m_data->getAs( as );
  }

  template< class DataType >
  const DataType *getAs( const DataType* &as ) const
  {
    if( !isReady() ) throw NotReady( this );
    return m_data->getAs( as );
  }

  void resolve( void );

  virtual void gc_clear( void );
  virtual void gc_getChildren( Children &children ) const;

  typedef Array< CountPtr< Input > > inputs_t;

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
  typedef Collectable Base;
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
  typedef Frame Base;
public:
  EXCEPTION_BASE( Exception );

  EXCEPTION_DERIVED( NotFound      , Exception );
  EXCEPTION_DERIVED( InputNotFound , NotFound  );
  EXCEPTION_DERIVED( OutputNotFound, NotFound  );
  EXCEPTION_DERIVED( ParamNotFound , NotFound  );

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

  bool     getBool  ( int input_index ) const;
  uint8_t  getUInt8 ( int input_index ) const;
  int8_t   getInt8  ( int input_index ) const;
  uint16_t getUInt16( int input_index ) const;
  int16_t  getInt16 ( int input_index ) const;
  uint32_t getUInt32( int input_index ) const;
  int32_t  getInt32 ( int input_index ) const;
  uint64_t getUInt64( int input_index ) const;
  int64_t  getInt64 ( int input_index ) const;
  float    getFloat ( int input_index ) const;
  double   getDouble( int input_index ) const;
  String   getString( int input_index ) const;

  template< class Scalar >
  Scalar getScalar( int input_index ) const;

private:
  friend class ::utest_Node;
  typedef Array< CountPtr< Input  > > inputs_t;
  typedef Array< CountPtr< Output > > outputs_t;
  typedef Array< CountPtr< Param  > > params_t;
  CountPtr< inputs_t  > m_inputs;
  CountPtr< outputs_t > m_outputs;
  CountPtr< params_t  > m_params;
  CountPtr< const SharedObject > m_so;
};

class Identity : public Node
{
  typedef Node Base;
public:
  Identity( GarbageCollector *gc, const String &identifier );
  virtual ~Identity( void );
  virtual const char *getName( void ) const;
  virtual bool tick( CountPtr< JobQueue > );
};

class InOut : public Collectable
{
  typedef Collectable Base;
public:
  InOut( GarbageCollector *_gc, const String &identifier );
  virtual ~InOut( void );

  virtual void gc_clear( void );
  virtual void gc_getChildren( Children &children ) const;

  const String &getIdentifier( void ) const;
  Input *getInput( void ) const;
  Output *getOutput( void ) const;

  void disconnect( void );
  void connect( Output *output );
  void connect( Input *input );

private:
  CountPtr< Identity > m_id;
};

typedef CountPtr< Function > (*create_Function_t)( GarbageCollector *gc, Frame *parent, const SharedObject *so );
typedef CountPtr< Node > (*create_Node_t)( GarbageCollector *gc, const String &identifier, const SharedObject *so );

template< class Element >
GetAsFailed::GetAsFailed( const Input *input, const Array< Element > *, int _dims )
: port( input )
, dims( _dims )
{
  const char *const t = ( input->getData() ? input->getData()->getTypeName() : "not connected or not ready" );
  _set< Element >( t );
}

template< class Element >
GetAsFailed::GetAsFailed( const Output *output, const Array< Element > *, int _dims )
: port( output )
, dims( _dims )
{
  const char *const t = ( output->getData() ? output->getData()->getTypeName() : "not ready" );
  _set< Element >( t );
}

template< class Element >
void GetAsFailed::_set( const char *t )
{
  (*this)
    << "Could not get '" << port->getIdentifier() << "'"
    << " as 'Array< " << getTypeName< Element >() << " >"
    << ", is 'Array< " << t << " >"
    ;
  if( dims >= 0 ) (*this) << "[ " << dims << " ]";
  (*this) << "'";
}

template< class Element >
void Output::init( const String &identifier, int dims, const index_t *size )
{
  setIdentifier( identifier );

  CountPtr< ArrayBase > out = getData();

  if(
       out.isNull()
    || TypeOf< Element >::E == Type::OTHER
    || out->getType() != TypeOf< Element >::E
    || out->getDims() != dims
    )
  {
    setData( new Array< Element >( getGC(), dims, size ) );
  }
  else
  {
    if( size ) out->resize_v( dims, size );
  }
}

template< class Element >
void Output::initData( int dims, const index_t *size )
{
  CountPtr< ArrayBase > out = getData();

  if(
       out.isNull()
    || TypeOf< Element >::E == Type::OTHER
    || out->getType() != TypeOf< Element >::E
    || out->getDims() != dims
    )
  {
    out.reset( new Array< Element >( getGC(), dims, size ) );
    setData( out );
  }

  out->resize_v( dims, size );
}

template< class Scalar >
Scalar Node::getScalar( int input_index ) const
{
  GET_INPUT_BASE( input_index, base );
  if( base->getDims() != 0 )
  {
    throw IncompatibleOutput( getInput( input_index ) )
      << ": Expected 0 dimensions, has " << base->getDims()
      ;
  }

  try
  {
    return base->getValue().save_cast( TypeOf< Scalar >::E );
  }
  catch( const Value::CastFailed &e )
  {
    throw IncompatibleOutput( getInput( input_index ) )
      << e.what()
      ;
  }
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


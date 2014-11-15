#ifndef RPGML_Node_h
#define RPGML_Node_h

#include "GarbageCollector.h"
#include "Frame.h"
#include "Data.h"
#include "Array.h"

#include <vector>

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

  virtual void gc_clear( void );
  virtual void gc_getChildren( Children &children ) const;

  void disconnect( void );
  void connect( Output *output );

  bool isConnected( void ) const;

  const Output *getOutput( void ) const;
  const Data *getData( void ) const;

private:
  CountPtr< Output > m_output;
};

class Output : public Port
{
public:
  Output( GarbageCollector *gc, Node *parent );
  virtual ~Output( void );

  void disconnect( void );
  void disconnect( Input *input );

  void connect( Input *input );

  bool isConnected( void ) const;

  void setData( Data *data );
  Data *getData( void );
  const Data *getData( void ) const;

  virtual void gc_clear( void );
  virtual void gc_getChildren( Children &children ) const;

private:
  typedef Array< CountPtr< Input >, 1 > inputs_t;
  inputs_t m_inputs;
  CountPtr< Data > m_data;
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

  virtual bool set( const Value &value ) = 0;
  virtual Node *getParent( void ) const = 0;
  const String &getIdentifier( void ) const { return m_identifier; }
private:
  const String m_identifier;
};

class Node : public Frame
{
public:
  Node( GarbageCollector *gc, const String &identifier, const SharedObject *so );

  virtual ~Node( void );

  virtual const char *getName( void ) const = 0;
  const String &getIdentifier( void ) const;
  const SharedObject *getSO( void ) const;

  virtual bool tick( void );

  virtual void gc_clear( void );
  virtual void gc_getChildren( Children &children ) const;

  Input *getInput( index_t i ) const;
  Input *getInput( const char *identifier ) const;
  Output *getOutput( index_t i ) const;
  Output *getOutput( const char *identifier ) const;

  void setNumInputs( index_t n );
  void setNumOutputs( index_t n );

protected:
  template< class ParentNode >
  class NodeParam : public Param
  {
  public:
    typedef bool (ParentNode::*callback_t)( const Value &value, index_t );

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

    virtual bool set( const Value &value )
    {
      if( !(m_parent->*m_callback)( value, m_index ) ) return false;
      return true;
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
  inputs_t  m_inputs;
  outputs_t m_outputs;
  const String m_identifier;
  CountPtr< const SharedObject > m_so;
};

} // namespace RPGML

#define RPGML_CREATE_FUNCTION( NAME ) \
  extern "C"\
  RPGML::CountPtr< RPGML::Function > NAME ## _create_Function( RPGML::GarbageCollector *gc, RPGML::Frame *parent, const RPGML::SharedObject *so )\
  {\
    return new RPGML::NAME( gc, parent, so );\
  }\

#define RPGML_CREATE_NODE( NAME ) \
  extern "C"\
  RPGML::CountPtr< RPGML::Node > NAME ## _create_Node( RPGML::GarbageCollector *gc, const RPGML::String &identifier, const RPGML::SharedObject *so )\
  {\
    return new RPGML::NAME::Node( gc, identifier, so );\
  }\

#endif


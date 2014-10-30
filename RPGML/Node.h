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
  Data *getData( void );
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

class Node : public Frame
{
public:
  Node( GarbageCollector *gc, const String &name, const String &identifier, const SharedObject *so );

  virtual ~Node( void );

  const String &getName( void ) const;
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

private:
  typedef Array< CountPtr< Input >, 1 > inputs_t;
  typedef Array< CountPtr< Output >, 1 > outputs_t;
  inputs_t m_inputs;
  outputs_t m_outputs;
  const String m_name;
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


#ifndef RPGML_Node_h
#define RPGML_Node_h

#include "GarbageCollector.h"
#include "Frame.h"

#include <vector>

namespace RPGML {

class Port;
class Input;
class Output;
class Node;

class Port : public Collectable
{
public:
  Port( GarbageCollector *gc, Node *parent, const String &identifier );
  virtual ~Port( void );

  Node *getParent( void ) const;
  const String &getIdentifier( void ) const;

  virtual void gc_clear( void );
  virtual void gc_getChildren( Children &children ) const;

private:
  CountPtr< Node > m_parent;
  String m_identifier;
};

class Input : public Port
{
public:
  Input( GarbageCollector *gc, Node *parent, const String &identifier );
  virtual ~Input( void );

  virtual void gc_clear( void );
  virtual void gc_getChildren( Children &children ) const;

  void disconnect( void );
  void connect( Output *output );

  bool isConnected( void ) const;

private:
  CountPtr< Output > m_output;
};

class Output : public Port
{
public:
  Output( GarbageCollector *gc, Node *parent, const String &identifier );
  virtual ~Output( void );

  void disconnect( void );
  void disconnect( Input *input );

  void connect( Input *input );

  bool isConnected( void ) const;

  virtual void gc_clear( void );
  virtual void gc_getChildren( Children &children ) const;

private:
  typedef std::vector< CountPtr< Input > > inputs_t;
  inputs_t m_inputs;
};

class Node : public Frame
{
public:
  Node( GarbageCollector *gc, const String &global_name, const SharedObject *so );

  virtual ~Node( void );

  const String &getGlobalName( void ) const;
  const SharedObject *getSO( void ) const;

  virtual void gc_clear( void );
  virtual void gc_getChildren( Children &children ) const;

  virtual void execute( void );

private:
  const String m_global_name;
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
  RPGML::CountPtr< RPGML::Node > NAME ## _create_Node( RPGML::GarbageCollector *gc, const RPGML::String &global_name, const RPGML::SharedObject *so )\
  {\
    return new RPGML::NAME::Node( gc, global_name, so );\
  }\

#endif


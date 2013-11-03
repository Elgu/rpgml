#ifndef RPGML_Node_h
#define RPGML_Node_h

#include "GarbageCollector.h"
#include "Map.h"

#include <vector>

namespace RPGML {

class Port;
class Input;
class Output;
class Param;
class Node;

class Port : public Collectable
{
public:
  Port( GarbageCollector *gc, Node *parent, const String *identifier );
  virtual ~Port( void );

  Node *getParent( void ) const;

  virtual void gc_clear( void );
  virtual CountPtr< Children > getChildren( void ) const;

private:
  CountPtr< Node > m_parent;
  CountPtr< const String > m_identifier;
};

class Input : public Port
{
public:
  Input( GarbageCollector *gc, Node *parent, const String *identifier );
  virtual ~Input( void );

  virtual void gc_clear( void );
  virtual CountPtr< Children > getChildren( void ) const;

  void disconnect( void );
  void connect( Output *output );

  bool isConnected( void ) const;

private:
  CountPtr< Output > m_output;
};

class Output : public Port
{
public:
  Output( GarbageCollector *gc, Node *parent, const String *identifier );
  virtual ~Output( void );

  void disconnect( void );
  void disconnect( Input *input );

  void connect( Input *input );

  bool isConnected( void ) const;

  virtual void gc_clear( void );
  virtual CountPtr< Children > getChildren( void ) const;

private:
  typedef std::vector< CountPtr< Input > > inputs_t;
  inputs_t m_inputs;
};

class Param : public Collectable
{
public:
  Param( GarbageCollector *gc, Node *parent, const String *identifier );
  virtual ~Param( void );

  bool set( const Value &value ) const;
  Node *getParent( void ) const;
  const String *getIdentifier( void ) const;

  virtual void gc_clear( void );
  virtual CountPtr< Children > getChildren( void ) const;

private:
  CountPtr< Node > m_parent;
  CountPtr< const String > m_identifier;
  std::vector< Value > m_values;
};

class Node : public Map
{
public:
  Node( GarbageCollector *gc );
  virtual ~Node( void );

  virtual void gc_clear( void );
  virtual CountPtr< Children > getChildren( void ) const;

private:
  std::vector< CountPtr< Input > > m_inputs;
  std::vector< CountPtr< Output > > m_outputs;
  std::vector< CountPtr< Port > > m_ports;
};

} // namespace RPGML

#endif


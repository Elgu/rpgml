#ifndef RPGML_Node_h
#define RPGML_Node_h

#include "GarbageCollector.h"

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
  Port( GarbageCollector *gc );
  virtual ~Port( void );

private:
  CountPtr< Node > m_node;
  CountPtr< String > m_identifier;
};

class Input : public Port
{
public:
  Input( GarbageCollector *gc );
  virtual ~Input( void );

  virtual void gc_clear( void );
  virtual CountPtr< Children > getChildren( void ) const;

private:
  std::vector< CountPtr< Output > > m_outputs;
};

class Output : public Port
{
public:
  Output( GarbageCollector *gc );
  virtual ~Output( void );

  virtual void gc_clear( void );
  virtual CountPtr< Children > getChildren( void ) const;

private:
  CountPtr< Input > m_input;
};

class Param : public Collectable
{
public:
  Param( GarbageCollector *gc );
  virtual ~Param( void );

  virtual void gc_clear( void );
  virtual CountPtr< Children > getChildren( void ) const;

private:
  CountPtr< Node > m_node;
  CountPtr< String > m_identifier;
  std::vector< Value > m_values;
};

class Node : public Collectable
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


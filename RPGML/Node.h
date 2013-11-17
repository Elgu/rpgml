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
  const String *getIdentifier( void ) const;

  virtual void gc_clear( void );
  virtual void getChildren( Children &children ) const;

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
  virtual void getChildren( Children &children ) const;

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
  virtual void getChildren( Children &children ) const;

private:
  typedef std::vector< CountPtr< Input > > inputs_t;
  inputs_t m_inputs;
};

class Param : public Collectable
{
public:
  Param( GarbageCollector *gc, Node *parent, const String *identifier, Type type = Type::Invalid() );
  virtual ~Param( void );

  Node *getParent( void ) const;

  virtual bool set( const Value &value );
  const Value &get( void ) const;

  virtual void gc_clear( void );
  virtual void getChildren( Children &children ) const;

private:
  CountPtr< Node > m_parent;
  CountPtr< const String > m_identifier;
  Value m_value;
  Type m_type;
};

class Node : public Map
{
public:
  explicit
  Node( GarbageCollector *gc, Map *parent=0 );

  virtual ~Node( void );

  virtual void gc_clear( void );
  virtual void getChildren( Children &children ) const;

protected:
  friend class Param;
  bool setParamRecord( const Param *param );

private:
  struct SetParameter
  {
    explicit
    SetParameter( const Param *_param, const Value &_value )
    : param( _param )
    , value( _value )
    {}

    CountPtr< const Param > param;
    Value value;
  };

  std::vector< SetParameter > m_setParameters;
};

} // namespace RPGML

#endif


#ifndef RPGML_Scope_h
#define RPGML_Scope_h

#include "Refcounted.h"

#include <string>

namespace RPGML {

class Context;
class Map;
class Value;
class String;

class Scope : public Refcounted
{
public:
  explicit
  Scope( Context *context );

  virtual ~Scope( void );

  Map *getCurr( void ) const { return m_curr; }

  Context *getContext( void ) const { return m_context; }
  GarbageCollector *getGC( void ) const;

  Value *lookup( const String *identifier ) const;
  Value *lookup( const char *identifier ) const;
  Value *lookup( const std::string &identifier ) const;

  Value *create( const String *identifier     , const Value &value ) const;
  Value *create( const char *identifier       , const Value &value ) const;
  Value *create( const std::string &identifier, const Value &value ) const;

  class EnterLeaveGuard;

  CountPtr< EnterLeaveGuard > enter( const String *identifier );
  CountPtr< EnterLeaveGuard > enter( const char *identifier );
  CountPtr< EnterLeaveGuard > enter( const std::string &identifier );

  Value *create_unified( const String *unified_identifier, const Value &value ) const;

  const String *unify( const String *identifier ) const;
  const String *unify( const char *identifier ) const;
  const String *unify( const std::string &identifier ) const;

  class EnterLeaveGuard : public Refcounted
  {
  public:
    EnterLeaveGuard( Scope *scope, Map *new_curr )
    : m_scope( scope )
    {
      if( new_curr )
      {
        m_old_curr = m_scope->getCurr();
        m_scope->setCurr( new_curr );
      }
    }
    virtual ~EnterLeaveGuard( void )
    {
      if( m_old_curr ) m_scope->setCurr( m_old_curr );
    }
  private:
    CountPtr< Scope > m_scope;
    CountPtr< Map > m_old_curr;
  };

private:
  friend class EnterLeaveGuard;

  Scope &setCurr( Map *curr );

  CountPtr< Context > m_context;
  CountPtr< Map > m_curr;
};

} // namespace RPGML

#endif

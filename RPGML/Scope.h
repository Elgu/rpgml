#ifndef RPGML_Scope_h
#define RPGML_Scope_h

#include "Refcounted.h"

#include <string>

namespace RPGML {

class Context;
class Frame;
class Value;
class String;
class StringData;
class StringUnifier;

class Scope : public Refcounted
{
public:
  explicit
  Scope( Context *context );

  virtual ~Scope( void );

  Frame *getCurr( void ) const { return m_curr; }
  Frame *getRoot( void ) const;

  Context *getContext( void ) const { return m_context; }
  GarbageCollector *getGC( void ) const;
  StringUnifier *getUnifier( void ) const;

  Value *lookup( const String &identifier ) const;
  Value *lookup( const char *identifier ) const;
  Value *lookup( const std::string &identifier ) const;

  Value *create( const String &identifier     , const Value &value ) const;
  Value *create( const char *identifier       , const Value &value ) const;
  Value *create( const std::string &identifier, const Value &value ) const;

  class EnterLeaveGuard;

  CountPtr< EnterLeaveGuard > enter( const String &identifier );
  CountPtr< EnterLeaveGuard > enter( const char *identifier );
  CountPtr< EnterLeaveGuard > enter( const std::string &identifier );

  Value *create_unified( const String &unified_identifier, const Value &value ) const;

  const StringData *unify( const String &identifier ) const;
  const StringData *unify( const char *identifier ) const;
  const StringData *unify( const std::string &identifier ) const;

  CountPtr< Frame > new_Frame( void ) const;

  String genGlobalName( const String &identifier ) const;

  size_t getNr( void ) const;

  class EnterLeaveGuard : public Refcounted
  {
  public:
    EnterLeaveGuard( Scope *scope, Frame *new_curr )
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
    CountPtr< Frame > m_old_curr;
  };

private:
  friend class EnterLeaveGuard;

  Scope &setCurr( Frame *curr );

  CountPtr< Context > m_context;
  CountPtr< Frame > m_curr;
};

} // namespace RPGML

#endif

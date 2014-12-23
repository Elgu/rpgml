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
#ifndef RPGML_Scope_h
#define RPGML_Scope_h

#include "Refcounted.h"
#include "Function.h"

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

  bool call(
      const Location *loc
    , index_t recursion_depth
    , const String &function_identifier
    , Value &ret
    , const Function::Args *call_args
    );

  bool call(
      const Location *loc
    , index_t recursion_depth
    , const String &function_identifier
    , Value &ret
    , const Value &arg0 = Value()
    , const Value &arg1 = Value()
    , const Value &arg2 = Value()
    , const Value &arg3 = Value()
    );

  CountPtr< Node > create_Node(
      const Location *loc
    , index_t recursion_depth
    , const String &node_name
    , const Value &arg0 = Value()
    , const Value &arg1 = Value()
    , const Value &arg2 = Value()
    , const Value &arg3 = Value()
    );

  CountPtr< Output > toOutput(
      const Location *loc
    , index_t recursion_depth
    , const Value &x
    );

  CountPtr< Frame > new_Frame( void ) const;

  //! like 'foo.bar.identifier'
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

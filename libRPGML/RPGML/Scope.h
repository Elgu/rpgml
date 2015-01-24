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

#include "GarbageCollector.h"
#include "Function.h"
#include "Frame.h"
#include "Exception.h"

#include <string>

namespace RPGML {

class Context;
class Frame;
class Value;
class String;
class StringData;
class StringUnifier;

class Scope : public Collectable
{
public:
  EXCEPTION_BASE( Exception );

  explicit
  Scope( GarbageCollector *_gc, Context *context );

  explicit
  Scope( GarbageCollector *_gc, const Scope *other, Frame *curr=0 );

  virtual ~Scope( void );

  virtual void gc_clear( void );
  virtual void gc_getChildren( Children &children ) const;

  Frame *getCurr( void ) const { return m_curr; }
  Frame *getRoot( void ) const;

  Context *getContext( void ) const { return m_context; }
  GarbageCollector *getGC( void ) const;
  StringUnifier *getUnifier( void ) const;

  Frame::Ref lookup( const String &identifier ) const;
  Frame::Ref lookup( const char *identifier ) const;
  Frame::Ref lookup( const std::string &identifier ) const;

  Frame::Ref create( const String &identifier     , const Value &value ) const;
  Frame::Ref create( const char *identifier       , const Value &value ) const;
  Frame::Ref create( const std::string &identifier, const Value &value ) const;

  class EnterLeaveGuard;

  CountPtr< EnterLeaveGuard > enter( const String &identifier );
  CountPtr< EnterLeaveGuard > enter( const char *identifier );
  CountPtr< EnterLeaveGuard > enter( const std::string &identifier );

  Frame::Ref create_unified( const String &unified_identifier, const Value &value ) const;

  const StringData *unify( const String &identifier ) const;
  const StringData *unify( const char *identifier ) const;
  const StringData *unify( const std::string &identifier ) const;

  Value call(
      const Location *loc
    , index_t recursion_depth
    , const String &function_identifier
    , const Function::Args *call_args
    );

  Value call(
      const Location *loc
    , index_t recursion_depth
    , const String &function_identifier
    , const Value &arg0 = Value()
    , const Value &arg1 = Value()
    , const Value &arg2 = Value()
    , const Value &arg3 = Value()
    , const Value &arg4 = Value()
    );

  Value call(
      const Location *loc
    , index_t recursion_depth
    , const String &function_identifier
    , index_t n_args
    , const Value *args
    );

  CountPtr< Node > createNode(
      const Location *loc
    , index_t recursion_depth
    , const String &node_name
    );

  CountPtr< Output > toOutput(
      const Location *loc
    , index_t recursion_depth
    , const Value &x
    );

  CountPtr< Frame > newFrame( void ) const;

  //! like 'foo.bar.identifier'
  String genGlobalName( const String &identifier ) const;
  String genGlobalName( const String &node_name, const Location *loc ) const;

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

  Scope( const Scope & );
  Scope &operator=( const Scope & );

  Scope &setCurr( Frame *curr );

  CountPtr< Context > m_context;
  CountPtr< Frame > m_curr;
};

} // namespace RPGML

#endif

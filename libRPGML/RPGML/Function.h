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
#ifndef RPGML_Function_h
#define RPGML_Function_h

#include "GarbageCollector.h"
#include "Refcounted.h"
#include "Value.h"
#include "String.h"
#include "Location.h"

namespace RPGML {

class Scope;
class Frame;
class String;
class SharedObject;

class Function : public Collectable
{
public:
  class Arg;
  class Args;

  Function( GarbageCollector *gc, Frame *parent, const Args *decl_args, const SharedObject *so=0 );
  virtual ~Function( void );

  Frame *getParent( void ) const;
  const Args *getDecl( void ) const;
  const SharedObject *getSO( void ) const;

  size_t getNumArgs( void ) const;

  //! Must at least be getNumArgs() (default impl)
  virtual size_t getFrameSize( void ) const;

  //! Sets up call and calls call_impl()
  virtual bool call( const Location *loc, index_t recursion_depth, Scope *scope, Value &ret, const Args *call_args );

  //! "Bare" Function call, use on own risk directly
  virtual bool call_impl( const Location *loc, index_t recursion_depth, Scope *scope, Value &ret, index_t n_args, const Value *args ) = 0;

  class Arg
  {
  public:
    Arg( void )
    {}

    explicit
    Arg( const String &_identifier )
    : identifier( _identifier )
    {}

    Arg( const String &_identifier, const Value &_value )
    : identifier( _identifier )
    , value( _value )
    {}

    String identifier;
    Value value;
  };

  class Args : public Refcounted, public std::vector< Arg >
  {
  public:
    Args( size_t n=0 )
    : std::vector< Arg >( n )
    {}
    ~Args( void ) {}
  };

  virtual void gc_clear( void );
  virtual void gc_getChildren( Children &children ) const;

  virtual const char *getName( void ) const = 0;

private:
  void fill_args( Frame &args, const Args &call_args );

  CountPtr< Frame > m_parent;
  CountPtr< const Args > m_decl;
  CountPtr< const SharedObject > m_so;
};

} // namespace RPGML

#endif


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
#ifndef RPGML_Map_h
#define RPGML_Map_h

#include "Value.h"
#include "String.h"
#include "GarbageCollector.h"
#include "types.h"
#include "Exception.h"
#include "Iterator.h"

#include <map>
#include <vector>

namespace RPGML {

class Scope;
class SharedObject;

class Frame : public Collectable
{
  typedef Collectable Base;
public:
  EXCEPTION_BASE( Exception );

  explicit
  Frame( GarbageCollector *gc, Frame *parent=0, const String &path = String() );
  virtual ~Frame( void );

  Frame *getParent( void ) const;
  const String &getPath( void ) const;

  Frame &setIdentifier( const String &identifier );
  const String &getIdentifier( void ) const;

  Frame &setThis( bool is_this=true );
  bool isThis( void ) const;

  index_t getDepth( void ) const;

  void reserve( index_t size );

  class Ref;
  class ConstRef;

  class Ref
  {
  public:
    Ref( void );
    explicit Ref( Frame *frame, index_t index );
    ~Ref( void );
    bool isNull( void ) const;
    Value *getValue( void ) const;
    index_t getIndex( void ) const;
    Frame *getFrame( void ) const;
    operator Value*( void ) const;
    Value &operator*( void ) const;
    Value *operator->( void ) const;
  private:
    friend class ConstRef;
    CountPtr< Frame > m_frame;
    index_t m_index;
  };

  class ConstRef
  {
  public:
    ConstRef( void );
    ConstRef( const Ref &ref );
    explicit ConstRef( const Frame *frame, index_t index );
    ~ConstRef( void );
    bool isNull( void ) const;
    const Value *getValue( void ) const;
    index_t getIndex( void ) const;
    const Frame *getFrame( void ) const;
    operator const Value*( void ) const;
    const Value &operator*( void ) const;
    const Value *operator->( void ) const;
  private:
    CountPtr< const Frame > m_frame;
    index_t m_index;
  };

  index_t getIndex( const char *identifier ) const;
  index_t getCreateIndex( const String &identifier, bool *existed_p=0 );

  index_t set( const String &identifier, const Value &value );

  Ref      getVariable( const char *identifier );
  ConstRef getVariable( const char *identifier ) const;
  Ref      getStack( index_t index );
  ConstRef getStack( index_t index ) const;

  Ref      operator[]( const char *identifier );
  ConstRef operator[]( const char *identifier ) const;
  Ref      operator[]( index_t index );
  ConstRef operator[]( index_t index ) const;

  //! like 'foo.bar.identifier'
  String genGlobalName( const String &identifier ) const;

  Ref push_back( const String &identifier, const Value &value );
  void pop_back( void );

  class PushPopGuard
  {
  public:
    PushPopGuard( Frame *frame, const String &identifier, const Value &value=Value() );
    ~PushPopGuard( void );
    Ref get( void ) const;
  private:
    Ref m_ref;
  };

  // Load from the directory associated with this Frame
  Ref load( const String &identifier, const Scope *scope );

  index_t getSize( void ) const { return index_t( m_values.size() ); }

  typedef std::vector< Value > values_t;
  typedef std::vector< String > identifiers_t;

  class Iterator : public RPGML::Iterator< std::pair< String, Value > >
  {
    friend class Frame;
  private:
    Iterator( const values_t *values, const identifiers_t *identifiers )
    : m_values( values ), m_identifiers( identifiers ), m_i( 0 )
    {
      assert( m_values );
      assert( m_identifiers );
      assert( m_values->size() == m_identifiers->size() );
    }
  public:
    typedef std::pair< String, Value > Type;
    typedef RPGML::Iterator< Type > Base;
    virtual ~Iterator( void ) {}
    virtual bool done( void ) { return ( m_i >= m_values->size() ); }
    virtual void next( void ) { ++m_i; }
    virtual Type get( void ) { return std::make_pair( (*m_identifiers)[ m_i ], (*m_values)[ m_i ] ); }
    virtual CountPtr< Base > clone( void ) const { return new Iterator( (*this) ); }
  private:
    const values_t *const m_values;
    const identifiers_t *const m_identifiers;
    index_t m_i;
  };

  CountPtr< Iterator > getIterator( void ) const
  {
    return new Iterator( &m_values, &m_identifiers );
  }

  virtual void gc_clear( void );
  virtual void gc_getChildren( Children &children ) const;

private:

  Ref load_local ( const String &identifier, const Scope *scope );
  Ref load_global( const String &identifier, const Scope *scope );
  Ref load( const String &path, const String &identifier, const Scope *scope );

  friend class Ref;
  friend class ConstRef;

  values_t m_values;
  identifiers_t m_identifiers;
  CountPtr< Frame > m_parent; // might create a closure, points to where the lookup continues
  String m_identifier; // might be null for anonymous Maps
  String m_path; // if this map corresponds to a directory
  index_t m_depth; // determined at setParent(), checked there
  bool m_is_this; // whether lookup for "this" returns this Frame
};

} // namespace RPGML

#endif


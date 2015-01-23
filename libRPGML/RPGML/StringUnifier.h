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
#ifndef RPGML_StringUnifier_h
#define RPGML_StringUnifier_h

#include "Refcounted.h"
#include "String.h"

#include <map>
#include <cstring>
#include <memory>

namespace RPGML {

class StringUnifier : public Refcounted
{
public:
  StringUnifier( void );
  virtual ~StringUnifier( void );

  void clear( void );
  void cleanup( void );

  class Unified : public StdString
  {
  private:
    friend class StringUnifier;
    Unified( StringUnifier *unifier, const char *s );
    Unified( StringUnifier *unifier, const std::string &s );
    Unified( void );
  public:
    virtual ~Unified( void );
    virtual const void *getUnifier( void ) const;
  private:
    Unified( const Unified & );
    Unified &operator=( const Unified & );
    StringUnifier *m_unifier;
  };

  template< class S >
  const Unified *unify( const S &s )
  {
    return unify_impl( getCharPtr( s ) );
  }

  const Unified *unify_move( std::string &s );

  template< class S >
  void remove( const S &s )
  {
    return remove_impl( getCharPtr( s ) );
  }

private:
  static const char *getCharPtr( const StringData *s );
  static const char *getCharPtr( const String &s );
  static const char *getCharPtr( const char *s );
  static const char *getCharPtr( const std::string &s );

  const Unified *unify_impl( const char *s );
  const Unified *get_impl( const char *s );
  void remove_impl( const char *s );

  template< class S >
  const Unified *get( const S &s )
  {
    return get_impl( getCharPtr( s ) );
  }

  struct compare
  {
    inline
    bool operator()( const char *x, const char *y )
    {
      return 0 > ::strcmp( x, y );
    }
  };

  typedef std::map< const char*, CountPtr< Unified >, compare > map_t;
  map_t m_map;
};

} // namespace RPGML

#endif


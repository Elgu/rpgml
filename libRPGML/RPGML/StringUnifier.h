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
  StringUnifier( void ) {}
  virtual ~StringUnifier( void ) {}

  template< class S >
  const StringData *unify( const S &s )
  {
    const char *const c_in = getCharPtr( s );
    const String *const ret1 = get( c_in );
    if( ret1 ) return ret1->getData();

    String ret2( s );
    const char *const c_out = ret2.c_str();
    m_map.insert( std::make_pair( c_out, ret2 ) );
    return ret2.getData();
  }

  const StringData *unify_move( String &s )
  {
    const char *const c_in = getCharPtr( s );
    const String *ret1 = get( c_in );
    if( ret1 ) return ret1->getData();

    const char *const c_out = s.c_str();
    m_map.insert( std::make_pair( c_out, s ) );
    return s.getData();
  }

  const StringData *unify_move( std::string &s )
  {
    const char *const c_in = getCharPtr( s );
    const String *const ret1 = get( c_in );
    if( ret1 ) return ret1->getData();

    String ret2;
    ret2.moveFrom( s );
    const char *const c_out = ret2.c_str();
    m_map.insert( std::make_pair( c_out, ret2 ) );
    return ret2.getData();
  }

  void clear( void )
  {
    m_map.clear();
  }

  void cleanup( void )
  {
    typedef std::vector< const char* > to_be_removed_t;
    to_be_removed_t to_be_removed;
    to_be_removed.reserve( 16 );

    for( map_t::const_iterator i( m_map.begin() ), end( m_map.end() ); i != end; ++i )
    {
      if( 1 == i->second.count() )
      {
        to_be_removed.push_back( i->second.c_str() );
      }
    }

    for( to_be_removed_t::const_iterator i( to_be_removed.begin() ), end( to_be_removed.end() ); i != end; ++i )
    {
      remove( (*i) );
    }
  }

  template< class S >
  void remove( const S &s )
  {
    m_map.erase( getCharPtr( s ) );
  }

private:
  static
  const char *getCharPtr( const String &s )
  {
    return s.c_str();
  }

  static
  const char *getCharPtr( const char *s )
  {
    return s;
  }

  static
  const char *getCharPtr( const std::string &s )
  {
    return s.c_str();
  }

  template< class S >
  const String *get( const S &s )
  {
    const char *const c_in = getCharPtr( s );
    map_t::const_iterator found = m_map.find( c_in );
    if( found != m_map.end() )
    {
      return &found->second;
    }
    else
    {
      return 0;
    }
  }

  struct compare
  {
    bool operator()( const char *x, const char *y )
    {
      return 0 > ::strcmp( x, y );
    }
  };

  typedef std::map< const char*, String, compare > map_t;
  map_t m_map;
};

} // namespace RPGML

#endif


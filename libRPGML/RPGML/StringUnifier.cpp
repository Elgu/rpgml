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
#include "StringUnifier.h"

#include <iostream>

// RPGML_CXXFLAGS =
// RPGML_LDFLAGS =

namespace RPGML {

StringUnifier::StringUnifier( void )
{}

StringUnifier::~StringUnifier( void )
{
  clear();
}

const StringUnifier::Unified *StringUnifier::unify_impl( const char *c_in )
{
  const Unified *const ret1 = get( c_in );
  if( ret1 ) return ret1;

  CountPtr< Unified > ret2 = new Unified( this, c_in );
  const char *const c_out = ret2->get();
  m_map.insert( std::make_pair( c_out, ret2 ) );
  return ret2;
}

const StringUnifier::Unified *StringUnifier::unify_move( std::string &s )
{
  const char *const c_in = getCharPtr( s );
  const Unified *const ret1 = get( c_in );
  if( ret1 ) return ret1;

  CountPtr< Unified > ret2 = new Unified();
  ret2->moveFrom( s );
  const char *const c_out = ret2->get();
  m_map.insert( std::make_pair( c_out, ret2 ) );
  return ret2;
}

void StringUnifier::clear( void )
{
  for( map_t::const_iterator i( m_map.begin() ), end( m_map.end() ); i != end; ++i )
  {
    Unified *const u = i->second;
    if( this == u->m_unifier )
    {
      u->m_unifier = 0;
    }
  }
  m_map.clear();
}

void StringUnifier::cleanup( void )
{
  typedef std::vector< const char* > to_be_removed_t;
  to_be_removed_t to_be_removed;
  to_be_removed.reserve( 16 );

  for( map_t::const_iterator i( m_map.begin() ), end( m_map.end() ); i != end; ++i )
  {
    if( 1 == i->second->count() )
    {
      to_be_removed.push_back( i->second->get() );
    }
  }

  for( to_be_removed_t::const_iterator i( to_be_removed.begin() ), end( to_be_removed.end() ); i != end; ++i )
  {
    remove( (*i) );
  }
}

void StringUnifier::remove_impl( const char *s )
{
  m_map.erase( s );
}

const char *StringUnifier::getCharPtr( const StringData *s )
{
  return s->get();
}

const char *StringUnifier::getCharPtr( const String &s )
{
  return s.c_str();
}

const char *StringUnifier::getCharPtr( const char *s )
{
  return s;
}

const char *StringUnifier::getCharPtr( const std::string &s )
{
  return s.c_str();
}

const StringUnifier::Unified *StringUnifier::get_impl( const char *c_in )
{
  map_t::const_iterator found = m_map.find( c_in );
  if( found != m_map.end() )
  {
    return found->second;
  }
  else
  {
    return 0;
  }
}

StringUnifier::Unified::Unified( StringUnifier *unifier, const char *s )
: StdString( s )
, m_unifier( unifier )
{
//  std::cerr << "create Unified " << (void*)this << std::endl;
}

StringUnifier::Unified::Unified( StringUnifier *unifier, const std::string &s )
: StdString( s )
, m_unifier( unifier )
{
//  std::cerr << "create Unified " << (void*)this << std::endl;
}

StringUnifier::Unified::Unified( void )
: m_unifier( 0 )
{
//  std::cerr << "create Unified " << (void*)this << std::endl;
}

StringUnifier::Unified::~Unified( void )
{
//  std::cerr << "delete Unified " << (void*)this << std::endl;
}

const void *StringUnifier::Unified::getUnifier( void ) const
{
  return m_unifier;
}

} // namespace RPGML


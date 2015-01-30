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
#ifndef RPGML_Context_h
#define RPGML_Context_h

#include "GarbageCollector.h"
#include "String.h"

#include <memory>
#include <vector>

namespace RPGML {

class Graph;
class Scope;
class Frame;
class StringUnifier;

//! Do not allocate on the stack
class Context : public Collectable
{
  typedef Collectable Base;
public:
  explicit
  Context( GarbageCollector *_gc, StringUnifier *unifier, const String &searchPath = String::Static( "." ) );

  virtual ~Context( void );

  Context &setSearchPath( const String &searchPath );
  const std::vector< String > &getSearchPaths( void ) const;

  Frame *getRoot( void ) const { return m_root; }

  CountPtr< Scope > createScope( void );

  StringUnifier *getUnifier( void ) const { return m_unifier; }

  CountPtr< Graph > createGraph( void ) const;

  size_t getNr( void );

  virtual void gc_clear( void );
  virtual void gc_getChildren( Children &children ) const;
private:
  template< class T >
  void addGlobal( const char *static_identifier, const T &value );
  CountPtr< Frame > m_root;
  CountPtr< StringUnifier > m_unifier;
  std::vector< String > m_searchPaths;
  size_t m_nr;
};

} // namespace RPGML

#endif


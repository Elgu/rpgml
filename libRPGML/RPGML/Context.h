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

#include <memory>

namespace RPGML {

class Graph;
class Scope;
class Frame;
class StringUnifier;

//! Do not allocate on the stack
class Context : public Refcounted
{
public:
  explicit
  Context( StringUnifier *unifier );

  virtual ~Context( void );

  Frame *getRoot( void ) const { return m_root; }

  CountPtr< Scope > createScope( void );

  StringUnifier *getUnifier( void ) const { return m_unifier; }
  GarbageCollector *getGC( void ) const { return m_gc.get(); }

  CountPtr< Graph > createGraph( void ) const;

  size_t getNr( void );
private:
  std::auto_ptr< GarbageCollector > m_gc;
  CountPtr< Frame > m_root;
  CountPtr< StringUnifier > m_unifier;
  size_t m_nr;
};

} // namespace RPGML

#endif


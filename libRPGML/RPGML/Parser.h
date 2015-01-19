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
#ifndef RPGML_Parser_h
#define RPGML_Parser_h

#include <RPGML/rpgml.tab.hh>
#include "Scanner.h"

namespace RPGML {

class GarbageCollector;

class Parser : public Scanner, public _Parser
{
public:
  explicit
  Parser( GarbageCollector *_gc, StringUnifier *unifier, Source *_source=0 );

  virtual ~Parser( void );

  virtual void append( CountPtr< Statement > statement ) = 0;
};

class CompoundingParser : public Parser
{
public:
  explicit
  CompoundingParser( GarbageCollector *_gc, StringUnifier *unifier, Source *_source=0 )
  : Parser( _gc, unifier, _source )
  {}

  virtual ~CompoundingParser( void )
  {}

  virtual void append( Statement *statement )
  {
    statements.push_back( statement );
  }

  std::vector< CountPtr< Statement > > statements;
};

} // namespace RPGML

#endif


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
#ifndef RPGML_InterpretingParser_h
#define RPGML_InterpretingParser_h

#include "Parser.h"
#include "StringUnifier.h"
#include "Scanner.h"
#include "Context.h"
#include "Scope.h"
#include "InterpretingASTVisitor.h"

namespace RPGML {

class InterpretingParser : public Refcounted, public Parser
{
public:
  explicit
  InterpretingParser( Scope *scope, Source *source )
  : Parser( scope->getUnifier(), source )
  , interpreter( new InterpretingASTVisitor( scope ) )
  {}

  virtual ~InterpretingParser( void )
  {}

  virtual void append( CountPtr< Statement > statement )
  {
    try
    {
      statement->invite( interpreter );
    }
    catch( const char *e )
    {
      std::cerr << e << std::endl;
    }
  }

private:
  CountPtr< InterpretingASTVisitor > interpreter;
};

} // namespace RPGML

#endif


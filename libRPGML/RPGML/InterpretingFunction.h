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
#ifndef RPGML_InterpretingFunction_h
#define RPGML_InterpretingFunction_h

#include "Function.h"
#include "AST.h"

namespace RPGML {

class Scope;

class InterpretingFunction : public Function
{
public:
  InterpretingFunction( GarbageCollector *gc, Frame *parent, const String &name, const Args *decl_args, const AST::CompoundStatement *body );
  virtual ~InterpretingFunction( void );

  virtual bool call_impl( const Location *loc, index_t recursion_depth, Scope *scope, Value &ret, index_t n_args, const Value *args );
  virtual const char *getName( void ) const { return m_name.get(); }

private:
  const CountPtr< const AST::CompoundStatement > m_body;
  const String m_name;
};


} // namespace RPGML

#endif

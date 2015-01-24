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
#include "InterpretingFunction.h"

#include "InterpretingASTVisitor.h"
#include "Frame.h"
#include "Scope.h"

namespace RPGML {

InterpretingFunction::InterpretingFunction(
    GarbageCollector *_gc
  , const Location *loc
  , Frame *parent
  , const String &name
  , const Args *decl_args
  , const AST::CompoundStatement *body
  , bool is_method
  )
: Function( _gc, loc, parent, decl_args, is_method )
, m_body( body )
, m_name( name )
{}

InterpretingFunction::~InterpretingFunction( void )
{}

Value InterpretingFunction::call_impl( const Location *loc, index_t recursion_depth, Scope *scope, index_t, const Value * )
{
  CountPtr< Frame > frame = new Frame( scope->getGC(), scope->getCurr() );
  frame->setThis( !isMethod() );

  Scope::EnterLeaveGuard guard( scope, frame );

  CountPtr< InterpretingASTVisitor > interpreter = new InterpretingASTVisitor( getGC(), scope, loc, recursion_depth+1 );
  m_body->invite( interpreter.get() );

  if( !interpreter->get_return_encountered() )
  {
    throw ParseException( loc ) << "Function '" << getName() << "': No return statement";
  }

  return interpreter->get_return_value();
}

} // namespace RPGML


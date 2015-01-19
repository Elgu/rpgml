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
#ifndef RPGML_InterpretingASTVisitor_h
#define RPGML_InterpretingASTVisitor_h

#include "AST.h"
#include "Array.h"
#include "GarbageCollector.h"

namespace RPGML {

class Scope;

class InterpretingASTVisitor : public Collectable, public AST::Visitor
{
public:
  explicit
  InterpretingASTVisitor( GarbageCollector *_gc, Scope *_scope, index_t recursion_depth=0 );

  virtual ~InterpretingASTVisitor( void );

  virtual void gc_clear( void );
  virtual void gc_getChildren( Children &children ) const;

  virtual bool visit( const AST::ConstantExpression           *node );
  virtual bool visit( const AST::ThisExpression               *node );
  virtual bool visit( const AST::ArrayConstantExpression      *node );
  virtual bool visit( const AST::FrameConstantExpression      *node );
  virtual bool visit( const AST::ParenthisSequenceExpression  *node );
  virtual bool visit( const AST::ExpressionSequenceExpression *node );
  virtual bool visit( const AST::FromToStepSequenceExpression *node );
  virtual bool visit( const AST::LookupVariableExpression     *node );
  virtual bool visit( const AST::FunctionCallExpression       *node );
  virtual bool visit( const AST::DotExpression                *node );
  virtual bool visit( const AST::FrameAccessExpression        *node );
  virtual bool visit( const AST::ArrayAccessExpression        *node );
  virtual bool visit( const AST::UnaryExpression              *node );
  virtual bool visit( const AST::BinaryExpression             *node );
  virtual bool visit( const AST::IfThenElseExpression         *node );
  virtual bool visit( const AST::TypeExpression               *node );
  virtual bool visit( const AST::DimensionsExpression         *node );

  virtual bool visit( const AST::CompoundStatement            *node );
  virtual bool visit( const AST::FunctionDefinitionStatement  *node );
  virtual bool visit( const AST::ConnectStatement             *node );
  virtual bool visit( const AST::AssignIdentifierStatement    *node );
  virtual bool visit( const AST::AssignDotStatement           *node );
  virtual bool visit( const AST::AssignBracketStatement       *node );
  virtual bool visit( const AST::IfStatement                  *node );
  virtual bool visit( const AST::NOPStatement                 *node );
  virtual bool visit( const AST::ForSequenceStatement         *node );
  virtual bool visit( const AST::ForContainerStatement        *node );
  virtual bool visit( const AST::ExpressionStatement          *node );
  virtual bool visit( const AST::VariableCreationStatement    *node );
  virtual bool visit( const AST::VariableConstructionStatement*node );
  virtual bool visit( const AST::ReturnStatement              *node );

  bool get_return_encountered( void ) const { return return_encountered; }
  const Value &get_return_value( void ) const { return return_value; }

  class TypeDescr : public Refcounted
  {
  public:
    TypeDescr(
        CountPtr< const TypeDescr > _of
      , CountPtr< const Array< Value, 1 > > _dims
      , Type _type
      )
    : of( _of ), dims( _dims ), type( _type )
    {}

    virtual ~TypeDescr( void ) {}

    const CountPtr< const TypeDescr > of;
    const CountPtr< const Array< Value, 1 > > dims;
    Type type;
  };

private:
  bool dot_access_impl( const Location *loc, Value &left, const String &identifier, Value *&value );
  bool assign_impl( const AST::AssignmentStatementBase *node, Value *lvalue );

  void determine_size( const ArrayBase *array, int dims, index_t *size, int dim );
  void fill_array( const ArrayBase *array, const ArrayBase::Size &size, Value *dest, int dim );

  CountPtr< Scope > scope;
  CountPtr< TypeDescr > return_value_type_descr;
  CountPtr< Array< Value, 1 > > return_value_dims;
  Value return_value;
  bool return_encountered;
};

} // namespace RPGML

#endif


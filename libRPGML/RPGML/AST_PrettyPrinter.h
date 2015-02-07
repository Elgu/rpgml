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
#ifndef RPGML_AST_PrettyPrinter_h
#define RPGML_AST_PrettyPrinter_h

#include "AST.h"

#include <iostream>

namespace RPGML {
namespace AST {

class PrettyPrinter : public Visitor
{
public:
  explicit
  PrettyPrinter( std::ostream *_o )
  : o( _o )
  , depth( 0 )
  {}

  std::ostream &getStream( void ) { return (*o); }

  virtual void visit( const ConstantExpression           *node );
  virtual void visit( const ThisExpression               *node );
  virtual void visit( const ArrayConstantExpression      *node );
  virtual void visit( const FrameConstantExpression      *node );
  virtual void visit( const ParenthisSequenceExpression  *node );
  virtual void visit( const ExpressionSequenceExpression *node );
  virtual void visit( const FromToStepSequenceExpression *node );
  virtual void visit( const LookupVariableExpression     *node );
  virtual void visit( const FunctionCallExpression       *node );
  virtual void visit( const DotExpression                *node );
  virtual void visit( const FrameAccessExpression        *node );
  virtual void visit( const ArrayAccessExpression        *node );
  virtual void visit( const UnaryExpression              *node );
  virtual void visit( const BinaryExpression             *node );
  virtual void visit( const IfThenElseExpression         *node );
  virtual void visit( const TypeExpression               *node );
  virtual void visit( const DimensionsExpression         *node );
  virtual void visit( const CastExpression               *node );

  virtual void visit( const CompoundStatement            *node );
  virtual void visit( const FunctionDefinitionStatement  *node );
  virtual void visit( const ConnectStatement             *node );
  virtual void visit( const AssignIdentifierStatement    *node );
  virtual void visit( const AssignDotStatement           *node );
  virtual void visit( const AssignBracketStatement       *node );
  virtual void visit( const IfStatement                  *node );
  virtual void visit( const NOPStatement                 *node );
  virtual void visit( const ForSequenceStatement         *node );
  virtual void visit( const ForContainerStatement        *node );
  virtual void visit( const ExpressionStatement          *node );
  virtual void visit( const VariableCreationStatement    *node );
  virtual void visit( const VariableConstructionStatement*node );
  virtual void visit( const ReturnStatement              *node );

private:
  void indent( void );

  void print( const ArrayConstantExpression::SequenceExpressionArray *seq_array );
  void print( const ArrayBase *array_base, int dims );
  void print( const FunctionCallExpression::Args *args );

  std::ostream *o;
  int depth;
};

} // namespace AST
} // namespace RPGML

namespace std {

static inline
std::ostream &operator<<( std::ostream &o, const RPGML::AST::TypeExpression *type )
{
  RPGML::AST::PrettyPrinter pretty( &o );
  pretty.visit( type );
  return o;
}

} // namespace std

#endif


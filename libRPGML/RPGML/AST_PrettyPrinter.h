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

  virtual bool visit( const ConstantExpression           *node );
  virtual bool visit( const ThisExpression               *node );
  virtual bool visit( const ArrayConstantExpression      *node );
  virtual bool visit( const FrameConstantExpression      *node );
  virtual bool visit( const ParenthisSequenceExpression  *node );
  virtual bool visit( const ExpressionSequenceExpression *node );
  virtual bool visit( const FromToStepSequenceExpression *node );
  virtual bool visit( const LookupVariableExpression     *node );
  virtual bool visit( const FunctionCallExpression       *node );
  virtual bool visit( const DotExpression                *node );
  virtual bool visit( const FrameAccessExpression        *node );
  virtual bool visit( const ArrayAccessExpression        *node );
  virtual bool visit( const UnaryExpression              *node );
  virtual bool visit( const BinaryExpression             *node );
  virtual bool visit( const IfThenElseExpression         *node );
  virtual bool visit( const TypeExpression               *node );
  virtual bool visit( const DimensionsExpression         *node );

  virtual bool visit( const CompoundStatement            *node );
  virtual bool visit( const FunctionDefinitionStatement  *node );
  virtual bool visit( const ConnectStatement             *node );
  virtual bool visit( const AssignIdentifierStatement    *node );
  virtual bool visit( const AssignDotStatement           *node );
  virtual bool visit( const AssignBracketStatement       *node );
  virtual bool visit( const IfStatement                  *node );
  virtual bool visit( const NOPStatement                 *node );
  virtual bool visit( const ForSequenceStatement         *node );
  virtual bool visit( const ForContainerStatement        *node );
  virtual bool visit( const ExpressionStatement          *node );
  virtual bool visit( const VariableCreationStatement    *node );
  virtual bool visit( const VariableConstructionStatement*node );
  virtual bool visit( const ReturnStatement              *node );

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

#endif


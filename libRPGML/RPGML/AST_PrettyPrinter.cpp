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
#include "AST_PrettyPrinter.h"

#include "make_printable.h"

using namespace std;

namespace RPGML {
namespace AST {

void PrettyPrinter::visit( const ConstantExpression           *node )
{
  switch( node->value.getEnum() )
  {
    case Type::STRING:
      (*o) << "\"" << make_printable( node->value.getString() ) << "\"";
      break;
    default          :
      node->value.print( (*o) );
  }
}

void PrettyPrinter::visit( const ThisExpression           * )
{
  (*o) << "this";
}

void PrettyPrinter::print( const ArrayConstantExpression::SequenceExpressionArray *seq_array )
{
  typedef ArrayConstantExpression::SequenceExpressionArray::ConstIterator I;

  bool first = true;
  for( CountPtr< I > i( seq_array->getIterator() ); !i->done(); i->next() )
  {
    if( first )
    {
      first = false;
    }
    else
    {
      (*o) << ";" << endl;
      indent();
    }
    i->get()->invite( this );
  }
}

void PrettyPrinter::print( const ArrayBase *array_base, int dims )
{
  if( dims <= 2 )
  {
    const ArrayConstantExpression::SequenceExpressionArray *seq_array = 0;
    if( !array_base->getAs( seq_array ) )
    {
      throw Exception()
        << "Could not get seq_array"
        << ": dims = " << dims
        << ", array_base->getType() = " << array_base->getType()
        << ", array_base->getSize() = " << array_base->getSize()
        ;
    }
    if( dims == 1 && seq_array->size() > 1 )
    {
      throw Exception()
        << "ArrayConstantExpression specified dims = 1"
        << ", but seq_array->size() = " << seq_array->size()
        ;
    }

    print( seq_array );
  }
  else
  {
    const ArrayConstantExpression::ArrayBaseArray *array_array = 0;
    if( !array_base->getAs( array_array ) )
    {
      throw Exception()
        << "Could not get array_array"
        << ": dims = " << dims
        << ", array_base->getType() = " << array_base->getType()
        << ", array_base->getSize() = " << array_base->getSize()
        ;
    }

    typedef ArrayConstantExpression::ArrayBaseArray::ConstIterator I;

    bool first = true;
    for( CountPtr< I > i( array_array->getIterator() ); !i->done(); i->next() )
    {
      if( first )
      {
        first = false;
      }
      else
      {
        (*o) << endl;
        indent();
        for( int d=0; d<dims-1; ++d ) (*o) << ";";
        (*o) << endl;
        indent();
      }
      ++depth; (*o) << "  ";
      print( i->get(), dims-1 );
      --depth;
    }
  }
}

void PrettyPrinter::visit( const ArrayConstantExpression      *node )
{
  (*o) << "[ ";
  if( node->dims > 1 )
  {
    (*o) << endl;
    ++depth;
    indent();
  }

  print( node->descr_array, node->dims );

  if( node->dims > 1 )
  {
    (*o) << endl;
    --depth;
    indent();
  }
  (*o) << " ]";
}

void PrettyPrinter::visit( const FrameConstantExpression        *node )
{
  (*o) << "Frame";
  node->body->invite( this );
}

void PrettyPrinter::visit( const ExpressionSequenceExpression *node )
{
  const size_t n = node->expressions.size();

  for( size_t i=0; i<n; ++i )
  {
    if( i > 0 ) (*o) << ", ";
    node->expressions[ i ]->invite( this );
  }
}

void PrettyPrinter::visit( const ParenthisSequenceExpression *node )
{
  if( node->sequence )
  {
    (*o) << "Sequence( ";
    node->sequence->invite( this );
    (*o) << " )";
  }
  else
  {
    (*o) << "Sequence()";
  }
}

void PrettyPrinter::visit( const FromToStepSequenceExpression *node )
{
  node->from->invite( this );
  (*o) << " to ";
  node->to->invite( this );
  if( node->step )
  {
    (*o) << " step ";
    node->step->invite( this );
  }
}

void PrettyPrinter::visit( const LookupVariableExpression     *node )
{
  if( node->at_root ) (*o) << ".";
  (*o) << node->identifier;
}

void PrettyPrinter::print( const FunctionCallExpression::Args *args )
{
  const size_t n = args->size();
  if( n > 0 )
  {
    (*o) << "( ";
    for( size_t i=0; i<n; ++i )
    {
      if( i > 0 ) (*o) << ", ";
      const FunctionCallExpression::Arg *const arg = args->at( i );
      if( !arg->identifier.empty() )
      {
        (*o) << arg->identifier;
        (*o) << "=";
      }
      arg->value->invite( this );
    }
    (*o) << " )";
  }
  else
  {
    (*o) << "()";
  }
}

void PrettyPrinter::visit( const FunctionCallExpression       *node )
{
  node->function->invite( this );
  print( node->args );
}

void PrettyPrinter::visit( const DotExpression             *node )
{
  node->left->invite( this );
  (*o) << ".";
  (*o) << node->member;
}

void PrettyPrinter::visit( const FrameAccessExpression             *node )
{
  node->left->invite( this );
  (*o) << "." << node->identifier;
}

void PrettyPrinter::visit( const ArrayAccessExpression             *node )
{
  node->left->invite( this );
  (*o) << "[ ";
  node->coord->invite( this );
  (*o) << " ]";
}

void PrettyPrinter::visit( const UnaryExpression              *node )
{
  (*o) << getUOPStr( node->op );
  (*o) << "( ";
  node->arg->invite( this );
  (*o) << " )";
}

void PrettyPrinter::visit( const BinaryExpression             *node )
{
  (*o) << "( ";
  node->left->invite( this );
  (*o) << " " << getBOPStr( node->op ) << " ";
  node->right->invite( this );
  (*o) << " )";
}

void PrettyPrinter::visit( const IfThenElseExpression         *node )
{
  (*o) << "( ";
  node->condition->invite( this );
  (*o) << " ? ";
  node->then_value->invite( this );
  (*o) << " : ";
  node->else_value->invite( this );
  (*o) << " )";
}

void PrettyPrinter::visit( const TypeExpression               *node )
{
  if( node->type.isArray() )
  {
    node->of->invite( this );
    (*o) << "[ ";
    node->dims->invite( this );
    (*o) << " ]";
  }
  else
  {
    (*o) << node->type.getTypeName();
  }
}

void PrettyPrinter::visit( const DimensionsExpression         *node )
{
  const size_t N = node->dims.size();

  for( size_t i=0; i<N; ++i )
  {
    if( i>0 ) (*o) << ", ";
    const Expression *const e = node->dims[ i ];
    if( e ) e->invite( this );
  }
}

void PrettyPrinter::visit( const CastExpression         *node )
{
  (*o) << node->to << "( ";
  node->arg->invite( this );
  (*o) << " )";
}

void PrettyPrinter::visit( const CompoundStatement            *node )
{
  // indent() must have been called, if neccessary
  const size_t n = node->statements.size();
  if( n > 0 )
  {
    (*o) << "{" << std::endl;
    ++depth;
    for( size_t i=0; i<n; ++i )
    {
      indent();
      node->statements[ i ]->invite( this );
      (*o) << std::endl;
    }
    --depth;
    indent();
    (*o) << "}";
  }
  else
  {
    (*o) << "{}";
  }
}

void PrettyPrinter::visit( const FunctionDefinitionStatement  *node )
{
  // indent() must have been called, if neccessary
  if( node->is_method )
  {
    (*o) << "Method";
  }
  else
  {
    node->ret->invite( this );
  }

  (*o) << " " << node->identifier;

  if( node->args->empty() )
  {
    (*o) << "()" << std::endl;
  }
  else
  {
    (*o) << "( ";
    const size_t n = node->args->size();
    for( size_t i=0; i<n; ++i )
    {
      if( i > 0 ) (*o) << ", ";
      const FunctionDefinitionStatement::ArgDecl *arg = node->args->at( i );
      (*o) << arg->identifier;
      if( arg->default_value )
      {
        (*o) << "=";
        arg->default_value->invite( this );
      }
    }
    (*o) << " )" << std::endl;
  }

  indent();
  node->body->invite( this );
}

void PrettyPrinter::visit( const ConnectStatement    *node )
{
  node->output->invite( this );
  (*o) << " -> ";
  node->input->invite( this );
  (*o) << ";";
}

void PrettyPrinter::visit( const AssignIdentifierStatement    *node )
{
  // indent() must have been called, if neccessary
  (*o) << node->identifier;
  (*o) << " " << getAssign( node->op ) << " ";
  ++depth;
  node->value->invite( this );
  --depth;
  (*o) << ";";
}

void PrettyPrinter::visit( const AssignDotStatement           *node )
{
  // indent() must have been called, if neccessary
  node->left->invite( this );
  (*o) << ".";
  (*o) << node->identifier;
  (*o) << " " << getAssign( node->op ) << " ";
  ++depth;
  node->value->invite( this );
  --depth;
  (*o) << ";";
}

void PrettyPrinter::visit( const AssignBracketStatement       *node )
{
  // indent() must have been called, if neccessary
  node->left->invite( this );
  (*o) << "[ ";
  node->coord->invite( this );
  (*o) << " ]";
  (*o) << " " << getAssign( node->op ) << " ";
  ++depth;
  node->value->invite( this );
  --depth;
  (*o) << ";";
}

void PrettyPrinter::visit( const IfStatement                  *node )
{
  // indent() must have been called, if neccessary
  (*o) << "if( ";
  node->condition->invite( this );
  (*o) << " )" << std::endl;
  ++depth;
  indent();
  node->then_body->invite( this );
  --depth;
  if( node->else_body )
  {
    (*o) << std::endl;
    indent();
    (*o) << "else" << std::endl;
    ++depth;
    indent();
    node->else_body->invite( this );
    --depth;
  }
}

void PrettyPrinter::visit( const NOPStatement                 * )
{
  // indent() must have been called, if neccessary
  (*o) << ";";
}

void PrettyPrinter::visit( const ForSequenceStatement         *node )
{
  // indent() must have been called, if neccessary
  (*o) << "for " << node->identifier << " = ";
  node->sequence->invite( this );
  (*o) << std::endl;
  ++depth;
  indent();
  node->body->invite( this );
  --depth;
}

void PrettyPrinter::visit( const ForContainerStatement        *node )
{
  // indent() must have been called, if neccessary
  (*o) << "for " << node->identifier << " in ";
  node->container->invite( this );
  (*o) << std::endl;
  ++depth;
  indent();
  node->body->invite( this );
  --depth;
}

void PrettyPrinter::visit( const ExpressionStatement        *node )
{
  // indent() must have been called, if neccessary
  node->expr->invite( this );
  (*o) << ";";
}

void PrettyPrinter::visit( const VariableCreationStatement    *node )
{
  // indent() must have been called, if neccessary
  node->type->invite( this );
  (*o) << " " << node->identifier;
  if( !node->value.isNull() )
  {
    (*o) << " = ";
    node->value->invite( this );
  }
  (*o) << ";";
}

void PrettyPrinter::visit( const VariableConstructionStatement *node )
{
  node->value->function->invite( this );
  (*o) << " " << node->identifier;
  print( node->value->args );
  (*o) << ";";
}

void PrettyPrinter::visit( const ReturnStatement              *node )
{
  // indent() must have been called, if neccessary
  (*o) << "return ";
  node->value->invite( this );
  (*o) << ";";
}

void PrettyPrinter::indent( void )
{
  for( int i=0; i<depth; ++i )
  {
    (*o) << "  ";
  }
}

} // namespace AST
} // namespace RPGML


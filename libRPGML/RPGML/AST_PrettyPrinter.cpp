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

bool PrettyPrinter::visit( const ConstantExpression           *node )
{
  switch( node->value.getEnum() )
  {
    case Type::STRING:
      (*o) << "\"" << make_printable( node->value.getString() ) << "\"";
      break;
    default          :
      node->value.print( (*o) );
  }
  return true;
}

bool PrettyPrinter::visit( const ThisExpression           * )
{
  (*o) << "this";
  return true;
}

void PrettyPrinter::print( const ArrayConstantExpression::SequenceExpressionArray *seq_array )
{
  typedef ArrayConstantExpression::SequenceExpressionArray::ConstElements I;

  bool first = true;
  for( CountPtr< I > i( seq_array->getElements() ); !i->done(); i->next() )
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

    typedef ArrayConstantExpression::ArrayBaseArray::ConstElements I;

    bool first = true;
    for( CountPtr< I > i( array_array->getElements() ); !i->done(); i->next() )
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

bool PrettyPrinter::visit( const ArrayConstantExpression      *node )
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

  return true;
}

bool PrettyPrinter::visit( const FrameConstantExpression        *node )
{
  (*o) << "Frame";
  node->body->invite( this );
  return true;
}

bool PrettyPrinter::visit( const ExpressionSequenceExpression *node )
{
  const size_t n = node->expressions.size();

  for( size_t i=0; i<n; ++i )
  {
    if( i > 0 ) (*o) << ", ";
    node->expressions[ i ]->invite( this );
  }

  return true;
}

bool PrettyPrinter::visit( const ParenthisSequenceExpression *node )
{
  if( node->sequence )
  {
    (*o) << "( ";
    node->sequence->invite( this );
    (*o) << " )";
  }
  else
  {
    (*o) << "()";
  }

  return true;
}

bool PrettyPrinter::visit( const FromToStepSequenceExpression *node )
{
  node->from->invite( this );
  (*o) << " to ";
  node->to->invite( this );
  if( node->step )
  {
    (*o) << " step ";
    node->step->invite( this );
  }
  return true;
}

bool PrettyPrinter::visit( const LookupVariableExpression     *node )
{
  if( node->at_root ) (*o) << ".";
  (*o) << node->identifier;
  return true;
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

bool PrettyPrinter::visit( const FunctionCallExpression       *node )
{
  node->function->invite( this );

  print( node->args );

  return true;
}

bool PrettyPrinter::visit( const DotExpression             *node )
{
  node->left->invite( this );
  (*o) << ".";
  (*o) << node->member;
  return true;
}

bool PrettyPrinter::visit( const FrameAccessExpression             *node )
{
  node->left->invite( this );
  (*o) << "." << node->identifier;
  return true;
}

bool PrettyPrinter::visit( const ArrayAccessExpression             *node )
{
  node->left->invite( this );
  (*o) << "[ ";
  node->coord->invite( this );
  (*o) << " ]";
  return true;
}

bool PrettyPrinter::visit( const UnaryExpression              *node )
{
  (*o) << getUOPStr( node->op );
  (*o) << "( ";
  node->arg->invite( this );
  (*o) << " )";
  return true;
}

bool PrettyPrinter::visit( const BinaryExpression             *node )
{
  (*o) << "( ";
  node->left->invite( this );
  (*o) << " " << getBOPStr( node->op ) << " ";
  node->right->invite( this );
  (*o) << " )";
  return true;
}

bool PrettyPrinter::visit( const IfThenElseExpression         *node )
{
  (*o) << "( ";
  node->condition->invite( this );
  (*o) << " ? ";
  node->then_value->invite( this );
  (*o) << " : ";
  node->else_value->invite( this );
  (*o) << " )";
  return true;
}

bool PrettyPrinter::visit( const TypeExpression               *node )
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
  return true;
}

bool PrettyPrinter::visit( const DimensionsExpression         *node )
{
  const size_t N = node->dims.size();

  for( size_t i=0; i<N; ++i )
  {
    if( i>0 ) (*o) << ", ";
    const Expression *const e = node->dims[ i ];
    if( e ) e->invite( this );
  }

  return true;
}

bool PrettyPrinter::visit( const CompoundStatement            *node )
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

  return true;
}

bool PrettyPrinter::visit( const FunctionDefinitionStatement  *node )
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

  return true;
}

bool PrettyPrinter::visit( const ConnectStatement    *node )
{
  node->output->invite( this );
  (*o) << " -> ";
  node->input->invite( this );
  (*o) << ";";
  return true;
}

bool PrettyPrinter::visit( const AssignIdentifierStatement    *node )
{
  // indent() must have been called, if neccessary
  (*o) << node->identifier;
  (*o) << " " << getAssign( node->op ) << " ";
  ++depth;
  node->value->invite( this );
  --depth;
  (*o) << ";";
  return true;
}

bool PrettyPrinter::visit( const AssignDotStatement           *node )
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
  return true;
}

bool PrettyPrinter::visit( const AssignBracketStatement       *node )
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
  return true;
}

bool PrettyPrinter::visit( const IfStatement                  *node )
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
  return true;
}

bool PrettyPrinter::visit( const NOPStatement                 * )
{
  // indent() must have been called, if neccessary
  (*o) << ";";
  return true;
}

bool PrettyPrinter::visit( const ForSequenceStatement         *node )
{
  // indent() must have been called, if neccessary
  (*o) << "for " << node->identifier << " = ";
  node->sequence->invite( this );
  (*o) << std::endl;
  ++depth;
  indent();
  node->body->invite( this );
  --depth;
  return true;
}

bool PrettyPrinter::visit( const ForContainerStatement        *node )
{
  // indent() must have been called, if neccessary
  (*o) << "for " << node->identifier << " in ";
  node->container->invite( this );
  (*o) << std::endl;
  ++depth;
  indent();
  node->body->invite( this );
  --depth;
  return true;
}

bool PrettyPrinter::visit( const ExpressionStatement        *node )
{
  // indent() must have been called, if neccessary
  node->expr->invite( this );
  (*o) << ";";
  return true;
}

bool PrettyPrinter::visit( const VariableCreationStatement    *node )
{
  // indent() must have been called, if neccessary
  node->type->invite( this );
  (*o) << " " << node->identifier << " = ";
  node->value->invite( this );
  (*o) << ";";
  return true;
}

bool PrettyPrinter::visit( const VariableConstructionStatement *node )
{
  node->value->function->invite( this );
  (*o) << " " << node->identifier;
  print( node->value->args );
  (*o) << ";";
  return true;
}

bool PrettyPrinter::visit( const ReturnStatement              *node )
{
  // indent() must have been called, if neccessary
  (*o) << "return ";
  node->value->invite( this );
  (*o) << ";";
  return true;
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


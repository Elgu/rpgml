#include "AST_PrettyPrinter.h"

namespace RPGML {
namespace AST {

bool PrettyPrinter::visit( const ConstantExpression           *node )
{
  switch( node->value.getEnum() )
  {
    case Type::BOOL  : (*o) << ( node->value.getBool() ? "true" : "false" ); break;
    case Type::INT   : (*o) << node->value.getInt(); break;
    case Type::FLOAT : (*o) << node->value.getFloat(); break;
    case Type::STRING: (*o) << "\"" << node->value.getString()->get() << "\""; break;
    default          : (*o) << "(unexpected '" << node->value.getTypeName() << "' constant)"; break;

  }
  return true;
}

bool PrettyPrinter::visit( const ArrayConstantExpression      *node )
{
  if( node->sequence  )
  {
    (*o) << "[ ";
    node->sequence->invite( this );
    (*o) << " ]";
  }
  else
  {
    (*o) << "[]";
  }

  return true;
}

bool PrettyPrinter::visit( const MapConstantExpression        *node )
{
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
  (*o) << node->identifier->get();
  return true;
}

bool PrettyPrinter::visit( const FunctionCallExpression       *node )
{
  node->function->invite( this );

  const size_t n = node->args->size();
  if( n > 0 )
  {
    (*o) << "( ";
    for( size_t i=0; i<n; ++i )
    {
      if( i > 0 ) (*o) << ", ";
      const FunctionCallExpression::Arg *const arg = node->args->at( i );
      if( arg->identifier )
      {
        (*o) << arg->identifier->get();
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

  return true;
}

bool PrettyPrinter::visit( const DotExpression             *node )
{
  node->left->invite( this );
  (*o) << ".";
  (*o) << node->member->get();
  return true;
}

bool PrettyPrinter::visit( const AccessExpression             *node )
{
  node->left->invite( this );
  (*o) << "[ ";
  node->key->invite( this );
  (*o) << " ]";
  return true;
}

bool PrettyPrinter::visit( const UnaryExpression              *node )
{
  (*o) << getUOP( node->op );
  (*o) << "( ";
  node->arg->invite( this );
  (*o) << " )";
  return true;
}

bool PrettyPrinter::visit( const BinaryExpression             *node )
{
  (*o) << "( ";
  node->left->invite( this );
  (*o) << " " << getBOP( node->op ) << " ";
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
  (*o) << "Function " << node->identifier->get();
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
      (*o) << arg->identifier->get();
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

bool PrettyPrinter::visit( const AssignIdentifierStatement    *node )
{
  // indent() must have been called, if neccessary
  (*o) << node->identifier->get();
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
  (*o) << node->identifier->get();
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
  node->key->invite( this );
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
  (*o) << "for " << node->identifier->get() << " = ";
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
  (*o) << "for " << node->identifier->get() << " in ";
  node->container->invite( this );
  (*o) << std::endl;
  ++depth;
  indent();
  node->body->invite( this );
  --depth;
  return true;
}

bool PrettyPrinter::visit( const FunctionCallStatement        *node )
{
  // indent() must have been called, if neccessary
  node->call->invite( this );
  (*o) << ";";
  return true;
}

bool PrettyPrinter::visit( const VariableCreationStatement    *node )
{
  // indent() must have been called, if neccessary
  (*o) << node->type.getTypeName() << " " << node->identifier->get() << " = ";
  node->value->invite( this );
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


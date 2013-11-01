#ifndef RPGML_InterpretingASTVisitor_h
#define RPGML_InterpretingASTVisitor_h

#include "AST.h"

namespace RPGML {

class Scope;

class InterpretingASTVisitor : public AST::Visitor, public Refcounted
{
public:
  explicit
  InterpretingASTVisitor( Scope *_scope, index_t recursion_depth=0 );

  virtual ~InterpretingASTVisitor( void );

  virtual bool visit( const AST::ConstantExpression           *node );
  virtual bool visit( const AST::ArrayConstantExpression      *node );
  virtual bool visit( const AST::MapConstantExpression        *node );
  virtual bool visit( const AST::ParenthisSequenceExpression  *node );
  virtual bool visit( const AST::ExpressionSequenceExpression *node );
  virtual bool visit( const AST::FromToStepSequenceExpression *node );
  virtual bool visit( const AST::LookupVariableExpression     *node );
  virtual bool visit( const AST::FunctionCallExpression       *node );
  virtual bool visit( const AST::DotExpression                *node );
  virtual bool visit( const AST::AccessExpression             *node );
  virtual bool visit( const AST::UnaryExpression              *node );
  virtual bool visit( const AST::BinaryExpression             *node );
  virtual bool visit( const AST::IfThenElseExpression         *node );

  virtual bool visit( const AST::CompoundStatement            *node );
  virtual bool visit( const AST::FunctionDefinitionStatement  *node );
  virtual bool visit( const AST::AssignIdentifierStatement    *node );
  virtual bool visit( const AST::AssignDotStatement           *node );
  virtual bool visit( const AST::AssignBracketStatement       *node );
  virtual bool visit( const AST::IfStatement                  *node );
  virtual bool visit( const AST::NOPStatement                 *node );
  virtual bool visit( const AST::ForSequenceStatement         *node );
  virtual bool visit( const AST::ForContainerStatement        *node );
  virtual bool visit( const AST::FunctionCallStatement        *node );
  virtual bool visit( const AST::VariableCreationStatement    *node );
  virtual bool visit( const AST::ReturnStatement              *node );

  CountPtr< Scope > scope;
  Value return_value;
  bool return_encountered;

private:
  bool assign_impl( const AST::AssignmentStatement *node, Value *lvalue );

  CountPtr< Function > unary_Function;
  CountPtr< Function > binary_Function;
};

} // namespace RPGML

#endif


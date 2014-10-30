#ifndef RPGML_InterpretingASTVisitor_h
#define RPGML_InterpretingASTVisitor_h

#include "AST.h"
#include "Array.h"

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

  bool get_return_encountered( void ) const { return return_encountered; }
  const Value &get_return_value( void ) const { return return_value; }

private:
  bool assign_impl( const AST::AssignmentStatement *node, Value *lvalue );

  class TypeDescr : public Refcounted
  {
  public:
    TypeDescr( void ) {}
    virtual ~TypeDescr( void ) {}

    CountPtr< TypeDescr > of;
    CountPtr< Array< Value, 1 > > dims;
    Type type;
  };

  CountPtr< Scope > scope;
  CountPtr< TypeDescr > return_value_type_descr;
  CountPtr< Array< Value, 1 > > return_value_dims;
  Value return_value;
  bool return_encountered;

  CountPtr< Function > unaryOp;
  CountPtr< Function > binaryOp;
};

} // namespace RPGML

#endif


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
  virtual bool visit( const ArrayConstantExpression      *node );
  virtual bool visit( const MapConstantExpression        *node );
  virtual bool visit( const ParenthisSequenceExpression  *node );
  virtual bool visit( const ExpressionSequenceExpression *node );
  virtual bool visit( const FromToStepSequenceExpression *node );
  virtual bool visit( const LookupVariableExpression     *node );
  virtual bool visit( const FunctionCallExpression       *node );
  virtual bool visit( const DotExpression                *node );
  virtual bool visit( const AccessExpression             *node );
  virtual bool visit( const UnaryExpression              *node );
  virtual bool visit( const BinaryExpression             *node );
  virtual bool visit( const IfThenElseExpression         *node );
  virtual bool visit( const CompoundStatement            *node );
  virtual bool visit( const FunctionDefinitionStatement  *node );
  virtual bool visit( const AssignIdentifierStatement    *node );
  virtual bool visit( const AssignDotStatement           *node );
  virtual bool visit( const AssignBracketStatement       *node );
  virtual bool visit( const IfStatement                  *node );
  virtual bool visit( const NOPStatement                 *node );
  virtual bool visit( const ForSequenceStatement         *node );
  virtual bool visit( const ForContainerStatement        *node );
  virtual bool visit( const FunctionCallStatement        *node );
  virtual bool visit( const VariableCreationStatement    *node );
  virtual bool visit( const ReturnStatement              *node );

private:
  void indent( void );

  std::ostream *o;
  int depth;
};

} // namespace AST
} // namespace RPGML

#endif


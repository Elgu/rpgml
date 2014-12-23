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
#ifndef RPGML_AST_h
#define RPGML_AST_h

#include "Refcounted.h"
#include "String.h"
#include "ParserEnums.h"
#include "Value.h"
#include "Location.h"

#include <vector>

namespace RPGML {
namespace AST {

class Node;
class Visitor;

class Expression;
class ConstantExpression;
class ThisExpression;
class ArrayConstantExpression;
class FrameConstantExpression;
class SequenceExpression;
class ParenthisSequenceExpression;
class ExpressionSequenceExpression;
class FromToStepSequenceExpression;
class LookupVariableExpression;
class FunctionCallExpression;
class DotExpression;
class FrameAccessExpression;
class ArrayAccessExpression;
class UnaryExpression;
class BinaryExpression;
class IfThenElseExpression;
class TypeExpression;
class DimensionsExpression;
typedef DimensionsExpression CoordinatesExpression;

class Statement;
class CompoundStatement;
class FunctionDefinitionStatement;
class ConnectStatement;
class AssignmentStatement;
class AssignIdentifierStatement;
class AssignDotStatement;
class AssignBracketStatement;
class IfStatement;
class NOPStatement;
class ForStatement;
class ForSequenceStatement;
class ForContainerStatement;
class FunctionCallStatement;
class VariableCreationStatement;
class ReturnStatement;

class Node : public Refcounted
{
public:
  Node( const Location *_loc ) : loc( _loc ) {}
  virtual ~Node( void ) {}

  // Implement using visitor->invite_impl( this )
  virtual bool invite( Visitor *visitor ) const = 0;

  // Overloads for "is a"
  virtual Expression         *getExpression        ( void ) { return 0; }
  virtual SequenceExpression *getSequenceExpression( void ) { return 0; }
  virtual Statement          *getStatement         ( void ) { return 0; }
  virtual CompoundStatement  *getCompoundStatement ( void ) { return 0; }

  const CountPtr< const Location > loc;
};

class Visitor
{
public:
  Visitor( index_t _recursion_depth=0 )
  : recursion_depth( _recursion_depth )
  {}
  virtual ~Visitor( void ) {}

  virtual bool visit( const Node *node )
  {
    return node->invite( this );
  }

  virtual bool visit( const ConstantExpression           *node ) = 0;
  virtual bool visit( const ThisExpression               *node ) = 0;
  virtual bool visit( const ArrayConstantExpression      *node ) = 0;
  virtual bool visit( const FrameConstantExpression      *node ) = 0;
  virtual bool visit( const ParenthisSequenceExpression  *node ) = 0;
  virtual bool visit( const ExpressionSequenceExpression *node ) = 0;
  virtual bool visit( const FromToStepSequenceExpression *node ) = 0;
  virtual bool visit( const LookupVariableExpression     *node ) = 0;
  virtual bool visit( const FunctionCallExpression       *node ) = 0;
  virtual bool visit( const DotExpression                *node ) = 0;
  virtual bool visit( const FrameAccessExpression        *node ) = 0;
  virtual bool visit( const ArrayAccessExpression        *node ) = 0;
  virtual bool visit( const UnaryExpression              *node ) = 0;
  virtual bool visit( const BinaryExpression             *node ) = 0;
  virtual bool visit( const IfThenElseExpression         *node ) = 0;
  virtual bool visit( const TypeExpression               *node ) = 0;
  virtual bool visit( const DimensionsExpression         *node ) = 0;

  virtual bool visit( const CompoundStatement            *node ) = 0;
  virtual bool visit( const FunctionDefinitionStatement  *node ) = 0;
  virtual bool visit( const ConnectStatement             *node ) = 0;
  virtual bool visit( const AssignIdentifierStatement    *node ) = 0;
  virtual bool visit( const AssignDotStatement           *node ) = 0;
  virtual bool visit( const AssignBracketStatement       *node ) = 0;
  virtual bool visit( const IfStatement                  *node ) = 0;
  virtual bool visit( const NOPStatement                 *node ) = 0;
  virtual bool visit( const ForSequenceStatement         *node ) = 0;
  virtual bool visit( const ForContainerStatement        *node ) = 0;
  virtual bool visit( const FunctionCallStatement        *node ) = 0;
  virtual bool visit( const VariableCreationStatement    *node ) = 0;
  virtual bool visit( const ReturnStatement              *node ) = 0;

  template< class NodeType >
  bool invite_impl( const NodeType *node )
  {
    ++recursion_depth;
    if( recursion_depth > 100 ) throw "Maximum recursion depth reached";
    bool ret = visit( node );
    --recursion_depth;
    return ret;
  }

  index_t getRecursionDepth( void ) const
  {
    return recursion_depth;
  }

private:
  index_t recursion_depth;
};

class Expression : public Node
{
public:
  Expression( const Location *_loc ) : Node( _loc ) {}
  virtual ~Expression( void ) {}
  virtual Expression *getExpression( void ) { return this; }
};

class ConstantExpression : public Expression
{
public:
  explicit ConstantExpression( const Location *_loc, bool b    ) : Expression( _loc ), value( b ) {}
  explicit ConstantExpression( const Location *_loc, int i     ) : Expression( _loc ), value( i ) {}
  explicit ConstantExpression( const Location *_loc, float f   ) : Expression( _loc ), value( f ) {}
  explicit ConstantExpression( const Location *_loc, const StringData *s ) : Expression( _loc ), value( s ) {}
  explicit ConstantExpression( const Location *_loc, const String &s ) : Expression( _loc ), value( s ) {}

  virtual ~ConstantExpression( void ) {}

  virtual bool invite( Visitor *visitor ) const { return visitor->invite_impl( this ); }

  const Value value;
};

class ThisExpression : public Expression
{
public:
  explicit ThisExpression( const Location *_loc ) : Expression( _loc ) {}
  virtual ~ThisExpression( void ) {}

  virtual bool invite( Visitor *visitor ) const { return visitor->invite_impl( this ); }
};

class ArrayConstantExpression : public Expression
{
public:
  explicit
  ArrayConstantExpression( const Location *_loc, const SequenceExpression *_sequence=0 )
  : Expression( _loc )
  , sequence( _sequence )
  {}

  virtual ~ArrayConstantExpression( void ) {}

  virtual bool invite( Visitor *visitor ) const { return visitor->invite_impl( this ); }

  const CountPtr< const SequenceExpression > sequence;
};

class FrameConstantExpression : public Expression
{
public:
  FrameConstantExpression( const Location *_loc, const CompoundStatement *_body )
  : Expression( _loc )
  , body( _body )
  {}

  virtual ~FrameConstantExpression( void ) {}

  virtual bool invite( Visitor *visitor ) const { return visitor->invite_impl( this ); }

  const CountPtr< const CompoundStatement > body;
};

class SequenceExpression : public Expression
{
public:
  SequenceExpression( const Location *_loc )
  : Expression( _loc )
  {}
  virtual ~SequenceExpression( void ) {}
  virtual SequenceExpression *getSequenceExpression( void ) { return this; }
};

class ExpressionSequenceExpression : public SequenceExpression
{
public:
  ExpressionSequenceExpression( const Location *_loc )
  : SequenceExpression( _loc )
  {}
  virtual ~ExpressionSequenceExpression( void ) {}

  virtual bool invite( Visitor *visitor ) const { return visitor->invite_impl( this ); }

  void append( const Expression *expression )
  {
    expressions.push_back( expression );
  }

  std::vector< CountPtr< const Expression > > expressions;
};

class FromToStepSequenceExpression : public SequenceExpression
{
public:
  FromToStepSequenceExpression( const Location *_loc, const Expression *_from, const Expression *_to, const Expression *_step = 0 )
  : SequenceExpression( _loc )
  , from( _from )
  , to( _to )
  , step( _step )
  {}

  virtual ~FromToStepSequenceExpression( void ) {}

  virtual bool invite( Visitor *visitor ) const { return visitor->invite_impl( this ); }

  const CountPtr< const Expression > from;
  const CountPtr< const Expression > to;
  const CountPtr< const Expression > step;
};

class ParenthisSequenceExpression : public Expression
{
public:
  ParenthisSequenceExpression( const Location *_loc, const SequenceExpression *_sequence )
  : Expression( _loc )
  , sequence( _sequence )
  {}
  virtual ~ParenthisSequenceExpression( void ) {}

  virtual bool invite( Visitor *visitor ) const { return visitor->invite_impl( this ); }

  const CountPtr< const SequenceExpression > sequence;
};

class LookupVariableExpression : public Expression
{
public:
  LookupVariableExpression( const Location *_loc, const String &_identifier )
  : Expression( _loc )
  , identifier( _identifier )
  {}
  virtual ~LookupVariableExpression( void ) {}

  virtual bool invite( Visitor *visitor ) const { return visitor->invite_impl( this ); }

  const String identifier;
};

class FunctionCallExpression : public Expression
{
public:
  class Args;

  FunctionCallExpression( const Location *_loc, const Expression *_function, const Args *_args )
  : Expression( _loc )
  , function( _function )
  , args( _args )
  {}
  virtual ~FunctionCallExpression( void ) {}

  virtual bool invite( Visitor *visitor ) const { return visitor->invite_impl( this ); }

  class Arg : public Refcounted
  {
  public:
    explicit
    Arg( const Location *_loc, Expression *_value, const String &_identifier = String() )
    : loc( _loc )
    , value( _value )
    , identifier( _identifier )
    {}
    virtual ~Arg( void ) {}

    CountPtr< const Location > loc;
    CountPtr< Expression > value;
    const String identifier;
  };

  class Args : public Refcounted, public std::vector< CountPtr< Arg > >
  {
  public:
    Args( void ) {}
    virtual ~Args( void ) {}

    void append( Arg *arg )
    {
      push_back( arg );
    }
  };

  const CountPtr< const Expression > function;
  const CountPtr< const Args > args;
};

class DotExpression : public Expression
{
public:
  DotExpression( const Location *_loc, Expression *_left, const String &_member )
  : Expression( _loc )
  , left( _left )
  , member( _member )
  {}
  virtual ~DotExpression( void ) {}

  virtual bool invite( Visitor *visitor ) const { return visitor->invite_impl( this ); }

  const CountPtr< const Expression > left;
  const String member;
};

class FrameAccessExpression : public Expression
{
public:
  FrameAccessExpression( const Location *_loc, const Expression *_left, const String &_identifier )
  : Expression( _loc )
  , left( _left )
  , identifier( _identifier )
  {}
  virtual ~FrameAccessExpression( void ) {}

  virtual bool invite( Visitor *visitor ) const { return visitor->invite_impl( this ); }

  const CountPtr< const Expression > left;
  const String identifier;
};

class ArrayAccessExpression : public Expression
{
public:
  ArrayAccessExpression( const Location *_loc, const Expression *_left, const CoordinatesExpression *_coord )
  : Expression( _loc )
  , left( _left )
  , coord( _coord )
  {}
  virtual ~ArrayAccessExpression( void ) {}

  virtual bool invite( Visitor *visitor ) const { return visitor->invite_impl( this ); }

  const CountPtr< const Expression > left;
  const CountPtr< const CoordinatesExpression > coord;
};

class UnaryExpression : public Expression
{
public:
  UnaryExpression( const Location *_loc, UOP _op, const Expression *_arg )
  : Expression( _loc )
  , arg( _arg )
  , op( _op )
  {}
  virtual ~UnaryExpression( void ) {}

  virtual bool invite( Visitor *visitor ) const { return visitor->invite_impl( this ); }

  const CountPtr< const Expression > arg;
  const UOP op;
};

class BinaryExpression : public Expression
{
public:
  BinaryExpression( const Location *_loc, const Expression *_left, BOP _op, const Expression *_right )
  : Expression( _loc )
  , left ( _left  )
  , right( _right )
  , op( _op )
  {}
  virtual ~BinaryExpression( void ) {}

  virtual bool invite( Visitor *visitor ) const { return visitor->invite_impl( this ); }

  const CountPtr< const Expression > left;
  const CountPtr< const Expression > right;
  const BOP op;
};

class IfThenElseExpression : public Expression
{
public:
  IfThenElseExpression( const Location *_loc, const Expression *_condition, const Expression *_then_value, const Expression *_else_value )
  : Expression( _loc )
  , condition( _condition )
  , then_value( _then_value )
  , else_value( _else_value )
  {}
  virtual ~IfThenElseExpression( void ) {}

  virtual bool invite( Visitor *visitor ) const { return visitor->invite_impl( this ); }

  const CountPtr< const Expression > condition;
  const CountPtr< const Expression > then_value;
  const CountPtr< const Expression > else_value;
};

class TypeExpression : public Expression
{
public:
  TypeExpression( const Location *_loc, Type _type, const TypeExpression *_of = 0, const DimensionsExpression *_dims = 0 )
  : Expression( _loc )
  , of( _of )
  , dims( _dims )
  , type( _type )
  {}
  virtual ~TypeExpression( void ) {}

  virtual bool invite( Visitor *visitor ) const { return visitor->invite_impl( this ); }

  const CountPtr< const TypeExpression > of;
  const CountPtr< const DimensionsExpression > dims;
  Type type;
};

class DimensionsExpression : public Expression
{
public:
  DimensionsExpression( const Location *_loc )
  : Expression( _loc )
  {}
  virtual ~DimensionsExpression( void ) {}

  virtual bool invite( Visitor *visitor ) const { return visitor->invite_impl( this ); }

  void push_back( const Expression *dim )
  {
    dims.push_back( CountPtr< const Expression >( dim ) );
  }

  std::vector< CountPtr< const Expression > > dims;
};

//// Statements ////


class Statement : public Node
{
public:
  Statement( const Location *_loc ) : Node( _loc ) {}
  virtual ~Statement( void ) {}
  virtual Statement *getStatement( void ) { return this; }
};

class CompoundStatement : public Statement
{
public:
  explicit
  CompoundStatement( const Location *_loc )
  : Statement( _loc )
  , own_frame( true )
  {}

  virtual ~CompoundStatement( void ) {}

  virtual CompoundStatement *getCompoundStatement ( void ) { return this; }

  virtual bool invite( Visitor *visitor ) const { return visitor->invite_impl( this ); }

  void append( const Statement *statement )
  {
    statements.push_back( statement );
  }

  std::vector< CountPtr< const Statement > > statements;
  bool own_frame;
};

class FunctionDefinitionStatement : public Statement
{
public:
  class ArgDecl;
  class ArgDeclList;

  FunctionDefinitionStatement( const Location *_loc, const String &_identifier, const ArgDeclList *_args, const CompoundStatement *_body )
  : Statement( _loc )
  , identifier( _identifier )
  , args( _args )
  , body( _body )
  {}

  virtual ~FunctionDefinitionStatement( void ) {}

  virtual bool invite( Visitor *visitor ) const { return visitor->invite_impl( this ); }

  class ArgDecl : public Refcounted
  {
  public:
    explicit
    ArgDecl( const Location *_loc, const String &_identifier, const Expression *_default_value=0 )
    : loc( _loc )
    , identifier( _identifier )
    , default_value( _default_value )
    {}
    virtual ~ArgDecl( void ) {}

    const CountPtr< const Location > loc;
    const String identifier;
    const CountPtr< const Expression > default_value;
  };

  class ArgDeclList : public Refcounted, public std::vector< CountPtr< const ArgDecl > >
  {
  public:
    ArgDeclList( void )
    {}

    virtual ~ArgDeclList( void ) {}

    void append( const ArgDecl *decl )
    {
      push_back( decl );
    }
  };

  const String identifier;
  const CountPtr< const ArgDeclList > args;
  const CountPtr< const CompoundStatement > body;
};

class ConnectStatement : public Statement
{
public:
  ConnectStatement ( const Location *_loc, const Expression *_output, const Expression *_input )
  : Statement( _loc )
  , output( _output )
  , input( _input )
  {}
  virtual ~ConnectStatement ( void ) {}

  virtual bool invite( Visitor *visitor ) const { return visitor->invite_impl( this ); }

  const CountPtr< const Expression > output;
  const CountPtr< const Expression > input;
};

class AssignmentStatement : public Statement
{
public:
  AssignmentStatement( const Location *_loc, ASSIGN _op, const Expression *_value )
  : Statement( _loc )
  , value( _value )
  , op( _op )
  {}
  virtual ~AssignmentStatement( void ) {}

  virtual bool invite( Visitor *visitor ) const { return visitor->invite_impl( this ); }

  const CountPtr< const Expression > value;
  const ASSIGN op;
};

class AssignIdentifierStatement : public AssignmentStatement
{
public:
  AssignIdentifierStatement( const Location *_loc, const String &_identifier, ASSIGN _op, const Expression *_value )
  : AssignmentStatement( _loc, _op, _value )
  , identifier( _identifier )
  {}
  virtual ~AssignIdentifierStatement( void ) {}

  virtual bool invite( Visitor *visitor ) const { return visitor->invite_impl( this ); }

  const String identifier;
};

class AssignDotStatement : public AssignmentStatement
{
public:
  AssignDotStatement( const Location *_loc, Expression *_left, const String &_identifier, ASSIGN _op, const Expression *_value )
  : AssignmentStatement( _loc, _op, _value )
  , left( _left )
  , identifier( _identifier )
  {}
  virtual ~AssignDotStatement( void ) {}

  virtual bool invite( Visitor *visitor ) const { return visitor->invite_impl( this ); }

  const CountPtr< const Expression > left;
  const String identifier;
};

class AssignBracketStatement : public AssignmentStatement
{
public:
  AssignBracketStatement( const Location *_loc, const Expression *_left, const CoordinatesExpression *_coord, ASSIGN _op, const Expression *_value )
  : AssignmentStatement( _loc, _op, _value )
  , left( _left )
  , coord( _coord )
  {}
  virtual ~AssignBracketStatement( void ) {}

  virtual bool invite( Visitor *visitor ) const { return visitor->invite_impl( this ); }

  const CountPtr< const Expression > left;
  const CountPtr< const CoordinatesExpression > coord;
};

class IfStatement : public Statement
{
public:
  IfStatement( const Location *_loc, const Expression *_condition, const Statement *_then_body, const Statement *_else_body=0 )
  : Statement( _loc )
  , condition( _condition )
  , then_body( _then_body )
  , else_body( _else_body )
  {}
  virtual ~IfStatement( void ) {}

  virtual bool invite( Visitor *visitor ) const { return visitor->invite_impl( this ); }

  const CountPtr< const Expression > condition;
  const CountPtr< const Statement > then_body;
  const CountPtr< const Statement > else_body;
};

class NOPStatement : public Statement
{
public:
  NOPStatement( const Location *_loc )
  : Statement( _loc )
  {}
  virtual ~NOPStatement( void ) {}

  virtual bool invite( Visitor *visitor ) const { return visitor->invite_impl( this ); }
};

class ForStatement : public Statement
{
public:
  ForStatement( const Location *_loc, const String &_identifier, const Statement *_body )
  : Statement( _loc )
  , identifier( _identifier )
  , body( _body )
  {}
  virtual ~ForStatement( void ) {}

  virtual bool invite( Visitor *visitor ) const { return visitor->invite_impl( this ); }

  const String identifier;
  const CountPtr< const Statement > body;
};

class ForSequenceStatement : public ForStatement
{
public:
  ForSequenceStatement( const Location *_loc, const String &_identifier, const SequenceExpression *_sequence, const Statement *_statement )
  : ForStatement( _loc, _identifier, _statement )
  , sequence( _sequence )
  {}
  virtual ~ForSequenceStatement( void ) {}

  virtual bool invite( Visitor *visitor ) const { return visitor->invite_impl( this ); }

  const CountPtr< const SequenceExpression > sequence;
};

class ForContainerStatement : public ForStatement
{
public:
  ForContainerStatement( const Location *_loc, const String &_identifier, const Expression *_container, const Statement *_statement )
  : ForStatement( _loc,_identifier, _statement )
  , container( _container )
  {}
  virtual ~ForContainerStatement( void ) {}

  virtual bool invite( Visitor *visitor ) const { return visitor->invite_impl( this ); }

  const CountPtr< const Expression > container;
};

class FunctionCallStatement : public Statement
{
public:
  explicit
  FunctionCallStatement( const Location *_loc, const Expression *_call )
  : Statement( _loc )
  , call( _call )
  {}
  virtual ~FunctionCallStatement( void ) {}

  virtual bool invite( Visitor *visitor ) const { return visitor->invite_impl( this ); }

  const CountPtr< const Expression > call;
};

class VariableCreationStatement : public Statement
{
public:
  VariableCreationStatement( const Location *_loc, const TypeExpression *_type, const String &_identifier, const Expression *_value )
  : Statement( _loc )
  , identifier( _identifier )
  , value( _value )
  , type( _type )
  {}
  virtual ~VariableCreationStatement( void ) {}

  virtual bool invite( Visitor *visitor ) const { return visitor->invite_impl( this ); }

  const String identifier;
  const CountPtr< const Expression > value;
  const CountPtr< const TypeExpression > type;
};

class ReturnStatement : public Statement
{
public:
  ReturnStatement( const Location *_loc, const Expression *_value )
  : Statement( _loc )
  , value( _value )
  {}
  virtual ~ReturnStatement( void ) {}

  virtual bool invite( Visitor *visitor ) const { return visitor->invite_impl( this ); }

  const CountPtr< const Expression > value;
};

} // namespace AST
} // namespace RPGML

#endif


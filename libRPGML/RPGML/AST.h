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

#include "GarbageCollector.h"
#include "String.h"
#include "ParserEnums.h"
#include "Value.h"
#include "Array.h"
#include "Location.h"
#include "ParseException.h"

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
class CastExpression;

class CompoundStatement;
class AssignmentStatementBase;
class AssignIdentifierStatement;
class AssignDotStatement;
class AssignBracketStatement;

// maybe expression
class FunctionDefinitionStatement;
class VariableCreationStatement;
class VariableConstructionStatement;

class Statement;
class ExpressionStatement;
class ConnectStatement;
class IfStatement;
class NOPStatement;
class ForStatement;
class ForSequenceStatement;
class ForContainerStatement;
class ReturnStatement;

class Node : public Collectable
{
  typedef Collectable Base;
public:
  Node( GarbageCollector *_gc, const Location *_loc )
  : Base( _gc )
  , loc( _loc )
  {}
  virtual ~Node( void ) {}

  // Implement using visitor->invite_impl( this )
  virtual void invite( Visitor *visitor ) const = 0;

  // Overloads for "is a"
  virtual Expression         *getExpression        ( void ) { return 0; }
  virtual SequenceExpression *getSequenceExpression( void ) { return 0; }
  virtual Statement          *getStatement         ( void ) { return 0; }
  virtual CompoundStatement  *getCompoundStatement ( void ) { return 0; }

  CountPtr< const Location > loc;
};

class Visitor
{
public:
  explicit
  Visitor( const Location *call_loc=0, index_t _recursion_depth=0 )
  : m_call_loc( call_loc )
  , m_rd( _recursion_depth )
  {}
  virtual ~Visitor( void ) {}

  virtual void visit( const Node *node )
  {
    node->invite( this );
  }

  virtual void visit( const ConstantExpression           *node ) = 0;
  virtual void visit( const ThisExpression               *node ) = 0;
  virtual void visit( const ArrayConstantExpression      *node ) = 0;
  virtual void visit( const FrameConstantExpression      *node ) = 0;
  virtual void visit( const ParenthisSequenceExpression  *node ) = 0;
  virtual void visit( const ExpressionSequenceExpression *node ) = 0;
  virtual void visit( const FromToStepSequenceExpression *node ) = 0;
  virtual void visit( const LookupVariableExpression     *node ) = 0;
  virtual void visit( const FunctionCallExpression       *node ) = 0;
  virtual void visit( const DotExpression                *node ) = 0;
  virtual void visit( const FrameAccessExpression        *node ) = 0;
  virtual void visit( const ArrayAccessExpression        *node ) = 0;
  virtual void visit( const UnaryExpression              *node ) = 0;
  virtual void visit( const BinaryExpression             *node ) = 0;
  virtual void visit( const IfThenElseExpression         *node ) = 0;
  virtual void visit( const TypeExpression               *node ) = 0;
  virtual void visit( const DimensionsExpression         *node ) = 0;
  virtual void visit( const CastExpression               *node ) = 0;

  virtual void visit( const CompoundStatement            *node ) = 0;
  virtual void visit( const FunctionDefinitionStatement  *node ) = 0;
  virtual void visit( const ConnectStatement             *node ) = 0;
  virtual void visit( const AssignIdentifierStatement    *node ) = 0;
  virtual void visit( const AssignDotStatement           *node ) = 0;
  virtual void visit( const AssignBracketStatement       *node ) = 0;
  virtual void visit( const IfStatement                  *node ) = 0;
  virtual void visit( const NOPStatement                 *node ) = 0;
  virtual void visit( const ForSequenceStatement         *node ) = 0;
  virtual void visit( const ForContainerStatement        *node ) = 0;
  virtual void visit( const ExpressionStatement          *node ) = 0;
  virtual void visit( const VariableCreationStatement    *node ) = 0;
  virtual void visit( const VariableConstructionStatement*node ) = 0;
  virtual void visit( const ReturnStatement              *node ) = 0;

  template< class NodeType >
  void invite_impl( const NodeType *node )
  {
    try
    {
      ++m_rd;
      if( m_rd > 100 ) throw "Maximum recursion depth reached";
      visit( node );
      --m_rd;
    }
    catch( const CallLocException & )
    {
      throw;
    }
    catch( const RPGML::Exception &e )
    {
      // Includes ParseException
      throw CallLocException( new Location( node->loc, getCallLoc() ), e );
    }
    catch( const char *e )
    {
      throw ParseException( new Location( node->loc, m_call_loc ), e );
    }
    catch( const std::exception &e )
    {
      throw ParseException( new Location( node->loc, m_call_loc ) ) << e.what();
    }
    catch( ... )
    {
      throw ParseException( new Location( node->loc, m_call_loc ), "Caught some unknown exception" );
    }
  }

  index_t getRD( void ) const
  {
    return m_rd;
  }

  const Location *getCallLoc( void ) const
  {
    return m_call_loc;
  }

protected:
  class CallLoc
  {
  public:
    CallLoc( Visitor *parent, const Location *new_call_loc )
    : m_parent( parent )
    {
      m_parent->m_call_loc = new Location( new_call_loc, m_parent->m_call_loc );
    }
    ~CallLoc( void )
    {
      m_parent->m_call_loc = m_parent->m_call_loc->getParent();
    }
  private:
    Visitor *m_parent;
  };

  class CallLocException : public ParseException
  {
    typedef ParseException Base;
  public:
    explicit
    CallLocException( const Location *call_loc )
    : ParseException( call_loc )
    {}
    CallLocException( const Location *call_loc, const RPGML::Exception &e )
    : ParseException( call_loc, e )
    {}
    EXCEPTION_BODY( CallLocException )
  };

private:
  friend class CallLoc;
  CountPtr< const Location > m_call_loc;
  index_t m_rd; //!< Current recursion depth (getRD())
};

class Expression : public Node
{
public:
  Expression( GarbageCollector *_gc, const Location *_loc ) : Node( _gc, _loc ) {}
  virtual ~Expression( void ) {}
  virtual Expression *getExpression( void ) { return this; }
};

class ConstantExpression : public Expression
{
  typedef Expression Base;
public:
  explicit ConstantExpression( GarbageCollector *_gc, const Location *_loc, const Value &v )
  : Expression( _gc, _loc )
  , value( v )
  {}
  virtual ~ConstantExpression( void ) {}

  virtual void gc_clear( void )
  {
    Base::gc_clear();
    value.clear();
  }

  virtual void gc_getChildren( Children &children ) const
  {
    Base::gc_getChildren( children );
    children << value;
  }

  virtual void invite( Visitor *visitor ) const { visitor->invite_impl( this ); }

  Value value;
};

class ThisExpression : public Expression
{
  typedef Expression Base;
public:
  explicit ThisExpression( GarbageCollector *_gc, const Location *_loc ) : Expression( _gc, _loc ) {}
  virtual ~ThisExpression( void ) {}

  virtual void invite( Visitor *visitor ) const { visitor->invite_impl( this ); }

  virtual void gc_clear( void ) { Base::gc_clear(); }
  virtual void gc_getChildren( Children &children ) const { Base::gc_getChildren( children ); }
};

class ArrayConstantExpression : public Expression
{
  typedef Expression Base;
public:
  typedef Array< CountPtr< const SequenceExpression > > SequenceExpressionArray;
  typedef Array< CountPtr< const ArrayBase          > > ArrayBaseArray;

  explicit
  ArrayConstantExpression( GarbageCollector *_gc, const Location *_loc, const ArrayBase *_descr_array, int _dims )
  : Expression( _gc, _loc )
  , descr_array( _descr_array )
  , dims( _dims )
  {}

  virtual ~ArrayConstantExpression( void ) {}

  virtual void invite( Visitor *visitor ) const { visitor->invite_impl( this ); }

  virtual void gc_clear( void )
  {
    Base::gc_clear();
    descr_array.reset();
  }

  virtual void gc_getChildren( Children &children ) const
  {
    Base::gc_getChildren( children );
    children << descr_array;
  }

  CountPtr< const ArrayBase > descr_array;
  int dims;
};

class FrameConstantExpression : public Expression
{
  typedef Expression Base;
public:
  FrameConstantExpression( GarbageCollector *_gc, const Location *_loc, const CompoundStatement *_body )
  : Expression( _gc, _loc )
  , body( _body )
  {}

  virtual ~FrameConstantExpression( void ) {}

  virtual void invite( Visitor *visitor ) const { visitor->invite_impl( this ); }

  virtual void gc_clear( void )
  {
    Base::gc_clear();
    body.reset();
  }

  virtual void gc_getChildren( Children &children ) const
  {
    Base::gc_getChildren( children );
    children << body;
  }

  CountPtr< const CompoundStatement > body;
};

class SequenceExpression : public Expression
{
  typedef Expression Base;
public:
  SequenceExpression( GarbageCollector *_gc, const Location *_loc )
  : Expression( _gc, _loc )
  {}
  virtual ~SequenceExpression( void ) {}
  virtual SequenceExpression *getSequenceExpression( void ) { return this; }
  virtual index_t length( void ) const = 0;
  virtual const Expression *first( void ) const = 0;
};

class ExpressionSequenceExpression : public SequenceExpression
{
  typedef SequenceExpression Base;
public:
  ExpressionSequenceExpression( GarbageCollector *_gc, const Location *_loc )
  : SequenceExpression( _gc, _loc )
  {}
  virtual ~ExpressionSequenceExpression( void ) {}

  virtual void invite( Visitor *visitor ) const { visitor->invite_impl( this ); }

  void append( const CountPtr< const Expression > &expression )
  {
    if( !expression.isNull() )
    {
      expressions.push_back( expression );
    }
    else
    {
      throw Exception()
        << "Internal: expression is Null at" << "\n"
        << Backtrace()
        ;
    }
  }

  virtual index_t length( void ) const { return index_t( expressions.size() ); }

  virtual const Expression *first( void ) const
  {
    if( expressions.empty() )
    {
      return 0;
    }
    else
    {
      return expressions.front();
    }
  }

  typedef std::vector< CountPtr< const Expression > > expressions_t;

  virtual void gc_clear( void )
  {
    Base::gc_clear();
    expressions.clear();
  }

  virtual void gc_getChildren( Children &children ) const
  {
    Base::gc_getChildren( children );
    for( const auto &i : expressions )
    {
      children << i;
    }
  }

  expressions_t expressions;
};

class FromToStepSequenceExpression : public SequenceExpression
{
  typedef SequenceExpression Base;
public:
  FromToStepSequenceExpression( GarbageCollector *_gc, const Location *_loc, const Expression *_from, const Expression *_to, const Expression *_step = 0 )
  : SequenceExpression( _gc, _loc )
  , from( _from )
  , to( _to )
  , step( _step )
  {}

  virtual ~FromToStepSequenceExpression( void ) {}

  virtual void invite( Visitor *visitor ) const { visitor->invite_impl( this ); }

  virtual index_t length( void ) const { return unknown; }

  virtual const Expression *first( void ) const
  {
    return from;
  }

  virtual void gc_clear( void )
  {
    Base::gc_clear();
    from.reset();
    to.reset();
    step.reset();
  }

  virtual void gc_getChildren( Children &children ) const
  {
    Base::gc_getChildren( children );
    children
      << from
      << to
      << step
      ;
  }

  CountPtr< const Expression > from;
  CountPtr< const Expression > to;
  CountPtr< const Expression > step;
};

class ParenthisSequenceExpression : public Expression
{
  typedef Expression Base;
public:
  explicit
  ParenthisSequenceExpression( GarbageCollector *_gc, const Location *_loc, const SequenceExpression *_sequence=0 )
  : Expression( _gc, _loc )
  , sequence( _sequence )
  {}
  virtual ~ParenthisSequenceExpression( void ) {}

  virtual void invite( Visitor *visitor ) const { visitor->invite_impl( this ); }

  virtual index_t length( void ) const { return sequence->length(); }

  virtual const Expression *first( void ) const
  {
    return sequence->first();
  }

  virtual void gc_clear( void )
  {
    Base::gc_clear();
    sequence.reset();
  }

  virtual void gc_getChildren( Children &children ) const
  {
    Base::gc_getChildren( children );
    children << sequence;
  }

  CountPtr< const SequenceExpression > sequence;
};

class LookupVariableExpression : public Expression
{
  typedef Expression Base;
public:
  LookupVariableExpression( GarbageCollector *_gc, const Location *_loc, const String &_identifier, bool _at_root = false )
  : Expression( _gc, _loc )
  , identifier( _identifier )
  , at_root( _at_root )
  {}
  virtual ~LookupVariableExpression( void ) {}

  virtual void invite( Visitor *visitor ) const { visitor->invite_impl( this ); }

  virtual void gc_clear( void ) { Base::gc_clear(); }
  virtual void gc_getChildren( Children &children ) const { Base::gc_getChildren( children ); }

  const String identifier;
  bool at_root;
};

class FunctionCallExpression : public Expression
{
  typedef Expression Base;
public:
  class Args;

  FunctionCallExpression( GarbageCollector *_gc, const Location *_loc, const Expression *_function, const Args *_args )
  : Expression( _gc, _loc )
  , function( _function )
  , args( _args )
  {}
  virtual ~FunctionCallExpression( void ) {}

  virtual void invite( Visitor *visitor ) const { visitor->invite_impl( this ); }

  class Arg : public Collectable
  {
    typedef Collectable Base;
  public:
    explicit
    Arg( GarbageCollector *_gc, const Location *_loc, const Expression *_value, const String &_identifier = String() )
    : Base( _gc )
    , loc( _loc )
    , value( _value )
    , identifier( _identifier )
    {}
    virtual ~Arg( void ) {}

    virtual void gc_clear( void )
    {
      Base::gc_clear();
      value.reset();
    }

    virtual void gc_getChildren( Children &children ) const
    {
      Base::gc_getChildren( children );
      children << value;
    }

    CountPtr< const Location > loc;
    CountPtr< const Expression > value;
    const String identifier;
  };

  class Args : public Array< CountPtr< Arg > >
  {
    typedef Array< CountPtr< Arg > > Base;
  public:
    Args( GarbageCollector *_gc ) : Base( _gc, 1 ) {}
    virtual ~Args( void ) {}

    void append( Arg *arg )
    {
      push_back( arg );
    }
  };

  virtual void gc_clear( void )
  {
    Base::gc_clear();
    function.reset();
    args.reset();
  }

  virtual void gc_getChildren( Children &children ) const
  {
    Base::gc_getChildren( children );
    children
      << function
      << args
      ;
  }

  CountPtr< const Expression > function;
  CountPtr< const Args > args;
};

class DotExpression : public Expression
{
  typedef Expression Base;
public:
  DotExpression( GarbageCollector *_gc, const Location *_loc, Expression *_left, const String &_member )
  : Expression( _gc, _loc )
  , left( _left )
  , member( _member )
  {}
  virtual ~DotExpression( void ) {}

  virtual void invite( Visitor *visitor ) const { visitor->invite_impl( this ); }

  virtual void gc_clear( void )
  {
    Base::gc_clear();
    left.reset();
  }

  virtual void gc_getChildren( Children &children ) const
  {
    Base::gc_getChildren( children );
    children
      << left
      ;
  }

  CountPtr< const Expression > left;
  const String member;
};

class FrameAccessExpression : public Expression
{
  typedef Expression Base;
public:
  FrameAccessExpression( GarbageCollector *_gc, const Location *_loc, const Expression *_left, const String &_identifier )
  : Expression( _gc, _loc )
  , left( _left )
  , identifier( _identifier )
  {}
  virtual ~FrameAccessExpression( void ) {}

  virtual void invite( Visitor *visitor ) const { visitor->invite_impl( this ); }

  virtual void gc_clear( void )
  {
    Base::gc_clear();
    left.reset();
  }

  virtual void gc_getChildren( Children &children ) const
  {
    Base::gc_getChildren( children );
    children
      << left
      ;
  }

  CountPtr< const Expression > left;
  const String identifier;
};

class ArrayAccessExpression : public Expression
{
  typedef Expression Base;
public:
  ArrayAccessExpression( GarbageCollector *_gc, const Location *_loc, const Expression *_left, const CoordinatesExpression *_coord )
  : Expression( _gc, _loc )
  , left( _left )
  , coord( _coord )
  {}
  virtual ~ArrayAccessExpression( void ) {}

  virtual void invite( Visitor *visitor ) const { visitor->invite_impl( this ); }

  virtual void gc_clear( void )
  {
    Base::gc_clear();
    left.reset();
    coord.reset();
  }

  virtual void gc_getChildren( Children &children ) const
  {
    Base::gc_getChildren( children );
    children
      << left
      << coord
      ;
  }

  CountPtr< const Expression > left;
  CountPtr< const CoordinatesExpression > coord;
};

class UnaryExpression : public Expression
{
  typedef Expression Base;
public:
  UnaryExpression( GarbageCollector *_gc, const Location *_loc, UOP _op, const Expression *_arg )
  : Expression( _gc, _loc )
  , arg( _arg )
  , op( _op )
  {}
  virtual ~UnaryExpression( void ) {}

  virtual void invite( Visitor *visitor ) const { visitor->invite_impl( this ); }

  virtual void gc_clear( void )
  {
    Base::gc_clear();
    arg.reset();
  }

  virtual void gc_getChildren( Children &children ) const
  {
    Base::gc_getChildren( children );
    children
      << arg
      ;
  }

  CountPtr< const Expression > arg;
  const UOP op;
};

class BinaryExpression : public Expression
{
  typedef Expression Base;
public:
  BinaryExpression( GarbageCollector *_gc, const Location *_loc, const Expression *_left, BOP _op, const Expression *_right )
  : Expression( _gc, _loc )
  , left ( _left  )
  , right( _right )
  , op( _op )
  {}
  virtual ~BinaryExpression( void ) {}

  virtual void invite( Visitor *visitor ) const { visitor->invite_impl( this ); }

  virtual void gc_clear( void )
  {
    Base::gc_clear();
    left.reset();
    right.reset();
  }

  virtual void gc_getChildren( Children &children ) const
  {
    Base::gc_getChildren( children );
    children
      << left
      << right
      ;
  }

  CountPtr< const Expression > left;
  CountPtr< const Expression > right;
  const BOP op;
};

class IfThenElseExpression : public Expression
{
  typedef Expression Base;
public:
  IfThenElseExpression( GarbageCollector *_gc, const Location *_loc, const Expression *_condition, const Expression *_then_value, const Expression *_else_value )
  : Expression( _gc, _loc )
  , condition( _condition )
  , then_value( _then_value )
  , else_value( _else_value )
  {}
  virtual ~IfThenElseExpression( void ) {}

  virtual void invite( Visitor *visitor ) const { visitor->invite_impl( this ); }

  virtual void gc_clear( void )
  {
    Base::gc_clear();
    condition.reset();
    then_value.reset();
    else_value.reset();
  }

  virtual void gc_getChildren( Children &children ) const
  {
    Base::gc_getChildren( children );
    children
      << condition
      << then_value
      << else_value
      ;
  }

  CountPtr< const Expression > condition;
  CountPtr< const Expression > then_value;
  CountPtr< const Expression > else_value;
};

class TypeExpression : public Expression
{
  typedef Expression Base;
public:
  TypeExpression( GarbageCollector *_gc, const Location *_loc, Type _type, const TypeExpression *_of = 0, const DimensionsExpression *_dims = 0 )
  : Expression( _gc, _loc )
  , of( _of )
  , dims( _dims )
  , type( _type )
  {}
  virtual ~TypeExpression( void ) {}

  virtual void invite( Visitor *visitor ) const { visitor->invite_impl( this ); }

  virtual void gc_clear( void )
  {
    Base::gc_clear();
    of.reset();
    dims.reset();
  }

  virtual void gc_getChildren( Children &children ) const
  {
    Base::gc_getChildren( children );
    children
      << of
      << dims
      ;
  }

  CountPtr< const TypeExpression > of;
  CountPtr< const DimensionsExpression > dims;
  Type type;
};

class DimensionsExpression : public Expression
{
  typedef Expression Base;
public:
  DimensionsExpression( GarbageCollector *_gc, const Location *_loc )
  : Expression( _gc, _loc )
  {}
  virtual ~DimensionsExpression( void ) {}

  virtual void invite( Visitor *visitor ) const { visitor->invite_impl( this ); }

  void push_back( const Expression *dim )
  {
    dims.push_back( CountPtr< const Expression >( dim ) );
  }

  typedef std::vector< CountPtr< const Expression > > dims_t;

  virtual void gc_clear( void )
  {
    Base::gc_clear();
    dims.clear();
  }

  virtual void gc_getChildren( Children &children ) const
  {
    Base::gc_getChildren( children );
    for( dims_t::const_iterator i( dims.begin() ), end( dims.end() ); i != end; ++i )
    {
      children << (*i);
    }
  }

  std::vector< CountPtr< const Expression > > dims;
};

class CastExpression : public Expression
{
  typedef Expression Base;
public:
  CastExpression( GarbageCollector *_gc, const Location *_loc, Type _to, const Expression *_arg )
  : Expression( _gc, _loc )
  , arg( _arg )
  , to( _to )
  {}
  virtual ~CastExpression( void ) {}

  virtual void invite( Visitor *visitor ) const { visitor->invite_impl( this ); }

  virtual void gc_clear( void )
  {
    Base::gc_clear();
    arg.reset();
  }

  virtual void gc_getChildren( Children &children ) const
  {
    Base::gc_getChildren( children );
    children
      << arg
      ;
  }

  CountPtr< const Expression > arg;
  Type to;
};

//// Statements ////


class Statement : public Node
{
public:
  Statement( GarbageCollector *_gc, const Location *_loc ) : Node( _gc, _loc ) {}
  virtual ~Statement( void ) {}
  virtual Statement *getStatement( void ) { return this; }
};

class CompoundStatement : public Statement
{
  typedef Statement Base;
public:
  explicit
  CompoundStatement( GarbageCollector *_gc, const Location *_loc )
  : Statement( _gc, _loc )
  , creates_own_frame( true )
  {}

  virtual ~CompoundStatement( void ) {}

  virtual CompoundStatement *getCompoundStatement ( void ) { return this; }

  virtual void invite( Visitor *visitor ) const { visitor->invite_impl( this ); }

  void append( const Statement *statement )
  {
    statements.push_back( statement );
  }

  virtual void gc_clear( void )
  {
    Base::gc_clear();
    statements.clear();
  }

  virtual void gc_getChildren( Children &children ) const
  {
    Base::gc_getChildren( children );
    for( auto &i : statements )
    {
      children << i;
    }
  }

  typedef std::vector< CountPtr< const Statement > > statements_t;
  statements_t statements;
  bool creates_own_frame;
};

class FunctionDefinitionStatement : public Statement
{
  typedef Statement Base;
public:
  class ArgDecl;
  class ArgDeclList;

  FunctionDefinitionStatement(
      GarbageCollector *_gc
    , const Location *_loc
    , const TypeExpression *_ret
    , const String &_identifier
    , const ArgDeclList *_args
    , const CompoundStatement *_body
    , bool _is_method = false
    )
  : Statement( _gc, _loc )
  , ret( _ret )
  , identifier( _identifier )
  , args( _args )
  , body( _body )
  , is_method( _is_method )
  {}

  virtual ~FunctionDefinitionStatement( void ) {}

  virtual void invite( Visitor *visitor ) const { visitor->invite_impl( this ); }

  class ArgDecl : public Collectable
  {
    typedef Collectable Base;
  public:
    explicit
    ArgDecl( GarbageCollector *_gc, const Location *_loc, const String &_identifier, const Expression *_default_value=0 )
    : Base( _gc )
    , loc( _loc )
    , identifier( _identifier )
    , default_value( _default_value )
    {}
    virtual ~ArgDecl( void ) {}

    virtual void gc_clear( void )
    {
      Base::gc_clear();
      default_value.reset();
    }

    virtual void gc_getChildren( Children &children ) const
    {
      Base::gc_getChildren( children );
      children
        << default_value
        ;
    }

    CountPtr< const Location > loc;
    const String identifier;
    CountPtr< const Expression > default_value;
  };

  class ArgDeclList : public Array< CountPtr< const ArgDecl > >
  {
    typedef Array< CountPtr< const ArgDecl > > Base;
  public:
    explicit
    ArgDeclList( GarbageCollector *_gc )
    : Base( _gc, 1 )
    {}

    virtual ~ArgDeclList( void ) {}

    void append( const ArgDecl *decl )
    {
      push_back( decl );
    }
  };

  virtual void gc_clear( void )
  {
    Base::gc_clear();
    ret.reset();
    args.reset();
    body.reset();
  }

  virtual void gc_getChildren( Children &children ) const
  {
    Base::gc_getChildren( children );
    children
      << ret
      << args
      << body
      ;
  }

  CountPtr< const TypeExpression > ret;
  const String identifier;
  CountPtr< const ArgDeclList > args;
  CountPtr< const CompoundStatement > body;
  bool is_method;
};

class ConnectStatement : public Statement
{
  typedef Statement Base;
public:
  ConnectStatement ( GarbageCollector *_gc, const Location *_loc, const Expression *_output, const Expression *_input )
  : Statement( _gc, _loc )
  , output( _output )
  , input( _input )
  {}
  virtual ~ConnectStatement ( void ) {}

  virtual void invite( Visitor *visitor ) const { visitor->invite_impl( this ); }

  virtual void gc_clear( void )
  {
    Base::gc_clear();
    input.reset();
    output.reset();
  }

  virtual void gc_getChildren( Children &children ) const
  {
    Base::gc_getChildren( children );
    children
      << input
      << output
      ;
  }

  CountPtr< const Expression > output;
  CountPtr< const Expression > input;
};

class AssignmentStatementBase : public Statement
{
  typedef Statement Base;
public:
  AssignmentStatementBase( GarbageCollector *_gc, const Location *_loc, ASSIGN _op, const Expression *_value )
  : Statement( _gc, _loc )
  , value( _value )
  , op( _op )
  {}
  virtual ~AssignmentStatementBase( void ) {}

  virtual void invite( Visitor *visitor ) const { visitor->invite_impl( this ); }

  virtual void gc_clear( void )
  {
    Base::gc_clear();
    value.reset();
  }

  virtual void gc_getChildren( Children &children ) const
  {
    Base::gc_getChildren( children );
    children
      << value
      ;
  }

  CountPtr< const Expression > value;
  const ASSIGN op;
};

class AssignIdentifierStatement : public AssignmentStatementBase
{
  typedef AssignmentStatementBase Base;
public:
  AssignIdentifierStatement( GarbageCollector *_gc, const Location *_loc, const String &_identifier, ASSIGN _op, const Expression *_value )
  : AssignmentStatementBase( _gc, _loc, _op, _value )
  , identifier( _identifier )
  {}
  virtual ~AssignIdentifierStatement( void ) {}

  virtual void invite( Visitor *visitor ) const { visitor->invite_impl( this ); }

  virtual void gc_clear( void ) { Base::gc_clear(); }
  virtual void gc_getChildren( Children &children ) const { Base::gc_getChildren( children ); }

  const String identifier;
};

class AssignDotStatement : public AssignmentStatementBase
{
  typedef AssignmentStatementBase Base;
public:
  AssignDotStatement( GarbageCollector *_gc, const Location *_loc, const Expression *_left, const String &_identifier, ASSIGN _op, const Expression *_value )
  : AssignmentStatementBase( _gc, _loc, _op, _value )
  , left( _left )
  , identifier( _identifier )
  {}
  virtual ~AssignDotStatement( void ) {}

  virtual void invite( Visitor *visitor ) const { visitor->invite_impl( this ); }

  virtual void gc_clear( void )
  {
    Base::gc_clear();
    left.reset();
  }

  virtual void gc_getChildren( Children &children ) const
  {
    Base::gc_getChildren( children );
    children
      << left
      ;
  }

  CountPtr< const Expression > left;
  const String identifier;
};

class AssignBracketStatement : public AssignmentStatementBase
{
  typedef AssignmentStatementBase Base;
public:
  AssignBracketStatement( GarbageCollector *_gc, const Location *_loc, const Expression *_left, const CoordinatesExpression *_coord, ASSIGN _op, const Expression *_value )
  : AssignmentStatementBase( _gc, _loc, _op, _value )
  , left( _left )
  , coord( _coord )
  {}
  virtual ~AssignBracketStatement( void ) {}

  virtual void invite( Visitor *visitor ) const { visitor->invite_impl( this ); }

  virtual void gc_clear( void )
  {
    Base::gc_clear();
    left.reset();
    coord.reset();
  }

  virtual void gc_getChildren( Children &children ) const
  {
    Base::gc_getChildren( children );
    children
      << left
      << coord
      ;
  }

  CountPtr< const Expression > left;
  CountPtr< const CoordinatesExpression > coord;
};

class IfStatement : public Statement
{
  typedef Statement Base;
public:
  IfStatement( GarbageCollector *_gc, const Location *_loc, const Expression *_condition, const Statement *_then_body, const Statement *_else_body=0 )
  : Statement( _gc, _loc )
  , condition( _condition )
  , then_body( _then_body )
  , else_body( _else_body )
  {}
  virtual ~IfStatement( void ) {}

  virtual void invite( Visitor *visitor ) const { visitor->invite_impl( this ); }

  virtual void gc_clear( void )
  {
    Base::gc_clear();
    condition.reset();
    then_body.reset();
    else_body.reset();
  }

  virtual void gc_getChildren( Children &children ) const
  {
    Base::gc_getChildren( children );
    children
      << condition
      << then_body
      << else_body
      ;
  }

  CountPtr< const Expression > condition;
  CountPtr< const Statement > then_body;
  CountPtr< const Statement > else_body;
};

class NOPStatement : public Statement
{
  typedef Statement Base;
public:
  NOPStatement( GarbageCollector *_gc, const Location *_loc )
  : Statement( _gc, _loc )
  {}
  virtual ~NOPStatement( void ) {}

  virtual void gc_clear( void ) { Base::gc_clear(); }
  virtual void gc_getChildren( Children &children ) const {Base::gc_getChildren( children );}

  virtual void invite( Visitor *visitor ) const { visitor->invite_impl( this ); }
};

class ForStatement : public Statement
{
  typedef Statement Base;
public:
  ForStatement( GarbageCollector *_gc, const Location *_loc, const String &_identifier, const Statement *_body )
  : Statement( _gc, _loc )
  , identifier( _identifier )
  , body( _body )
  {}
  virtual ~ForStatement( void ) {}

  virtual void invite( Visitor *visitor ) const { visitor->invite_impl( this ); }

  virtual void gc_clear( void )
  {
    Base::gc_clear();
    body.reset();
  }

  virtual void gc_getChildren( Children &children ) const
  {
    Base::gc_getChildren( children );
    children
      << body
      ;
  }

  const String identifier;
  CountPtr< const Statement > body;
};

class ForSequenceStatement : public ForStatement
{
  typedef ForStatement Base;
public:
  ForSequenceStatement( GarbageCollector *_gc, const Location *_loc, const String &_identifier, const SequenceExpression *_sequence, const Statement *_statement )
  : ForStatement( _gc, _loc, _identifier, _statement )
  , sequence( _sequence )
  {}
  virtual ~ForSequenceStatement( void ) {}

  virtual void invite( Visitor *visitor ) const { visitor->invite_impl( this ); }

  virtual void gc_clear( void )
  {
    Base::gc_clear();
    sequence.reset();
  }

  virtual void gc_getChildren( Children &children ) const
  {
    Base::gc_getChildren( children );
    children
      << sequence
      ;
  }

  CountPtr< const SequenceExpression > sequence;
};

class ForContainerStatement : public ForStatement
{
  typedef ForStatement Base;
public:
  ForContainerStatement( GarbageCollector *_gc, const Location *_loc, const String &_identifier, const Expression *_container, const Statement *_statement )
  : ForStatement( _gc, _loc,_identifier, _statement )
  , container( _container )
  {}
  virtual ~ForContainerStatement( void ) {}

  virtual void invite( Visitor *visitor ) const { visitor->invite_impl( this ); }

  virtual void gc_clear( void )
  {
    Base::gc_clear();
    container.reset();
  }

  virtual void gc_getChildren( Children &children ) const
  {
    Base::gc_getChildren( children );
    children
      << container
      ;
  }

  CountPtr< const Expression > container;
};

class ExpressionStatement : public Statement
{
  typedef Statement Base;
public:
  explicit
  ExpressionStatement( GarbageCollector *_gc, const Location *_loc, const Expression *_expr )
  : Statement( _gc, _loc )
  , expr( _expr )
  {}
  virtual ~ExpressionStatement( void ) {}

  virtual void invite( Visitor *visitor ) const { visitor->invite_impl( this ); }

  virtual void gc_clear( void )
  {
    Base::gc_clear();
    expr.reset();
  }

  virtual void gc_getChildren( Children &children ) const
  {
    Base::gc_getChildren( children );
    children
      << expr
      ;
  }

  CountPtr< const Expression > expr;
};

class VariableCreationStatement : public Statement
{
  typedef Statement Base;
public:
  VariableCreationStatement( GarbageCollector *_gc, const Location *_loc, const TypeExpression *_type, const String &_identifier, const Expression *_value = 0 )
  : Statement( _gc, _loc )
  , identifier( _identifier )
  , value( _value )
  , type( _type )
  {}
  virtual ~VariableCreationStatement( void ) {}

  virtual void invite( Visitor *visitor ) const { visitor->invite_impl( this ); }

  virtual void gc_clear( void )
  {
    Base::gc_clear();
    value.reset();
    type.reset();
  }

  virtual void gc_getChildren( Children &children ) const
  {
    Base::gc_getChildren( children );
    children
      << value
      << type
      ;
  }

  const String identifier;
  CountPtr< const Expression > value;
  CountPtr< const TypeExpression > type;
};

class VariableConstructionStatement : public Statement
{
  typedef Statement Base;
public:
  VariableConstructionStatement( GarbageCollector *_gc, const Location *_loc, const String &_identifier, const FunctionCallExpression *_value )
  : Statement( _gc, _loc )
  , identifier( _identifier )
  , value( _value )
  {}
  virtual ~VariableConstructionStatement( void ) {}

  virtual void invite( Visitor *visitor ) const { visitor->invite_impl( this ); }

  virtual void gc_clear( void )
  {
    Base::gc_clear();
    value.reset();
  }

  virtual void gc_getChildren( Children &children ) const
  {
    Base::gc_getChildren( children );
    children
      << value
      ;
  }

  const String identifier;
  CountPtr< const FunctionCallExpression > value;
};

class ReturnStatement : public Statement
{
  typedef Statement Base;
public:
  ReturnStatement( GarbageCollector *_gc, const Location *_loc, const Expression *_value )
  : Statement( _gc, _loc )
  , value( _value )
  {}
  virtual ~ReturnStatement( void ) {}

  virtual void invite( Visitor *visitor ) const { visitor->invite_impl( this ); }

  virtual void gc_clear( void )
  {
    Base::gc_clear();
    value.reset();
  }

  virtual void gc_getChildren( Children &children ) const
  {
    Base::gc_getChildren( children );
    children
      << value
      ;
  }

  CountPtr< const Expression > value;
};

} // namespace AST
} // namespace RPGML

#endif


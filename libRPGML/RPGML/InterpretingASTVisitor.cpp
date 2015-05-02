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
#include "InterpretingASTVisitor.h"

#include "Refcounted.h"
#include "Array.h"
#include "Sequence.h"
#include "Scope.h"
#include "Function.h"
#include "Frame.h"
#include "Node.h"
#include "InterpretingFunction.h"
#include "ParseException.h"

#include <iostream>

using namespace std;

namespace RPGML {

InterpretingASTVisitor::InterpretingASTVisitor( GarbageCollector *_gc, Scope *_scope, const Location *_call_loc, index_t _recursion_depth )
: Collectable( _gc )
, AST::Visitor( _call_loc, _recursion_depth )
, scope( _scope )
, return_encountered( false )
{}

InterpretingASTVisitor::~InterpretingASTVisitor( void )
{}

void InterpretingASTVisitor::gc_clear( void )
{
  Base::gc_clear();
  scope.reset();
  return_value_type_descr.reset();
  return_value_dims.reset();
  return_value.clear();
}

void InterpretingASTVisitor::gc_getChildren( Children &children ) const
{
  Base::gc_getChildren( children );
  children
    << scope
    << return_value_type_descr
    << return_value_dims
    << return_value
    ;
}

void InterpretingASTVisitor::visit( const AST::ConstantExpression           *node )
{
  return_value = node->value;
}

void InterpretingASTVisitor::visit( const AST::ThisExpression           * )
{
  for( Frame *frame = scope->getCurr(); frame; frame = frame->getParent() )
  {
    if( frame->isThis() )
    {
      return_value = Value( frame );
      return;
    }
  }

  throw Exception() << "No parent Frame that qualifies as 'this'";
  return_value.clear();
}

void InterpretingASTVisitor::determine_size( const ArrayBase *array, int dims, index_t *size, int dim )
{
  if( dim <= 1 )
  {
    const AST::ArrayConstantExpression::SequenceExpressionArray *seq_array = 0;
    if( !array->getAs( seq_array ) )
    {
      throw Exception()
        << "Could not get seq_array"
        << ": dims = " << dims
        << ", array_base->getType() = " << array->getType()
        << ", array_base->getSize() = " << array->getSize()
        << ", dim = " << dim
        ;
    }
    const index_t seq_array_size = seq_array->size();

    if( dims == 1 )
    {
      if( seq_array_size == 1 )
      {
        size[ 0 ] = seq_array->at( 0 )->length();
        if( unknown == size[ 0 ] )
        {
          seq_array->at( 0 )->invite( this );
          Value sequence; sequence.swap( return_value );
          size[ 0 ] = sequence.getSequence()->length();
        }
      }
      else if( seq_array_size == 0 )
      {
        size[ 0 ] = 0;
      }
      else
      {
        throw Exception()
          << "ArrayConstantExpression specified dims = 1"
          << ", but seq_array->size() = " << seq_array_size
          ;
      }
      return;
    }
    else
    {
      if( seq_array_size == 0 )
      {
        size[ 0 ] = 0;
      }
      else
      {
        size[ 0 ] = seq_array->at( 0 )->length();
        if( unknown == size[ 0 ] )
        {
          seq_array->at( 0 )->invite( this );
          Value sequence; sequence.swap( return_value );
          size[ 0 ] = sequence.getSequence()->length();
        }
      }
      size[ 1 ] = seq_array_size;
      return;
    }
  }
  else
  {
    const AST::ArrayConstantExpression::ArrayBaseArray *array_array = 0;
    if( !array->getAs( array_array ) )
    {
      throw Exception()
        << "Could not get array_array"
        << ": dims = " << dims
        << ", array_base->getType() = " << array->getType()
        << ", array_base->getSize() = " << array->getSize()
        << ", dim = " << dim
        ;
    }

    const index_t array_array_size = array_array->size();
    size[ dim ] = array_array_size;

    if( array_array_size > 0 )
    {
      determine_size( array_array->at( 0 ), dims, size, dim-1 );
    }
  }
}

void InterpretingASTVisitor::fill_array( const ArrayBase *array, const ArrayBase::Size &size, Value *dest, int dim )
{
  const int dims = size.getDims();

  if( dim <= 1 )
  {
    const AST::ArrayConstantExpression::SequenceExpressionArray *seq_array = 0;
    if( !array->getAs( seq_array ) )
    {
      throw Exception()
        << "Could not get seq_array"
        << ": dim = " << dim
        << ", array_base->getType() = " << array->getType()
        << ", array_base->getSize() = " << array->getSize()
        ;
    }
    const index_t seq_array_size = seq_array->size();

    if( dims == 1 && seq_array_size > 1 )
    {
      throw Exception()
        << "ArrayConstantExpression specified dims = 1"
        << ", but seq_array->size() = " << seq_array_size
        ;
    }

//    cerr << "size = " << size << endl;

    typedef AST::ArrayConstantExpression::SequenceExpressionArray::ConstIterator E;
    const index_t sizeX = size[ 0 ];
    index_t y = 0;
    for( CountPtr< E > e( seq_array->getIterator() ); !e->done(); e->next(), ++y )
    {
      if( dims > 1 && y >= size[ 1 ] ) goto not_equal_sides;

      const AST::SequenceExpression *const seq = e->get();

      seq->invite( this );
      Value sequence; sequence.swap( return_value );
      if( !sequence.isSequence() )
      {
        throw ParseException( seq->loc )
          << "Is not a Sequence, is a " << sequence.getType()
          ;
      }

      typedef Sequence::Iter I;
      index_t x = 0;
      for( CountPtr< I > i = sequence.getSequence()->getIter(); !i->done(); i->next(), ++x )
      {
        if( x >= size[ 0 ] ) goto not_equal_sides;
        dest[ y*sizeX + x ] = i->get();
      }
      if( x != size[ 0 ] ) goto not_equal_sides;
    }
    if( dims > 1 && y != size[ 1 ] ) goto not_equal_sides;
    return;
  }
  else
  {
    index_t stride = 1;
    for( int d=0; d<dim; ++d )
    {
      stride *= size[ d ];
    }

    const AST::ArrayConstantExpression::ArrayBaseArray *array_array = 0;
    if( !array->getAs( array_array ) )
    {
      throw Exception()
        << "Could not get array_array"
        << ": dims = " << dims
        << ", array_base->getType() = " << array->getType()
        << ", array_base->getSize() = " << array->getSize()
        ;
    }

    const index_t array_array_size = array_array->size();
//    cerr << "array_array_size " << dim << " = " << array_array_size << ", size = " << size << endl;
    if( array_array_size != size[ dim ] ) goto not_equal_sides;

    typedef AST::ArrayConstantExpression::ArrayBaseArray::ConstIterator I;
    index_t s = 0;
    for( CountPtr< I > i( array_array->getIterator() ); !i->done(); i->next(), ++s )
    {
      fill_array( i->get(), size, dest + s*stride, dim-1 );
    }
  }

  return;

not_equal_sides:
  throw Exception()
    << "Not all rows or columns etc. have the same length"
    ;
}

void InterpretingASTVisitor::visit( const AST::ArrayConstantExpression      *node )
{
  const ArrayBase *const array = node->descr_array;

  const int dims = node->dims;
  index_t size_array[ dims ];
  fill( size_array, size_array+dims, index_t( 0 ) );

  determine_size( array, dims, size_array, dims-1 );

  const ArrayBase::Size size( dims, size_array );

//  cerr << "ArrayConstantExpression" << ": determined size " << size << endl ;

  CountPtr< ValueArray > ret = new ValueArray( getGC(), node->dims, size );

  fill_array( array, size, ret->elements(), dims-1 );

  return_value = Value( ret.get() );
}

void InterpretingASTVisitor::visit( const AST::FrameConstantExpression *node )
{
  CountPtr< Frame > ret = new Frame( scope->getGC(), scope->getCurr() );
  ret->setThis( true );
  Scope::EnterLeaveGuard guard( scope, ret );

  node->body->invite( this );

  return_value = Value( ret.get() );
}

void InterpretingASTVisitor::visit( const AST::ParenthisSequenceExpression  *node )
{
  if( node->sequence )
  {
    node->sequence->invite( this );
  }
  else
  {
    return_value = Value( genSequenceFromToStep( getGC(), Value( 1 ), Value( -1 ), Value( 1 ) ) );
  }
}

void InterpretingASTVisitor::visit( const AST::ExpressionSequenceExpression *node )
{
  const index_t n = index_t( node->expressions.size() );

  CountPtr< ValueArray > array = new ValueArray( scope->getGC(), 1 );
  array->resize( n );

  for( index_t i=0; i<n; ++i )
  {
    node->expressions[ i ]->invite( this );
    array->at( i ).swap( return_value );
  }

  return_value = Value( new SequenceValueArray( scope->getGC(), array ) );
}

void InterpretingASTVisitor::visit( const AST::FromToStepSequenceExpression *node )
{
  node->from->invite( this );
  Value from; from.swap( return_value );

  if( !from.isScalar() )
  {
    throw ParseException( node->from->loc )
      << "'from' must be scalar, is " << from.getTypeName()
      ;
  }

  node->to->invite( this );
  Value to; to.swap( return_value );

  if( !to.isScalar() )
  {
    throw ParseException( node->to->loc )
      << "'to' must be scalar, is " << to.getTypeName()
      ;
  }

  Value step;
  if( node->step )
  {
    node->step->invite( this );
    step.swap( return_value );

    if( !step.isScalar() )
    {
      throw ParseException( node->step->loc )
        << "'step' must be scalar, is " << step.getTypeName()
        ;
    }
  }
  else
  {
    // "smallest" possible type
    step = Value( bool( 1 ) );
  }

  return_value.set( genSequenceFromToStep( scope->getGC(), from, to, step ) );
}

void InterpretingASTVisitor::visit( const AST::LookupVariableExpression     *node )
{
  const String identifier =
    ( node->at_root
    ? "." + node->identifier
    : node->identifier
    );

  const Value *const value = scope->lookup( identifier );
  if( value )
  {
//    cerr << "AST::LookupVariableExpression( '" << node->identifier << "' ): value = '" << (*value) << "'" << endl;
    return_value = (*value);
  }
  else
  {
    return_value.clear();
    throw Exception() << "Identifier '" << identifier << "' not found";
  }
}

void InterpretingASTVisitor::visit( const AST::FunctionCallExpression       *node )
{
  node->function->invite( this );
  Value function; function.swap( return_value );

  if( !function.isFunction() )
  {
    throw ParseException( node->function->loc )
      << "Is not a Function, is " << function.getTypeName()
      ;
  }

//  cerr << "calling Function '" << function.getFunction()->getName() << "'" << endl;

  const index_t n_args = node->args->size();
  Function::Args args;
  args.reserve( n_args );

  for( index_t i=0; i < n_args; ++i )
  {
    node->args->at( i )->value->invite( this );
    Value value; value.swap( return_value );
    args.push_back( Function::Arg( node->args->at( i )->identifier, value ) );
  }

  CallLoc guard( this, node->loc );
  return_value = function.getFunction()->call( getCallLoc(), getRD()+1, scope, &args );
}

void InterpretingASTVisitor::visit( const AST::DotExpression                *node )
{
  node->left->invite( this );
  Value left; left.swap( return_value );

  if( !left.isFrame() )
  {
    throw ParseException( node->left->loc )
      << "left of '.' is not a Frame, is " << left.getTypeName()
      ;
  }

  Value *const value = left.getFrame()->getVariable( node->member );
  if( value )
  {
    return_value = (*value);
  }
  else
  {
    throw Exception() << "Member '" << node->member << "' not found";
  }
}

void InterpretingASTVisitor::visit( const AST::FrameAccessExpression             *node )
{
  node->left->invite( this );
  Value left; left.swap( return_value );

  Value *value = 0;

  CallLoc guard( this, node->loc );
  dot_access_impl( left, node->identifier, value );
  return_value = (*value);
}

void InterpretingASTVisitor::visit( const AST::ArrayAccessExpression             *node )
{
  node->left->invite( this );
  Value left; left.swap( return_value );

  node->coord->invite( this );
  CountPtr< ValueArray > coord;
  swap( coord, return_value_dims );

  if( left.isArray() )
  {
    const ArrayBase *const array = left.getArray();
    const index_t Dims = array->getDims();

    if( coord->size() != Dims )
    {
      throw ParseException( node->coord->loc )
        << "Dimensions do not match, array has " << Dims << ", specified are " << coord->size()
        ;
    }

    vector< index_t > coords_int( Dims );
    for( index_t i=0; i<Dims; ++i )
    {
      const Value &c = coord->at( i );
      if( c.isNil() )
      {
        throw ParseException( node->coord->dims[ i ]->loc )
          << "All coordinates must be defined for access, coordinate " << i << " isn't"
          ;
      }
      else if( c.isInteger() )
      {
        coords_int[ i ] = c.save_cast( typeOf( index_t() ) ).get< index_t >();
      }
      else if( c.isOutput() )
      {
        Value at_args[ 1+Dims ];
        at_args[ 0 ] = left;
        for( index_t j=0; j<Dims; ++j )
        {
          at_args[ 1+j ] = coord->at( j );
        }

        CallLoc guard( this, node->loc );
        return_value = scope->call( getCallLoc(), getRD()+1, String::Static( ".core.at" ), 1+Dims, at_args );
        if( !return_value.isOutput() )
        {
          throw ParseException( node->loc )
            << "Calling at() with at least one Output should have returned an Output"
            << ", returned " << return_value.getType()
            ;
        }
        return;
      }
      else
      {
        throw ParseException( node->coord->dims[ i ]->loc )
          << "Type of coordinates must be an integer or Output, is " << c.getTypeName()
          ;
      }
    }

    ArrayBase::Coordinates X( Dims, &coords_int[ 0 ] );
    return_value = array->getValue( X );
    return;
  }
  else if( left.isFrame() || left.isNode() )
  {
    if( coord->size() != 1 )
    {
      throw Exception() << "Frame can only be accessed as 1 dimensional entity";
    }

    const Value &identifier = coord->at( 0 );

    if( !identifier.isString() )
    {
      throw ParseException( node->coord->loc )
        << "When accessing a Frame, coordinate must be a string, is " << identifier.getTypeName()
        ;
    }

    Frame *const frame = ( left.isFrame() ? left.getFrame() : left.getNode() );
    Value *const value = frame->load( identifier.getString(), scope );

    if( !value )
    {
      throw ParseException( node->coord->loc )
        << "Identifier '" << identifier << "' not found."
        ;
    }

    return_value = (*value);
    return;
  }
  else if( left.isOutput() )
  {
    Output *const output = left.getOutput();

    CountPtr< Node > at;
    {
      CallLoc guard( this, node->loc );
      at = scope->createNode( getCallLoc(), getRD()+1, ".core.At" );
      output->connect( at->getInput( String::Static( "in" ) ) );
    }

    const index_t Dims = coord->size();
    if( Dims > 4 )
    {
      throw ParseException( node->coord->loc )
        << "Only up to 4 dimensions supported for now, is " << Dims
        ;
    }

    for( index_t i=0; i<Dims; ++i )
    {
      CallLoc guard( this, node->coord->dims[ i ]->loc );
      const Value &c = coord->at( i );
      if( c.isNil() )
      {
        throw CallLocException( getCallLoc() )
          << "All coordinates must be specified for access, coordinate " << i << " isn't"
          ;
      }
      else
      {
        CountPtr< Output > oc = toOutput( c );
        static const char *const xyzt[] = { "x", "y", "z", "t" };
        oc->connect( at->getInput( String::Static( xyzt[ i ] ) ) );
      }
    }

    return_value = Value( at->getOutput( String::Static( "out" ) ) );
    return;
  }
  else
  {
    throw ParseException( node->left->loc )
      << "left of '[]' is not an Array or a Frame or an Output, is " << left.getTypeName()
      ;
  }
}

void InterpretingASTVisitor::visit( const AST::UnaryExpression              *node )
{
  Value args[ 2 ];

  args[ 0 ].set( getUOPStr( node->op ) );

  node->arg->invite( this );
  args[ 1 ].swap( return_value );

  CallLoc guard( this, node->loc );
  return_value = scope->call( getCallLoc(), getRD()+1, String::Static( ".math.mathOp1" ), 2, args );
}

void InterpretingASTVisitor::visit( const AST::BinaryExpression             *node )
{
  Value args[ 3 ];

  node->left->invite( this );
  args[ 0 ].swap( return_value );

  args[ 1 ].set( getBOPStr( node->op ) );

  node->right->invite( this );
  args[ 2 ].swap( return_value );

  CallLoc guard( this, node->loc );
  return_value = scope->call( getCallLoc(), getRD()+1, String::Static( ".binaryOp" ), 3, args );
}

void InterpretingASTVisitor::visit( const AST::IfThenElseExpression         *node )
{
  node->condition->invite( this );
  Value condition; condition.swap( return_value );

  bool is_true;
  try
  {
    is_true = condition.to( Type::Bool() ).getBool();
  }
  catch( const RPGML::Exception &e )
  {
    throw ParseException( node->condition->loc, e )
      << ": Unsupported type for condition."
      ;
  }

  if( is_true )
  {
    node->then_value->invite( this );
  }
  else
  {
    node->else_value->invite( this );
  }
}

void InterpretingASTVisitor::visit( const AST::TypeExpression               *node )
{
  if( !node->of.isNull() )
  {
    node->of->invite( this );
  }
  else
  {
    return_value_type_descr.reset();
  }

  if( !node->dims.isNull() )
  {
    node->dims->invite( this );
  }
  else
  {
    return_value_dims.reset();
  }

  return_value_type_descr =
    new TypeDescr(
        return_value_type_descr
      , return_value_dims
      , node->type
      );
  return_value_dims.reset();
}

void InterpretingASTVisitor::visit( const AST::DimensionsExpression         *node )
{
  CountPtr< ValueArray > dims( new ValueArray( scope->getGC(), 1, index_t( node->dims.size() ) ) );

  for( size_t i=0; i<node->dims.size(); ++i )
  {
    if( !node->dims[ i ].isNull() )
    {
      node->dims[ i ]->invite( this );
      swap( dims->at( index_t( i ) ), return_value );
    }
  }

  swap( return_value_dims, dims );
}

void InterpretingASTVisitor::visit( const AST::CastExpression         *node )
{
  node->arg->invite( this );
  Value arg; arg.swap( return_value );

  if( node->to.isPrimitive() )
  {
    Value cast_args[ 2 ];
    cast_args[ 0 ] = Value( String::Static( node->to.getTypeName() ) );
    cast_args[ 1 ].swap( arg );

    return_value = scope->call( node->loc, getRD()+1, String::Static( ".core.cast" ), 2, cast_args );
    return;
  }
  else if( node->to.isOutput() )
  {
    CallLoc guard( this, node->arg->loc );
    return_value.set( toOutput( arg ) );
    return;
  }
  else
  {
    throw ParseException( node->loc )
      << "Unsupported type '" << node->to << "' for cast"
      ;
  }
}

void InterpretingASTVisitor::visit( const AST::CompoundStatement            *node )
{
  CountPtr< Frame > body = ( node->creates_own_frame ? new Frame( scope->getGC(), scope->getCurr() ) : 0 );
  Scope::EnterLeaveGuard guard( scope, body );

  const index_t n = index_t( node->statements.size() );

  for( index_t i=0; i<n && !return_encountered; ++i )
  {
    node->statements[ i ]->invite( this );
  }

  // do not modify return_value, could have been set by return statement
}

void InterpretingASTVisitor::visit( const AST::FunctionDefinitionStatement  *node )
{
  if( !node->ret->type.isFunction() )
  {
    throw ParseException( node->ret->loc )
      << "Expected 'Function', got type expression"
      ;
  }

  const index_t n_args = node->args->size();
  CountPtr< Function::Args > decl_args = new Function::Args( n_args );

  for( index_t i=0; i<n_args; ++i )
  {
    const AST::FunctionDefinitionStatement::ArgDecl *const decl = node->args->at( i );
    const String &identifier = decl->identifier;
    const AST::Expression *const default_value_expr = decl->default_value;
    if( default_value_expr )
    {
      default_value_expr->invite( this );
      const Value &default_value = return_value;
      decl_args->at( i ) = Function::Arg( identifier, default_value );
    }
    else
    {
      decl_args->at( i ) = Function::Arg( identifier );
    }
  }

  Value function( new InterpretingFunction( scope->getGC(), node->loc, scope->getCurr(), node->identifier, decl_args, node->body, node->is_method ) );

  scope->create_unified( node->identifier, function );
  return_value = function;
}

void InterpretingASTVisitor::dot_access_impl( Value &left, const String &identifier, Value *&value )
{
  value = 0;

  if( left.isFrame() )
  {
    value = left.getFrame()->load( identifier, scope );
  }
  else if( left.isNode() )
  {
    value = left.getNode()->getVariable( identifier );
  }
  else
  {
    throw CallLocException( getCallLoc() )
      << "Left of '.' is not accessible via '.', is " << left.getTypeName()
      ;
  }

  if( !value )
  {
    throw CallLocException( getCallLoc() )
      << "Identifier '" << identifier << "' not found right of '.'"
      ;
  }
}

void InterpretingASTVisitor::assign_impl( const AST::AssignmentStatementBase *node, Value *lvalue )
{
  node->value->invite( this );
  Value value; value.swap( return_value );

  const Type lvalue_type = lvalue->getType();

  if( node->op == ID_ASSIGN )
  {
    if( Type::Param() == lvalue_type )
    {
      Param *const param = lvalue->getParam();

      try
      {
        param->set( value );
      }
      catch( const RPGML::Exception &e )
      {
        throw CallLocException( getCallLoc(), e );
      }
      catch( const std::exception &e )
      {
        throw CallLocException( getCallLoc() ) << e.what();
      }
      catch( const char *e )
      {
        throw CallLocException( getCallLoc() ) << e;
      }
    }
    else
    {
      if( lvalue_type.isOutput() )
      {
        CallLoc guard( this, node->value->loc );
        lvalue->set( toOutput( value ) );
      }
      else
      {
        CallLoc guard( this, node->loc );
        save_assign( (*lvalue), value );
      }
    }
  }
  else
  {
    CallLoc guard( this, node->loc );
    const BOP bop = getAssignBOP( node->op );

    Value args[ 3 ];
    args[ 0 ] = (*lvalue);
    args[ 1 ] = Value( String::Static( getBOPStr( bop ) ) );
    args[ 2 ] = value;

    Value new_value = scope->call( getCallLoc(), getRD()+1, String::Static( ".binaryOp" ), 3, args );

    save_assign( (*lvalue), new_value );
  }
}

void InterpretingASTVisitor::visit( const AST::ConnectStatement *node )
{
  CountPtr< Output > output;
  CountPtr< Input > input;

  {
    CallLoc guard( this, node->output->loc );
    node->output->invite( this );
    Value output_v; output_v.swap( return_value );
    output = toOutput( output_v );
  }

  {
    CallLoc guard( this, node->input->loc );
    node->input->invite( this );
    Value input_v; input_v.swap( return_value );
    if( input_v.isInput() )
    {
      input = input_v.getInput();
    }
    else if( input_v.isInOut() )
    {
      input = input_v.getInOut()->getInput();
    }
    else
    {
      throw CallLocException( getCallLoc() )
        << "Right of '->' is not an Input, is " << input_v.getTypeName()
        ;
    }
  }

  // output might be null, but that is ok
  CallLoc guard( this, node->loc );
  input->connect( output );
}

void InterpretingASTVisitor::visit( const AST::AssignIdentifierStatement    *node )
{
  const String &identifier = node->identifier;

  Value *const lvalue = scope->lookup( identifier );
  if( !lvalue )
  {
    throw Exception() << "Identifier '" << identifier << "' not found";
  }

  assign_impl( node, lvalue );
}

void InterpretingASTVisitor::visit( const AST::AssignDotStatement           *node )
{
  node->left->invite( this );
  Value left; left.swap( return_value );

  Value *lvalue = 0;

  CallLoc guard( this, node->loc );
  dot_access_impl( left, node->identifier, lvalue );
  assign_impl( node, lvalue );
}

void InterpretingASTVisitor::visit( const AST::AssignBracketStatement       *node )
{
  node->left->invite( this );
  Value left; left.swap( return_value );

  node->coord->invite( this );
  CountPtr< ValueArray > coords;
  swap( coords, return_value_dims );

  if( left.isArray() )
  {
    ArrayBase *const array = left.getArray();
    const index_t Dims = array->getDims();

    if( coords->size() != Dims )
    {
      throw ParseException( node->coord->loc )
        << "Dimensions do not match, array has " << Dims << ", specified are " << coords->size()
        ;
    }

    vector< index_t > coords_int( Dims );
    for( index_t i=0; i<Dims; ++i )
    {
      const Value &c = coords->at( i );
      if( c.isInteger() )
      {
        coords_int[ i ] = c.save_cast( typeOf( index_t() ) ).get< index_t >();
      }
      else if( c.isNil() )
      {
        throw ParseException( node->coord->dims[ i ]->loc )
          << "All coordinates must be defined for access, coordinate " << i << " isn't"
          ;
      }
      else
      {
        throw ParseException( node->coord->dims[ i ]->loc )
          << "Type of coordinates must be an integer, is " << c.getTypeName()
          ;
      }
    }

    // Assign to temporary
    Value lvalue = array->getValue_v( int( coords_int.size() ), &coords_int[ 0 ] );
    assign_impl( node, &lvalue );
    // Copy temporary to final location
    array->setValue_v( lvalue, int( coords_int.size() ), &coords_int[ 0 ] );
    return;
  }
  else if( left.isFrame() || left.isNode() )
  {
    // Determine identifier for access
    if( coords->size() != 1 || !coords->at( 0 ).isString() )
    {
      throw ParseException( node->coord->loc )
        << "When accessing a Frame of a Node via [], the coordinate argument must be a single string"
        << ", specified are " << coords->size() << " " << coords->at( 0 ).getTypeName() << " coordinates"
        ;
    }
    const String &identifier = coords->at( 0 ).getString();

    // Get lvalue to write to
    Value *lvalue = 0;
    if( left.isFrame() )
    {
      lvalue = left.getFrame()->load( identifier, scope );
    }
    else
    {
      lvalue = left.getNode()->getVariable( identifier );
    }

    if( !lvalue )
    {
      throw ParseException( node->loc )
        << "Identifier '" << identifier << "' not found"
        ;
    }

    // Do assignment
    assign_impl( node, lvalue );
  }
  else
  {
    throw ParseException( node->left->loc )
      << "Left of '[]' must be Array, Frame, or Node, is " << left.getTypeName()
      ;
  }
}

void InterpretingASTVisitor::visit( const AST::IfStatement                  *node )
{
  node->condition->invite( this );
  Value condition; condition.swap( return_value );

  bool condition_is_true;
  try
  {
    condition_is_true = condition.to( Type::Bool() ).getBool();
  }
  catch( const Value::CastFailed &e )
  {
    throw ParseException( node->condition->loc, e )
      << ": Unsupported type '" << condition << "' for condition"
      ;
  }

  if( condition_is_true )
  {
    node->then_body->invite( this );
  }
  else if( node->else_body )
  {
    node->else_body->invite( this );
  }
}

void InterpretingASTVisitor::visit( const AST::NOPStatement                 * )
{
  // Do nothing
}

void InterpretingASTVisitor::visit( const AST::ForSequenceStatement         *node )
{
  node->sequence->invite( this );
  Value sequence; sequence.swap( return_value );

  if( !sequence.isSequence() )
  {
    throw ParseException( node->sequence->loc )
      << "Right of '=' is not a sequence, is " << sequence.getTypeName()
      ;
  }

  const String &identifier = node->identifier;

  for( CountPtr< Sequence::Iter > i = sequence.getSequence()->getIter(); !i->done(); i->next() )
  {
    CountPtr< Frame > body_frame = new Frame( scope->getGC(), scope->getCurr() );
    Scope::EnterLeaveGuard guard( scope, body_frame );
    body_frame->push_back( identifier, i->get() );

    node->body->invite( this );

    if( return_encountered ) break;
  }
}

void InterpretingASTVisitor::visit( const AST::ForContainerStatement        *node )
{
  node->container->invite( this );
  Value container; container.swap( return_value );

  const String &identifier = node->identifier;

  if( container.isArray() )
  {
    const ArrayBase *const array = container.getArray();

    for( index_t i=0; i<array->size(); ++i )
    {
      CountPtr< Frame > body_frame = new Frame( scope->getGC(), scope->getCurr() );
      Scope::EnterLeaveGuard guard( scope, body_frame );
      body_frame->push_back( identifier, array->getValue( i ) );

      node->body->invite( this );

      if( return_encountered ) break;
    }
  }
  else if( container.isFrame() )
  {
    const Frame *const frame = container.getFrame();

    for( CountPtr< Frame::Iterator > i( frame->getIterator() ); !i->done(); i->next() )
    {
      CountPtr< Frame > body_frame = new Frame( scope->getGC(), scope->getCurr() );
      Scope::EnterLeaveGuard guard( scope, body_frame );
      body_frame->push_back( identifier, i->get().second );

      node->body->invite( this );

      if( return_encountered ) break;
    }
  }
  else if( container.isSequence() )
  {
    const Sequence *const sequence = container.getSequence();
    for( CountPtr< Sequence::Iter > i = sequence->getIter(); !i->done(); i->next() )
    {
      CountPtr< Frame > body_frame = new Frame( scope->getGC(), scope->getCurr() );
      Scope::EnterLeaveGuard guard( scope, body_frame );
      body_frame->push_back( identifier, i->get() );

      node->body->invite( this );

      if( return_encountered ) break;
    }
  }
  else
  {
    throw ParseException( node->container->loc )
      << "Invalid type '" << container.getType() << "' for container after 'in'"
      ;
  }
}

void InterpretingASTVisitor::visit( const AST::ExpressionStatement        *node )
{
  node->expr->invite( this );
}

Value InterpretingASTVisitor::create_default_value( const TypeDescr *of, const String &identifier )
{
  switch( of->type.getEnum() )
  {
    case Type::UINT8 : return Value( uint8_t ( 0 ) ); break;
    case Type::INT8  : return Value( int8_t  ( 0 ) ); break;
    case Type::UINT16: return Value( uint16_t( 0 ) ); break;
    case Type::INT16 : return Value( int16_t ( 0 ) ); break;
    case Type::UINT32: return Value( uint32_t( 0 ) ); break;
    case Type::INT32 : return Value( int32_t ( 0 ) ); break;
    case Type::UINT64: return Value( uint64_t( 0 ) ); break;
    case Type::INT64 : return Value( int64_t ( 0 ) ); break;
    case Type::FLOAT : return Value( float   ( 0 ) ); break;
    case Type::DOUBLE: return Value( double  ( 0 ) ); break;
    case Type::STRING: return Value( String() ); break;
    case Type::OUTPUT: return Value( (Output*)0 ); break;

    case Type::INPUT : // fallthrough to INOUT
    case Type::INOUT : return Value( new InOut( getGC(), identifier ) ); break;

    case Type::ARRAY :
      {
        CountPtr< ArrayBase > a = create_array( of->of, of->dims );
//        cerr << "create_default_value Type::ARRAY " << a->getSize() << endl;
        for( CountPtr< ArrayBase::CoordinatesIterator > c( a->getCoordinatesIterator() ); !c->done(); c->next() )
        {
          const ArrayBase::Coordinates C = c->get();
          a->setValue( create_default_value( of->of, identifier + toString( C ) ), C );
//          cerr << " set " << C << " to " << a->getValue( C ) << endl;
        }
        return Value( a );
      }
      break;

    default:
      throw Exception()
        << "Type '" << of->type << "'"
        << " does not have a default value, so an initializer has to be specified."
        ;
  }
}

void InterpretingASTVisitor::visit( const AST::VariableCreationStatement *node )
{
  const String &identifier = node->identifier;

  // Evaluate type expression
  CountPtr< TypeDescr > expected_type;
  node->type->invite( this );
  swap( expected_type, return_value_type_descr );

  // Evaluate initializer value expression
  Value value;

  if( node->value )
  {
    node->value->invite( this );
    value.swap( return_value );

    CallLoc guard( this, node->loc );
    value = save_cast( value, expected_type );
  }
  else
  {
    value = create_default_value( expected_type, node->identifier );
  }

  if( !scope->create_unified( identifier, value ) )
  {
    throw CallLocException( getCallLoc() ) << "Could not create variable '" << identifier << "'";
  }
}

void InterpretingASTVisitor::visit( const AST::VariableConstructionStatement *node )
{
  CallLoc guard( this, node->loc );
  node->value->invite( this );
  Value value; value.swap( return_value );

  const String &identifier = node->identifier;

  if( !scope->create_unified( identifier, value ) )
  {
    throw CallLocException( getCallLoc() ) << "Could not create variable '" << identifier << "'";
  }
}

void InterpretingASTVisitor::visit( const AST::ReturnStatement              *node )
{
  node->value->invite( this );
  return_encountered = true;
}

Value InterpretingASTVisitor::save_cast( const Value &x, Type type )
{
  if( type.isOutput() )
  {
    return Value( toOutput( x ) );
  }
  else
  {
    return x.save_cast( type );
  }
}

CountPtr< ArrayBase > InterpretingASTVisitor::create_array( const TypeDescr *of, const ValueArray *dims )
{
  if( !dims ) throw Exception() << "Internal: No array dimensions specified in TypeDescr";
  if( !of ) throw Exception() << "Internal: No array 'of'-type specified in TypeDescr";

  // Get Array dimensions and size

  const int to_dims = int( dims->size() );
  const Type to_type_of = of->type;

  vector< index_t > to_size( to_dims );

  for( int d=0; d<to_dims; ++d )
  {
    const Value &dims_d = dims->at( d );
    if( dims_d.isNil() )
    {
      to_size[ d ] = 1; //( value.isArray() ? unknown : 0 );
    }
    else if( dims_d.isInteger() )
    {
      const int64_t dims_d_i = dims_d.save_cast( Type::Int64() ).getInt64();
      if( dims_d_i < 0 )
      {
        throw Exception()
          << "Array size must not be negative"
          << ", " << ("xyzt"[ d ]) << "-dim is " << dims_d_i
          ;
      }
      to_size[ d ] = index_t( dims_d_i );
    }
    else
    {
      throw Exception()
        << "Array dimension must be of an integer type"
        << ", " << ("xyzt"[ d ]) << "-dim is " << dims_d.getType()
        ;
    }
  }

  // Create array of specified type
  CountPtr< ArrayBase > ret = new_Array( getGC(), to_type_of, to_dims );
  ret->resize_v( to_dims, &to_size[ 0 ] );
  return ret;
}

Value InterpretingASTVisitor::save_cast( const Value &x, const TypeDescr *to )
{
  if( !to ) return x;
  if( !to->type.isArray() ) return save_cast( x, to->type );

  // to Array

  // Create array of specified type
  CountPtr< ArrayBase > ret = create_array( to->of, to->dims );
  const ArrayBase::Size to_size = ret->getSize();
  const int to_dims = to_size.getDims();
  const Type to_type_of = to->of->type;

  // Assign x to 'to'
  if( x.isArray() )
  {
    // Array <- Array

    const ArrayBase *const x_base = x.getArray();
    const ArrayBase::Size x_size = x_base->getSize();

    if( x_size.getDims() != to_dims )
    {
      throw Exception()
        << "Number of Array dimensions do not match"
        << ": Expected " << to_dims
        << ", got " << x_size.getDims()
        ;
    }

    // Check size of x
    for( int d=0; d<to_dims; ++d )
    {
      if( to->dims->at( d ).isNil() ) continue;

      if( x_size[ d ] != to_size[ d ] )
      {
        throw Exception()
          << "Dimension " << ("xyzt"[ d ]) << "-size does not match the specified size"
          << ": Expected " << to_size[ d ]
          << ", is " << x_size[ d ]
          ;
      }
    }

    // Resize array to size of x
    ret->resize( x_size );

    // Try to assign the values in x to the values in the new array
    for( CountPtr< ArrayBase::CoordinatesIterator > c( ret->getCoordinatesIterator() ); !c->done(); c->next() )
    {
      const ArrayBase::Coordinates C = c->get();
      if( to_type_of.isArray() )
      {
        ret->setValue( save_cast( x_base->getValue( C ), to->of ), C );
      }
      else
      {
        ret->setValue( save_cast( x_base->getValue( C ), to_type_of ), C );
      }
    }

    return Value( ret );
  }
  else
  {
    // Array <- Scalar

    // Resize array to specified size
    ret->resize_v( to_dims, &to_size[ 0 ] );

    // Fill with x cast to the correct type
    ret->fillValue( save_cast( x, to_type_of ) );

    return Value( ret );
  }
}

void InterpretingASTVisitor::save_assign( Value &lvalue, const Value &x )
{
  const Type lvalue_type = lvalue.getType();
  if( lvalue_type.isNil() )
  {
    lvalue = x;
    return;
  }

  if( lvalue.isArray() )
  {
    ArrayBase *const      lvalue_base    = lvalue.getArray();
    const ArrayBase::Size lvalue_size    = lvalue_base->getSize();
    const Type            lvalue_type_of = lvalue_base->getType();

    if( x.isArray() )
    {
      const ArrayBase *const x_base = x.getArray();
      const ArrayBase::Size  x_size = x_base->getSize();

      if( lvalue_size.getDims() == lvalue_size.getDims() )
      {
        lvalue_base->resize( x_size );

        for( CountPtr< ArrayBase::CoordinatesIterator > c( x_base->getCoordinatesIterator() ); !c->done(); c->next() )
        {
          const ArrayBase::Coordinates C = c->get();
          const Value v = save_cast( x_base->getValue( C ), lvalue_type_of );
          lvalue_base->setValue( v, C );
        }
      }
      else
      {
        throw Exception() << "Assigning Array with different dimension not supported (yet?)";
      }
    }
    else
    {
      try
      {
        const Value fill_value = save_cast( x, lvalue_base->getType() );
        lvalue_base->fillValue( fill_value );
      }
      catch( const Value::GetFailed &e )
      {
        throw;
      }
    }
  }
  else
  {
    lvalue = save_cast( x, lvalue_type );
    return;
  }
}

CountPtr< Output > InterpretingASTVisitor::toOutput( const Value &x )
{
  return scope->toOutput( getCallLoc(), getRD()+1, x );
}

} // namespace RPGML


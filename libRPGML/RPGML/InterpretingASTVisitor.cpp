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

namespace InterpretingASTVisitor_impl {

  Value save_cast( const Value &x, Type type )
  {
    return x.save_cast( type );
  }

  Value save_cast( const Value &x, const InterpretingASTVisitor::TypeDescr *to, GarbageCollector *gc )
  {
    if( !to ) return x;
    if( !to->type.isArray() ) return save_cast( x, to->type );

    // to Array

    if( !to->dims ) throw Exception() << "No array dimensions specified in TypeDescr";
    if( !to->of ) throw Exception() << "No array 'of'-type specified in TypeDescr";

    // Get Array dimensions and size

    const index_t to_dims = to->dims->size();
    const Type to_type_of = to->of->type;

    vector< index_t > to_size( to_dims );
    vector< bool > to_size_known( to_dims, false );

    for( index_t d=0; d<to_dims; ++d )
    {
      const Value &dims_d = to->dims->at( d );
      if( dims_d.isNil() )
      {
        to_size[ d ] = 1; //( value.isArray() ? unknown : 0 );
      }
      else if( dims_d.isInteger() )
      {
        const int64_t dims_d_i = save_cast( dims_d, Type::Int64() ).getInt64();
        if( dims_d_i < 0 ) throw Exception() << "Array size must not be negative.";
        to_size[ d ] = index_t( dims_d_i );
        to_size_known[ d ] = true;
      }
      else
      {
        throw Exception() << "Array dimension must be of an integer type";
      }
    }

    // Assign x to 'to'

    // Create array of specified type
    CountPtr< ArrayBase > ret( new_Array( gc, to_type_of, to_dims ) );

    if( x.isArray() )
    {
      // Array <- Array

      const ArrayBase *const x_base = x.getArray();

      if( x_base->getDims() != to_dims )
      {
        throw Exception() << "Number of Array dimensions do not match";
      }

      const ArrayBase::Size x_size = x_base->getSize();

      // Check size of x
      for( index_t d=0; d<to_dims; ++d )
      {
        if( !to_size_known[ d ] ) continue;

        if( x_size[ d ] != to_size[ d ] )
        {
          throw Exception() << "Dimension size does not match the specified size";
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
          ret->setValue( save_cast( x_base->getValue( C ), to->of, gc ), C );
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

  void save_assign( Value &lvalue, const Value &x )
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
          throw Exception() << "Assigning Array with different dimension not supported yet";
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



} // namespace InterpretingASTVisitor_impl

using namespace InterpretingASTVisitor_impl;

InterpretingASTVisitor::InterpretingASTVisitor( GarbageCollector *_gc, Scope *_scope, index_t _recursion_depth )
: Collectable( _gc )
, AST::Visitor( _recursion_depth )
, scope( _scope )
, return_encountered( false )
{}

InterpretingASTVisitor::~InterpretingASTVisitor( void )
{}

void InterpretingASTVisitor::gc_clear( void )
{
  scope.reset();
  return_value_type_descr.reset();
  return_value_dims.reset();
  return_value.clear();
}

void InterpretingASTVisitor::gc_getChildren( Children &children ) const
{
  children.add( scope );
//  children.add( return_value_type_descr );
  children.add( return_value_dims );
  children.add( return_value.getCollectable() );
}

bool InterpretingASTVisitor::visit( const AST::ConstantExpression           *node )
{
  return_value = node->value;
  return true;
}

bool InterpretingASTVisitor::visit( const AST::ThisExpression           * )
{
  for( Frame *frame = scope->getCurr(); frame; frame = frame->getParent() )
  {
    if( frame->isThis() )
    {
      return_value = Value( frame );
      return true;
    }
  }

  throw Exception() << "No parent Frame that qualifies as 'this'";
  return_value.clear();
  return false;
}

bool InterpretingASTVisitor::visit( const AST::ArrayConstantExpression      *node )
{
  if( !node->sequence->invite( this ) ) return false;
  Value sequence; sequence.swap( return_value );

  if( !sequence.isSequence() )
  {
    throw ParseException( node->sequence->loc )
      << "Array definition content is not a Sequence, is " << sequence.getTypeName()
      ;
  }

  CountPtr< Array< Value, 1 > > array = new Array< Value, 1 >( scope->getGC() );

  for( CountPtr< Sequence::Iter > i = sequence.getSequence()->getIter(); !i->done(); i->next() )
  {
    array->push_back( i->get() );
  }

  return_value = Value( array.get() );
  return true;
}

bool InterpretingASTVisitor::visit( const AST::FrameConstantExpression *node )
{
  CountPtr< Frame > ret = new Frame( scope->getGC(), scope->getCurr() );
  ret->setThis( true );
  Scope::EnterLeaveGuard guard( scope, ret );

  if( !node->body->invite( this ) ) return false;

  return_value = Value( ret.get() );
  return true;
}

bool InterpretingASTVisitor::visit( const AST::ParenthisSequenceExpression  *node )
{
  if( !node->sequence->invite( this ) ) return false;
  return true;
}

bool InterpretingASTVisitor::visit( const AST::ExpressionSequenceExpression *node )
{
  const index_t n = index_t( node->expressions.size() );

  CountPtr< Array< Value, 1 > > array = new Array< Value, 1 >( scope->getGC() );
  array->resize( n );

  for( index_t i=0; i<n; ++i )
  {
    if( !node->expressions[ i ]->invite( this ) ) return false;
    array->at( i ).swap( return_value );
  }

  return_value = Value( new SequenceValueArray( scope->getGC(), array ) );
  return true;
}

bool InterpretingASTVisitor::visit( const AST::FromToStepSequenceExpression *node )
{
  if( !node->from->invite( this ) ) return false;
  Value from; from.swap( return_value );

  if( !from.isScalar() )
  {
    throw ParseException( node->from->loc )
      << "'from' must be scalar, is " << from.getTypeName()
      ;
  }

  if( !node->to->invite( this ) ) return false;
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
    if( !node->step->invite( this ) ) return false;
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

  return true;
}

bool InterpretingASTVisitor::visit( const AST::LookupVariableExpression     *node )
{
  const Value *const value = scope->lookup( node->identifier );
  if( value )
  {
//    cerr << "AST::LookupVariableExpression( '" << node->identifier << "' ): value = '" << (*value) << "'" << endl;
    return_value = (*value);
    return true;
  }
  else
  {
    return_value.clear();
    throw Exception() << "Identifier '" << node->identifier << "' not found";
  }
}

bool InterpretingASTVisitor::visit( const AST::FunctionCallExpression       *node )
{
  if( !node->function->invite( this ) ) return false;
  Value function; function.swap( return_value );

  if( !function.isFunction() )
  {
    throw ParseException( node->function->loc )
      << "Is not a Function, is " << function.getTypeName()
      ;
  }

//  cerr << "calling Function '" << function.getFunction()->getName() << "'" << endl;

  const size_t n_args = node->args->size();
  Function::Args args;
  args.reserve( n_args );

  for( size_t i=0; i < n_args; ++i )
  {
    if( !node->args->at( i )->value->invite( this ) ) return false;
    Value value; value.swap( return_value );
    args.push_back( Function::Arg( node->args->at( i )->identifier, value ) );
  }

  return function.getFunction()->call( node->loc, getRD()+1, scope, return_value, &args );
}

bool InterpretingASTVisitor::visit( const AST::DotExpression                *node )
{
  if( !node->left->invite( this ) ) return false;
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
    return true;
  }
  else
  {
    throw Exception() << "Member '" << node->member << "' not found";
  }
}

bool InterpretingASTVisitor::visit( const AST::FrameAccessExpression             *node )
{
  if( !node->left->invite( this ) ) return false;
  Value left; left.swap( return_value );

  Value *value = 0;

  if( dot_access_impl( node->loc, left, node->identifier, value ) )
  {
    return_value = (*value);
    return true;
  }
  else
  {
    throw Exception() << "Identifier '" << node->identifier << "' not found right of '.'";
  }
}

bool InterpretingASTVisitor::visit( const AST::ArrayAccessExpression             *node )
{
  if( !node->left->invite( this ) ) return false;
  Value left; left.swap( return_value );

  if( !node->coord->invite( this ) ) return false;
  CountPtr< Array< Value, 1 > > coord;
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

    Array< index_t, 1 > coords_int( 0, Dims );
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
        coords_int[ i ] = save_cast( c, typeOf( index_t() ) ).get< index_t >();
      }
      else if( c.isOutput() )
      {
        Value at_args[ 1+Dims ];
        at_args[ 0 ] = left;
        for( index_t j=0; j<Dims; ++j )
        {
          at_args[ 1+j ] = coord->at( j );
        }

        scope->call( node->coord->loc, getRD()+1, String::Static( ".at" ), return_value, 1+Dims, at_args );
        if( !return_value.isOutput() )
        {
          throw ParseException( node->loc )
            << "Calling at() with at least one Output should have returned an Output"
            << ", returned " << return_value.getType()
            ;
        }
        return true;
      }
      else
      {
        throw ParseException( node->coord->dims[ i ]->loc )
          << "Type of coordinates must be an integer or Output, is " << c.getTypeName()
          ;
      }
    }

    return_value = array->getValue_v( Dims, coords_int.elements() );
    return true;
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
    return true;
  }
  else if( left.isOutput() )
  {
    Output *const output = left.getOutput();

    CountPtr< Node > at = scope->createNode( node->loc, getRD()+1, ".At" );
    if( at.isNull() ) throw Exception() << "Could not create Node 'At'";

    output->connect( at->getInput( String::Static( "in" ) ) );

    const index_t Dims = coord->size();
    if( Dims > 4 )
    {
      throw ParseException( node->coord->loc )
        << "Only up to 4 dimensions supported for now, is " << Dims
        ;
    }

    for( index_t i=0; i<Dims; ++i )
    {
      const Location *const loc = node->coord->dims[ i ]->loc;
      const Value &c = coord->at( i );
      if( c.isNil() )
      {
        throw ParseException( loc )
          << "All coordinates must be specified for access, coordinate " << i << " isn't"
          ;
      }
      else
      {
        CountPtr< Output > oc = scope->toOutput( loc, getRD()+1, c );
        static const char *const xyzt[] = { "x", "y", "z", "t" };
        oc->connect( at->getInput( String::Static( xyzt[ i ] ) ) );
      }
    }

    return_value = Value( at->getOutput( String::Static( "out" ) ) );
    return true;
  }
  else
  {
    throw ParseException( node->left->loc )
      << "left of '[]' is not an Array or a Frame or an Output, is " << left.getTypeName()
      ;
  }
}

bool InterpretingASTVisitor::visit( const AST::UnaryExpression              *node )
{
  Value args[ 2 ];

  args[ 0 ].set( getUOPStr( node->op ) );

  if( !node->arg->invite( this ) ) return false;
  args[ 1 ].swap( return_value );

  return scope->call( node->loc, getRD()+1, String::Static( ".unaryOp" ), return_value, 2, args );
}

bool InterpretingASTVisitor::visit( const AST::BinaryExpression             *node )
{
  Value args[ 3 ];

  if( !node->left->invite( this ) ) return false;
  args[ 0 ].swap( return_value );

  args[ 1 ].set( getBOPStr( node->op ) );

  if( !node->right->invite( this ) ) return false;
  args[ 2 ].swap( return_value );

  return scope->call( node->loc, getRD()+1, String::Static( ".binaryOp" ), return_value, 3, args );
}

bool InterpretingASTVisitor::visit( const AST::IfThenElseExpression         *node )
{
  if( !node->condition->invite( this ) ) return false;
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
    if( !node->then_value->invite( this ) ) return false;
    return true;
  }
  else
  {
    if( !node->else_value->invite( this ) ) return false;
    return true;
  }
}

bool InterpretingASTVisitor::visit( const AST::TypeExpression               *node )
{
  if( !node->of.isNull() )
  {
    if( !node->of->invite( this ) ) return false;
  }
  else
  {
    return_value_type_descr.reset();
  }

  if( !node->dims.isNull() )
  {
    if( !node->dims->invite( this ) ) return false;
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

  return true;
}

bool InterpretingASTVisitor::visit( const AST::DimensionsExpression         *node )
{
  CountPtr< Array< Value, 1 > > dims( new Array< Value, 1 >( scope->getGC(), index_t( node->dims.size() ) ) );

  for( size_t i=0; i<node->dims.size(); ++i )
  {
    if( !node->dims[ i ].isNull() )
    {
      if( !node->dims[ i ]->invite( this ) ) return false;
      swap( dims->at( index_t( i ) ), return_value );
    }
  }

  swap( return_value_dims, dims );
  return true;
}

bool InterpretingASTVisitor::visit( const AST::CompoundStatement            *node )
{
  CountPtr< Frame > body = ( node->own_frame ? new Frame( scope->getGC(), scope->getCurr() ) : 0 );
  Scope::EnterLeaveGuard guard( scope, body );

  const index_t n = index_t( node->statements.size() );

  for( index_t i=0; i<n && !return_encountered; ++i )
  {
    if( !node->statements[ i ]->invite( this ) ) return false;
  }

  // do not modify return_value, could have been set by return statement
  return true;
}

bool InterpretingASTVisitor::visit( const AST::FunctionDefinitionStatement  *node )
{
  if( !node->ret->type.isFunction() )
  {
    throw ParseException( node->ret->loc )
      << "Expected 'Function', got type expression"
      ;
  }

  const size_t n_args = node->args->size();
  CountPtr< Function::Args > decl_args = new Function::Args( n_args );

  for( size_t i=0; i<n_args; ++i )
  {
    const AST::FunctionDefinitionStatement::ArgDecl *const decl = node->args->at( i );
    const String &identifier = decl->identifier;
    const AST::Expression *const default_value_expr = decl->default_value;
    if( default_value_expr )
    {
      if( !default_value_expr->invite( this ) ) return false;
      const Value &default_value = return_value;
      decl_args->at( i ) = Function::Arg( identifier, default_value );
    }
    else
    {
      decl_args->at( i ) = Function::Arg( identifier );
    }
  }

  Value function( new InterpretingFunction( scope->getGC(), scope->getCurr(), node->identifier, decl_args, node->body, node->is_method ) );

  if( !scope->create_unified( node->identifier, function ) ) return false;

  return_value = function;
  return true;
}

bool InterpretingASTVisitor::dot_access_impl( const Location *loc, Value &left, const String &identifier, Value *&value )
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
    throw ParseException( loc )
      << "Left of '.' is not accessible via '.', is " << left.getTypeName()
      ;
  }

  if( !value )
  {
    throw ParseException( loc )
      << "Identifier '" << identifier << "' not found right of '.'"
      ;
  }

  return true;
}

bool InterpretingASTVisitor::assign_impl( const AST::AssignmentStatement *node, Value *lvalue )
{
  using namespace InterpretingASTVisitor_impl;

  if( !node->value->invite( this ) ) return false;
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
      catch( ... )
      {
        throw;
      }
    }
    else
    {
      if( lvalue_type.isOutput() )
      {
        lvalue->set( scope->toOutput( node->loc, getRD()+1, value ) );
      }
      else
      {
        save_assign( (*lvalue), value );
      }
    }
  }
  else
  {
    const BOP bop = getAssignBOP( node->op );

    Value args[ 3 ];
    args[ 0 ] = (*lvalue);
    args[ 1 ] = Value( String::Static( getBOPStr( bop ) ) );
    args[ 2 ] = value;

    Value new_value;
    return scope->call( node->loc, getRD()+1, String::Static( ".binaryOp" ), new_value, 3, args );

    save_assign( (*lvalue), new_value );
  }

  return true;
}

bool InterpretingASTVisitor::visit( const AST::ConnectStatement *node )
{
  if( !node->output->invite( this ) ) return false;
  Value output_v; output_v.swap( return_value );

  CountPtr< Output > output( scope->toOutput( node->loc, getRD()+1, output_v ) );

  if( !node->input->invite( this ) ) return false;
  Value input; input.swap( return_value );

  if( !input.isInput() )
  {
    throw ParseException( node->input->loc )
      << "Right of '->' is not an Input, is " << input.getTypeName()
      ;
  }

  // output might be null, but that is ok
  input.getInput()->connect( output );
  return true;
}

bool InterpretingASTVisitor::visit( const AST::AssignIdentifierStatement    *node )
{
  const String &identifier = node->identifier;

  Value *const lvalue = scope->lookup( identifier );
  if( !lvalue )
  {
    throw Exception() << "Identifier '" << identifier << "' not found";
  }

  return assign_impl( node, lvalue );
}

bool InterpretingASTVisitor::visit( const AST::AssignDotStatement           *node )
{
  if( !node->left->invite( this ) ) return false;
  Value left; left.swap( return_value );

  Value *lvalue = 0;

  if( !dot_access_impl( node->loc, left, node->identifier, lvalue ) )
  {
    return false;
  }

  return assign_impl( node, lvalue );
}

bool InterpretingASTVisitor::visit( const AST::AssignBracketStatement       *node )
{
  if( !node->left->invite( this ) ) return false;
  Value left; left.swap( return_value );

  if( !node->coord->invite( this ) ) return false;
  CountPtr< Array< Value, 1 > > coords;
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

    Array< index_t, 1 > coords_int( 0, Dims );
    for( index_t i=0; i<Dims; ++i )
    {
      const Value &c = coords->at( i );
      if( c.isInteger() )
      {
        coords_int[ i ] = save_cast( c, typeOf( index_t() ) ).get< index_t >();
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
    Value lvalue = array->getValue_v( coords_int.size(), coords_int.elements() );
    if( !assign_impl( node, &lvalue ) ) return false;
    // Copy temporary to final location
    array->setValue_v( lvalue, coords_int.size(), coords_int.elements() );
    return true;
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
    return assign_impl( node, lvalue );
  }
  else
  {
    throw ParseException( node->left->loc )
      << "Left of '[]' must be Array, Frame, or Node, is " << left.getTypeName()
      ;
  }
}

bool InterpretingASTVisitor::visit( const AST::IfStatement                  *node )
{
  if( !node->condition->invite( this ) ) return false;
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
    if( !node->then_body->invite( this ) ) return false;
  }
  else if( node->else_body )
  {
    if( !node->else_body->invite( this ) ) return false;
  }

  return true;
}

bool InterpretingASTVisitor::visit( const AST::NOPStatement                 * )
{
  // Do nothing
  return true;
}

bool InterpretingASTVisitor::visit( const AST::ForSequenceStatement         *node )
{
  if( !node->sequence->invite( this ) ) return false;
  Value sequence; sequence.swap( return_value );

  if( !sequence.isSequence() )
  {
    throw ParseException( node->sequence->loc )
      << "Right of '=' is not a sequence, is " << sequence.getTypeName()
      ;
  }

  const String &identifier = node->identifier;

  Frame::PushPopGuard push_for_variable( scope->getCurr(), identifier );
  Frame::Ref for_variable = push_for_variable.get();
  if( for_variable.isNull() )
  {
    throw Exception()
      << "Could not create for-variable '" << identifier << "'"
      ;
  }

  for( CountPtr< Sequence::Iter > i = sequence.getSequence()->getIter(); !i->done(); i->next() )
  {
    (*for_variable) = i->get();

    if( !node->body->invite( this ) ) return false;

    if( return_encountered ) break;
  }

  return true;
}

bool InterpretingASTVisitor::visit( const AST::ForContainerStatement        *node )
{
  if( !node->container->invite( this ) ) return false;
  Value container; container.swap( return_value );

  const String &identifier = node->identifier;

  Frame::PushPopGuard push_for_variable( scope->getCurr(), identifier );
  Frame::Ref for_variable = push_for_variable.get();
  if( for_variable.isNull() )
  {
    throw Exception() << "Could not create for-variable '" << identifier << "'";
  }

  if( container.isArray() )
  {
    const ArrayBase *const array = container.getArray();

    for( index_t i=0; i<array->size(); ++i )
    {
      (*for_variable) = array->getValue( i );

      if( !node->body->invite( this ) ) return false;

      if( return_encountered ) break;
    }
  }
  else
  {
    throw ParseException( node->container->loc )
      << "Invalid type '" << container.getTypeName() << "' for container after 'in'"
      ;
  }

  return true;
}

bool InterpretingASTVisitor::visit( const AST::FunctionCallStatement        *node )
{
  return node->call->invite( this );
}

bool InterpretingASTVisitor::visit( const AST::VariableCreationStatement *node )
{
  using namespace InterpretingASTVisitor_impl;

  const String &identifier = node->identifier;

  // Evaluate type expression
  CountPtr< TypeDescr > expected_type;
  if( !node->type->invite( this ) ) return false;
  swap( expected_type, return_value_type_descr );

  // Evaluate initializer value expression
  if( !node->value->invite( this ) ) return false;
  Value value; value.swap( return_value );

  if( expected_type->type.isOutput() )
  {
    value.set( scope->toOutput( node->value->loc, getRD()+1, value ) );
  }
  else
  {
    value = save_cast( value, expected_type, scope->getGC() );
  }

  if( !scope->create_unified( identifier, value ) )
  {
    throw Exception() << "Could not create variable '" << identifier << "'";
  }
  return true;
}

bool InterpretingASTVisitor::visit( const AST::ReturnStatement              *node )
{
  node->value->invite( this );
  return_encountered = true;
  return true;
}

} // namespace RPGML


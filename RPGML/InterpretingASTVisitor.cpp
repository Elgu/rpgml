#include "InterpretingASTVisitor.h"

#include "Refcounted.h"
#include "Array.h"
#include "Sequence.h"
#include "Scope.h"
#include "Function.h"
#include "Frame.h"
#include "InterpretingFunction.h"

#include <iostream>

namespace RPGML {

InterpretingASTVisitor::InterpretingASTVisitor( Scope *_scope, index_t _recursion_depth )
: AST::Visitor( _recursion_depth )
, scope( _scope )
, return_encountered( false )
{
  const Value *const unaryOp_v  = scope->lookup( "unaryOp" );
  if( !unaryOp_v )
  {
    throw "'unaryOp' not found";
  }
  if( !unaryOp_v->isFunction() )
  {
    throw "'unaryOp' is not a Function";
  }
  unaryOp = unaryOp_v->getFunction();

  const Value *const binaryOp_v = scope->lookup( "binaryOp" );
  if( !binaryOp_v )
  {
    throw "'binaryOp' not found";
  }
  if( !binaryOp_v->isFunction() )
  {
    throw "'binaryOp' is not a Function";
  }
  binaryOp = binaryOp_v->getFunction();
}

InterpretingASTVisitor::~InterpretingASTVisitor( void )
{}

bool InterpretingASTVisitor::visit( const AST::ConstantExpression           *node )
{
  return_value = node->value;
  return true;
}

bool InterpretingASTVisitor::visit( const AST::ArrayConstantExpression      *node )
{
  if( !node->sequence->invite( this ) ) return false;
  Value sequence; sequence.swap( return_value );

  if( !sequence.isSequence() )
  {
    throw "sequence is not a Sequence";
  }

  CountPtr< Array > array = new Array( scope->getGC() );

  for( CountPtr< Sequence::Iter > i = sequence.getSequence()->getIter(); !i->done(); i->next() )
  {
    const Value *const value_i = i->get();
    if( !value_i ) return false;

    array->append( *value_i );
  }

  return_value = Value( array.get() );
  return true;
}

bool InterpretingASTVisitor::visit( const AST::FrameConstantExpression        *node )
{
  CountPtr< Frame > ret = new Frame( scope->getGC(), scope->getCurr() );
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

  CountPtr< Array > array = new Array( scope->getGC() );
  array->resize( n );

  for( index_t i=0; i<n; ++i )
  {
    if( !node->expressions[ i ]->invite( this ) ) return false;
    array->set( i, return_value );
  }

  return_value = Value( new SequenceValueArray( scope->getGC(), array ) );
  return true;
}

bool InterpretingASTVisitor::visit( const AST::FromToStepSequenceExpression *node )
{
  if( !node->from->invite( this ) ) return false;
  Value from; from.swap( return_value );

  if( !from.isInt() && !from.isFloat() )
  {
    throw "'from' must be int or float.";
  }

  if( !node->to->invite( this ) ) return false;
  Value to; to.swap( return_value );

  if( !to.isInt() && !to.isFloat() )
  {
    throw "'to' must be int or float.";
  }

  Value step;
  if( node->step )
  {
    if( !node->step->invite( this ) ) return false;
    step.swap( return_value );

    if( !step.isInt() && !step.isFloat() )
    {
      throw "'step' must be int or float.";
    }
  }
  else
  {
    step = Value( 1 );
  }

  if( from.isFloat() || to.isFloat() || step.isFloat() )
  {
    const float from_f = ( from.isFloat() ? from.getFloat() : float( from.getInt() ) );
    const float to_f   = ( to  .isFloat() ? to  .getFloat() : float( to  .getInt() ) );
    const float step_f = ( step.isFloat() ? step.getFloat() : float( step.getInt() ) );

    return_value.set( genSequenceFromToStep( scope->getGC(), from_f, to_f, step_f ) );
  }
  else
  {
    const int from_i = from.getInt();
    const int to_i   = to  .getInt();
    const int step_i = step.getInt();

    return_value.set( genSequenceFromToStep( scope->getGC(), from_i, to_i, step_i ) );
  }

  return true;
}

bool InterpretingASTVisitor::visit( const AST::LookupVariableExpression     *node )
{
  const Value *const value = scope->lookup( node->identifier );
  if( value )
  {
    return_value = (*value);
    return true;
  }
  else
  {
    throw "Identifier not found.";
    return false;
  }
}

bool InterpretingASTVisitor::visit( const AST::FunctionCallExpression       *node )
{
  if( !node->function->invite( this ) ) return false;
  Value function; function.swap( return_value );

  if( !function.isFunction() )
  {
    throw "Is not a Function.";
  }

  const size_t n_args = node->args->size();
  Function::Args args;
  args.reserve( n_args );

  for( size_t i=0; i < n_args; ++i )
  {
    if( !node->args->at( i )->value->invite( this ) ) return false;
    Value value; value.swap( return_value );
    args.push_back( Function::Arg( node->args->at( i )->identifier, value ) );
  }

  return function.getFunction()->call( node->loc, scope, return_value, &args, getRecursionDepth() );
}

bool InterpretingASTVisitor::visit( const AST::DotExpression                *node )
{
  if( !node->left->invite( this ) ) return false;
  Value left; left.swap( return_value );

  if( !left.isFrame() )
  {
    throw "left is not a Frame";
  }

  Value *const value = left.getFrame()->get( node->member );
  if( value )
  {
    return_value = (*value);
    return true;
  }
  else
  {
    throw "member not found.";
    return false;
  }
}

bool InterpretingASTVisitor::visit( const AST::AccessExpression             *node )
{
  if( !node->left->invite( this ) ) return false;
  Value left; left.swap( return_value );

  if( !node->key->invite( this ) ) return false;
  Value key; key.swap( return_value );

  if( left.isFrame() )
  {
    if( key.isString() )
    {
      Value *const value = left.getFrame()->get( key.getString() );

  //    std::cerr
  //      << "InterpretingASTVisitor::visit( AST::AccessExpression )"
  //      << ": Accessing " << key
  //      << std::endl
  //      ;

      if( value )
      {
        return_value = (*value);
        return true;
      }
      else
      {
        throw "key not found in Frame.";
        return false;
      }
    }
    else
    {
      throw "key is not a string";
      return false;
    }
  }
  else if( left.isArray() )
  {
    index_t index = -1;
    if( key.isBool() )
    {
      index = index_t( key.getBool() );
    }
    else if( key.isInt() )
    {
      index = key.getInt();
    }
    else if( key.isFloat() )
    {
      index = index_t( key.getFloat() );
    }
    else
    {
      throw "key must be int or float for Array access.";
    }

    Value *const value = left.getArray()->get( index );
    if( value )
    {
      return_value = (*value);
      return true;
    }
    else
    {
      throw "index out of range.";
      return false;
    }
  }
  else
  {
    throw "left is neither Frame nor Array.";
    return false;
  }
}

bool InterpretingASTVisitor::visit( const AST::UnaryExpression              *node )
{
  Value args[ 2 ];

  args[ 0 ].set( getUOPStr( node->op ) );

  if( !node->arg->invite( this ) ) return false;
  args[ 1 ].swap( return_value );

  return unaryOp->call_impl( node->loc, scope, return_value, 2, args, getRecursionDepth() );
}

bool InterpretingASTVisitor::visit( const AST::BinaryExpression             *node )
{
  Value args[ 3 ];

  if( !node->left->invite( this ) ) return false;
  args[ 0 ].swap( return_value );

  args[ 1 ].set( getBOPStr( node->op ) );

  if( !node->right->invite( this ) ) return false;
  args[ 2 ].swap( return_value );

  return binaryOp->call_impl( node->loc, scope, return_value, 3, args, getRecursionDepth() );
}

bool InterpretingASTVisitor::visit( const AST::IfThenElseExpression         *node )
{
  if( !node->condition->invite( this ) ) return false;
  Value condition; condition.swap( return_value );

  bool is_true;
  if( condition.isBool() )
  {
    is_true = condition.getBool();
  }
  else if( condition.isInt() )
  {
    is_true = bool( condition.getInt() );
  }
  else if( condition.isFloat() )
  {
    is_true = bool( condition.getFloat() );
  }
  else
  {
    throw "unsupported type for condition.";
    return false;
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

bool InterpretingASTVisitor::visit( const AST::CompoundStatement            *node )
{
  CountPtr< Frame > body = ( node->own_map ? new Frame( scope->getGC(), scope->getCurr() ) : 0 );
  Scope::EnterLeaveGuard guard( scope, body );

  const index_t n = index_t( node->statements.size() );

  for( index_t i=0; i<n && !return_encountered; ++i )
  {
    if( !node->statements[ i ]->invite( this ) ) return false;
  }

  return true;
}

bool InterpretingASTVisitor::visit( const AST::FunctionDefinitionStatement  *node )
{
  const size_t n_args = node->args->size();
  CountPtr< Function::Args > decl_args = new Function::Args( n_args );

  for( size_t i=0; i<n_args; ++i )
  {
    Value default_value;
    const AST::FunctionDefinitionStatement::ArgDecl *const decl = node->args->at( i );
    const String &identifier = decl->identifier;
    const AST::Expression *const default_value_expr = decl->default_value;
    if( default_value_expr )
    {
      if( !default_value_expr->invite( this ) ) return false;
      default_value.swap( return_value );
      if( default_value.isInvalid() )
      {
        throw "Default value is invalid";
      }
    }

    decl_args->at( i ) = Function::Arg( identifier, default_value );
  }

  Value function( new InterpretingFunction( scope->getGC(), scope->getCurr(), decl_args, node->body ) );

  if( !scope->create_unified( node->identifier, function ) ) return false;

  return_value = function;
  return true;
}

bool InterpretingASTVisitor::assign_impl( const AST::AssignmentStatement *node, Value *lvalue )
{
  if( !node->value->invite( this ) ) return false;
  Value value; value.swap( return_value );

  const Type lvalue_type = lvalue->getType();

  if( node->op == ID_ASSIGN )
  {
    if( value.getType() != lvalue_type )
    {
      throw "expression type does not match lvalue type";
    }
    lvalue->swap( value );
  }
  else
  {
    const BOP bop = getAssignBOP( node->op );

    Value args[ 3 ];
    args[ 0 ].swap( (*lvalue) ); // move lvalue into arg, will be overwritten anyway
    args[ 1 ] = Value( getBOPStr( bop ) );
    args[ 2 ].swap( value );

    binaryOp->call_impl( node->loc, scope, (*lvalue), 3, args, getRecursionDepth() );

    if( lvalue->getType() != lvalue_type )
    {
      throw "expression type does not match lvalue type";
    }
  }

  return true;
}

bool InterpretingASTVisitor::visit( const AST::AssignIdentifierStatement    *node )
{
  const String &identifier = node->identifier;

  Value *const lvalue = scope->lookup( identifier );
  if( !lvalue )
  {
    throw "lvalue not found";
  }

  return assign_impl( node, lvalue );
}

bool InterpretingASTVisitor::visit( const AST::AssignDotStatement           *node )
{
  if( !node->left->invite( this ) ) return false;
  Value left; left.swap( return_value );

  if( !left.isFrame() )
  {
    throw "left of '.' is not a Frame";
  }

  Value *const lvalue = left.getFrame()->get( node->identifier );
  if( !lvalue )
  {
    throw "member not found in Frame";
  }

  return assign_impl( node, lvalue );
}

bool InterpretingASTVisitor::visit( const AST::AssignBracketStatement       *node )
{
  if( !node->left->invite( this ) ) return false;
  Value left; left.swap( return_value );

  if( !node->key->invite( this ) ) return false;
  Value key; key.swap( return_value );

  if( key.isInvalid() )
  {
    throw "key in [] is invalid";
  }

  Value *lvalue = 0;

  if( left.isFrame() )
  {
    if( key.isString() )
    {
      lvalue = left.getFrame()->get( key.getString() );
      if( !lvalue )
      {
        throw "key not found";
      }
    }
    else
    {
      throw "key must be string";
    }
  }
  else if( left.isArray() )
  {
    if( key.isInt() )
    {
      lvalue = left.getArray()->get( key.getInt() );
      if( !lvalue )
      {
        throw "index out of range";
      }
    }
    else
    {
      throw "unsupported type for key in [] for Array";
    }
  }
  else
  {
    throw "left of '.' is neither Frame not Array";
  }

  return assign_impl( node, lvalue );
}

bool InterpretingASTVisitor::visit( const AST::IfStatement                  *node )
{
  if( !node->condition->invite( this ) ) return false;
  Value condition; condition.swap( return_value );

  bool condition_is_true;

  if( condition.isBool() )
  {
    condition_is_true = condition.getBool();
  }
  else if( condition.isInt() )
  {
    condition_is_true = bool( condition.getInt() );
  }
  else if( condition.isFloat() )
  {
    condition_is_true = bool( condition.getFloat() );
  }
  else
  {
    throw "Unsupported type for condition";
  }

  if( condition_is_true )
  {
    if( !node->then_body->invite( this ) ) return false;
  }
  else
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
    throw "right of '=' is not a sequence";
  }

  const String &identifier = node->identifier;

  Frame::PushPopGuard push_for_variable( scope->getCurr(), identifier );
  Value *const for_variable = push_for_variable.get();
  if( 0 == for_variable )
  {
    throw "Could not create for-variable";
  }

  for( CountPtr< Sequence::Iter > i = sequence.getSequence()->getIter(); !i->done(); i->next() )
  {
    const Value *const value = i->get();
    if( !value ) return false;

    (*for_variable) = (*value);

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
  Value *const for_variable = push_for_variable.get();
  if( 0 == for_variable )
  {
    throw "Could not create for-variable";
  }

  if( container.isArray() )
  {
    const Array *const array = container.getArray();

    for( index_t i=0; i<array->size(); ++i )
    {
      const Value *const value = array->get( i );
      (*for_variable) = (*value);

      if( !node->body->invite( this ) ) return false;

      if( return_encountered ) break;
    }
  }
  else
  {
    throw "Invalid type for container after 'in'";
  }

  return true;
}

bool InterpretingASTVisitor::visit( const AST::FunctionCallStatement        *node )
{
  return node->call->invite( this );
}

bool InterpretingASTVisitor::visit( const AST::VariableCreationStatement    *node )
{
  const String &identifier = node->identifier;
  const Type type = node->type;

  if( !node->value->invite( this ) ) return false;
  Value value; value.swap( return_value );

  if( value.getType() != type )
  {
    throw "Type right of '=' does not match declared type";
  }

  if( !scope->create_unified( identifier, value ) )
  {
    throw "could not create variable";
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


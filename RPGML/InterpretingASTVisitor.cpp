#include "InterpretingASTVisitor.h"

#include "Refcounted.h"
#include "Array.h"
#include "Sequence.h"
#include "Scope.h"
#include "Function.h"
#include "Frame.h"
#include "Node.h"
#include "InterpretingFunction.h"

#include <iostream>

using namespace std;

namespace RPGML {

namespace InterpretingASTVisitor_impl {

  template< class Element, index_t Dims >
  CountPtr< Array< Element, Dims > >
  new_Array( GarbageCollector *gc, index_t dims, const index_t *size, const Element &fill_value )
  {
    CountPtr< Array< Element, Dims > > ret( new Array< Element, Dims >( gc, dims, size ) );
    ret->fill( fill_value );
    return ret;
  }

  template< class Element >
  Value new_Array( GarbageCollector *gc, const Array< index_t, 1 > &size, Type expected_scalar_type, const Value &scalar )
  {
    const index_t Dims = size.size();

    if( expected_scalar_type != scalar.getType() )
    {
      throw "Expected corresponding array or int scalar type as initializer";
    }

    Element e;
    if( !scalar.get( e ) )
    {
      throw "Could not get initializer value";
    }

    switch( Dims )
    {
      case 1: return Value( new_Array< Element, 1 >( gc, Dims, size.elements(), e ) );
      case 2: return Value( new_Array< Element, 2 >( gc, Dims, size.elements(), e ) );
      case 3: return Value( new_Array< Element, 3 >( gc, Dims, size.elements(), e ) );
      case 4: return Value( new_Array< Element, 4 >( gc, Dims, size.elements(), e ) );
      default:
        throw "More than 4 dimensions not supported";
    }

    return Value();
  }

  template< class Element, class SrcElement, index_t Dims >
  CountPtr< Array< Element, Dims > > new_Array( GarbageCollector *gc, const Array< SrcElement, Dims > *src )
  {
    typedef Array< Element, Dims > Ret;
    typedef Array< SrcElement, Dims > Src;

    if( !src ) return (Ret*)0;

    const index_t *const size = src->getSize();
    CountPtr< Ret > ret( new Ret( gc, Dims, size ) );

    typename Ret::iterator r = ret->begin();
    const typename Ret::iterator r_end = ret->end();

    typename Src::const_iterator s = src->begin();
    const typename Src::const_iterator s_end = src->end();

    for(; r != r_end; ++r, ++s )
    {
      (*r) = Element(*s);
    }

    if( s != s_end ) throw "Interal: array sizes not generated correctly";
    return ret;
  }

  template< class Element, class SrcElement >
  CountPtr< ArrayElements< Element > > new_Array( GarbageCollector *gc, const ArrayElements< SrcElement > *src )
  {
    typedef ArrayElements< Element > Ret;

    if( !src ) return (Ret*)0;

    const index_t Dims = src->getDims();

    switch( Dims )
    {
      case 1:
      {
        const Array< SrcElement, 1 > *src_array = 0;
        if( !src->getAs( src_array ) ) return (Ret*)0;
        return new_Array< Element, SrcElement, 1 >( gc, src_array );
      }

      case 2:
      {
        const Array< SrcElement, 2 > *src_array = 0;
        if( !src->getAs( src_array ) ) return (Ret*)0;
        return new_Array< Element, SrcElement, 2 >( gc, src_array );
      }

      case 3:
      {
        const Array< SrcElement, 3 > *src_array = 0;
        if( !src->getAs( src_array ) ) return (Ret*)0;
        return new_Array< Element, SrcElement, 3 >( gc, src_array );
      }

      case 4:
      {
        const Array< SrcElement, 4 > *src_array = 0;
        if( !src->getAs( src_array ) ) return (Ret*)0;
        return new_Array< Element, SrcElement, 4 >( gc, src_array );
      }

      default:
        throw "More than 4 dimensions not supported";
    }
  }

  /*
  template< class Element >
  CountPtr< ArrayElements< Element > > new_Array( GarbageCollector *gc, const ArrayBase *array_base )
  {
    const Type src_type = array_base->getType();

    switch( src_type.getEnum() )
    {
      case Type::INVALID :
      case Type::BOOL    :
      case Type::INT     :
      case Type::FLOAT   :
      case Type::STRING  :
      case Type::FRAME   :
      case Type::FUNCTION:
      case Type::NODE    :
      case Type::OUTPUT  :
      case Type::INPUT   :
      case Type::SEQUENCE:
      case Type::ARRAY   :
    }

    return ret;
  }
  */


} // namespace InterpretingASTVisitor_impl

using namespace InterpretingASTVisitor_impl;

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

  throw "No frame that qualifies as 'this'";
  return_value.clear();
  return false;
}

bool InterpretingASTVisitor::visit( const AST::ArrayConstantExpression      *node )
{
  if( !node->sequence->invite( this ) ) return false;
  Value sequence; sequence.swap( return_value );

  if( !sequence.isSequence() )
  {
    throw "sequence is not a Sequence";
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
//    cerr << "AST::LookupVariableExpression( '" << node->identifier << "' ): value = '" << (*value) << "'" << endl;
    return_value = (*value);
    return true;
  }
  else
  {
    return_value.clear();
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
//    cerr << "Is not a Function, is '" << function << "'." << endl;
    throw "Is not a Function.";
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

  return function.getFunction()->call( node->loc, getRecursionDepth(), scope, return_value, &args );
}

bool InterpretingASTVisitor::visit( const AST::DotExpression                *node )
{
  if( !node->left->invite( this ) ) return false;
  Value left; left.swap( return_value );

  if( !left.isFrame() )
  {
    throw "left is not a Frame";
  }

  Value *const value = left.getFrame()->getVariable( node->member );
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
    throw "identifier not found right of '.'.";
    return false;
  }
}

bool InterpretingASTVisitor::visit( const AST::ArrayAccessExpression             *node )
{
  if( !node->left->invite( this ) ) return false;
  Value left; left.swap( return_value );

  if( !node->coord->invite( this ) ) return false;
  CountPtr< Array< Value, 1 > > coord;
  swap( coord, return_value_dims );

  if( !left.isArray() )
  {
    throw "left of '[]' is not an Array.";
  }

  const ArrayBase *const array = left.getArray();
  const index_t Dims = array->getDims();

  if( coord->size() != Dims )
  {
    throw "dimensions do not match";
  }

  Array< index_t, 1 > coords_int( 0, Dims );
  for( index_t i=0; i<Dims; ++i )
  {
    const Value &c = coord->at( i );
    if( c.isInt() )
    {
      coords_int[ i ] = index_t( c.getInt() );
    }
    else if( c.isInvalid() )
    {
      throw "all coordinates must be defined for access";
    }
    else
    {
      throw "type of coordinate must be int";
    }
  }

  return_value = array->getValue( Dims, coords_int.elements() );
  return true;
}

bool InterpretingASTVisitor::visit( const AST::UnaryExpression              *node )
{
  Value args[ 2 ];

  args[ 0 ].set( getUOPStr( node->op ) );

  if( !node->arg->invite( this ) ) return false;
  args[ 1 ].swap( return_value );

  return unaryOp->call_impl( node->loc, getRecursionDepth(), scope, return_value, 2, args );
}

bool InterpretingASTVisitor::visit( const AST::BinaryExpression             *node )
{
  Value args[ 3 ];

  if( !node->left->invite( this ) ) return false;
  args[ 0 ].swap( return_value );

  args[ 1 ].set( getBOPStr( node->op ) );

  if( !node->right->invite( this ) ) return false;
  args[ 2 ].swap( return_value );

  return binaryOp->call_impl( node->loc, getRecursionDepth(), scope, return_value, 3, args );
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

bool InterpretingASTVisitor::visit( const AST::TypeExpression               *node )
{
  CountPtr< TypeDescr > ret( new TypeDescr );
  if( !node->of.isNull() )
  {
    if( !node->of->invite( this ) ) return false;
    swap( ret->of, return_value_type_descr );
  }
  if( !node->dims.isNull() )
  {
    if( !node->dims->invite( this ) ) return false;
    swap( ret->dims, return_value_dims );
  }
  ret->type = node->type;
  swap( return_value_type_descr, ret );
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

  Value function( new InterpretingFunction( scope->getGC(), scope->getCurr(), node->identifier, decl_args, node->body ) );

  if( !scope->create_unified( node->identifier, function ) ) return false;

  return_value = function;
  return true;
}

bool InterpretingASTVisitor::dot_access_impl( const Location *loc, Value &left, const String &identifier, Value *&value )
{
  value = 0;

  if( left.isFrame() )
  {
    value = left.getFrame()->getVariable( identifier );
  }
  else if( left.isNode() )
  {
    value = left.getNode()->getVariable( identifier );
  }
  else
  {
    (void)loc;
    throw "left of '.' is not accessible via '.'";
  }

  if( !value )
  {
    (void)loc;
    throw "identifier not found right of '.'.";
    return false;
  }

  return true;
}

bool InterpretingASTVisitor::assign_impl( const AST::AssignmentStatement *node, Value *lvalue )
{
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
      catch( const char *e )
      {
        throw;
      }
    }
    else
    {
      if( value.getType() != lvalue_type )
      {
        throw "expression type does not match lvalue type";
      }
      lvalue->swap( value );
    }
  }
  else
  {
    const BOP bop = getAssignBOP( node->op );

    Value args[ 3 ];
    args[ 0 ].swap( (*lvalue) ); // move lvalue into arg, will be overwritten anyway
    args[ 1 ] = Value( getBOPStr( bop ) );
    args[ 2 ].swap( value );

    binaryOp->call_impl( node->loc, getRecursionDepth(), scope, (*lvalue), 3, args );

    if( lvalue->getType() != lvalue_type )
    {
      throw "expression type does not match lvalue type";
    }
  }

  return true;
}

bool InterpretingASTVisitor::visit( const AST::ConnectStatement *node )
{
  if( !node->output->invite( this ) ) return false;
  Value output_v; output_v.swap( return_value );

  CountPtr< Output > output( scope->toOutput( node->loc, getRecursionDepth(), output_v ) );
  if( output.isNull() )
  {
    throw "left of '->' is not an Output";
  }

  if( !node->input->invite( this ) ) return false;
  Value input; input.swap( return_value );

  if( !input.isInput() )
  {
    throw "right of '->' is not an Input";
  }

  input.getInput()->connect( output );
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
      throw "dimensions do not match";
    }

    Array< index_t, 1 > coords_int( 0, Dims );
    for( index_t i=0; i<Dims; ++i )
    {
      const Value &c = coords->at( i );
      if( c.isInt() )
      {
        coords_int[ i ] = index_t( c.getInt() );
      }
      else if( c.isInvalid() )
      {
        throw "all coordinates must be defined for access";
      }
      else
      {
        throw "type of coordinate must be int";
      }
    }

    Value lvalue = array->getValue( coords_int.size(), coords_int.elements() );
    if( !assign_impl( node, &lvalue ) ) return false;
    array->setValue( lvalue, coords_int.size(), coords_int.elements() );
    return true;
  }
  else
  {
    throw "left of '[]' must be an Array";
  }
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
  Value *const for_variable = push_for_variable.get();
  if( 0 == for_variable )
  {
    throw "Could not create for-variable";
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
    throw "Invalid type for container after 'in'";
  }

  return true;
}

bool InterpretingASTVisitor::visit( const AST::FunctionCallStatement        *node )
{
  return node->call->invite( this );
}

bool InterpretingASTVisitor::visit( const AST::VariableCreationStatement *node )
{
  const String &identifier = node->identifier;

  CountPtr< TypeDescr > expected_type;
  if( !node->type->invite( this ) ) return false;
  swap( expected_type, return_value_type_descr );

  if( !node->value->invite( this ) ) return false;
  Value value; value.swap( return_value );

  if( expected_type->type.isArray() )
  {
    const index_t expected_dims = expected_type->dims->size();
    Array< index_t, 1 > expected_size( 0, expected_dims );

    for( index_t i=0; i<expected_dims; ++i )
    {
      const Value &d_i = expected_type->dims->at( i );
      if( d_i.isInvalid() )
      {
        expected_size[ i ] = ( value.isArray() ? unknown : 0 );
      }
      else if( d_i.isInt() )
      {
        const int d = d_i.getInt();
        if( d < 0 ) throw "Array size must not be negative";
        expected_size[ i ] = index_t( d );
      }
      else
      {
        throw "Invalid expression type for array dimension, must be static int";
      }
    }

    GarbageCollector *const gc = scope->getGC();
    const Type expected_type_of = expected_type->of->type;
    const index_t *const ex_size = expected_size.elements();

    switch( value.getType().getEnum() )
    {
      // One scalar
      case Type::BOOL    : value = new_Array<           bool             >( gc, expected_size, expected_type_of, value ); break;
      case Type::INT     : value = new_Array<           int              >( gc, expected_size, expected_type_of, value ); break;
      case Type::FLOAT   : value = new_Array<           float            >( gc, expected_size, expected_type_of, value ); break;
      case Type::STRING  : value = new_Array<           String           >( gc, expected_size, expected_type_of, value ); break;
      case Type::FRAME   : value = new_Array< CountPtr< Frame          > >( gc, expected_size, expected_type_of, value ); break;
      case Type::FUNCTION: value = new_Array< CountPtr< Function       > >( gc, expected_size, expected_type_of, value ); break;
      case Type::NODE    : value = new_Array< CountPtr< Node           > >( gc, expected_size, expected_type_of, value ); break;
      case Type::OUTPUT  : value = new_Array< CountPtr< Output         > >( gc, expected_size, expected_type_of, value ); break;
      case Type::INPUT   : value = new_Array< CountPtr< Input          > >( gc, expected_size, expected_type_of, value ); break;
      case Type::PARAM   : value = new_Array< CountPtr< Param          > >( gc, expected_size, expected_type_of, value ); break;
      case Type::SEQUENCE: value = new_Array< CountPtr< Sequence const > >( gc, expected_size, expected_type_of, value ); break;

      // From Array
      case Type::ARRAY:
      {
        const ArrayBase *const value_array_base = value.getArray();

        if( value_array_base->getDims() == expected_dims )
        {
          const index_t *const value_array_size = value_array_base->getSize();
          for( index_t i=0; i<expected_dims; ++i )
          {
            const index_t d_i = expected_size[ i ];
            if( d_i != unknown && d_i != value_array_size[ i ] )
            {
              throw "Size of the array does not match";
            }
          }

          const index_t value_array_dims = value_array_base->getDims();
          if( value_array_base->isValue() )
          {
            const ArrayElements< Value > *value_array = 0;
            if( !value_array_base->getAs( value_array ) )
            {
              throw "Internal: Could not getAs ArrayElements< Value >";
            }

            switch( expected_type_of.getEnum() )
            {
              case Type::BOOL    : value.set( new_Array<           bool             >( gc, value_array ).get() ); break;
              case Type::INT     : value.set( new_Array<           int              >( gc, value_array ).get() ); break;
              case Type::FLOAT   : value.set( new_Array<           float            >( gc, value_array ).get() ); break;
              case Type::STRING  : value.set( new_Array<           String           >( gc, value_array ).get() ); break;
              case Type::FRAME   : value.set( new_Array< CountPtr< Frame          > >( gc, value_array ).get() ); break;
              case Type::FUNCTION: value.set( new_Array< CountPtr< Function       > >( gc, value_array ).get() ); break;
              case Type::NODE    : value.set( new_Array< CountPtr< Node           > >( gc, value_array ).get() ); break;
              case Type::OUTPUT  : value.set( new_Array< CountPtr< Output         > >( gc, value_array ).get() ); break;
              case Type::INPUT   : value.set( new_Array< CountPtr< Input          > >( gc, value_array ).get() ); break;
              case Type::PARAM   : value.set( new_Array< CountPtr< Param          > >( gc, value_array ).get() ); break;
              case Type::SEQUENCE: value.set( new_Array< CountPtr< Sequence const > >( gc, value_array ).get() ); break;
              case Type::ARRAY   :
                {
                  throw "Not implemented yet";
                }
              default:
                throw "Unhandled Type Enum case";
            }
          }
          else if( expected_type->of->type == value_array_base->getType() )
          {
            // already fine
          }
          else
          {
            throw "Type of Array does not match";
          }
        }
        else
        {
          throw "Number of dimensions does not match.";
        }
      }
      break;

      default:
        throw "Unhandled Type Enum case";
    }
  } // isArray
  else
  {
    if(
         expected_type->type != Type::Invalid()
      && value.getType() != expected_type->type
    )
    {
      cerr << "value '" << value.getTypeName() << "' != expected_type '" << expected_type->type.getTypeName() << "'" << endl;
      throw "Type right of '=' does not match declared type";
    }
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


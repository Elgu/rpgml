#include "Function.h"

#include "Scope.h"
#include "Frame.h"
#include "String.h"
#include "SharedObject.h"

namespace RPGML {

Function::Function( GarbageCollector *_gc, Frame *parent, const Args *decl, const SharedObject *so )
: Collectable( _gc )
, m_parent( parent )
, m_decl( decl )
, m_so( so )
{}

Function::~Function( void )
{}

Frame *Function::getParent( void ) const
{
  return m_parent;
}

const Function::Args *Function::getDecl( void ) const
{
  return m_decl.get();
}

const SharedObject *Function::getSO( void ) const
{
  return m_so.get();
}

size_t Function::getNumArgs( void ) const
{
  return m_decl->size();
}

size_t Function::getFrameSize( void ) const
{
  return getNumArgs();
}

bool Function::call( const Location *loc, index_t recursion_depth, Scope *scope, Value &ret, const Args *call_args )
{
  Scope::EnterLeaveGuard guard1( scope, getParent() );
  CountPtr< Frame > args = new Frame( scope->getGC(), scope->getCurr() );
  Scope::EnterLeaveGuard guard2( scope, args );

  fill_args( *args, *call_args );
  const index_t n_args = index_t( args->size() );
  return call_impl( loc, recursion_depth, scope, ret, n_args, ( n_args ? args->getStack( 0 ) : 0 ) );
}

void Function::fill_args( Frame &args, const Args &call_args )
{
  const Args &decl_args = *(m_decl.get());

  const index_t n_decl_args = index_t( decl_args.size() );
  const index_t n_call_args = index_t( call_args.size() );

  if( n_call_args > n_decl_args )
  {
    throw "too many arguments";
  }

  args.reserve( n_decl_args );
  std::vector< int8_t > used( call_args.size(), int8_t( false ) );

  index_t pos = 0;

  // positional arguments, no identifier
  for( ; pos < call_args.size(); ++pos )
  {
    const String &call_identifier = call_args[ pos ].identifier;
    if( call_identifier.empty() )
    {
      args.push_back( decl_args[ pos ].identifier, call_args[ pos ].value );
      used[ pos ] = true;
    }
    else
    {
      break;
    }
  }

  const index_t first_init = pos;

  // identifier arguments
  for( ; pos < decl_args.size(); ++pos )
  {
    const String &decl_identifier = decl_args[ pos ].identifier;
    bool found = false;
    for( index_t i=first_init; i < n_call_args; ++i )
    {
      const String &call_identifier = call_args[ i ].identifier;
      if( !call_identifier.empty() )
      {
        if( !used[ i ] && decl_identifier == call_identifier )
        {
          found = true;
          // Was not yet initialized (even with invalid Value)
          args.push_back( call_identifier, call_args[ i ].value );
          used[ i ] = true;
        }
      }
      else
      {
        throw "no fixed position argument allowed after init argument";
      }
    }

    // No identifier argument specified, try default value
    if( !found )
    {
      const Value &default_value = decl_args[ pos ].value;
      if( !default_value.isInvalid() )
      {
        args.push_back( decl_identifier, default_value );
      }
      else
      {
        throw "no default value for argument, must be specified";
      }
    }
  }

  for( index_t i=first_init; i < call_args.size(); ++i )
  {
    if( !used[ i ] )
    {
      throw "init argument identifier not found or already initialized by fixed argument or dupplicate";
    }
  }
}

void Function::gc_clear( void )
{
  m_parent.reset();
  m_decl.reset();
}

void Function::gc_getChildren( Children &children ) const
{
  children.push_back( m_parent );

  for( size_t i( 0 ), end( m_decl->size() ); i<end; ++i )
  {
    const Value &value = m_decl->at( i ).value;

    if( value.isCollectable() )
    {
      children.push_back( value.getCollectable() );
    }
  }
}

} // namespace RPGML


#include "Function.h"

#include "Scope.h"
#include "Map.h"
#include "String.h"

namespace RPGML {

Function::Function( GarbageCollector *_gc, Map *parent, const Args *decl )
: Collectable( _gc )
, m_parent( parent )
, m_decl( decl )
{}

Function::~Function( void )
{}

Map *Function::getParent( void ) const
{
  return m_parent;
}

bool Function::call( Scope *scope, Value &ret, const Args *call_args, index_t recursion_depth )
{
  Scope::EnterLeaveGuard guard1( scope, getParent() );
  CountPtr< Map > args = new Map( scope->getGC(), scope->getCurr() );
  Scope::EnterLeaveGuard guard2( scope, args );

  fill_args( *args, *call_args );
  const index_t n_args = index_t( args->size() );
  return call_impl( scope, ret, n_args, ( n_args ? &(*args)[ 0 ] : 0 ), recursion_depth );
}

void Function::fill_args( Map &args, const Args &call_args )
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
  for( ; pos < call_args.size(); ++pos )
  {
    const String *const call_identifier = call_args[ pos ].identifier;
    if( !call_identifier )
    {
      args.set( call_identifier, call_args[ pos ].value );
      used[ pos ] = true;
    }
    else
    {
      break;
    }
  }

  const index_t first_init = pos;

  for( ; pos < decl_args.size(); ++pos )
  {
    const String *const decl_identifier = decl_args[ pos ].identifier;
    bool found = false;
    for( index_t i=first_init; i < call_args.size(); ++i )
    {
      const String *const call_identifier = call_args[ i ].identifier;
      if( call_identifier )
      {
        if( !used[ i ] && decl_identifier->equals( call_identifier ) )
        {
          found = true;
          // Was not yet initialized (even with invalid Value)
          args.set( call_identifier, call_args[ i ].value );
          used[ i ] = true;
        }
      }
      else
      {
        throw "no fixed position argument allowed after init argument";
      }
    }

    if( !found )
    {
      const Value &default_value = decl_args[ pos ].value;
      if( !default_value.isInvalid() )
      {
        args.set( decl_identifier, default_value );
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

CountPtr< Collectable::Children > Function::getChildren( void ) const
{
  return
    new MultiIterator< Children >(
        new ArgsChildren( m_decl )
      , new ItemIterator< Children >( m_parent )
      );
}

Function::ArgsChildren::ArgsChildren( const Args *args, index_t i )
: m_args( args )
, m_i( i )
{
  find_next();
}

Function::ArgsChildren::~ArgsChildren( void )
{}

bool Function::ArgsChildren::done( void )
{
  return m_i >= m_args->size();
}

void Function::ArgsChildren::next( void )
{
  ++m_i;
  find_next();
}

const Collectable *Function::ArgsChildren::get( void )
{
  return m_args->at( m_i ).value.getCollectable();
}

CountPtr< Collectable::Children > Function::ArgsChildren::clone( void ) const
{
  return new ArgsChildren( (*this) );
}

void Function::ArgsChildren::find_next( void )
{
  while(
        m_i < m_args->size()
    && !m_args->at( m_i ).value.isCollectable()
    )
  {
    ++m_i;
  }
}

} // namespace RPGML


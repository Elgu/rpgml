#include "Context.h"

#include "Scope.h"
#include "Map.h"
#include "StringUnifier.h"

#include "Print.h"

namespace RPGML {

Context::Context( StringUnifier *unifier )
: m_unifier( unifier )
{
  m_gc.reset( new GarbageCollector() );
  m_root = new Map( getGC() );

  const String unified_print = m_unifier->unify( String::Static( "print" ) );

  m_root->set( Value( unified_print ), Value( new Print( getGC(), m_root ) ) );
}

Context::~Context( void )
{}

CountPtr< Scope > Context::createScope( void )
{
  return new Scope( this );
}

} // namespace RPGML


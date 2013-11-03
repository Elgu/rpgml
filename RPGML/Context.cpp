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

  m_root->set( m_unifier->unify( "print" ), new Print( getGC(), m_root ) );
}

Context::~Context( void )
{}

CountPtr< Scope > Context::createScope( void )
{
  return new Scope( this );
}

} // namespace RPGML


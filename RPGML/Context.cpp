#include "Context.h"

#include "Scope.h"
#include "Frame.h"
#include "StringUnifier.h"

namespace RPGML {

Context::Context( StringUnifier *unifier )
: m_unifier( unifier )
, m_nr( 0 )
{
  m_gc.reset( new GarbageCollector() );
  m_root = new Frame( getGC() );
}

Context::~Context( void )
{}

CountPtr< Scope > Context::createScope( void )
{
  return new Scope( this );
}

size_t Context::getNr( void )
{
  return m_nr++;
}

} // namespace RPGML


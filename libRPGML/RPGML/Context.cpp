#include "Context.h"

#include "Graph.h"
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

CountPtr< Graph > Context::createGraph( void ) const
{
  CountPtr< Graph > graph( new Graph( getGC() ) );

  // Just add every Node
  std::vector< const Frame* > stack;
  stack.push_back( m_root );

  while( !stack.empty() )
  {
    const Frame *frame = stack.back();
    stack.pop_back();
    if( !frame ) continue;

    for( index_t i( 0 ), end( frame->getSize() ); i < end; ++i )
    {
      const Value *const value = frame->getStack( i );
      if( !value ) continue;

      switch( value->getEnum() )
      {
        case Type::FRAME:
          stack.push_back( value->getFrame() );
          break;

        case Type::NODE:
          graph->addNode( value->getNode() );
          break;

        default: {}
      }
    }
  }

  return graph;
}

size_t Context::getNr( void )
{
  return m_nr++;
}

} // namespace RPGML


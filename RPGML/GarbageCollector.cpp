#include "GarbageCollector.h"

#include <cassert>
#include <memory>

namespace RPGML {

GarbageCollector::GarbageCollector( void )
{}

GarbageCollector::~GarbageCollector( void )
{
  sweep( m_cs );
}

void GarbageCollector::add( const Collectable *c )
{
  if( 0 == this ) return;
  if( c->gc ) c->gc->remove( c );

  c->gc = this;
  c->gc_index = index_t( m_cs.size() );

  m_cs.push_back( c );

#ifdef GC_DEBUG
  std::cerr << "add " << c << " to " << this << ", index = " << c->gc_index << std::endl;
#endif
}

void GarbageCollector::remove( const Collectable *c )
{
  if( 0 == this ) return;
  assert( c->gc == this );

  const index_t index = c->gc_index;
  if( index < m_cs.size() )
  {
    if( m_cs[ index ] == c )
    {
#ifdef GC_DEBUG
      std::cerr << "remove " << c << " from " << this << ", index = " << c->gc_index << std::endl;
#endif

      m_cs[ index ] = 0;
      c->gc = 0;
      c->gc_index = 0;
    }
  }
}

void GarbageCollector::compact( std::vector< const Collectable* > &cs_new, const Collectable *root )
{
  if( !root ) return;

  std::vector< const Collectable* > stack;
  stack.reserve( 16 );

  // NOTE: This could also be done the other way, just schedule for checking,
  //       but there would be the risk of having a very large schedule stack
  //       with dupplicate entries

  // Remove from old storage, add to new
  m_cs[ root->gc_index ] = 0;
  root->gc_index = index_t( cs_new.size() );
  cs_new.push_back( root );

  // Schedule root
  stack.push_back( root );

  // Process children of scheduled (already moved) Collectables
  while( !stack.empty() )
  {
    const Collectable *const c = stack.back();
    stack.pop_back();

    for( CountPtr< Collectable::Children > iter( c->getChildren() ); !iter->done(); iter->next() )
    {
      const Collectable *const child = iter->get();

      // Check, if not compacted yet
      if( child->gc == this && m_cs[ child->gc_index ] == child )
      {
        // Remove from old storage, add to new
        m_cs[ child->gc_index ] = 0;
        child->gc_index = index_t( cs_new.size() );
        cs_new.push_back( child );

        // Schedule child
        stack.push_back( child );
      }
    }
  }
}

void GarbageCollector::sweep( std::vector< const Collectable* > &garbage )
{
  for( index_t i=0; i<garbage.size(); ++i )
  {
    const Collectable *const chunk = garbage[ i ];
    if( chunk ) chunk->deactivate_deletion();
  }

  for( index_t i=0; i<garbage.size(); ++i )
  {
    const Collectable *const chunk = garbage[ i ];
    if( chunk ) const_cast< Collectable* >( chunk )->gc_clear();
  }

  for( index_t i=0; i<garbage.size(); ++i )
  {
    const Collectable *const chunk = garbage[ i ];
    if( chunk ) delete const_cast< Collectable* >( chunk );
  }
}

void GarbageCollector::run( void )
{
  if( m_cs.empty() ) return;

  const Collectable *const root = m_cs[ 0 ];
  if( 0 == root ) return;

  std::vector< const Collectable* > cs_new;
  cs_new.reserve( m_cs.size() / 2 + 1 );

  // compact recursively
  compact( cs_new, root );

  // make new cs current
  std::swap( cs_new, m_cs );

  // cs_new now contains garbage
  sweep( cs_new );
}

} // namespace RPGML


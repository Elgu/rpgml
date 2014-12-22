#include "GarbageCollector.h"

#include <cassert>
#include <memory>

namespace RPGML {

GarbageCollector::GarbageCollector( void )
: m_lock( Mutex::Recursive() )
, m_root( 0 )
{}

GarbageCollector::~GarbageCollector( void )
{
  sweep( m_cs );
}

void GarbageCollector::add( const Collectable *c )
{
  if( 0 == this ) return;
  if( 0 == c ) return;
	if( c->gc == this ) return;
  if( c->gc ) c->gc->remove( c );

  { Mutex::ScopedLock lock( &m_lock );

    c->gc = this;
    c->gc_index = index_t( m_cs.size() );
    m_cs.push_back( c );
	  if( 0 == m_root ) m_root = c;
  } // m_lock

#ifdef GC_DEBUG
  std::cerr << "add " << c << " to " << this << ", index = " << c->gc_index << std::endl;
#endif
}

void GarbageCollector::remove( const Collectable *c )
{
  if( 0 == this ) return;
  assert( c->gc == this );

  { Mutex::ScopedLock lock( &m_lock );

    if( c == m_root ) m_root = 0;

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
  } // m_lock
}

void GarbageCollector::setRoot( const Collectable *c )
{
	add( c );
	m_root = c;
}

void GarbageCollector::compact( std::vector< const Collectable* > &cs_new )
{
  if( !m_root ) return;

  std::vector< const Collectable* > stack;
  stack.reserve( 512 );

  // NOTE: This could also be done the other way, just schedule for checking,
  //       but there would be the risk of having a very large schedule stack
  //       with dupplicate entries

  // Remove from old storage, add to new
  m_cs[ m_root->gc_index ] = 0;
  m_root->gc_index = index_t( cs_new.size() );
  cs_new.push_back( m_root );

  // Schedule root
  stack.push_back( m_root );

  Collectable::Children children;
  children.reserve( 512 );

  // Process children of scheduled (already moved) Collectables
  while( !stack.empty() )
  {
    const Collectable *const c = stack.back();
    stack.pop_back();

    children.clear();
    c->gc_getChildren( children );

    for( size_t i( 0 ), end( children.size() ); i<end; ++i )
    {
      const Collectable *const child = children[ i ];
      if( !child ) continue;

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
  if( 0 == m_root ) return;

  { Mutex::ScopedLock lock( &m_lock );

    std::vector< const Collectable* > cs_new;
    cs_new.reserve( m_cs.size() / 2 + 1 );

    // compact recursively
    compact( cs_new );

    // make new cs current
    std::swap( cs_new, m_cs );

    // cs_new now contains garbage
    sweep( cs_new );

  } // m_lock
}

bool Collectable::Children::contains( Collectable *c ) const
{
  return ( m_children.end() != std::find( m_children.begin(), m_children.end(), c ) );
}

} // namespace RPGML


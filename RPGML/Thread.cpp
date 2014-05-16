#include "Thread.h"

namespace RPGML {

class Thread::Impl
{
public:
  Impl( void )
  {}

  ~Impl( void )
  {}

private:
};

Thread::Thread( GarbageCollector *_gc )
: Collectable( _gc )
, impl( new Impl )
{}

Thread::~Thread( void )
{
  delete impl;
}

void Thread::gc_clear( void )
{}

void Thread::gc_getChildren( Children &children ) const
{}

} // namespace RPGML


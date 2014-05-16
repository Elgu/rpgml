#ifndef RPGML_Thread_h
#define RPGML_Thread_h

#include "GarbageCollector.h"

namespace RPGML {

class Thread : public Collectable
{
public:
  explicit
  Thread( GarbageCollector *_gc );

  virtual ~Thread( void );

  virtual void gc_clear( void );
  virtual void gc_getChildren( Children &children ) const;

protected:


private:
  class Impl;
  Impl *const impl;
};


} // namespace RPGML

#endif


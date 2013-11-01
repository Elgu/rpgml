#ifndef RPGML_Context_h
#define RPGML_Context_h

#include "GarbageCollector.h"

#include <memory>

namespace RPGML {

class Scope;
class Map;
class StringUnifier;

//! Do not allocate on the stack
class Context : public Refcounted
{
public:
  explicit
  Context( StringUnifier *unifier );

  virtual ~Context( void );

  Map *getRoot( void ) { return m_root; }

  CountPtr< Scope > createScope( void );

  StringUnifier *getUnifier( void ) const { return m_unifier; }
  GarbageCollector *getGC( void ) const { return m_gc.get(); }

private:
  std::auto_ptr< GarbageCollector > m_gc;
  CountPtr< Map > m_root;
  CountPtr< StringUnifier > m_unifier;
};

} // namespace RPGML

#endif


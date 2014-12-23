#ifndef RPGML_Context_h
#define RPGML_Context_h

#include "GarbageCollector.h"

#include <memory>

namespace RPGML {

class Graph;
class Scope;
class Frame;
class StringUnifier;

//! Do not allocate on the stack
class Context : public Refcounted
{
public:
  explicit
  Context( StringUnifier *unifier );

  virtual ~Context( void );

  Frame *getRoot( void ) const { return m_root; }

  CountPtr< Scope > createScope( void );

  StringUnifier *getUnifier( void ) const { return m_unifier; }
  GarbageCollector *getGC( void ) const { return m_gc.get(); }

  CountPtr< Graph > createGraph( void ) const;

  size_t getNr( void );
private:
  std::auto_ptr< GarbageCollector > m_gc;
  CountPtr< Frame > m_root;
  CountPtr< StringUnifier > m_unifier;
  size_t m_nr;
};

} // namespace RPGML

#endif


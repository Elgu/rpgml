#ifndef RPGML_types_h
#define RPGML_types_h

#include <stdint.h>

namespace RPGML {

  typedef uint32_t index_t;
  static const index_t unknown = index_t(-1);

  template< bool b > struct StaticAssert { enum{ valid }; };
  template<> struct StaticAssert< false >{};

  #define rpgml_static_assert( B ) (void)StaticAssert< B >::valid

} // namespace RPGML

#endif


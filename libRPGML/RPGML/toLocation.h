#ifndef RPGML_toLocation_h
#define RPGML_toLocation_h

#include "location.hh"
#include "Location.h"

namespace RPGML {

inline
CountPtr< Location > toLocation( const String &filename, const location &loc )
{
  return
    new Location(
        filename
      , loc.begin.line
      , loc.begin.column
      , loc.end.line
      , loc.end.column
      );
}

} // namespace RPGML

#endif


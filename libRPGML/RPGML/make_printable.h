#ifndef RPGML_make_printable_h
#define RPGML_make_printable_h

#include "String.h"

#include <ctype.h>
#include <string>

namespace RPGML {

static inline
String make_printable( const String &s )
{
  std::string ret;

  for( const char *c = s.c_str(); (*c) != '\0'; ++c )
  {
    switch( (*c) )
    {
      case '\a': ret += "\\a"; break;
      case '\b': ret += "\\b"; break;
      case '\f': ret += "\\f"; break;
      case '\n': ret += "\\n"; break;
      case '\r': ret += "\\r"; break;
      case '\t': ret += "\\t"; break;
      case '\v': ret += "\\v"; break;
      default:
        if( ::isprint( (*c) ) )
        {
          ret += (*c);
        }
        else
        {
          ret += "\\x";
          ret += char( '0' + char(((*c)&0xF0)>>4) );
          ret += char( '0' + char( (*c)&0x0F)   );
        }
        break;
    }
  }

  return String::MoveFrom( ret );
}

} // namespace RPGML

#endif

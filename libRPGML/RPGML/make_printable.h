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
          static const char *const ascii = "0123456789ABCDEF";
          ret += ascii[ ((*c)&0xF0)>>4 ];
          ret += ascii[  (*c)&0x0F     ];
        }
        break;
    }
  }

  return String::MoveFrom( ret );
}

static inline
String from_printable( const String &s )
{
  std::string ret;

  for( const char *c = s.c_str(); (*c) != '\0'; ++c )
  {
    if( (*c) == '\\' )
    {
      ++c;
      switch( (*c) )
      {
        case '\0': break;
        case 'a': ret += '\a'; break;
        case 'b': ret += '\b'; break;
        case 'f': ret += '\f'; break;
        case 'n': ret += '\n'; break;
        case 'r': ret += '\r'; break;
        case 't': ret += '\t'; break;
        case 'v': ret += '\v'; break;
        case 'x':
          {
            int x = 0;
            ++c;
            for( int i=0; i<2 && (*c); ++c )
            {
              int xi = 0;
              if     ( (*c) >= '0' && (*c) <= '9' ) xi = (*c) - '0';
              else if( (*c) >= 'a' && (*c) <= 'z' ) xi = (*c) - 'a';
              else if( (*c) >= 'A' && (*c) <= 'Z' ) xi = (*c) - 'A';
              else break;

              x <<= 4;
              x += xi;
            }
            ret += char( x );
          }
          break;

        default:
          ret += (*c);
          break;
      }
    }
    else
    {
      ret += (*c);
    }
  }

  return String::MoveFrom( ret );
}

} // namespace RPGML

#endif

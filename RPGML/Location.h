#ifndef RPGML_Location_h
#define RPGML_Location_h

#include "String.h"

#include <ostream>

namespace RPGML {

class Location : public Refcounted
{
public:
  struct Position
  {
    Position( int _line=0, int _column=0 )
    : line( _line ), column( _column )
    {}
    void set( int _line=0, int _column=0 )
    {
      line = _line;
      column = _column;
    }
    void swap( Position &other )
    {
      std::swap( line, other.line );
      std::swap( column, other.column );
    }
    int line;
    int column;
  };

  Location( void )
  {}

  explicit
  Location(
      const String &_filename
    , int begin_line=0, int begin_column=0
    , int end_line=0, int end_column=0
    )
  : filename( _filename )
  , begin( begin_line, begin_column )
  , end( end_line, end_column )
  {}

  virtual ~Location( void )
  {}

  Location &set(
      const String &_filename
    , int begin_line=0, int begin_column=0
    , int end_line=0, int end_column=0
    )
  {
    filename = _filename;
    begin.set( begin_line, begin_column );
    end.set( end_line, end_column );
    return (*this);
  }

  void swap( Location &other )
  {
    std::swap( filename, other.filename );
    begin.swap( other.begin );
    end.swap( other.end );
  }

  std::ostream &print( std::ostream &o )
  {
    if( filename )
    {
      o << filename << ":";
    }
    if( begin.line > 0 )
    {
      o << begin.line;
      if( begin.column > 0 )
      {
        o << "." << begin.column;
      }
      if( end.line >= begin.line )
      {
        o << "-";
        if( end.line > begin.line )
        {
          o << end.line;
        }
        if(
            end.column > 0
          &&
            (
               ( end.line == begin.line && end.column > begin.column )
            || ( end.line > begin.line )
            )
          )
        {
          o << "." << end.column;
        }
      }
    }
    return o;
  }

private:
  String filename;
  Position begin;
  Position end;
};

} // namespace RPGML

namespace std {

inline
void swap( RPGML::Location::Position &x1, RPGML::Location::Position &x2 )
{
  x1.swap( x2 );
}

inline
void swap( RPGML::Location &x1, RPGML::Location &x2 )
{
  x1.swap( x2 );
}

inline
std::ostream &operator<<( std::ostream &o, RPGML::Location &p )
{
  return p.print( o );
}

} // namespace std

#endif


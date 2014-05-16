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

//  Location( void )
//  {}

  explicit
  Location(
      const String &_filename
    , int begin_line=0, int begin_column=0
    , int end_line=0, int end_column=0
    , const Location *_parent=0
    )
  : filename( _filename )
  , begin( begin_line, begin_column )
  , end( end_line, end_column )
  , parent( _parent )
  {}

  virtual ~Location( void )
  {}


  std::ostream &print( std::ostream &o, bool with_filename=true, bool is_parent=false ) const
  {
    if( parent )
    {
      parent->print( o, with_filename, true );
      o << std::endl;
    }

    if( is_parent ) o << "at ";

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

  class WithoutFilename
  {
    friend class Location;
  private:
    explicit
    WithoutFilename( const Location *_loc )
    : loc( _loc )
    {}
  public:
    CountPtr< const Location > loc;
  };

  WithoutFilename withoutFilename( void ) const
  {
    return WithoutFilename( this );
  }

private:
  const String filename;
  const Position begin;
  const Position end;
  const CountPtr< const Location > parent;
};

} // namespace RPGML

inline
std::ostream &operator<<( std::ostream &o, const RPGML::Location &p )
{
  return p.print( o );
}

inline
std::ostream &operator<<( std::ostream &o, const RPGML::Location::WithoutFilename &p )
{
  return p.loc->print( o, false );
}

#endif


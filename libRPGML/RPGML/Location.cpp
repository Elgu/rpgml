/* This file is part of RPGML.
 *
 * Copyright (c) 2015, Gunnar Payer, All rights reserved.
 *
 * RPGML is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 3.0 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library.
 */
#include "Location.h"

namespace RPGML {

Location::Location(
    const String &_filename
  , int begin_line, int begin_column
  , int end_line, int end_column
  , const Location *_parent
  )
: filename( _filename )
, begin( begin_line, begin_column )
, end( end_line, end_column )
, parent( _parent )
{}

Location::~Location( void )
{}

std::ostream &Location::print( std::ostream &o, bool with_filename, bool is_parent ) const
{
  if( parent )
  {
    parent->print( o, with_filename, true );
    o << std::endl;
  }

  if( is_parent ) o << "at ";

  if( !filename.empty() )
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
        if( end.line > begin.line ) o << "."; // end.line has been printed
        o << end.column;
      }
    }
  }
  return o;
}

} // namespace std


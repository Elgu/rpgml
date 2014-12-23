/* This file is part of RPGML.
 * 
 * Copyright (c) 2014, Gunnar Payer, All rights reserved.
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


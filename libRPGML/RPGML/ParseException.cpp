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
#include "ParseException.h"

namespace RPGML {

ParseException::ParseException( const Location *_loc )
: loc( _loc )
{}

ParseException::ParseException( const Location *_loc, const ParseException &e )
: Base( e.getText() )
, loc( new Location( e.loc, _loc ) )
{
  e.copyBacktrace( *this );
}

ParseException::ParseException( const Location *_loc, const RPGML::Exception &e )
: Base( e.getText() )
, loc( _loc )
{
  e.copyBacktrace( *this );
}

ParseException::ParseException( const Location *_loc, const char *e )
: Base( e )
, loc( _loc )
{}

const char *ParseException::what() const throw()
{
  std::ostringstream o;
  o << (*loc) << ": " << Base::what();
  m_what = o.str();
  return m_what.c_str();
}

} // namespace RPGML


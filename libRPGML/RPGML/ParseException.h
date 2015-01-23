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
#ifndef RPGML_ParseException_h
#define RPGML_ParseException_h

#include "Exception.h"
#include "Location.h"

namespace RPGML {

class ParseException : public RPGML::Exception
{
public:
  typedef RPGML::Exception Base;

  explicit
  ParseException( const Location *_loc );

  //! @brief Takes Location of e and makes _loc parent
  ParseException( const Location *_loc, const ParseException &e );
  ParseException( const Location *_loc, const RPGML::Exception &e );
  ParseException( const Location *_loc, const char *e );

  virtual const char *what() const throw();

  EXCEPTION_BODY( ParseException );

  CountPtr< const Location > loc;
private:
  mutable std::string m_what;
};

} // namespace RPGML

#endif


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
#ifndef RPGML_SaveBool_h
#define RPGML_SaveBool_h

namespace RPGML {

class SaveBool
{
public:
  typedef void (SaveBool::*bool_type)() const;

  static
  bool_type toSaveBool( bool b )
  {
    return ( b ? &SaveBool::bool_type_method : 0 );
  }

private:
  void bool_type_method( void ) const {}
};

} // namespace RPGML

#endif


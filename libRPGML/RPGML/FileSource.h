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
#ifndef RPGML_FileSource_h
#define RPGML_FileSource_h

#include "Scanner.h"

#include <cstdio>

namespace RPGML {

class FileSource : public Source
{
public:
  FileSource( FILE *file )
  : m_file( file )
  {}

  virtual ~FileSource( void )
  {
    fclose( m_file );
  }

protected:
  virtual char nextChar( void )
  {
    const int ret = fgetc( m_file );
    if( EOF == ret )
    {
      return '\0';
    }
    return char( ret );
  }

private:
  FILE *m_file;
};

} // namespace RPGML

#endif


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
    ::fclose( m_file );
  }

  virtual const char *nextChars( void )
  {
    const size_t n = ::fread( m_buffer, 1, BUFFER_SIZE-1, m_file );
    if( 0 == n ) return 0;
    m_buffer[ n ] = '\0';
    return m_buffer;
  }

private:
  static const int BUFFER_SIZE = 4096;
  char m_buffer[ BUFFER_SIZE ];
  FILE *m_file;
};

} // namespace RPGML

#endif


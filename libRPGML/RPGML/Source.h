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
#ifndef RPGML_Source_h
#define RPGML_Source_h

#include "Refcounted.h"

#include <iostream>

namespace RPGML {

class Source : public Refcounted
{
  typedef Refcounted Base;
public:
  Source( void );
  virtual ~Source( void );

  virtual const char *nextChars( void ) = 0;
};

class CStringSource : public Source
{
  typedef Source Base;
public:
  explicit
  CStringSource( const char *str );
  virtual ~CStringSource( void );

protected:
  virtual const char *nextChars( void );

private:
  const char *m_str;
};

class StdIStreamSource : public Source
{
  typedef Source Base;
public:
  explicit
  StdIStreamSource( std::istream *stream );
  virtual ~StdIStreamSource( void );

protected:
  virtual const char *nextChars( void );

private:
  static const int BUFFER_SIZE = 256;
  char m_buffer[ BUFFER_SIZE ];
  std::istream *const m_stream;
};

} // namespace RPGML

#endif


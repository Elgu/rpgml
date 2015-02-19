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
#include "Source.h"

namespace RPGML {

Source::Source( void )
{}

Source::~Source( void )
{}

CStringSource::CStringSource( const char *str )
: m_str( str )
{}

CStringSource::~CStringSource( void )
{}

const char *CStringSource::nextChars( void )
{
  const char *const ret = m_str;
  m_str = 0;
  return ret;
}

StdIStreamSource::StdIStreamSource( std::istream *stream )
: m_stream( stream )
{
  m_buffer[ 0 ] = '\0';
}

StdIStreamSource::~StdIStreamSource( void )
{}

const char *StdIStreamSource::nextChars( void )
{
  m_stream->read( m_buffer, BUFFER_SIZE-1 );
  const size_t chars_read = m_stream->gcount();
  m_buffer[ chars_read ] = '\0';
  return m_buffer;
}

} // namespace RPGML


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
#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

#include "Backtrace.h"
#include "Guard.h"
#include "types.h"

#include <algorithm>
#include <execinfo.h>
#include <unistd.h>
#include <sstream>
#include <iostream>
#include <vector>
#include <cstring>
#include <cerrno>
#include <cstdio>

#include <dlfcn.h>

using namespace std;

namespace RPGML {

Backtrace::Backtrace( void )
: m_backtrace_n( ::backtrace( m_backtrace_buffer, BACKTRACE_BUFFER_SIZE ) )
{}

Backtrace::~Backtrace( void )
{}

namespace Backtrace_impl {

  static void free_charpp( char **p )
  {
    ::free( p );
  }

} // namespace Backtrace_impl

void Backtrace::print( std::ostream &o ) const
{
  // cxxabi.h
  // char *__cxxabiv1::__cxa_demangle (const char *__mangled_name, char *__output_buffer, size_t *__length, int *__status)

//  ::backtrace_symbols_fd( m_backtrace_buffer, m_backtrace_n, STDERR_FILENO );

  Guard< char* > strings( ::backtrace_symbols( m_backtrace_buffer, m_backtrace_n ), Backtrace_impl::free_charpp );

  for( int i=0; i<m_backtrace_n; ++i )
  {
    ostringstream command;

    string binary;

    char *c = strings[ i ];

    // Binary name
    for( ; (*c) != '\0'; ++c )
    {
      if( (*c) == '[' || (*c) == '(' )
      {
        break;
      }
      binary += (*c); // Part of the binary name
    }

    // Find the address
    const char *addr = 0;
    for( ; (*c) != '\0'; ++c )
    {
      if( (*c) == '[' )
      {
        addr = c+1;
      }
      if( (*c) == ']' )
      {
        (*c) = '\0';
        break;
      }
    }

    if( !addr ) continue;
    uint8_t *addr_vp = 0;
    sscanf( addr, "%p", &addr_vp );

    uint8_t *addr_offset = 0;
    Dl_info info;
    if( dladdr( addr_vp, &info ) )
    {
      addr_offset = (uint8_t*)info.dli_fbase;
    }

    command
      << "addr2line -e " << binary
      << " -C -f -i -p "
      << (void*)( addr_vp - size_t( addr_offset ) )
      ;

//    cerr << "command = '" << command.str() << "'" << endl;
    Guard< FILE, int > demangled( ::popen( command.str().c_str(), "r" ), ::pclose );

    if( demangled.isNull() )
    {
      o << strings[ i ] << endl;
      break;
    }
    else
    {
      char buffer[ 256+1 ];
      size_t num_read = 0;
      while( 0 != ( num_read = fread( buffer, 1, 256, demangled ) ) )
      {
        buffer[ num_read ] = '\0';
        o << buffer;
      }
    }
  }
}

void Backtrace::copy( Backtrace &dest ) const
{
  std::copy(
      m_backtrace_buffer
    , m_backtrace_buffer + BACKTRACE_BUFFER_SIZE
    , dest.m_backtrace_buffer
    );
}

} // namespace RPGML


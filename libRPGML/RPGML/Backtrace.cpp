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
#include "Backtrace.h"

#include <algorithm>
#include <execinfo.h>
#include <unistd.h>

namespace RPGML {

Backtrace::Backtrace( void )
: m_backtrace_n( ::backtrace( m_backtrace_buffer, BACKTRACE_BUFFER_SIZE ) )
{}

Backtrace::~Backtrace( void )
{}

void Backtrace::print( void ) const
{
  ::backtrace_symbols_fd( m_backtrace_buffer, m_backtrace_n, STDERR_FILENO );
  // cxxabi.h
  // char *__cxxabiv1::__cxa_demangle (const char *__mangled_name, char *__output_buffer, size_t *__length, int *__status)
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


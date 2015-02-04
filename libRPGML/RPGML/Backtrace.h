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
#ifndef RPGML_Backtrace_h
#define RPGML_Backtrace_h

#include <ostream>

namespace RPGML {

class Backtrace
{
public:
  Backtrace( void );
  ~Backtrace( void );

  void print( std::ostream &o ) const;
  void copy( Backtrace &dest ) const;

private:
  static const int BACKTRACE_BUFFER_SIZE = 64;
  void *m_backtrace_buffer[ BACKTRACE_BUFFER_SIZE ];
  int m_backtrace_n;
};

} // namespace RPGML

namespace std {

static inline
std::ostream &operator<<( std::ostream &o, const RPGML::Backtrace &backtrace )
{
  backtrace.print( o );
  return o;
}

} // namespace std

#endif


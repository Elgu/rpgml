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
/*! @file Exception.cpp
 * @brief Exception class implementation
 * @author Gunnar Payer
 * @date November 27th 2014
 */
#include "Exception.h"

namespace RPGML {

Exception::Exception( void ) throw()
{
  breakpoint();
}

Exception::Exception( const std::string &text ) throw()
: m_text( text )
{
  breakpoint();
}

void NOINLINE Exception::breakpoint( void )
{
  return;
}

void Exception::print_backtrace( void ) const
{
  m_backtrace.print();
}

Exception::~Exception( void ) throw()
{}

const std::string &Exception::getText( void ) const
{
  return m_text;
}

const char *Exception::what() const throw()
{
  return getText().c_str();
}

Exception &Exception::append( const std::string &text )
{
  m_text += text;
  return (*this);
}

const Backtrace &Exception::getBacktrace( void ) const
{
  return m_backtrace;
}

void Exception::copyBacktrace( Exception &dest ) const
{
  m_backtrace.copy( dest.m_backtrace );
}

} // namespace RPGML


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
#include "SharedObject.h"

#include <cerrno>
#include <cstring>
#include <dlfcn.h>

namespace RPGML {

SharedObject::SharedObject( const String      &so, String &err )
: m_so( so )
, m_handle( dlopen( so.c_str(), RTLD_NOW | RTLD_DEEPBIND | RTLD_LOCAL ) )
{
  if( !isValid() )
    err = String( dlerror() );
  else
    err.clear();
}

SharedObject::SharedObject( const std::string &so, String &err )
: m_so( so )
, m_handle( dlopen( so.c_str(), RTLD_NOW | RTLD_DEEPBIND | RTLD_LOCAL ) )
{
  if( !isValid() )
    err = String( dlerror() );
  else
    err.clear();
}

SharedObject::SharedObject( const char *so, String &err )
: m_so( so )
, m_handle( dlopen( so, RTLD_NOW | RTLD_DEEPBIND | RTLD_LOCAL ) )
{
  if( !isValid() )
    err = String( dlerror() );
  else
    err.clear();
}

SharedObject::~SharedObject( void )
{
//  if( m_handle ) dlclose( m_handle );
}

bool SharedObject::isValid( void ) const
{
  return ( 0 != m_handle );
}

void *SharedObject::getVoidPtrSymbol( const char *sym, String &err ) const
{
  if( !isValid() )
  {
    err = "No shared object loaded";
    return 0;
  }
  else
  {
    void *const ret = dlsym( m_handle, sym );
    if( !ret )
    {
      err = dlerror();
    }
    else
    {
      err.clear();
    }
    return ret;
  }
}

} // namespace RPGML


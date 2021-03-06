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
#ifndef RPGML_SharedObject_h
#define RPGML_SharedObject_h

#include "String.h"
#include <string>

namespace RPGML {

  class SharedObject : public Refcounted
  {
  public:
    explicit SharedObject( const String      &so, String &err );
    explicit SharedObject( const char        *so, String &err );
    explicit SharedObject( const std::string &so, String &err );

    virtual ~SharedObject( void );

    bool isValid( void ) const;

    template< class SymbolType >
    bool getSymbol( const char *sym_id, SymbolType *&sym, String &err ) const
    {
      sym = (SymbolType*)getVoidPtrSymbol( sym_id, err );
      return ( sym != NULL );
    }

    const String &getSO( void ) const { return m_so; }

  private:
    const String m_so;
    void *getVoidPtrSymbol( const char *sym, String &err ) const;
    void *const m_handle;
  };

} // namespace RPGML

#endif


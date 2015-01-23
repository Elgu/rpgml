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
#ifndef RPGML_core_Function_get_h
#define RPGML_core_Function_get_h

#include <RPGML/Function.h>

namespace RPGML {
namespace core {

class Function_get : public Function
{
public:
  EXCEPTION_BASE( Exception );

  Function_get( GarbageCollector *gc, Frame *parent, const SharedObject *so=0 );
  virtual ~Function_get( void );

  virtual bool call_impl( const Location *loc, index_t recursion_depth, Scope *scope, Value &ret, index_t n_args, const Value *args );
  virtual const char *getName( void ) const;

private:
  enum ArgNr
  {
    ARG_WHAT ,
    ARG_IN   ,
    NUM_ARGS
  };

  static
  CountPtr< Args > genDeclArgs( void );
};

} // namespace core
} // namespace RPGML

#endif

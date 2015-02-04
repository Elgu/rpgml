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
#ifndef RPGML_types_h
#define RPGML_types_h

#include <stdint.h>

namespace RPGML {

  typedef uint32_t index_t;
  static const index_t unknown = index_t(-1);

  typedef int32_t stride_t;

  template< bool B > struct StaticAssert { /* false */ };
  template<> struct StaticAssert< true >{ static const bool B = true; };

  #define rpgml_static_assert( X ) (void)StaticAssert< X >::B

} // namespace RPGML

#endif


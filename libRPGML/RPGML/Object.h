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
#ifndef RPGML_OBJECT_H
#define RPGML_OBJECT_H

#include "GarbageCollector.h"

namespace RPGML {

class Object : public Collectable
{
  typedef Collectable Base;
public:
  explicit
  Object( GarbageCollector *_gc );
  virtual ~Object( void );

  virtual
};

} // namespace RPGML


#endif // RPGML_OBJECT_H

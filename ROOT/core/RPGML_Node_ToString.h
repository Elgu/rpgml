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
#ifndef RPGML_Node_core_ToString_h
#define RPGML_Node_core_ToString_h

#include <RPGML/Node.h>

namespace RPGML {
namespace core {

class ToString : public Node
{
  typedef Node Base;
public:
  EXCEPTION_BASE( Exception );

  ToString( GarbageCollector *gc, const String &identifier, const SharedObject *so );
  virtual ~ToString( void );

  virtual const char *getName( void ) const;

  virtual bool tick( void );

  virtual void gc_clear( void );
  virtual void gc_getChildren( Children &children ) const;

private:
  typedef NodeParam< ToString > NParam;

  enum Inputs
  {
    INPUT_IN,
    NUM_INPUTS
  };

  enum Outputs
  {
    OUTPUT_OUT,
    NUM_OUTPUTS
  };

  enum Params
  {
    NUM_PARAMS
  };
};

 } // namespace core {
} // namespace RPGML

#endif


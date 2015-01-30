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
#ifndef RPGML_Node_core_At_h
#define RPGML_Node_core_At_h

#include <RPGML/Node.h>

namespace RPGML {
namespace core {

class At : public Node
{
  typedef Node Base;
public:
  EXCEPTION_BASE( Exception );

  At( GarbageCollector *gc, const String &identifier, const SharedObject *so );
  virtual ~At( void );

  virtual const char *getName( void ) const;

  virtual bool tick( void );

  virtual void gc_clear( void );
  virtual void gc_getChildren( Children &children ) const;

private:
  typedef NodeParam< At > NParam;

  enum Inputs
  {
    INPUT_IN,
    INPUT_X,
    INPUT_Y,
    INPUT_Z,
    INPUT_T,
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

  template< class InType >
  bool tick1( const ArrayBase *in_base, Type coord_type, int coord_dims );

  template< class InType, class CoordType >
  bool tick2( const ArrayBase *in_base, int coord_dims );
};

} // namespae core
} // namespace RPGML

#endif


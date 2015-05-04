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
#ifndef RPGML_Node_util_Paste_h
#define RPGML_Node_util_Paste_h

#include <RPGML/Node.h>

namespace RPGML {
namespace util {

class Paste : public Node
{
  typedef Node Base;
public:
  EXCEPTION_BASE( Exception );

  Paste( GarbageCollector *gc, const String &identifier, const SharedObject *so );
  virtual ~Paste( void );

  virtual const char *getName( void ) const;

  virtual bool tick( void );

  virtual void gc_clear( void );
  virtual void gc_getChildren( Children &children ) const;

private:
  typedef NodeParam< Paste > NParam;

  enum Inputs
  {
    INPUT_IN  ,
    INPUT_DEST,
    INPUT_X   ,
    INPUT_Y   ,
    INPUT_Z   ,
    INPUT_T   ,
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

  template< class T >
  bool tick2(
      const ArrayBase *in_base
    , const ArrayBase *dest_base
    , int dims
    , const index_t *pos_in
    , const index_t *pos_dest
    , const index_t *size
    );
};

 } // namespace util {
} // namespace RPGML

#endif


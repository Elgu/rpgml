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
#ifndef RPGML_Node_core_ConstantArray_h
#define RPGML_Node_core_ConstantArray_h

#include <RPGML/Node.h>

namespace RPGML {
namespace core {

class ConstantArray : public Node
{
public:
  typedef Node Base;
  EXCEPTION_BASE( Exception );

  ConstantArray( GarbageCollector *gc, const String &identifier, const SharedObject *so );
  virtual ~ConstantArray( void );

  virtual const char *getName( void ) const;

  virtual bool tick( void );

  virtual void gc_clear( void );
  virtual void gc_getChildren( Children &children ) const;

  void set_type( const Value &value, index_t, int, const index_t* );
  void set_dims( const Value &value, index_t, int, const index_t* );
  void set_fill( const Value &value, index_t, int, const index_t* );
  void set_size( const Value &value, index_t d, int, const index_t* );
  void set_in  ( const Value &value, index_t, int n_coords, const index_t *coords );

private:
  typedef NodeParam< ConstantArray > NParam;

  enum Inputs
  {
    INPUT_IN0,
    NUM_INPUTS
  };

  enum Outputs
  {
    OUTPUT_OUT,
    NUM_OUTPUTS
  };

  enum Params
  {
    PARAM_FILL , // must be first
    PARAM_TYPE ,
    PARAM_DIMS ,
    PARAM_SIZEX,
    PARAM_SIZEY,
    PARAM_SIZEZ,
    PARAM_SIZET,
    PARAM_IN   ,
    NUM_PARAMS
  };

  bool all_set( void ) const;
  void create_array( void );

  index_t m_size[ 4 ];
  CountPtr< ArrayBase > m_array;
  Value m_fill;
  Type m_type;
  int  m_dims;
  bool m_first;
};

} // namespace core
} // namespace RPGML

#endif


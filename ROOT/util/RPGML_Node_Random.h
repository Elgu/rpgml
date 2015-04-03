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
#ifndef RPGML_Node_util_Random_h
#define RPGML_Node_util_Random_h

#include <RPGML/Node.h>

namespace RPGML {
namespace util {

class Random : public Node
{
  typedef Node Base;
public:
  EXCEPTION_BASE( Exception );

  Random( GarbageCollector *gc, const String &identifier, const SharedObject *so );
  virtual ~Random( void );

  virtual const char *getName( void ) const;

  virtual bool tick( void );

  virtual void gc_clear( void );
  virtual void gc_getChildren( Children &children ) const;

  void set_const_seed( const Value &value, index_t index, int n_coords, const index_t *coords );
  void set_dims( const Value &value, index_t index, int n_coords, const index_t *coords );
  void set_type( const Value &value, index_t index, int n_coords, const index_t *coords );

private:
  typedef NodeParam< Random > NParam;

  enum Inputs
  {
    INPUT_SEED ,
    INPUT_SIZEX,
    INPUT_SIZEY,
    INPUT_SIZEZ,
    INPUT_SIZET,
    NUM_INPUTS
  };

  enum Outputs
  {
    OUTPUT_OUT,
    NUM_OUTPUTS
  };

  enum Params
  {
    PARAM_CONST_SEED,
    PARAM_TYPE,
    PARAM_DIMS,
    NUM_PARAMS
  };

  template< class Element >
  bool tick2( const index_t *size, uint64_t seed );

  uint64_t m_const_seed;
  Type m_type;
  int m_dims;
};

 } // namespace util {
} // namespace RPGML

#endif


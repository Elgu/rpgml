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
#ifndef RPGML_Node_Splice_h
#define RPGML_Node_Splice_h

#include <RPGML/Node.h>

namespace RPGML {


class Splice : public Node
{
  typedef Node Base;
public:
  EXCEPTION_BASE( Exception );

  Splice( GarbageCollector *gc, const String &identifier, const SharedObject *so );
  virtual ~Splice( void );

  virtual const char *getName( void ) const;

  virtual bool tick( void );

  virtual void gc_clear( void );
  virtual void gc_getChildren( Children &children ) const;

  void set_dims( const Value &value, index_t index, int n_coords, const index_t *coords );
  void set_n   ( const Value &value, index_t index, int n_coords, const index_t *coords );

private:
  typedef NodeParam< Splice > NParam;

  enum Inputs
  {
    INPUT_LIKE_THIS,
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
    PARAM_DIMS,
    PARAM_NX,
    PARAM_NY,
    PARAM_NZ,
    PARAM_NT,
    NUM_PARAMS
  };

  void alloc_in( void );

  template< class Element >
  bool tick2( const index_t *size, const index_t *like_this_size );

  CountPtr< InputArray > m_in;
  index_t m_n[ 4 ] = { 0 };
  int m_dims = 2;
};

 //
} // namespace RPGML

#endif


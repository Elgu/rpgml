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
#ifndef RPGML_Node_BinaryOp_h
#define RPGML_Node_BinaryOp_h

#include <RPGML/Node.h>
#include <RPGML/ParserEnums.h>

namespace RPGML {

class BinaryOp : public Node
{
  typedef Node Base;
public:
  EXCEPTION_BASE( Exception );

  BinaryOp( GarbageCollector *gc, const String &identifier, const SharedObject *so );
  virtual ~BinaryOp( void );

  virtual const char *getName( void ) const;

  virtual bool tick( void );

  virtual void gc_clear( void );
  virtual void gc_getChildren( Children &children ) const;

  void set_op( const Value &value, index_t, int, const index_t* );
private:
  enum Inputs
  {
    INPUT_IN1,
    INPUT_IN2,
    NUM_INPUTS
  };

  enum Outputs
  {
    OUTPUT_OUT,
    NUM_OUTPUTS
  };

  enum Params
  {
    PARAM_OP,
    NUM_PARAMS
  };

  template< BOP OP > bool tick1          ( void );
  template< BOP OP > bool tick1_int      ( void );
  template< BOP OP > bool tick1_no_string( void );

  template< BOP OP, class T1 > bool tick2          ( const ArrayBase *in1_base );
  template< BOP OP, class T1 > bool tick2_int      ( const ArrayBase *in1_base );
  template< BOP OP, class T1 > bool tick2_no_string( const ArrayBase *in1_base );

  template< BOP OP, class T1, class T2 > bool tick3( const ArrayElements< T1 > *in1, const ArrayBase *in2_base );

  typedef NodeParam< BinaryOp > NParam;
  BOP m_op;
};

} // namespace RPGML

#endif


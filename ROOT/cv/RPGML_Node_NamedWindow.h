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
#ifndef RPGML_Node_NamedWindow_h
#define RPGML_Node_NamedWindow_h

#include <RPGML/Node.h>

#include <opencv2/highgui/highgui.hpp>

namespace RPGML {
namespace cv {

class NamedWindow : public Node
{
  typedef Node Base;
public:
  EXCEPTION_BASE( Exception );

  NamedWindow( GarbageCollector *gc, const String &identifier, const SharedObject *so );
  virtual ~NamedWindow( void );

  virtual const char *getName( void ) const;

  virtual bool tick( void );

  virtual void gc_clear( void );
  virtual void gc_getChildren( Children &children ) const;

  void set_flags( const Value &value, index_t, int, const index_t* );

private:
  typedef NodeParam< NamedWindow > NParam;

  enum Inputs
  {
    INPUT_WINNAME,
    INPUT_X      ,
    INPUT_Y      ,
    INPUT_WIDTH  ,
    INPUT_HEIGHT ,
    NUM_INPUTS
  };

  enum Outputs
  {
    OUTPUT_OUT,
    NUM_OUTPUTS
  };

  enum Params
  {
    PARAM_FLAGS,
    NUM_PARAMS
  };

  String m_winname;
  int m_flags;
  int m_x;
  int m_y;
  int m_width;
  int m_height;
};

 } // namespace cv {
} // namespace RPGML

#endif


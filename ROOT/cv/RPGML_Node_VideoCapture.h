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
#ifndef RPGML_Node_cv_VideoCapture_h
#define RPGML_Node_cv_VideoCapture_h

#include <RPGML/Node.h>
#include <opencv2/highgui/highgui.hpp>

namespace RPGML {
namespace cv {

class VideoCapture : public Node
{
  typedef Node Base;
public:
  EXCEPTION_BASE( Exception );

  VideoCapture( GarbageCollector *gc, const String &identifier, const SharedObject *so );
  virtual ~VideoCapture( void );

  virtual const char *getName( void ) const;

  virtual bool tick( void );

  virtual void gc_clear( void );
  virtual void gc_getChildren( Children &children ) const;

  void set_foo( const Value &value, index_t , int, const index_t*);

private:
  typedef NodeParam< VideoCapture > NParam;

  enum Inputs
  {
    INPUT_DEVICE  ,
    INPUT_FILENAME,
    NUM_INPUTS
  };

  enum Outputs
  {
    OUTPUT_RED  ,
    OUTPUT_GREEN,
    OUTPUT_BLUE ,
    NUM_OUTPUTS
  };

  enum Params
  {
    PARAM_FOO,
    NUM_PARAMS
  };

  ::cv::VideoCapture m_capture;
  String m_filename;
  int    m_device;
};

 } // namespace cv {
} // namespace RPGML

#endif


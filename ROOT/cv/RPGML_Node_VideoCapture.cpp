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
#include "RPGML_Node_VideoCapture.h"

// RPGML_CXXFLAGS=
// RPGML_LDFLAGS=-lopencv_core -lopencv_highgui

#include "RPGML_cv.h"
#include <algorithm>

using namespace std;

namespace RPGML {
namespace cv {

VideoCapture::VideoCapture( GarbageCollector *_gc, const String &identifier, const RPGML::SharedObject *so )
: Node( _gc, identifier, so, NUM_INPUTS, NUM_OUTPUTS, NUM_PARAMS )
, m_device( -1 )
{
  DEFINE_INPUT ( INPUT_DEVICE  , "device"   );
  DEFINE_INPUT ( INPUT_FILENAME, "filename" );
  DEFINE_OUTPUT( OUTPUT_RED  , "red"   );
  DEFINE_OUTPUT( OUTPUT_GREEN, "green" );
  DEFINE_OUTPUT( OUTPUT_BLUE , "blue"  );
  DEFINE_PARAM ( PARAM_FOO , "foo", VideoCapture::set_foo );
}

VideoCapture::~VideoCapture( void )
{}

const char *VideoCapture::getName( void ) const
{
  return "cv.VideoCapture";
}

void VideoCapture::gc_clear( void )
{
  Base::gc_clear();
}

void VideoCapture::gc_getChildren( Children &children ) const
{
  Base::gc_getChildren( children );
}

void VideoCapture::set_foo( const Value &value, index_t, int, const index_t* )
{
}

bool VideoCapture::tick( void )
{
  GET_INPUT_BASE_IF_CONNECTED( INPUT_DEVICE, device_base );
  GET_INPUT_AS_DIMS_IF_CONNECTED( INPUT_FILENAME, filename, String, 0 );

  if( device_base && filename )
  {
    throw Exception() << "Only one of 'device' or 'filename' may be connected";
  }

  if( !device_base && !filename )
  {
    throw Exception() << "None of 'device' and 'filename' is connected";
  }

  if( filename && m_filename != (**filename) )
  {
    m_filename = (**filename);
    try
    {
      m_capture.open( m_filename.c_str() );
      if( !m_capture.isOpened() )
      {
        throw Exception()
          << "Could not open video file '" << m_filename << "'"
          ;
      }
  //    m_capture.set( CV_CAP_PROP_CONVERT_RGB, 1 ); does not work
    }
    catch( const ::cv::Exception &e )
    {
      throw Exception() << "Opening VideoCapture failed: " << e.what();
    }
  }

  if( device_base )
  {
    if(
          device_base->getDims() != 0
      || !device_base->getType().isInteger() )
    {
      throw Exception()
        << "Device index must be a single integer"
        << ", is " << device_base->getType() << device_base->getSize()
        ;
    }

    const int device = device_base->getValue().to( Type::Int() ).getInt();

    if( device != m_device )
    {
      try
      {
        m_device = device;
        m_capture.open( m_device );
        if( !m_capture.isOpened() )
        {
          throw Exception()
            << "Could not open video device " << m_device
            ;
        }
  //      m_capture.set( CV_CAP_PROP_CONVERT_RGB, 1 ); does not work
      }
      catch( const ::cv::Exception &e )
      {
        throw Exception() << "Opening VideoCapture failed: " << e.what();
      }
    }
  }

  ::cv::Mat frame;
  try
  {
    if( !m_capture.read( frame ) )
    {
      throw ExitRequest() << "No more frames";
    }
  }
  catch( const ::cv::Exception &e )
  {
    throw Exception() << "Opening VideoCapture failed: " << e.what();
  }

  CountPtr< ArrayArray > channels = createArrays( getGC(), frame );

  if( channels->size() == 1 )
  {
    getOutput( OUTPUT_RED   )->setData( channels->at( 0 ) );
    getOutput( OUTPUT_GREEN )->setData( channels->at( 0 ) );
    getOutput( OUTPUT_BLUE  )->setData( channels->at( 0 ) );
  }
  else if( channels->size() == 3 )
  {
    getOutput( OUTPUT_RED   )->setData( channels->at( 0 ) );
    getOutput( OUTPUT_GREEN )->setData( channels->at( 1 ) );
    getOutput( OUTPUT_BLUE  )->setData( channels->at( 2 ) );
  }
  else
  {
    throw Exception()
      << "Expected number of channels to be 3, is " << channels->size()
      ;
  }

  getOutput( OUTPUT_RED   )->setChanged();
  getOutput( OUTPUT_GREEN )->setChanged();
  getOutput( OUTPUT_BLUE  )->setChanged();

  return true;
}

 } // namespace cv {
} // namespace RPGML

RPGML_CREATE_NODE( VideoCapture, cv:: )


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
#include "RPGML_Node_NamedWindow.h"

// RPGML_LDFLAGS=-lopencv_core -lopencv_highgui

#include <algorithm>

using namespace std;

namespace RPGML {
namespace cv {

NamedWindow::NamedWindow( GarbageCollector *_gc, const String &identifier, const RPGML::SharedObject *so )
: Node( _gc, identifier, so, NUM_INPUTS, NUM_OUTPUTS, NUM_PARAMS )
, m_flags( ::cv::WINDOW_NORMAL )
, m_x( 0 )
, m_y( 0 )
, m_width( 0 )
, m_height( 0 )
{
  DEFINE_INPUT ( INPUT_WINNAME, "winname"  );
  DEFINE_INPUT ( INPUT_X      , "x"  );
  DEFINE_INPUT ( INPUT_Y      , "y"  );
  DEFINE_INPUT ( INPUT_WIDTH  , "width"  );
  DEFINE_INPUT ( INPUT_HEIGHT , "height" );
  DEFINE_OUTPUT_INIT( OUTPUT_OUT, "out", String, 0 );
  DEFINE_PARAM ( PARAM_FLAGS , "flags", NamedWindow::set_flags );
}

NamedWindow::~NamedWindow( void )
{}

const char *NamedWindow::getName( void ) const
{
  return "cv.NamedWindow";
}

void NamedWindow::set_flags( const Value &value, index_t )
{
  if( value.getType() != Type::String() )
  {
    throw Exception() << "Param 'flags' must be set with string 'WINDOW_NORMAL' or 'WINDOW_AUTOSIZE' (default 'WINDOW_NORMAL').";
  }

  const String flags = value.getString();

  if( flags == "WINDOW_NORMAL" )
  {
    m_flags = ::cv::WINDOW_NORMAL;
  }
  else if( flags == "WINDOW_AUTOSIZE" )
  {
    m_flags = ::cv::WINDOW_NORMAL;
  }
  else
  {
    throw Exception() << "Param 'flags' must be 'WINDOW_NORMAL' or 'WINDOW_AUTOSIZE'.";
  }
}

bool NamedWindow::tick( void )
{
  GET_INPUT_MANDATORY( INPUT_WINNAME, winname, String, 0 );
  GET_INPUT_IF_CONNECTED( INPUT_X      , x     , int, 0 );
  GET_INPUT_IF_CONNECTED( INPUT_Y      , y     , int, 0 );
  GET_INPUT_IF_CONNECTED( INPUT_WIDTH  , width , int, 0 );
  GET_INPUT_IF_CONNECTED( INPUT_HEIGHT , height, int, 0 );

  if( (**winname) != m_winname )
  {
    GET_OUTPUT_AS( OUTPUT_OUT, out, String, 0 );

    if( !m_winname.empty() )
    {
      ::cv::destroyWindow( m_winname.c_str() );
      m_winname.clear();
    }

    m_winname = (**winname);
    ::cv::namedWindow( m_winname.c_str(), m_flags );
    (**out) = m_winname;
  }

  if( x || y )
  {
    if( !x || !y )
    {
      throw Exception() << "Either both inputs 'x' and 'y' must be connected or none.";
    }

    if( (**x) != m_x || (**y) != m_y )
    {
      m_x = (**x);
      m_y = (**y);
      ::cv::moveWindow( m_winname.c_str(), m_x, m_y );
    }
  }

  if( width || height )
  {
    if( !width || !height )
    {
      throw Exception() << "Either both inputs 'width' and 'height' must be connected or none.";
    }

    if( (**width) != m_width || (**height) != m_height )
    {
      m_width = (**width);
      m_height = (**height);
      ::cv::resizeWindow( m_winname.c_str(), m_width, m_height );
    }
  }

  ::cv::updateWindow( m_winname.c_str() );
  return true;
}

void NamedWindow::gc_clear( void )
{
  Node::gc_clear();
}

void NamedWindow::gc_getChildren( Children &children ) const
{
  Node::gc_getChildren( children );
}

 } // namespace cv {
} // namespace RPGML

RPGML_CREATE_NODE( NamedWindow, cv:: )


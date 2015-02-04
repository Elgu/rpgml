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
#include "RPGML_Node_Image.h"

// RPGML_CXXFLAGS=`sdl2-config --cflags`
// RPGML_LDFLAGS=`sdl2-config --libs` -lSDL2_image

#include "RPGML_SDL.h"
#include <SDL2/SDL_image.h>
#include <algorithm>
#include <iostream>

using namespace std;

namespace RPGML {
namespace SDL {

Image::Image( GarbageCollector *_gc, const String &identifier, const RPGML::SharedObject *so )
: Node( _gc, identifier, so, NUM_INPUTS, NUM_OUTPUTS, NUM_PARAMS )
{
  DEFINE_INPUT ( INPUT_FILENAME    , "filename"  );
  DEFINE_OUTPUT_INIT( OUTPUT_RED   , "red"  , uint8_t, 2 );
  DEFINE_OUTPUT_INIT( OUTPUT_GREEN , "green", uint8_t, 2 );
  DEFINE_OUTPUT_INIT( OUTPUT_BLUE  , "blue" , uint8_t, 2 );
  DEFINE_OUTPUT_INIT( OUTPUT_WIDTH , "width" , int, 0 );
  DEFINE_OUTPUT_INIT( OUTPUT_HEIGHT, "height", int, 0 );
  DEFINE_PARAM ( PARAM_FOO , "foo", Image::set_foo );

  IMG_Init( IMG_INIT_JPG | IMG_INIT_PNG | IMG_INIT_TIF );
}

Image::~Image( void )
{}

const char *Image::getName( void ) const
{
  return "SDL.Image";
}

void Image::set_foo( const Value &value, index_t, int, const index_t* )
{
  if( value.getType() != Type::String() )
  {
    throw Exception() << "Param 'foo' must be set with string.";
  }

  m_foo = value.getString();
}

bool Image::tick( void )
{
  if( !hasAnyInputChanged() ) return true;

  GET_INPUT_AS_DIMS( INPUT_FILENAME, filename, String, 0 );

  SDL_Surface_Guard image( IMG_Load( (**filename) ) );
  if( !image.get() )
  {
    throw Exception()
      << "Failed to load image '" << (**filename) << "': " << IMG_GetError()
      ;
  }

  cerr << "Loaded image '" << (**filename) << "'" << endl;

  SDL_LockSurface_Guard lock_image( image );
  const SDL_PixelFormat *const format = image->format;
  const int width  = image->w;
  const int height = image->h;
  const int pitch  = image->pitch;
  const uint8_t *const pixels = (const uint8_t*)( image->pixels );

  if( format->BitsPerPixel != 24 && format->BitsPerPixel != 32 )
  {
    throw Exception()
      << "Internal: Image loader returned something other than 24 or 32 bits per pixel."
      ;
  }

  GET_OUTPUT_AS( OUTPUT_WIDTH , o_width , int );
  GET_OUTPUT_AS( OUTPUT_HEIGHT, o_height, int );
  GET_OUTPUT_AS( OUTPUT_RED   , red     , uint8_t );
  GET_OUTPUT_AS( OUTPUT_GREEN , green   , uint8_t );
  GET_OUTPUT_AS( OUTPUT_BLUE  , blue    , uint8_t );

  if( (**o_width) != width )
  {
    (**o_width) = width;
    getOutput( OUTPUT_WIDTH )->setChanged();
  }

  if( (**o_height) != height )
  {
    (**o_height) = height;
    getOutput( OUTPUT_HEIGHT )->setChanged();
  }

  red  ->resize( width, height );
  green->resize( width, height );
  blue ->resize( width, height );

  const index_t bpp = format->BytesPerPixel;
  const bool red_first = ( format->Rmask > format->Bmask );

  uint8_t *const red_p   = red  ->elements();
  uint8_t *const green_p = green->elements();
  uint8_t *const blue_p  = blue ->elements();

  for( int y( 0 ), i( 0 ); y<height; ++y )
  for( int x( 0 )        ; x<width ; ++x, ++i )
  {
    const uint8_t *const p = &pixels[ y*pitch + bpp*x ];
    if( red_first )
    {
      red_p  [ i ] = p[ 0 ];
      green_p[ i ] = p[ 1 ];
      blue_p [ i ] = p[ 2 ];
    }
    else
    {
      blue_p [ i ] = p[ 0 ];
      green_p[ i ] = p[ 1 ];
      red_p  [ i ] = p[ 2 ];
    }
  }

  getOutput( OUTPUT_RED   )->setChanged();
  getOutput( OUTPUT_GREEN )->setChanged();
  getOutput( OUTPUT_BLUE  )->setChanged();

  return true;
}

void Image::gc_clear( void )
{
  Node::gc_clear();
}

void Image::gc_getChildren( Children &children ) const
{
  Base::gc_getChildren( children );
}

 } // namespace SDL {
} // namespace RPGML

RPGML_CREATE_NODE( Image, SDL:: )


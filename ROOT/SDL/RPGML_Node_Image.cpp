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

  IMG_Init( IMG_INIT_JPG | IMG_INIT_PNG | IMG_INIT_TIF );
}

Image::~Image( void )
{}

const char *Image::getName( void ) const
{
  return "SDL.Image";
}

bool Image::tick( void )
{
  if( !hasAnyInputChanged() ) return true;

  GET_INPUT_AS_DIMS( INPUT_FILENAME, filename, String, 0 );

  m_image.set( IMG_Load( (**filename) ) );
  if( !m_image.get() )
  {
    throw Exception()
      << "Failed to load image '" << (**filename) << "': " << IMG_GetError()
      ;
  }

//  cerr << "Loaded image '" << (**filename) << "'" << endl;

  if( m_image->format->BitsPerPixel != 24 && m_image->format->BitsPerPixel != 32 )
  {
    m_image.set( SDL_ConvertSurfaceFormat( m_image, SDL_PIXELFORMAT_RGBA8888, 0 ) );
  }

  if( m_image->format->BitsPerPixel != 24 && m_image->format->BitsPerPixel != 32 )
  {
    throw Exception()
      << "Internal: Image loader returned something other than 24 or 32 bits per pixel."
      ;
  }

  //SDL_LockSurface_Guard lock_image( m_image );
  const SDL_PixelFormat *const format = m_image->format;
  const int width  = m_image->w;
  const int height = m_image->h;
  const int pitch  = m_image->pitch;
  uint8_t *const pixels = (uint8_t*)( m_image->pixels );
  const index_t bpp = format->BytesPerPixel;

  GET_OUTPUT_AS( OUTPUT_WIDTH , o_width , int );
  GET_OUTPUT_AS( OUTPUT_HEIGHT, o_height, int );

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

  union
  {
    Uint8 u8[ 4 ];
    Uint32 u32;
  } static_offsets;
  for( Uint8 i=0; i<4; ++i ) static_offsets.u8[ i ] = i;

  Uint8 offsets[ 4 ];
  SDL_GetRGBA(
      static_offsets.u32
    , m_image->format
    , &offsets[ 0 ]
    , &offsets[ 1 ]
    , &offsets[ 2 ]
    , &offsets[ 3 ]
    );

  // Wrap image per component
  index_t  size  [ 2 ] = { index_t( width ), index_t( height ) };
  stride_t stride[ 2 ] = { stride_t( bpp ) , stride_t( pitch ) };
  for( index_t c=0; c<3; ++c )
  {
    getOutput( OUTPUT_RED+c )->setData( new UInt8Array( getGC(), pixels + offsets[ c ], 2, size, stride ) );
  }

  getOutput( OUTPUT_RED   )->setChanged();
  getOutput( OUTPUT_GREEN )->setChanged();
  getOutput( OUTPUT_BLUE  )->setChanged();

  return true;
}

void Image::gc_clear( void )
{
  Base::gc_clear();
}

void Image::gc_getChildren( Children &children ) const
{
  Base::gc_getChildren( children );
}

 } // namespace SDL {
} // namespace RPGML

RPGML_CREATE_NODE( Image, SDL:: )


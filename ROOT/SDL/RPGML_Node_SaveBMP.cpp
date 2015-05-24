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
#include "RPGML_Node_SaveBMP.h"

// RPGML_CXXFLAGS=`sdl2-config --cflags`
// RPGML_LDFLAGS=`sdl2-config --libs` -lSDL2_image

#include "RPGML_SDL.h"
#include <SDL2/SDL_image.h>
#include <algorithm>

using namespace std;

namespace RPGML {
namespace SDL {

SaveBMP::SaveBMP( GarbageCollector *_gc, const String &identifier, const RPGML::SharedObject *so )
: Node( _gc, identifier, so, NUM_INPUTS, NUM_OUTPUTS, NUM_PARAMS )
{
  DEFINE_INPUT ( INPUT_FILENAME, "filename"  );
  DEFINE_INPUT ( INPUT_RED  , "red"  );
  DEFINE_INPUT ( INPUT_GREEN, "green"  );
  DEFINE_INPUT ( INPUT_BLUE , "blue"  );
  DEFINE_OUTPUT( OUTPUT_OUT, "out" );
}

SaveBMP::~SaveBMP( void )
{}

const char *SaveBMP::getName( void ) const
{
  return "SDL.SaveBMP";
}

void SaveBMP::gc_clear( void )
{
  Base::gc_clear();
}

void SaveBMP::gc_getChildren( Children &children ) const
{
  Base::gc_getChildren( children );
}

bool SaveBMP::tick( void )
{
  if( !hasAnyInputChanged() ) return true;
  setAllOutputChanged();

  GET_INPUT_AS_DIMS( INPUT_FILENAME, filename, String, 0 );
  GET_INPUT_AS_DIMS( INPUT_RED  , red  , uint8_t, 2 );
  GET_INPUT_AS_DIMS( INPUT_GREEN, green, uint8_t, 2 );
  GET_INPUT_AS_DIMS( INPUT_BLUE , blue , uint8_t, 2 );

  const ArrayBase::Size size = red->getSize();

  if( green->getSize() != size )
  {
    throw Exception()
      << "Size of Input 'green' does not match size of Input 'red'"
      << ": 'red' is " << size
      << ", 'green' is " << green->getSize()
      ;
  }

  if( blue->getSize() != size )
  {
    throw Exception()
      << "Size of Input 'blue' does not match size of Input 'red'"
      << ": 'red' is " << size
      << ", 'blue' is " << blue->getSize()
      ;
  }

  int bpp = 0;
  Uint32 red_mask = 0;
  Uint32 green_mask = 0;
  Uint32 blue_mask = 0;
  Uint32 alpha_mask = 0;

  if(
    !SDL_PixelFormatEnumToMasks(
         SDL_PIXELFORMAT_RGBA8888, &bpp
       , &red_mask, &green_mask, &blue_mask, &alpha_mask
       )
  )
  {
    throw Exception()
      << "Could not create masks for SDL_PIXELFORMAT_RGBA8888"
      << ": " << SDL_GetError()
      ;
  }

  if( bpp != 32 )
  {
    throw Exception() << "Internal: bpp is not 32, is " << bpp;
  }

  const index_t width = size[ 0 ];
  const index_t height = size[ 1 ];

  SDL_Surface_Guard surface(
    SDL_CreateRGBSurface(
        0, width, height, bpp
      , red_mask, green_mask, blue_mask, alpha_mask
      )
    );

  if( !surface.get() )
  {
    throw Exception() << "Could not create SDL Surface: " << SDL_GetError();
  }

  SDL_LockSurface_Guard lock( surface );
  const SDL_PixelFormat *const format = surface->format;
  Uint8 *const pixels_bytes = (Uint8*)surface->pixels;
  const int pitch = surface->pitch;

  UInt8Array::const_iterator r = red  ->begin();
  UInt8Array::const_iterator g = green->begin();
  UInt8Array::const_iterator b = blue ->begin();

  for( index_t y=0; y<height; ++y )
  {
    Uint32 *const p = (Uint32*)( pixels_bytes + y * pitch );
    for( index_t x=0; x<width; ++x, ++r, ++g, ++b )
    {
      p[ x ] = SDL_MapRGB( format, (*r), (*g), (*b) );
    }
  }

  if( 0 != SDL_SaveBMP( surface, (**filename).c_str() ) )
  {
    throw Exception() << "Could not write BMP: " << SDL_GetError();
  }

  getOutput( OUTPUT_OUT )->setData( const_cast< StringArray* >( filename ) );
  return true;
}

 } // namespace SDL {
} // namespace RPGML

RPGML_CREATE_NODE( SaveBMP, SDL:: )


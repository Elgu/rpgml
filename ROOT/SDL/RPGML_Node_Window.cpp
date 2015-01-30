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
#include "RPGML_Node_Window.h"

// RPGML_LDFLAGS=`sdl2-config --libs`
// RPGML_CXXFLAGS=`sdl2-config --cflags`

#include <algorithm>
#include <iostream>

using namespace std;

namespace RPGML {
namespace SDL {

Window::Window( GarbageCollector *_gc, const String &identifier, const RPGML::SharedObject *so )
: Node( _gc, identifier, so, NUM_INPUTS, NUM_OUTPUTS, NUM_PARAMS )
, m_flags( 0 )
, m_texture_width( 0 )
, m_texture_height( 0 )
, m_width( 0 )
, m_height( 0 )
, m_window_width( 0 )
, m_window_height( 0 )
, m_x( 0 )
, m_y( 0 )
, m_window_x( 0 )
, m_window_y( 0 )
{
  m_update = new Update( getGC(), this );
  m_purge = new PurgeEvents( getGC() );

  DEFINE_INPUT ( INPUT_TITLE , "title" );
  DEFINE_INPUT ( INPUT_RED   , "red"  );
  DEFINE_INPUT ( INPUT_GREEN , "green" );
  DEFINE_INPUT ( INPUT_BLUE  , "blue"  );
  DEFINE_INPUT ( INPUT_X     , "x"  );
  DEFINE_INPUT ( INPUT_Y     , "y"  );
  DEFINE_INPUT ( INPUT_WIDTH , "width"  );
  DEFINE_INPUT ( INPUT_HEIGHT, "height" );
  DEFINE_OUTPUT( OUTPUT_OUT, "out" );
  DEFINE_PARAM_INDEX( PARAM_FULLSCREEN         , "fullscreen"        , Window::set_flag, SDL_WINDOW_FULLSCREEN         );
  DEFINE_PARAM_INDEX( PARAM_FULLSCREEN_DESKTOP , "fullscreen_desktop", Window::set_flag, SDL_WINDOW_FULLSCREEN_DESKTOP );
  DEFINE_PARAM_INDEX( PARAM_SHOWN              , "shown"             , Window::set_flag, SDL_WINDOW_SHOWN              );
  DEFINE_PARAM_INDEX( PARAM_HIDDEN             , "hidden"            , Window::set_flag, SDL_WINDOW_HIDDEN             );
  DEFINE_PARAM_INDEX( PARAM_BORDERLESS         , "borderless"        , Window::set_flag, SDL_WINDOW_BORDERLESS         );
  DEFINE_PARAM_INDEX( PARAM_RESIZABLE          , "resizable"         , Window::set_flag, SDL_WINDOW_RESIZABLE          );
  DEFINE_PARAM_INDEX( PARAM_MINIMIZED          , "minimized"         , Window::set_flag, SDL_WINDOW_MINIMIZED          );
  DEFINE_PARAM_INDEX( PARAM_MAXIMIZED          , "maximized"         , Window::set_flag, SDL_WINDOW_MAXIMIZED          );
  DEFINE_PARAM_INDEX( PARAM_INPUT_GRABBED      , "input_grabbed"     , Window::set_flag, SDL_WINDOW_INPUT_GRABBED      );
  DEFINE_PARAM_INDEX( PARAM_INPUT_FOCUS        , "input_focus"       , Window::set_flag, SDL_WINDOW_INPUT_FOCUS        );
  DEFINE_PARAM_INDEX( PARAM_FOREIGN            , "foreign"           , Window::set_flag, SDL_WINDOW_FOREIGN            );
  DEFINE_PARAM_INDEX( PARAM_ALLOW_HIGHDPI      , "allow_highdpi"     , Window::set_flag, SDL_WINDOW_ALLOW_HIGHDPI      );

  // Construction is normaly single-threaded, run by the main thread
  if( !SDL_WasInit( SDL_INIT_VIDEO ) )
  {
    if( 0 != SDL_Init( SDL_INIT_VIDEO ) )
    {
      throw Exception()
        << "Unable to initialize SDL video subsystem: "
        << SDL_GetError()
        ;
    }
  }
  atexit( SDL_Quit );

  SDL_SetEventFilter( event_filter_dispatch, this );
}

Window::~Window( void )
{}

const char *Window::getName( void ) const
{
  return "SDL.Window";
}

void Window::set_flag( const Value &value, index_t flag, int, const index_t* )
{
  try
  {
    const bool set_it = value.to( Type::Bool() ).getBool();

    if( set_it )
    {
      m_flags |= Uint32( flag );
    }
    else
    {
      m_flags &= ~Uint32( flag );
    }
  }
  catch( const Value::CastFailed &e )
  {
    throw Exception() << "Could not set flag: " << e.what();
  }
}

bool Window::tick( CountPtr< JobQueue > main_thread )
{
  const bool update_window =
       m_rgba.isNull()
    || m_window.isNull()
    || getInput( INPUT_X )->hasChanged()
    || getInput( INPUT_Y )->hasChanged()
    || getInput( INPUT_WIDTH  )->hasChanged()
    || getInput( INPUT_HEIGHT )->hasChanged()
    || getInput( INPUT_RED   )->hasChanged()
    || getInput( INPUT_GREEN )->hasChanged()
    || getInput( INPUT_BLUE  )->hasChanged()
    ;

  GET_INPUT_IF_CONNECTED( INPUT_TITLE , title , String, 0 );
  GET_INPUT_IF_CONNECTED( INPUT_X     , x     , int   , 0 );
  GET_INPUT_IF_CONNECTED( INPUT_Y     , y     , int   , 0 );
  GET_INPUT_IF_CONNECTED( INPUT_WIDTH , width , int   , 0 );
  GET_INPUT_IF_CONNECTED( INPUT_HEIGHT, height, int   , 0 );

  const ArrayBase *const red_base = getInput( INPUT_RED )->getData();
  if( !red_base ) throw NotConnected( getInput( INPUT_RED ) );

  if( red_base->getDims() != 2 )
  {
    throw IncompatibleOutput( getInput( INPUT_RED ) );
  }

  const index_t *const size = red_base->getSize();
  m_width  = ( width  ? (**width)  : size[ 0 ] );
  m_height = ( height ? (**height) : size[ 1 ] );
  m_x      = ( x      ? (**x)      : SDL_WINDOWPOS_UNDEFINED );
  m_y      = ( y      ? (**y)      : SDL_WINDOWPOS_UNDEFINED );
  m_title  = ( title  ? (**title)  : String::Static( "RPGML.SDL.Window" ) );

  if( update_window )
  {
    if( m_rgba.isNull() )
    {
      m_rgba.reset( new Array< uint8_t, 2 >( getGC() ) );
    }
    m_rgba->resize( size[ 0 ]*4, size[ 1 ] );
    fill_rgba( size[ 0 ], size[ 1 ] );
  }

  if( 0 != main_thread->doJob( m_update ) )
  {
    throw Exception() << "Texture update failed: " << m_error;
  }

  purge_events( main_thread );
  check_events();
  return true;
}

Window::Update::Update( GarbageCollector *_gc, Window *self )
: JobQueue::Job( _gc )
, m_self( self )
{}

Window::Update::~Update( void )
{}

size_t Window::Update::doit( CountPtr< JobQueue > )
{
  return m_self->update();
}

void Window::Update::gc_clear( void )
{
  Base::gc_clear();
  m_self.reset();
}

void Window::Update::gc_getChildren( Children &children ) const
{
  Base::gc_getChildren( children );
  children << m_self;
}

size_t Window::update( void )
{
  if( !create_window_and_renderer() ) return 1;
  if( !update_texture() ) return 2;

  SDL_RenderPresent( m_renderer );
  return 0;
}

bool Window::create_window_and_renderer( void )
{
  if( !m_window.get() )
  {
    m_window.set(
      SDL_CreateWindow(
          m_title
        , m_x
        , m_y
        , m_width
        , m_height
        , m_flags
        )
      );

    if( !m_window.get() )
    {
      m_error += "Could not create window: ";
      m_error += SDL_GetError();
      return false;
    }

    std::cerr << "Created window with ID " << SDL_GetWindowID( m_window ) << std::endl;

    m_renderer.set( SDL_CreateRenderer( m_window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC ) );
    //m_renderer.set( SDL_CreateRenderer( m_window, -1, SDL_RENDERER_SOFTWARE ) );

    if( !m_renderer.get() )
    {
      m_error += "Could not create renderer: ";
      m_error += SDL_GetError();
      return false;
    }

    m_window_x = m_x;
    m_window_y = m_y;
    m_window_width  = m_width;
    m_window_height = m_height;
    m_window_title = m_title;
  }
  else
  {
    if( m_title != m_window_title )
    {
      SDL_SetWindowTitle( m_window, m_title );
      m_window_title = m_title;
    }

    if(
         m_x != m_window_x
      || m_y != m_window_y
    )
    {
      SDL_SetWindowPosition( m_window, m_x, m_y );
      m_window_x = m_x;
      m_window_y = m_y;
    }

    if(
         m_width  != m_window_width
      || m_height != m_window_height
    )
    {
      SDL_SetWindowSize( m_window, m_width, m_height );
      m_window_width = m_width;
      m_window_height = m_height;
    }
  }

  return true;
}

bool Window::update_texture( void )
{
  const index_t *const size = m_rgba->getSize();
  const int width  = size[ 0 ] / 4;
  const int height = size[ 1 ];

  if(
       !m_texture.get()
    || m_texture_width != width
    || m_texture_height != height
  )
  {
    m_texture.set(
      SDL_CreateTexture(
          m_renderer
        , SDL_PIXELFORMAT_RGBA8888
        , SDL_TEXTUREACCESS_STREAMING
        , width, height
        )
      );

    if( !m_texture.get() )
    {
      m_error += "Could not create texture: ";
      m_error += SDL_GetError();
      return false;
    }

    m_texture_width = width;
    m_texture_height = height;
  }

  SDL_Rect update_rect;
  update_rect.x = 0;
  update_rect.y = 0;
  update_rect.w = width;
  update_rect.h = height;

  if( 0 != SDL_UpdateTexture( m_texture, &update_rect, m_rgba->elements(), width*4 ) )
  {
    m_error += "Could not update texture: ";
    m_error += SDL_GetError();
    return false;
  }

  if( 0 != SDL_RenderCopy( m_renderer, m_texture, 0, 0 ) )
  {
    m_error += "Rendering texture failed: ";
    m_error += SDL_GetError();
    return false;
  }

  return true;
}

namespace Window_impl {

  static inline uint8_t saturate(  int8_t x ) { return uint8_t( x ); }
  static inline uint8_t saturate( uint8_t x ) { return x; }

  template< class T >
  static inline uint8_t saturate( T x ) { return uint8_t( min( T( 255 ), max( T( 0 ), x ) ) ); }

} // namespace Window_impl

template< class T >
void Window::fill_rgba_t( const ArrayBase *chan_base, int chan )
{
  using namespace Window_impl;

  const ArrayElements< T > *chan_elements = 0;
  if( !chan_base->getAs( chan_elements ) ) throw GetAsFailed( getInput( INPUT_RED+chan ), chan_elements );

  const index_t n = chan_elements->size();
  uint8_t *const rgba_p = m_rgba->elements();
  const T *const c     = chan_elements->elements();

  for( index_t i=0; i<n; ++i )
  {
    rgba_p[ 4*i + chan ] = saturate( c[ i ] );
  }
}

void Window::fill_rgba( int width, int height )
{
  static const char *const chan_names[] = { "red", "green", "blue", "alpha" };

  for( int chan = 0; chan < 3; ++chan )
  {
    const ArrayBase *const chan_base = getInput( INPUT_RED+chan )->getData();
    if( !chan_base ) throw NotConnected( getInput( INPUT_RED+chan ) );

    const index_t *const chan_size = chan_base->getSize();
    if( chan_size[ 0 ] != index_t( width ) || chan_size[ 1 ] != index_t( height ) )
    {
      throw Exception()
        << "Channel inputs 'red', 'green', and 'blue' must match in size."
        << " 'red' is [ " << width << ", " << height << " ]"
        << ", '" << chan_names[ chan ] << "'"
        << " is [ " << chan_size[ 0 ] << ", " << chan_size[ 1 ] << " ]"
        ;
    }

    switch( chan_base->getType().getEnum() )
    {
      case Type::UINT8 : fill_rgba_t< uint8_t  >( chan_base, chan ); break;
      case Type::INT8  : fill_rgba_t< int8_t   >( chan_base, chan ); break;
      case Type::UINT16: fill_rgba_t< uint16_t >( chan_base, chan ); break;
      case Type::INT16 : fill_rgba_t< int16_t  >( chan_base, chan ); break;
      case Type::UINT32: fill_rgba_t< uint32_t >( chan_base, chan ); break;
      case Type::INT32 : fill_rgba_t< int32_t  >( chan_base, chan ); break;
      case Type::UINT64: fill_rgba_t< uint64_t >( chan_base, chan ); break;
      case Type::INT64 : fill_rgba_t< int64_t  >( chan_base, chan ); break;
      case Type::FLOAT : fill_rgba_t< float    >( chan_base, chan ); break;
      case Type::DOUBLE: fill_rgba_t< double   >( chan_base, chan ); break;
      default:
        throw IncompatibleOutput( getInput( INPUT_RED+chan ) );
    }
  }
}

void Window::handle_event( const SDL_Event &event )
{
  switch( event.type )
  {
    case SDL_WINDOWEVENT:
      switch( event.window.event )
      {
        case SDL_WINDOWEVENT_CLOSE:
          cerr << "SDL_WINDOWEVENT_CLOSE" << endl;
          throw ExitRequest();
          break;

        default:
          break;
      }
      break;

    default: break;
  }
}

void Window::purge_events( JobQueue *main_thread )
{
  if( 0 != main_thread->doJob( m_purge ) )
  {
    if( m_purge->hasExitRequest() )
    {
      throw ExitRequest();
    }
  }
}

void Window::check_events( void )
{
  SDL_Event event;
  while( pop_event( &event ) )
  {
    handle_event( event );
  }
}

int Window::event_filter( SDL_Event *event )
{
  switch( event->type )
  {
    case SDL_WINDOWEVENT:
      {
        if( m_window.get() && event->window.windowID == SDL_GetWindowID( m_window ) )
        {
          push_event( event );
          // Handled by this window
          return 0;
        }
        else
        {
          // Not for this window
          return 1;
        }
      }
      break;

    default:
      return 1;
  }
}

int Window::event_filter_dispatch( void *self, SDL_Event *event )
{
  return ((Window*)self)->event_filter( event );
}

void Window::push_event( const SDL_Event *event )
{
  Mutex::ScopedLock lock( &m_event_queue_lock );
  m_event_queue.push( *event );
}

bool Window::pop_event( SDL_Event *event )
{
  Mutex::ScopedLock lock( &m_event_queue_lock );
  if( m_event_queue.empty() ) return false;
  (*event) = m_event_queue.front();
  m_event_queue.pop();
  return true;
}

void Window::gc_clear( void )
{
  Node::gc_clear();
  m_update.reset();
  m_purge.reset();
  m_rgba.reset();
}

void Window::gc_getChildren( Children &children ) const
{
  Node::gc_getChildren( children );
  children.add( m_update );
  children.add( m_purge );
  children.add( m_rgba );
}

 } // namespace SDL {
} // namespace RPGML

RPGML_CREATE_NODE( Window, SDL:: )


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
#ifndef RPGML_Node_SDL_Window_h
#define RPGML_Node_SDL_Window_h

#include <RPGML/Node.h>
#include <RPGML/Mutex.h>
#include "RPGML_SDL.h"

#include <queue>

namespace RPGML {
namespace SDL {

class Window : public Node
{
public:
  EXCEPTION_BASE( Exception );

  Window( GarbageCollector *gc, const String &identifier, const SharedObject *so );
  virtual ~Window( void );

  virtual const char *getName( void ) const;

  virtual bool tick( CountPtr< JobQueue > main_thread );

  virtual void gc_clear( void );
  virtual void gc_getChildren( Children &children ) const;

  void set_flag( const Value &value, index_t flag, int, const index_t* );

private:
  typedef NodeParam< Window > NParam;

  enum Inputs
  {
    INPUT_TITLE ,
    INPUT_RED   ,
    INPUT_GREEN ,
    INPUT_BLUE  ,
    INPUT_X     ,
    INPUT_Y     ,
    INPUT_WIDTH ,
    INPUT_HEIGHT,
    NUM_INPUTS
  };

  enum Outputs
  {
    OUTPUT_OUT,
    NUM_OUTPUTS
  };

  enum Params
  {
    PARAM_FULLSCREEN        ,
    PARAM_FULLSCREEN_DESKTOP,
    PARAM_SHOWN             ,
    PARAM_HIDDEN            ,
    PARAM_BORDERLESS        ,
    PARAM_RESIZABLE         ,
    PARAM_MINIMIZED         ,
    PARAM_MAXIMIZED         ,
    PARAM_INPUT_GRABBED     ,
    PARAM_INPUT_FOCUS       ,
    PARAM_FOREIGN           ,
    PARAM_ALLOW_HIGHDPI     ,
    PARAM_MOUSE_CAPTURE     ,
    NUM_PARAMS
  };

  class Update : public JobQueue::Job
  {
  public:
    typedef JobQueue::Job Base;
    Update( GarbageCollector *_gc, Window *self );
    virtual ~Update( void );
    virtual void gc_clear( void );
    virtual void gc_getChildren( Children &children ) const;
  protected:
    virtual size_t doit( CountPtr< JobQueue > queue );
  private:
    CountPtr< Window > m_self;
  };
  friend class Update;

  size_t update( void );

  bool update_texture( void );
  bool create_window_and_renderer( void );

  template< class T >
  void fill_rgba_t( const ArrayBase *chan_base, int chan );

  void fill_rgba( int width, int height );

  void purge_events( JobQueue *main_thread );
  void check_events( void );
  void handle_event( const SDL_Event &event );
  int event_filter( SDL_Event *event );

  static
  int event_filter_dispatch( void *self, SDL_Event *event );

  void push_event( const SDL_Event *event );
  bool pop_event( SDL_Event *event );

  std::queue< SDL_Event > m_event_queue;
  CountPtr< Update > m_update;
  CountPtr< PurgeEvents > m_purge;
  CountPtr< Array< uint8_t > > m_rgba;
  std::string m_error;
  Mutex m_event_queue_lock;
  SDL_Window_Guard m_window;
  SDL_Renderer_Guard m_renderer;
  SDL_Texture_Guard m_texture;
  String m_window_title;
  String m_title;
  Uint32 m_flags;
  int m_texture_width; //!< Compared to m_rgba
  int m_texture_height;//!< Compared to m_rgba
  int m_width; //!< Set this to update m_window_width
  int m_height;//!< Set this to update m_window_height
  int m_window_width; //!< Last set window width
  int m_window_height;//!< Last set window height
  int m_x; //!< Set this to update m_window_x
  int m_y; //!< Set this to update m_window_y
  int m_window_x; //!< Last set window x
  int m_window_y; //!< Last set window y
};

 } // namespace SDL {
} // namespace RPGML

#endif


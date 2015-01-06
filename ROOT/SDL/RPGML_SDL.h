#ifndef RPGML_SDL_h
#define RPGML_SDL_h

#include <RPGML/Guard.h>
#include <RPGML/JobQueue.h>
#include <SDL2/SDL.h>

namespace RPGML {
namespace SDL {

  class SDL_Surface_Guard : public Guard< SDL_Surface >
  {
  public:
    typedef Guard< SDL_Surface > Base;

    SDL_Surface_Guard( void )
    : Base( 0, SDL_FreeSurface )
    {}

    explicit
    SDL_Surface_Guard( SDL_Surface *surface )
    : Base( surface, SDL_FreeSurface )
    {}
  };

  class SDL_Window_Guard : public Guard< SDL_Window >
  {
  public:
    typedef Guard< SDL_Window > Base;

    SDL_Window_Guard( void )
    : Base( 0, SDL_DestroyWindow )
    {}

    explicit
    SDL_Window_Guard( SDL_Window *window )
    : Base( window, SDL_DestroyWindow )
    {}
  };

  class SDL_Renderer_Guard : public Guard< SDL_Renderer >
  {
  public:
    typedef Guard< SDL_Renderer > Base;

    SDL_Renderer_Guard( void )
    : Base( 0, SDL_DestroyRenderer )
    {}

    explicit
    SDL_Renderer_Guard( SDL_Renderer *renderer )
    : Base( renderer, SDL_DestroyRenderer )
    {}
  };

  class SDL_Texture_Guard : public Guard< SDL_Texture >
  {
  public:
    typedef Guard< SDL_Texture > Base;

    SDL_Texture_Guard( void )
    : Base( 0, SDL_DestroyTexture )
    {}

    explicit
    SDL_Texture_Guard( SDL_Texture *texture )
    : Base( texture, SDL_DestroyTexture )
    {}
  };

  class SDL_LockSurface_Guard : public Guard< SDL_Surface >
  {
  public:
    typedef Guard< SDL_Surface > Base;

    SDL_LockSurface_Guard( void )
    : Base( 0, SDL_UnlockSurface )
    {}

    explicit
    SDL_LockSurface_Guard( SDL_Surface *surface )
    : Base( 0, SDL_UnlockSurface )
    {
      set( surface );
    }

    void set( SDL_Surface *surface, Base::Destructor* = 0 )
    {
      clear();
      SDL_LockSurface( surface );
      Base::set( surface, 0 );
    }
  };

  static inline
  const char *getEventName( Uint32 event_type )
  {
    switch( event_type )
    {
      case SDL_DOLLARGESTURE  : return "dollar gesture";
      case SDL_DROPFILE       : return "drop file";
      case SDL_FINGERMOTION   : return "finger motion";
      case SDL_FINGERDOWN     : return "finger down";
      case SDL_FINGERUP       : return "finger up";
      case SDL_JOYAXISMOTION  : return "joystick axis motion";
      case SDL_JOYBALLMOTION  : return "joyball motion";
      case SDL_JOYHATMOTION   : return "joyhat motion";
      case SDL_JOYBUTTONDOWN  : return "joystick button down";
      case SDL_JOYBUTTONUP    : return "joystick button up";
      case SDL_MOUSEMOTION    : return "mouse motion";
      case SDL_MOUSEBUTTONDOWN: return "mouse button down";
      case SDL_MOUSEBUTTONUP  : return "mouse button up";
      case SDL_MOUSEWHEEL     : return "mouse wheel";
      case SDL_MULTIGESTURE   : return "multi-gesture";
      case SDL_QUIT           : return "quit";
      case SDL_SYSWMEVENT     : return "system window manager event";
      case SDL_TEXTEDITING    : return "text editing";
      case SDL_TEXTINPUT      : return "text input";
      case SDL_USEREVENT      : return "user event";
      case SDL_WINDOWEVENT    : return "window event";
      default                 : return "unknown event";
    }
  }

  static inline
  const char *getWindowEventName( Uint8 window_event )
  {
    switch( window_event )
    {
      case SDL_WINDOWEVENT_SHOWN       : return "shown";
      case SDL_WINDOWEVENT_HIDDEN      : return "hidden";
      case SDL_WINDOWEVENT_EXPOSED     : return "exposed";
      case SDL_WINDOWEVENT_MOVED       : return "moved";
      case SDL_WINDOWEVENT_RESIZED     : return "resized";
      case SDL_WINDOWEVENT_MINIMIZED   : return "minimized";
      case SDL_WINDOWEVENT_MAXIMIZED   : return "maximized";
      case SDL_WINDOWEVENT_RESTORED    : return "restored";
      case SDL_WINDOWEVENT_ENTER       : return "enter";
      case SDL_WINDOWEVENT_LEAVE       : return "leave";
      case SDL_WINDOWEVENT_FOCUS_GAINED: return "focus gained";
      case SDL_WINDOWEVENT_FOCUS_LOST  : return "focus lost";
      case SDL_WINDOWEVENT_CLOSE       : return "close";
      default                          : return "unknown window event";
    }
  }

  class PurgeEvents: public JobQueue::Job
  {
  public:
    PurgeEvents( GarbageCollector *_gc )
    : JobQueue::Job( _gc )
    , m_exit_request( false )
    {}

    virtual ~PurgeEvents( void )
    {}

    virtual size_t doit( CountPtr< JobQueue > )
    {
      SDL_Event event;
      while( SDL_PollEvent( &event ) )
      {
        if( event.type == SDL_QUIT )
        {
          m_exit_request = true;
          return 1;
        }
      }
      return 0;
    }

    bool hasExitRequest( void ) const
    {
      return m_exit_request;
    }

  private:
    bool m_exit_request;
  };

} // namespace SDL
} // namespace RPGML

#endif


#include "SharedObject.h"

namespace RPGML {

SharedObject::SharedObject( const String      &so, String &err )
: m_handle( SDL_LoadObject( so.c_str() ) )
{
  if( !isValid() )
    err = String( SDL_GetError() );
  else
    err.clear();
}

SharedObject::SharedObject( const std::string &so, String &err )
: m_handle( SDL_LoadObject( so.c_str() ) )
{
  if( !isValid() )
    err = String( SDL_GetError() );
  else
    err.clear();
}

SharedObject::SharedObject( const char *so, String &err )
: m_handle( SDL_LoadObject( so ) )
{
  if( !isValid() )
    err = String( SDL_GetError() );
  else
    err.clear();
}

SharedObject::~SharedObject( void )
{
  if( m_handle ) SDL_UnloadObject( m_handle );
}

bool SharedObject::isValid( void ) const
{
  return ( 0 != m_handle );
}

void *SharedObject::getVoidPtrSymbol( const char *sym, String &err ) const
{
  if( !isValid() )
  {
    err = "No shared object loaded";
    return 0;
  }
  else
  {
    void *const ret = SDL_LoadFunction( m_handle, sym );
    if( !ret )
    {
      err = SDL_Error();
    }
    else
    {
      err.clear();
    }
    return ret;
  }
}

} // namespace RPGML


#include "SharedObject.h"

#include <cerrno>
#include <cstring>
#include <dlfcn.h>

namespace RPGML {

SharedObject::SharedObject( const String      &so, String &err )
: m_so( so )
, m_handle( dlopen( so.c_str(), RTLD_NOW | RTLD_DEEPBIND | RTLD_LOCAL ) )
{
  if( !isValid() )
    err = String( dlerror() );
  else
    err.clear();
}

SharedObject::SharedObject( const std::string &so, String &err )
: m_so( so )
, m_handle( dlopen( so.c_str(), RTLD_NOW | RTLD_DEEPBIND | RTLD_LOCAL ) )
{
  if( !isValid() )
    err = String( dlerror() );
  else
    err.clear();
}

SharedObject::SharedObject( const char *so, String &err )
: m_so( so )
, m_handle( dlopen( so, RTLD_NOW | RTLD_DEEPBIND | RTLD_LOCAL ) )
{
  if( !isValid() )
    err = String( dlerror() );
  else
    err.clear();
}

SharedObject::~SharedObject( void )
{
//  if( m_handle ) dlclose( m_handle );
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
    void *const ret = dlsym( m_handle, sym );
    if( !ret )
    {
      err = dlerror();
    }
    else
    {
      err.clear();
    }
    return ret;
  }
}

} // namespace RPGML


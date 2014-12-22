#ifndef RPGML_Mutex_h
#define RPGML_Mutex_h

#include <pthread.h>

#include "Exception.h"
#include "ScopedLock.h"

namespace RPGML {

class Mutex
{
public:
  typedef RPGML::ScopedLock< Mutex > ScopedLock;

  EXCEPTION_BASE( Exception );
  EXCEPTION_DERIVED_FIXED_TEXT( AlreadyLocked, Exception );
  EXCEPTION_DERIVED_FIXED_TEXT( NotOwned, Exception );

  class Attr
  {
  protected:
    explicit Attr( int type );
  public:
    ~Attr( void );
    operator const pthread_mutexattr_t *( void ) const;
  private:
    pthread_mutexattr_t m_attr;
  };

  class Fast : public Attr
  {
  public:
    Fast( void );
  };

  class Recursive : public Attr
  {
  public:
    Recursive( void );
  };

  class ErrorCheck : public Attr
  {
  public:
    ErrorCheck( void );
  };

  Mutex( void );
  //! Not explicit
  Mutex( const Attr &attr );
  ~Mutex( void );

  void lock( void );
  bool trylock( void );
  void unlock( void );

private:
  pthread_mutex_t m_mutex;

  //! forbidden
  Mutex &operator=( const Mutex & );
  //! forbidden
  Mutex( const Mutex & );
};

} // namespace RPGML

#endif


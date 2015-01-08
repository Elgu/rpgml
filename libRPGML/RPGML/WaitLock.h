#ifndef RPGML_WaitLock_h
#define RPGML_WaitLock_h

#include "Semaphore.h"
#include "Atomic.h"
#include "Refcounted.h"

namespace RPGML {

class WaitLock
{
public:
  WaitLock( void );
  ~WaitLock( void );

  void wait( void );
  void done( void );

  class Token : public Refcounted
  {
  private:
    explicit Token( WaitLock *lock );
  public:
    virtual ~Token( void );
    void wait( void );
  private:
    friend class WaitLock;
    WaitLock *m_lock;
    Token( const Token & );
    Token &operator=( const Token & );
  };

  CountPtr< Token > getToken( void );

protected:
  friend class Token;
  Semaphore m_wait_sem;
  Atomic< int > m_num_waiting;
};

} // namespace RPGML

#endif


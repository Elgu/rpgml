#ifndef RPGML_WaitLock_h
#define RPGML_WaitLock_h

#include "Semaphore.h"
#include "Atomic.h"

namespace RPGML {

class WaitLock
{
public:
  WaitLock( void )
  : m_num_waiting( 0 )
  {}

  ~WaitLock( void )
  {
    done();
  }

  void wait( void )
  {
    ++m_num_waiting;
    m_wait_sem.lock();
  }

  void done( void )
  {
    while( m_num_waiting.trydec() )
    {
      m_wait_sem.unlock();
    }
  }

protected:
  Semaphore m_wait_sem;
  Atomic< int > m_num_waiting;
};

} // namespace RPGML

#endif


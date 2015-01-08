#include "WaitLock.h"

namespace RPGML {

WaitLock::WaitLock( void )
: m_num_waiting( 0 )
{}

WaitLock::~WaitLock( void )
{
  done();
}

void WaitLock::wait( void )
{
  ++m_num_waiting;
  m_wait_sem.lock();
}

void WaitLock::done( void )
{
  while( m_num_waiting.trydec() )
  {
    m_wait_sem.unlock();
  }
}

CountPtr< WaitLock::Token > WaitLock::getToken( void )
{
  return new Token( this );
}

WaitLock::Token::Token( WaitLock *lock )
: m_lock( lock )
{
  if( m_lock )
  {
    ++m_lock->m_num_waiting;
  }
}

WaitLock::Token::~Token( void )
{
  wait();
}

void WaitLock::Token::wait( void )
{
  if( m_lock )
  {
    m_lock->m_wait_sem.lock();
    m_lock = 0;
  }
}

} // namespace RPGML


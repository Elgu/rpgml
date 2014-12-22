/*! @file ScopedLock.h
 * @brief ScopedLock template class
 * @author Gunnar Payer
 * @date November 27th 2014
 */
#ifndef RPGML_ScopedLock_h
#define RPGML_ScopedLock_h

namespace RPGML {

//! @brief Class for automatic (exception-safe) unlocking
template< class Lockable >
class ScopedLock
{
public:
  /*! @brief Locks the Lockable object, if non-null
   * @param lockable [in] If non-null the method lock() is called on that object, unlock() will be called in the destructor
   */
  explicit
  ScopedLock( Lockable *lockable )
  : m_lockable( lockable )
  {
    if( m_lockable ) m_lockable->lock();
  }

  //! @brief Unocks the Lockable object, if it was specified in the constructor
  ~ScopedLock( void )
  {
    if( m_lockable ) m_lockable->unlock();
  }

private:
  Lockable *const m_lockable; //!< The Lockable object specified at the constructor
};

} // namespace RPGML

#endif


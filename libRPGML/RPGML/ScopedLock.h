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


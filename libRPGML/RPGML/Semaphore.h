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
/*! @file Semaphore.h
 * @brief Semaphore class
 * @author Gunnar Payer
 * @date November 27th 2014
 */
#ifndef RPGML_Semaphore_h
#define RPGML_Semaphore_h

#include "ScopedLock.h"
#include "Exception.h"

#ifndef THREAD_USE_PTHREAD
#define THREAD_USE_PTHREAD
#endif

#ifdef THREAD_USE_PTHREAD
#include <semaphore.h>
#else
#error No Thread implementation specified
#endif // THREAD_USE_PTHREAD

namespace RPGML {

//! @brief Semaphore class for thread synchronisation
class Semaphore
{
public:
  typedef RPGML::ScopedLock< Semaphore > ScopedLock; //!< For scoped locking of a Samaphore
  typedef unsigned int value_t; //!< Semaphore counter value

  //! @brief Semaphore exception base class
  EXCEPTION_BASE( Exception );
  //! @brief Exception for when the value is out of range
  EXCEPTION_DERIVED_FIXED_TEXT( ValueExceedsMax, Exception );

  /*! @brief Initializes the semaphore with the initial value
   * @param initial_value [in] Must be at most SEM_VALUE_MAX
   * @throws ValueExceedsMax When initial_value exceeds SEM_VALUE_MAX
   */
  explicit Semaphore( value_t initial_value=0 );

  //! @brief destructor
  ~Semaphore( void );

  /*! @brief Try to decrement the semaphore by 1
   *
   * This operation is non-blocking
   * @return Returns whether the semaphore value could be decemented
   */
  bool trylock( void );

  /*! @brief decrement the semaphore by 1
   *
   * This operation is blocks until the semaphore value was big enough
   */
  void lock( void );

  /*! @brief increment the semaphore by 1
   *
   * This operation does not block, incrementing is always permitted
   * @throws ValueExceedsMax When semaphore value would exceed SEM_VALUE_MAX
    */
  void unlock( void );

  //! @brief Synonym for unlock()
  void post( void ) { unlock(); }
  //! @brief Synonym for lock()
  void wait( void ) { lock(); }

  //! @brief Synonym for unlock()
  void operator++( int ) { unlock(); }
  //! @brief Synonym for unlock()
  void operator++( void ) { unlock(); }

  //! @brief Synonym for lock()
  void operator--( int ) { lock(); }
  //! @brief Synonym for lock()
  void operator--( void ) { lock(); }

private:
  //! @brief forbidden
  Semaphore( const Semaphore & );
  //! @brief forbidden
  Semaphore &operator=( const Semaphore & );
#ifdef THREAD_USE_PTHREAD
  sem_t m_sem;
#endif // THREAD_USE_PTHREAD
};

} // namespace RPGML

#endif


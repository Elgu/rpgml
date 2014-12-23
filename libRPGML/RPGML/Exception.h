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
/*! @file Exception.h
 * @brief Exception base class
 * @author Gunnar Payer
 * @date November 27th 2014
 */
#ifndef RPGML_Exception_h
#define RPGML_Exception_h

#include <exception>
#include <string>
#include <sstream>

namespace RPGML {

//! @brief exception base class, implements std::exception, so is caught by default
class Exception : public std::exception
{
public:
  //! @brief Create Exception with empty exception text
  Exception( void ) throw();
  /*! @brief Create Exception with an exception text
   * @param text [in] The exception text to be returned by what()
   */
  explicit Exception( const std::string &text ) throw();
  //! @brief destructor
  virtual ~Exception( void ) throw();
  //! @brief Returns the exception text specified at creation
  virtual const char *what() const throw();
  //! @brief appends text to exception text
  Exception &append( const std::string &text );

  template< class T >
  Exception &operator<<( const T &x )
  {
    std::ostringstream s;
    s << x;
    return append( s.str() );
  }

private:
  std::string m_text; //!< The exception text
};

#define EXCEPTION_BODY( NAME ) \
    NAME( void ) throw() \
    {} \
    explicit \
    NAME( const std::string &text ) throw() \
    : Base( text ) \
    {} \
    virtual ~NAME( void ) throw() {}; \
    template< class T > \
    NAME &operator<<( const T &x ) \
    { \
      Base::operator<<( x ); \
      return (*this); \
    } \

#define EXCEPTION_BASE( NAME ) \
  class NAME : public RPGML::Exception \
  { \
  public: \
    typedef RPGML::Exception Base;\
    EXCEPTION_BODY( NAME )\
  }

#define EXCEPTION_DERIVED( NAME, BASE ) \
  class NAME : public BASE \
  { \
  public: \
    typedef BASE Base;\
    EXCEPTION_BODY( NAME )\
  }

#define EXCEPTION_DERIVED_FIXED_TEXT( NAME, BASE ) \
  class NAME : public BASE \
  { \
  public: \
    typedef BASE Base;\
    NAME( void ) throw(); \
    virtual ~NAME( void ) throw() {}; \
    template< class T > \
    NAME &operator<<( const T &x ) \
    { \
      Base::operator<<( x ); \
      return (*this); \
    } \
  }

#define EXCEPTION_DERIVED_DEFINE_FIXED_TEXT( PARENT, NAME, FIXED_TEXT ) \
  PARENT::NAME::NAME( void ) throw() \
  : Base( FIXED_TEXT ) \
  {}

} // namespace RPGML

#endif


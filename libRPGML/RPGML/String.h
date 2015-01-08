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
#ifndef RPGML_String_h
#define RPGML_String_h

#include "Refcounted.h"
#include <string>
#include <cstring>
#include <ostream>
#include <sstream>

namespace RPGML {
  class String;
  String operator+( const String &s1, const String &s2 );
  String operator+( const String &s1, const std::string &s2 );
  String operator+( const String &s1, const char *s2 );
  String operator+( const std::string &s1, const String &s2 );
  String operator+( const char *s1, const String &s2 );
} // namespace RPGML

namespace std {
  std::ostream &operator<<( std::ostream &o, const RPGML::String &s );
} // namespace std

namespace RPGML {

class StringData : public Refcounted
{
public:
  StringData( void ) : m_begin( 0 ) {}
  explicit StringData( const char *begin ) : m_begin( begin ) {}
  virtual ~StringData( void ) {}
  const char *get( void ) const { return m_begin; }
protected:
  void set( const char *begin ) { m_begin = begin; }
private:
  const char *m_begin;
};

class MallocString : public StringData
{
private:
  //! private, use create()
  MallocString( void ) : m_str() { set( m_str ); }
public:
  virtual ~MallocString( void ) {}
  static CountPtr< const StringData > create( const char *s, size_t len, const char *s2=0, size_t len2=0 );
private:
  const char m_str[ 0 ];
};

class StdString : public StringData
{
public:
  StdString( void ) { set( m_str.c_str() ); }
  explicit StdString( const char *s ) : m_str( s ) { set( m_str.c_str() ); }
  explicit StdString( const std::string &s ) : m_str( s ) { set( m_str.c_str() ); }

  void moveFrom( std::string &str )
  {
    m_str.clear();
    std::swap( m_str, str );
    set( m_str.c_str() );
  }

  virtual ~StdString( void ) {}
private:
  std::string m_str;
};

class String
{
public:
  static const size_t npos;

  String( void );

  //! Adds ref to str
  String( const StringData *str );

  //! Adds ref to str
  String( const CountPtr< StringData > &str );

  //! Adds ref to str
  String( const CountPtr< const StringData > &str );

  //! Adds ref to str
  String( const String &str );

  //! Copies string
  String( const char *str, size_t len, const char *str2=0, size_t len2=0 );

  //! Copies string
  String( const char *str, const char *str2=0 );

  //! Copies string
  String( const std::string &str );

  //! Wrapps static const string, use String::Static( "foo" )
  static
  String Static( const char *str="" );

  static
  String MoveFrom( std::string &str );

  virtual ~String( void );

  void clear( void );

  bool empty( void ) const;

  String &operator=( const String &str );
  String &operator=( const std::string &str );
  String &operator=( const char *str );

  template< class S >
  String &assign( const S &s )
  {
    return this->operator=( s );
  }

  const char *get( void ) const
  {
    return m_c_str;
  }

  const char *c_str( void ) const
  {
    return get();
  }

  operator const char* ( void ) const
  {
    return get();
  }

  CountPtr< const StringData > getData( void ) const;

  String &moveFrom( std::string &str );

  void swap( String &other );

  int compare( const char *other ) const;
  int compare( const String &other ) const;

  bool equals( const char *other ) const;
  bool equals( const String &other ) const;

  size_t length( void ) const;
  size_t size( void ) const;

  size_t find( char c, size_t start_pos = 0 ) const;
  String mid( size_t first_pos, size_t last_pos = npos ) const;

  index_t count( void ) const;

  bool operator< ( const String &other ) const;
  bool operator> ( const String &other ) const;
  bool operator<=( const String &other ) const;
  bool operator>=( const String &other ) const;
  bool operator==( const String &other ) const;
  bool operator!=( const String &other ) const;
  bool operator< ( const char *other ) const;
  bool operator> ( const char *other ) const;
  bool operator<=( const char *other ) const;
  bool operator>=( const char *other ) const;
  bool operator==( const char *other ) const;
  bool operator!=( const char *other ) const;

  char operator[] ( size_t i ) const;

  String &operator+=( const String &other );
  String &operator+=( const std::string &other );
  String &operator+=( const char *other );

private:
  CountPtr< const StringData > m_str;
  const char *m_c_str;
  static const char *const empty_c_str;
};

inline
String operator+( const String &s1, const String &s2 )
{
  return String( s1.c_str(), s1.length(), s2.c_str(), s2.length() );
}

inline
String operator+( const String &s1, const std::string &s2 )
{
  return String( s1.c_str(), s1.length(), s2.c_str(), s2.length() );
}

inline
String operator+( const String &s1, const char *s2 )
{
  return String( s1.c_str(), s1.length(), s2, ( s2 ? strlen( s2 ) : 0 ) );
}

inline
String operator+( const std::string &s1, const String &s2 )
{
  return String( s1.c_str(), s1.length(), s2.c_str(), s2.length() );
}

inline
String operator+( const char *s1, const String &s2 )
{
  return String( s1, ( s1 ? strlen( s1 ) : 0 ), s2.c_str(), s2.length() );
}

template< class Type >
inline
String toString( const Type &x )
{
  std::ostringstream str;
  str << x;
  return String( str.str() );
}

template<>
inline
String toString< String >( const String &x )
{
  return x;
}

template<>
inline
String toString< const char* >( const char *const &x )
{
  return String( x );
}

template<>
inline
String toString< std::string >( const std::string &x )
{
  return String( x );
}

} // namespace RPGML

namespace std {

  inline
  std::ostream &operator<<( std::ostream &o, const RPGML::String &s )
  {
    return o << s.c_str();
  }

} // namespace std

#endif

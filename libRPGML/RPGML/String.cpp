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
#include "String.h"

namespace RPGML {

  const char *const String::empty_c_str = "";

  CountPtr< const StringData > MallocString::create( const char *s, size_t len, const char *s2, size_t len2 )
  {
    assert( s2 || !len2 );
    const size_t memsize = ( sizeof( MallocString )+1 ) + ( len + len2 );
    CountPtr< MallocString > ret( new ( ::operator new( memsize ) ) MallocString() );

    char *str = (char*)ret->m_str;
    strncpy( str, s, len );
    if( s2 ) strncpy( str+len, s2, len2 );
    str[ len+len2 ] = '\0';

    // CountPtr for return must be created before CountPtr "ret" is destroyed
    return CountPtr< const StringData >( ret.get() );
  }

  String::String( void )
  : m_c_str( empty_c_str )
  {}

  String::String( const StringData *str )
  : m_str( str )
  , m_c_str( str ? str->get() : empty_c_str )
  {}

  String::String( const String &str )
  : m_str( str.getData() )
  , m_c_str( str.get() ) // str can be from Static(), so m_str would be 0
  {
    if( !m_c_str ) m_c_str = empty_c_str;
  }

  String::String( const char *str, size_t len, const char *str2, size_t len2 )
  : m_str( MallocString::create( str, len, str2, len2 ) )
  , m_c_str( m_str->get() )
  {
    if( !m_c_str ) m_c_str = empty_c_str;
  }

  String::String( const char *str, const char *str2 )
  : m_c_str( empty_c_str )
  {
    const size_t len = strlen( str );
    const size_t len2 = ( str2 ? strlen( str2 ) : 0 );
    m_str = MallocString::create( str, len, str2, len2 );
    m_c_str = m_str->get();
    if( !m_c_str ) m_c_str = empty_c_str;
  }

  String::String( const std::string &str )
  : m_c_str( empty_c_str )
  {
    const size_t len = str.length();
    m_str = MallocString::create( str.c_str(), len );
    m_c_str = m_str->get();
    if( !m_c_str ) m_c_str = empty_c_str;
  }

  String String::Static( const char *str )
  {
    String ret;
    ret.m_c_str = str; // might be NULL
    return ret;
  }

  String String::MoveFrom( std::string &str )
  {
    String ret;
    ret.moveFrom( str );
    return ret;
  }

  String::~String( void )
  {
    clear();
  }

  void String::clear( void )
  {
    m_str.reset();
    m_c_str = empty_c_str;
  }

  bool String::empty( void ) const
  {
    assert( m_c_str );
    return m_c_str[ 0 ] == '\0';
  }

  String &String::operator=( const String &str )
  {
    String tmp( str );
    this->swap( tmp );
    return (*this);
  }

  String &String::operator=( const std::string &str )
  {
    String tmp( str );
    this->swap( tmp );
    return (*this);
  }

  String &String::operator=( const char *str )
  {
    String tmp( str );
    this->swap( tmp );
    return (*this);
  }

  const StringData *String::getData( void ) const
  {
    if(
         m_str.isNull()
      && m_c_str != empty_c_str // static string literal used in this module
      )
    {
      m_str.reset( new StdString( m_c_str ) );
    }
    return m_str;
  }

  String &String::moveFrom( std::string &str )
  {
    StdString *stdstring;
    m_str.reset( stdstring = new StdString() );
    stdstring->moveFrom( str );
    m_c_str = m_str->get();
    if( !m_c_str ) m_c_str = empty_c_str;
    return (*this);
  }

  void String::swap( String &other )
  {
    std::swap( m_str, other.m_str );
    std::swap( m_c_str, other.m_c_str );
  }

  int String::compare( const char *other ) const
  {
    if( !m_c_str )
    {
      if( !other ) return 0;
      else return -1;
    }
    else if( !other )
    {
      return 1;
    }
    else
    {
      return strcmp( m_c_str, other );
    }
  }

  int String::compare( const String &other ) const
  {
    if( this == &other )
    {
      return 0;
    }
    else
    {
      return compare( other.get() );
    }
  }

  bool String::equals( const char *other ) const
  {
    return 0 == compare( other );
  }

  bool String::equals( const String &other ) const
  {
    return 0 == compare( other );
  }

  size_t String::length( void ) const
  {
    const char *const s = get();
    return ( s ? strlen( s ) : 0 );
  }

  size_t String::size( void ) const
  {
    return length();
  }

  index_t String::count( void ) const
  {
    return ( m_str ? m_str->count() : index_t(-1) );
  }

  bool String::operator< ( const String &other ) const { return compare( other ) <  0; }
  bool String::operator> ( const String &other ) const { return compare( other ) >  0; }
  bool String::operator<=( const String &other ) const { return compare( other ) <= 0; }
  bool String::operator>=( const String &other ) const { return compare( other ) >= 0; }
  bool String::operator==( const String &other ) const { return compare( other ) == 0; }
  bool String::operator!=( const String &other ) const { return compare( other ) != 0; }
  bool String::operator< ( const char *other ) const { return compare( other ) <  0; }
  bool String::operator> ( const char *other ) const { return compare( other ) >  0; }
  bool String::operator<=( const char *other ) const { return compare( other ) <= 0; }
  bool String::operator>=( const char *other ) const { return compare( other ) >= 0; }
  bool String::operator==( const char *other ) const { return compare( other ) == 0; }
  bool String::operator!=( const char *other ) const { return compare( other ) != 0; }

  char String::operator[] ( size_t i ) const { return m_str->get()[ i ]; }

  String &String::operator+=( const String &other )
  {
    String tmp = (*this) + other;
    this->swap( tmp );
    return (*this);
  }

  String &String::operator+=( const std::string &other )
  {
    String tmp = (*this) + other;
    this->swap( tmp );
    return (*this);
  }

  String &String::operator+=( const char *other )
  {
    String tmp = (*this) + other;
    this->swap( tmp );
    return (*this);
  }
} // namespace RPGML


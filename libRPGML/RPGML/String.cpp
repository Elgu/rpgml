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

#include <cstring>
#include <cstdlib>

namespace RPGML {

  const char *const String::empty_c_str = "";

  StringData::StringData( void )
  : m_begin( 0 )
  {}

  StringData::StringData( const char *begin )
  : m_begin( begin )
  {}

  StringData::~StringData( void )
  {}

  const char *StringData::get( void ) const
  {
    return m_begin;
  }

  bool StringData::isUnified( void ) const
  {
    return 0 != getUnifier();
  }

  const void *StringData::getUnifier( void ) const
  {
    return 0;
  }

  void StringData::set( const char *begin )
  {
    m_begin = begin;
  }

  int StringData::compare( const StringData &other ) const
  {
    const char *const this_str = get();
    const char *const other_str = other.get();
    return ::strcmp( this_str, other_str );
  }

  CountPtr< const MallocString > MallocString::create( const char *s, size_t len, const char *s2, size_t len2 )
  {
    if( !s ) len = 0;
    if( !s2 ) len2 = 0;
    const size_t memsize = ( sizeof( MallocString )+1 ) + ( len + len2 );
    CountPtr< MallocString > ret( new ( ::operator new( memsize ) ) MallocString() );

    char *str = (char*)ret->m_str;
    if( s ) strncpy( str, s, len );
    if( s2 ) strncpy( str+len, s2, len2 );
    str[ len+len2 ] = '\0';

    // CountPtr for return must be created before CountPtr "ret" is destroyed
    return CountPtr< const MallocString >( ret );
  }

  const size_t String::npos = size_t(-1);

  String::String( void )
  : m_c_str( empty_c_str )
  {}

  String::String( const StringData *str )
  : m_str( str )
  , m_c_str( str ? str->get() : empty_c_str )
  {
    if( !m_c_str ) m_c_str = empty_c_str;
  }

  String::String( const CountPtr< StringData > &str )
  : m_str( str )
  , m_c_str( str ? str->get() : empty_c_str )
  {
    if( !m_c_str ) m_c_str = empty_c_str;
  }

  String::String( const CountPtr< const StringData > &str )
  : m_str( str )
  , m_c_str( str ? str->get() : empty_c_str )
  {
    if( !m_c_str ) m_c_str = empty_c_str;
  }

  String::String( const String &str )
  : m_str( str.m_str )
  , m_c_str( str.m_c_str )
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
    const size_t len = ( str ? strlen( str ) : 0 );
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
    ret.m_c_str = ( str ? str : empty_c_str );
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

  CountPtr< const StringData > String::getData( void ) const
  {
    if( !m_str.isNull() )
    {
      return m_str;
    }
    else
    {
      assert( m_c_str );
      return new StdString( m_c_str );
    }
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
    assert( m_c_str );
    if( m_c_str == other )
    {
      return 0;
    }
    else if( !other )
    {
      // NULL string is the same as an empty string
      return ( empty() ? 0 : 1 );
    }
    else
    {
      return ::strcmp( m_c_str, other );
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
    const void *this_unifier;
    if(
        !m_str.isNull() && !other.m_str.isNull()
      && ( 0 != ( this_unifier = m_str->getUnifier() ) )
      && ( this_unifier == other.m_str->getUnifier() )
      )
    {
      return ( get() == other.get() );
    }
    return 0 == compare( other );
  }

  size_t String::length( void ) const
  {
    const char *const s = get();
    return ( s ? ::strlen( s ) : 0 );
  }

  size_t String::size( void ) const
  {
    return length();
  }

  size_t String::find( char c, size_t start_pos ) const
  {
    const size_t len = length();
    const char *const s = c_str();
    for( size_t pos = start_pos; pos < len; ++pos )
    {
      if( s[ pos ] == c ) return pos;
    }
    return npos;
  }

  String String::mid( size_t first_pos, size_t last_pos ) const
  {
    if( last_pos < first_pos ) return String();
    const size_t len = length();
    if( first_pos >= len ) return String();
    if( last_pos >= len ) last_pos = len-1;
    const char *const s = c_str();
    return String( s+first_pos, last_pos - first_pos + 1 );
  }

  index_t String::count( void ) const
  {
    return ( m_str ? m_str->refCount() : index_t(-1) );
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

  char String::operator[] ( size_t i ) const
  {
    assert( m_c_str ); // m_c_str is always valid
    return m_c_str[ i ];
  }

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

  void String::parse( bool     & ) const
  {
    throw Exception() << "Cannot parse bool directly from string, use something defined like != ''";
  }

  void String::parse( uint8_t  &x, int base ) const
  {
    x = uint8_t( strtol( c_str(), 0, base ) );
  }

  void String::parse( int8_t   &x, int base ) const
  {
    x = int8_t( strtol( c_str(), 0, base ) );
  }

  void String::parse( uint16_t &x, int base ) const
  {
    x = uint16_t( strtol( c_str(), 0, base ) );
  }

  void String::parse( int16_t  &x, int base ) const
  {
    x = int16_t( strtol( c_str(), 0, base ) );
  }

  void String::parse( uint32_t &x, int base ) const
  {
    x = uint32_t( strtol( c_str(), 0, base ) );
  }

  void String::parse( int32_t  &x, int base ) const
  {
    x = int32_t( strtol( c_str(), 0, base ) );
  }

  void String::parse( uint64_t &x, int base ) const
  {
    x = uint64_t( strtoull( c_str(), 0, base ) );
  }

  void String::parse( int64_t  &x, int base ) const
  {
    x = int64_t( strtoll( c_str(), 0, base ) );
  }

  void String::parse( float    &x ) const
  {
    x = strtof( c_str(), 0 );
  }

  void String::parse( double   &x ) const
  {
    x = strtod( c_str(), 0 );
  }

  void String::parse( String   &x ) const
  {
    x = (*this);
  }


} // namespace RPGML


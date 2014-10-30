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
  static CountPtr< const StringData > create( const char *s, size_t len, const char *s2=0, size_t len2=0 )
  {
    assert( s2 || !len2 );
    const size_t memsize = ( sizeof( MallocString )+1 ) + ( len + len2 );
    CountPtr< MallocString > ret( new ( ::operator new( memsize ) ) MallocString() );

    char *str = (char*)ret->m_str;
    strncpy( str, s, len );
    if( s2 ) strncpy( str+len, s2, len2 );
    str[ len+len2 ] = '\0';

    return ret.get();
  }
private:
  const char m_str[ 0 ];
};

class StdString : public StringData
{
public:
  StdString( void ) {}
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

typedef StringData StaticString;

class String
{
public:
  String( void )
  {}

  //! Adds ref to str
  String( const StringData *str )
  : m_str( str )
  {}

  //! Adds ref to str
  String( const String &str )
  : m_str( str.getData() )
  {}

  //! Copies string
  String( const char *str, size_t len, const char *str2=0, size_t len2=0 )
  : m_str( MallocString::create( str, len, str2, len2 ) )
  {}

  //! Copies string
  String( const char *str, const char *str2=0 )
  {
    const size_t len = strlen( str );
    const size_t len2 = ( str2 ? strlen( str2 ) : 0 );
    m_str = MallocString::create( str, len, str2, len2 );
  }

  //! Copies string
  String( const std::string &str )
  {
    const size_t len = str.length();
    m_str = MallocString::create( str.c_str(), len );
  }

  //! Wrapps static const string, use String::Static( "foo" )
  static
  String Static( const char *str )
  {
    return String( new StaticString( str ) );
  }

  virtual ~String( void )
  {
    clear();
  }

  void clear( void )
  {
    m_str.reset();
  }

  bool empty( void ) const
  {
    return !m_str || (m_str->get())[ 0 ] == '\0';
  }

  String &operator=( String str )
  {
    this->swap( str );
    return (*this);
  }

  String &operator=( const std::string &str )
  {
    String tmp( str );
    this->swap( tmp );
    return (*this);
  }

  String &operator=( const char *str )
  {
    String tmp( str );
    this->swap( tmp );
    return (*this);
  }

  template< class S >
  String &assign( const S &s )
  {
    return this->operator=( s );
  }

  const char *get( void ) const
  {
    return ( m_str ? m_str->get() : 0 );
  }

  const char *c_str( void ) const
  {
    return get();
  }

  operator const char* ( void ) const
  {
    return get();
  }

  const StringData *getData( void ) const
  {
    return m_str;
  }

  String &moveFrom( std::string &str )
  {
    StdString *stdstring;
    m_str.reset( stdstring = new StdString() );
    stdstring->moveFrom( str );
    return (*this);
  }

  void swap( String &other )
  {
    std::swap( m_str, other.m_str );
  }

  int compare( const char *other ) const
  {
    if( !m_str )
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
      return strcmp( m_str->get(), other );
    }
  }

  int compare( const String &other ) const
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

  bool equals( const char *other ) const
  {
    return 0 == compare( other );
  }

  bool equals( const String &other ) const
  {
    return 0 == compare( other );
  }

  size_t length( void ) const
  {
    return ( m_str ? strlen( m_str->get() ) : 0 );
  }

  size_t size( void ) const
  {
    return length();
  }

  index_t count( void ) const
  {
    return ( m_str ? m_str->count() : index_t(-1) );
  }

  bool operator< ( const String &other ) const { return compare( other ) <  0; }
  bool operator> ( const String &other ) const { return compare( other ) >  0; }
  bool operator<=( const String &other ) const { return compare( other ) <= 0; }
  bool operator>=( const String &other ) const { return compare( other ) >= 0; }
  bool operator==( const String &other ) const { return compare( other ) == 0; }
  bool operator!=( const String &other ) const { return compare( other ) != 0; }
  bool operator< ( const char *other ) const { return compare( other ) <  0; }
  bool operator> ( const char *other ) const { return compare( other ) >  0; }
  bool operator<=( const char *other ) const { return compare( other ) <= 0; }
  bool operator>=( const char *other ) const { return compare( other ) >= 0; }
  bool operator==( const char *other ) const { return compare( other ) == 0; }
  bool operator!=( const char *other ) const { return compare( other ) != 0; }

  char operator[] ( size_t i ) const { return m_str->get()[ i ]; }

  String &operator+=( const String &other )
  {
    String tmp = (*this) + other;
    this->swap( tmp );
    return (*this);
  }

  String &operator+=( const std::string &other )
  {
    String tmp = (*this) + other;
    this->swap( tmp );
    return (*this);
  }

  String &operator+=( const char *other )
  {
    String tmp = (*this) + other;
    this->swap( tmp );
    return (*this);
  }

private:
  CountPtr< const StringData > m_str;
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

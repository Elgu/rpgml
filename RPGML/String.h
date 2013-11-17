#ifndef RPGML_String_h
#define RPGML_String_h

#include "Refcounted.h"
#include <string>

namespace RPGML {

class String : public Refcounted
{
public:
  String( void )
  {}

  explicit String( const std::string &str )
  : m_str( str )
  {}

  virtual ~String( void )
  {}

  String &operator=( String str )
  {
    this->swap( str );
    return (*this);
  }

  String &operator=( std::string str )
  {
    std::swap( m_str, str );
    return (*this);
  }

  const std::string &get( void ) const
  {
    return m_str;
  }

  std::string &get( void )
  {
    return m_str;
  }

  const char *c_str( void ) const
  {
    return m_str.c_str();
  }

  String &moveFrom( std::string &str )
  {
    std::swap( m_str, str );
    str.clear();
    return (*this);
  }

  void swap( String &other )
  {
    std::swap( m_str, other.m_str );
  }

  bool equals( const String &other ) const
  {
    return equals( &other );
  }

  bool equals( const String *other ) const
  {
    return ( this->get() == other->get() );
  }

private:
  std::string m_str;
};

} // namespace RPGML

#endif

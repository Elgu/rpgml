/*! @file Exception.cpp
 * @brief Exception class implementation
 * @author Gunnar Payer
 * @date November 27th 2014
 */
#include "Exception.h"

namespace RPGML {

Exception::Exception( void ) throw()
{}

Exception::Exception( const std::string &text ) throw()
: m_text( text )
{}

Exception::~Exception( void ) throw()
{}

const char *Exception::what() const throw()
{
  return m_text.c_str();
}

Exception &Exception::append( const std::string &text )
{
  m_text += text;
  return (*this);
}

} // namespace RPGML


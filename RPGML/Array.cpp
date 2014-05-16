#include "Array.h"

#include <memory>

namespace RPGML {

Array::Array( GarbageCollector *_gc )
: Collectable( _gc )
{}

Array::~Array( void )
{}

index_t Array::size( void ) const
{
  return index_t( m_values.size() );
}

Array &Array::resize( index_t new_size )
{
  m_values.resize( new_size );
  return (*this);
}

Value *Array::set( index_t i, const Value &value )
{
  if( i < size() )
  {
    Value &target = m_values[ i ];
    target = value;
    return &target;
  }
  else
  {
    return 0;
  }
}

Value *Array::get( index_t i )
{
  if( i < size() )
  {
    return &m_values[ i ];
  }
  return 0;
}

const Value *Array::get( index_t i ) const
{
  if( i < size() )
  {
    return &m_values[ i ];
  }
  return 0;
}

Value *Array::append( const Value &value )
{
  m_values.push_back( value );
  return &m_values.back();
}

Value const &Array::operator[]( index_t i ) const
{
  if( i < size() )
  {
    return m_values[ i ];
  }
  throw "Index out of range";
}

Value       &Array::operator[]( index_t i )
{
  if( i < size() )
  {
    return m_values[ i ];
  }
  throw "Index out of range";
}

void Array::swap( Array &other )
{
  std::swap( m_values, other.m_values );
}

void Array::gc_getChildren( Children &children ) const
{
  const Array::values_t &values = m_values;
  for( index_t i( 0 ), end( index_t( values.size() ) ); i<end; ++i )
  {
    const Value &value = values[ i ];
    if( values[ i ].isCollectable() )
    {
      children.push_back( value.getCollectable() );
    }
  }
}

void Array::gc_clear( void )
{
  m_values.clear();
}

CountPtr< Array::Elements > Array::getElements( void )
{
  return new _Elements( this );
}

CountPtr< Array::ConstElements > Array::getElements( void ) const
{
  return new _ConstElements( this );
}

Array::_ConstElements::_ConstElements( const Array *array, index_t i )
: m_array( array )
, m_i( i )
{}

Array::_ConstElements::~_ConstElements( void )
{}

bool Array::_ConstElements::done( void )
{
  const Array::values_t &values = m_array->m_values;
  return m_i >= values.size();
}

void Array::_ConstElements::next( void )
{
  ++m_i;
}

const Value *Array::_ConstElements::get( void )
{
  const Array::values_t &values = m_array->m_values;
  if( m_i < values.size() )
  {
    return &values[ m_i ];
  }
  else
  {
    return 0;
  }
}

CountPtr< Array::ConstElements > Array::_ConstElements::clone( void ) const
{
  return new _ConstElements( m_array, m_i );
}

Array::_Elements::_Elements( Array *array, index_t i )
: m_array( array )
, m_i( i )
{}

Array::_Elements::~_Elements( void )
{}

bool Array::_Elements::done( void )
{
  Array::values_t &values = m_array->m_values;
  return m_i >= values.size();
}

void Array::_Elements::next( void )
{
  ++m_i;
}

Value *Array::_Elements::get( void )
{
  Array::values_t &values = m_array->m_values;
  if( m_i < values.size() )
  {
    return &values[ m_i ];
  }
  else
  {
    return 0;
  }
}

CountPtr< Array::Elements > Array::_Elements::clone( void ) const
{
  return new _Elements( m_array, m_i );
}

} // namespace RPGML


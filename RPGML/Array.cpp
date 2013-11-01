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

Value *Array::set( index_t i, Value value )
{
  if( i < size() )
  {
    Value &target = m_values[ i ];
    target.swap( value );
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

Value *Array::append( Value value )
{
  m_values.push_back( value );
  return &m_values.back();
}

void Array::swap( Array &other )
{
  std::swap( m_values, other.m_values );
}

CountPtr< Collectable::Children > Array::getChildren( void ) const
{
  return new _Children( this );
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

Array::_Children::_Children( const Array *array, index_t i )
: m_array( array )
, m_i( i )
{
  const Array::values_t &values = m_array->m_values;
  while(
       m_i < values.size()
    && !values[ m_i ].isCollectable()
    )
  {
    ++m_i;
  }
}

Array::_Children::~_Children( void )
{}

bool Array::_Children::done( void )
{
  const Array::values_t &values = m_array->m_values;
  return ( m_i >= values.size() );
}

void Array::_Children::next( void )
{
  const Array::values_t &values = m_array->m_values;
  do
  {
    ++m_i;
  }
  while(
       m_i < values.size()
    && !values[ m_i ].isCollectable()
    );
}

const Collectable *Array::_Children::get( void )
{
  const Array::values_t &values = m_array->m_values;
  return values[ m_i ].getCollectable();
}

CountPtr< Collectable::Children > Array::_Children::clone( void ) const
{
  return new _Children( m_array, m_i );
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


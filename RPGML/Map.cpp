#include "Map.h"

#include "String.h"

namespace RPGML {

Map::Map( GarbageCollector *_gc, Map *parent )
: Collectable( _gc )
, m_parent( parent )
, m_depth( parent ? parent->getDepth()+1 : 0 )
, m_is_this_map( false )
{}

Map::~Map( void )
{}

Map *Map::getParent( void ) const
{
  return m_parent;
}

Map &Map::setIdentifier( const String *identifier )
{
  m_identifier = identifier;
  return (*this);
}

const String *Map::getIdentifier( void ) const
{
  return m_identifier;
}

Map &Map::setThisMap( bool is_this_map )
{
  m_is_this_map = is_this_map;
  return (*this);
}

bool Map::isThisMap( void ) const
{
  return m_is_this_map;
}

index_t Map::getDepth( void ) const
{
  return m_depth;
}

index_t Map::getIndex( const Value &key ) const
{
  map_t::const_iterator iter = m_map.find( key );
  if( iter != m_map.end() )
  {
    return iter->second;
  }
  else
  {
    return unknown;
  }
}

index_t Map::getCreateIndex( const Value &key, bool *existed_p )
{
  if( key.isInvalid() ) throw "key is invalid";
  map_t::const_iterator iter = m_map.find( key );
  if( iter != m_map.end() )
  {
    if( existed_p ) (*existed_p) = true;
    return iter->second;
  }
  else
  {
    if( existed_p ) (*existed_p) = false;
    const index_t index = index_t( m_values.size() );
    m_map.insert( std::make_pair( key, index ) );
    m_values.resize( index+1 );
    return index;
  }
}

const Value *Map::get( const Value &key ) const
{
  const index_t index = getIndex( key );

  if( index != unknown )
  {
    return &m_values[ index ];
  }
  else
  {
    return 0;
  }
}

Value *Map::get( const Value &key )
{
  const index_t index = getIndex( key );

  if( index != unknown )
  {
    return &m_values[ index ];
  }
  else
  {
    return 0;
  }
}

Value *Map::set( const Value &key, const Value &value )
{
  if( key.isInvalid() ) throw "key is invalid";
  Value &ret = (*this)[ key ];
  ret = value;
  return &ret;
}

Value &Map::operator[]( const Value &key )
{
  if( key.isInvalid() ) throw "key is invalid";
  return m_values[ getCreateIndex( key ) ];
}

const Value &Map::operator[]( const Value &key ) const
{
  const Value *const ret = get( key );
  if( !ret ) throw "Unknown key";
  return (*ret);
}

Value &Map::operator[]( index_t index )
{
  if( index < m_values.size() )
  {
    return m_values[ index ];
  }
  throw "Index out of range";
}

const Value &Map::operator[]( index_t index ) const
{
  if( index < m_values.size() )
  {
    return m_values[ index ];
  }
  throw "Index out of range";
}

index_t Map::push( const Value &key )
{
  if( key.isInvalid() ) throw "key is invalid";
  return getCreateIndex( key );
}

index_t Map::pop( index_t n )
{
  const index_t old_size = index_t( m_values.size() );
  const index_t new_size = ( n >= old_size ? old_size - n : 0 );
  m_values.resize( new_size );
  return new_size;
}

void Map::gc_clear( void )
{
  m_values.clear();
  m_map.clear();
  m_identifier.reset();
  m_parent.reset();
}

CountPtr< Collectable::Children > Map::getChildren( void ) const
{
  return new _Children( this, m_parent );
}

Map::_Children::_Children( const Map *_m, const Map *_parent, index_t _i )
: m( _m )
, parent( _parent )
, i( !_parent && 0 == _i ? 1 : _i )
{}

Map::_Children::~_Children( void )
{}

bool Map::_Children::done( void )
{
  return i > m->size(); // not >=, 0 is parent
}

void Map::_Children::next( void )
{
  const values_t &values = m->m_values;
  const index_t n = index_t( values.size() );
  do
  {
    ++i;
  }
  while(
       i <= n // not "<"
    && !values[ i-1 ].isCollectable()
    );
}

const Collectable *Map::_Children::get( void )
{
  if( i > 0 )
  {
    return m->m_values[ i-1 ].getCollectable();
  }
  else
  {
    return parent;
  }
}

CountPtr< Collectable::Children > Map::_Children::clone( void ) const
{
  return new _Children( (*this) );
}

} // namespace RPGML


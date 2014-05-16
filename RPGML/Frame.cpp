#include "Map.h"

#include "String.h"
#include "Scope.h"
#include "SharedObject.h"
#include "Function.h"
#include "InterpretingParser.h"
#include "FileSource.h"

#include <sys/types.h>
#include <dirent.h>

namespace RPGML {

Map::Map( GarbageCollector *_gc, Map *parent, const String &path )
: Collectable( _gc )
, m_parent( parent )
, m_path( path )
, m_depth( parent ? parent->getDepth()+1 : 0 )
, m_is_this_map( false )
{}

Map::~Map( void )
{}

Map *Map::getParent( void ) const
{
  return m_parent;
}

const String &Map::getPath( void ) const
{
  return m_path;
}

Map &Map::setIdentifier( const String &identifier )
{
  m_identifier = identifier;
  return (*this);
}

const String &Map::getIdentifier( void ) const
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

Value *Map::load( const Value &key, const Scope *scope )
{
  const index_t index = getIndex( key );

  if( index != unknown )
  {
    return &m_values[ index ];
  }

  const bool is_root = ( this == scope->getRoot() );

  if( !getPath().empty() || is_root )
  {
    String identifier;
    try
    {
      identifier = key.to( Type::String() ).getString();
    }
    catch( const char * )
    {}

    if( !identifier.empty() )
    {
      if( is_root )
      {
        return load_global( identifier, scope );
      }
      else
      {
        return load_local( identifier, scope );
      }
    }
  }

  return 0;
}

Value *Map::load_local ( const String &identifier, const Scope *scope )
{
  return load( getPath(), identifier, scope );
}

Value *Map::load_global( const String &identifier, const Scope *scope )
{
  return load( ".", identifier, scope );
}

Value *Map::load( const String &path, const String &identifier, const Scope *scope )
{
  String dir = path + "/" + identifier;
  DIR *const dir_p = opendir( dir.c_str() );
  if( dir_p )
  {
    closedir( dir_p );
    return set( Value( identifier ), Value( new Map( getGC(), this, dir ) ) );
  }

  const String so = path + "/libRPGML_" + identifier + ".so";
  String err;
  CountPtr< SharedObject > so_p = new SharedObject( so, err );
  if( so_p->isValid() )
  {
    CountPtr< SharedObject::Symbol< function_creator_f > > create_function =
      so_p->getSymbol< function_creator_f >( identifier + "_create", err );
    CountPtr< Function > function = (**create_function)( getGC(), create_function.get() );

    return set( Value( identifier ), Value( function ) );
  }

  const String rpgml = path + "/" + identifier + ".rpgml";
  FILE *const rpgml_p = fopen( rpgml.c_str(), "r" );
  if( rpgml_p )
  {
    CountPtr< Source > source = new FileSource( rpgml_p );
    CountPtr< Scope > new_scope = new Scope( *scope );
    CountPtr< InterpretingParser > parser = new InterpretingParser( new_scope, source );
    try
    {
      parser->parse();
    }
    catch( const char *e )
    {
      throw e;
    }

    Value *const ret = get( Value( identifier ) );
    if( !ret )
    {
      throw "variable was not defined in rpgml file";
    }
    return ret;
  }

  return 0;
}

Value *Map::set( const Value &key, const Value &value )
{
  if( key.isInvalid() ) throw "key is invalid";
  Value &ret = (*this)[ key ];
  ret = value;

//  std::cerr << "Map::set( " << key << ", " << value << " )" << std::endl;
//  std::cerr << "  " << Value( this ) << std::endl;

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
  m_identifier.clear();
  m_parent.reset();
}

void Map::getChildren( Children &children ) const
{
  children.push_back( m_parent );

  for( size_t i( 0 ), end( m_values.size() ); i<end; ++i )
  {
    const Value &value = m_values[ i ];
    if( value.isCollectable() )
    {
      children.push_back( value.getCollectable() );
    }
  }

  for( map_t::const_iterator i( m_map.begin() ), end( m_map.end() ); i != end; ++i )
  {
    const Value &value = i->first;
    if( value.isCollectable() )
    {
      children.push_back( value.getCollectable() );
    }
  }
}

bool Map::map_less_than::operator()( const Value &x, const Value &y ) const
{
  const Type::Enum e1 = x.getType().getEnum();
  const Type::Enum e2 = y.getType().getEnum();
  if( e1 < e2 ) return true;
  if( e1 > e2 ) return false;

  if( x.isString() )
  {
    return x.getString() < y.getString();
  }
  else
  {
    return ( x.getP() < y.getP() );
  }
}

} // namespace RPGML


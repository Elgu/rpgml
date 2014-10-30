#include "Frame.h"

#include "String.h"
#include "Scope.h"
#include "SharedObject.h"
#include "Function.h"
#include "InterpretingParser.h"
#include "FileSource.h"

#include <sys/types.h>
#include <dirent.h>

namespace RPGML {

Frame::Frame( GarbageCollector *_gc, Frame *parent, const String &path )
: Collectable( _gc )
, m_parent( parent )
, m_path( path )
, m_depth( parent ? parent->getDepth()+1 : 0 )
, m_is_this( false )
{}

Frame::~Frame( void )
{}

Frame *Frame::getParent( void ) const
{
  return m_parent;
}

const String &Frame::getPath( void ) const
{
  return m_path;
}

Frame &Frame::setIdentifier( const String &identifier )
{
  m_identifier = identifier;
  return (*this);
}

const String &Frame::getIdentifier( void ) const
{
  return m_identifier;
}

Frame &Frame::setThis( bool is_this )
{
  m_is_this = is_this;
  return (*this);
}

bool Frame::isThis( void ) const
{
  return m_is_this;
}

index_t Frame::getDepth( void ) const
{
  return m_depth;
}

void Frame::reserve( index_t n )
{
  m_values.reserve( n );
}

index_t Frame::getIndex( const char *identifier ) const
{
  const int n = int( m_identifiers.size() );
  for( int i = n-1; i>=0; --i )
  {
    if( m_identifiers[ i ] == identifier )
    {
      return index_t( i );
    }
  }
  return unknown;
}

index_t Frame::getCreateIndex( const String &identifier, bool *existed_p )
{
  index_t index = getIndex( identifier );
  if( unknown != index )
  {
    if( existed_p ) (*existed_p) = true;
    return index;
  }

  assert( m_values.size() == m_identifiers.size() );
  index = index_t( m_values.size() );
  m_values     .push_back( Value() );
  m_identifiers.push_back( identifier );

  if( existed_p ) (*existed_p) = false;
  return index;
}

index_t Frame::set( const String &identifier, const Value &value )
{
  const index_t index = getCreateIndex( identifier );
  m_values[ index ] = value;
  return index;
}

const Value *Frame::getVariable( const char *identifier ) const
{
  const index_t index = getIndex( identifier );

  if( index != unknown )
  {
    return &m_values[ index ];
  }
  else
  {
    return 0;
  }
}

Value *Frame::getVariable( const char *identifier )
{
  return const_cast< Value* >( ((const Frame*)this)->getVariable( identifier ) );
}

const Value *Frame::getStack( index_t index ) const
{
  if( size_t( index ) < m_values.size() )
  {
    return &m_values[ index ];
  }
  else
  {
    return 0;
  }
}

Value       *Frame::getStack( index_t index )
{
  return const_cast< Value* >( ((const Frame*)this)->getStack( index ) );
}

Value *Frame::push_back( const String &identifier, const Value &value )
{
  assert( m_values.size() == m_identifiers.size() );
  const index_t index = index_t( m_values.size() );

  m_identifiers.push_back( identifier );
  m_values.push_back( value );

  return &m_values[ index ];
}

void Frame::pop_back( void )
{
  assert( m_values.size() == m_identifiers.size() );
  if( m_values.empty() ) throw "Frame already empty";

  const size_t new_size = m_values.size()-1;
  m_values.resize( new_size );
  m_identifiers.resize( new_size );
}

Frame::PushPopGuard::PushPopGuard( Frame *frame, const String &identifier, const Value &value )
: m_frame( frame )
, m_value( m_frame ? m_frame->push_back( identifier, value ) : 0 )
{}

Frame::PushPopGuard::~PushPopGuard( void )
{
  if( m_frame ) m_frame->pop_back();
}

Value *Frame::PushPopGuard::get( void ) const
{
  return m_value;
}

Value *Frame::load( const String &identifier, const Scope *scope )
{
  const index_t index = getIndex( identifier );

  if( index != unknown )
  {
    return &m_values[ index ];
  }

  const bool is_root = ( this == scope->getRoot() );

  if( !getPath().empty() || is_root )
  {
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

Value *Frame::load_local ( const String &identifier, const Scope *scope )
{
  return load( getPath(), identifier, scope );
}

Value *Frame::load_global( const String &identifier, const Scope *scope )
{
  return load( ".", identifier, scope );
}

Value *Frame::load( const String &path, const String &identifier, const Scope *scope )
{
	// Check whether identifier refers to a directory, use it as namespace
  String dir = path + "/" + identifier;
  DIR *const dir_p = opendir( dir.c_str() );
  if( dir_p )
  {
    closedir( dir_p );
    return getStack( set( identifier, Value( new Frame( getGC(), this, dir ) ) ) );
  }

	// Check whether identifier refers to a plugin
  const String so = path + "/libRPGML_" + identifier + ".so";
  FILE *so_file = fopen( so.c_str(), "rb" );
  if( so_file )
  {
//    std::cerr << "Found shared object '" << so << "'" << std::endl;
    fclose( so_file );

    String err;
    CountPtr< SharedObject > so_p = new SharedObject( so, err );
    if( so_p->isValid() )
    {
//      std::cerr << "Loaded shared object '" << so << "'" << std::endl;

      create_Function_t create_function;
      const String symbol = identifier + "_create_Function";
      if( so_p->getSymbol( symbol, create_function, err ) )
      {
//        std::cerr << "Got symbol '" << symbol << " from shared object '" << so << "'" << std::endl;
        CountPtr< Function > function = create_function( getGC(), this, so_p.get() );
        return getStack( set( identifier, Value( function.get() ) ) );
      }
    }
    else
    {
      std::cerr << "Opening so file '" << so << "' failed: " << err << std::endl;
      throw "Opening so failed";
    }
  }

	// Check whether identifier refers to a script
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

    Value *const ret = getVariable( identifier );
    if( !ret )
    {
      throw "variable was not defined in rpgml file";
    }
    return ret;
  }

  return 0;
}

void Frame::gc_clear( void )
{
  m_values.clear();
  m_identifiers.clear();
  m_identifier.clear();
  m_parent.reset();
}

void Frame::gc_getChildren( Children &children ) const
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
}

} // namespace RPGML


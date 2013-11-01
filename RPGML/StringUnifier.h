#ifndef RPGML_StringUnifier_h
#define RPGML_StringUnifier_h

#include "Refcounted.h"
#include "String.h"

#include <map>
#include <cstring>

namespace RPGML {

class StringUnifier : public Refcounted
{
public:
  StringUnifier( void ) {}
  virtual ~StringUnifier( void ) {}

  template< class S >
  const String *unify( const S &s )
  {
    const char *const c_in = getCharPtr( s );
    const String *const ret1 = get( c_in );
    if( ret1 ) return ret1;

    CountPtr< String > ret2 = new String( c_in );
    const char *const c_out = getCharPtr( ret2.get() );
    m_map.insert( std::make_pair( c_out, ret2 ) );
    return ret2.get();
  }

  const String *unify_move( String *s )
  {
    const char *const c_in = getCharPtr( s );
    const String *const ret1 = get( c_in );
    if( ret1 ) return ret1;

    const char *const c_out = getCharPtr( s );
    m_map.insert( std::make_pair( c_out, s ) );
    return s;
  }

  const String *unify_move( std::string &s )
  {
    const char *const c_in = getCharPtr( s );
    const String *const ret1 = get( c_in );
    if( ret1 ) return ret1;

    CountPtr< String > ret2 = new String();
    ret2->moveFrom( s );
    const char *const c_out = getCharPtr( ret2.get() );
    m_map.insert( std::make_pair( c_out, ret2 ) );
    return ret2.get();
  }

  void clear( void )
  {
    m_map.clear();
  }

  void cleanup( void )
  {
    typedef std::vector< CountPtr< String > > to_be_removed_t;
    to_be_removed_t to_be_removed;
    to_be_removed.reserve( 16 );

    for( map_t::const_iterator i( m_map.begin() ), end( m_map.end() ); i != end; ++i )
    {
      if( 1 == i->second->count() )
      {
        to_be_removed.push_back( i->second );
      }
    }

    for( to_be_removed_t::const_iterator i( to_be_removed.begin() ), end( to_be_removed.end() ); i != end; ++i )
    {
      remove( i->get() );
    }
  }

  void remove( const String *s )
  {
    m_map.erase( getCharPtr( s ) );
  }

private:
  static
  const char *getCharPtr( const String *s )
  {
    return s->get().c_str();
  }

  static
  const char *getCharPtr( const char *s )
  {
    return s;
  }

  static
  const char *getCharPtr( const std::string &s )
  {
    return s.c_str();
  }

  template< class S >
  const String *get( const S &s )
  {
    const char *const c_in = getCharPtr( s );
    map_t::const_iterator found = m_map.find( c_in );
    if( found != m_map.end() )
    {
      return found->second.get();
    }
    else
    {
      return 0;
    }
  }

  struct compare
  {
    bool operator()( const char *x, const char *y )
    {
      return 0 > ::strcmp( x, y );
    }
  };

  typedef std::map< const char*, CountPtr< String >, compare > map_t;
  map_t m_map;
};

} // namespace RPGML

#endif


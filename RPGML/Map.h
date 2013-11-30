#ifndef RPGML_Map_h
#define RPGML_Map_h

#include "Value.h"
#include "String.h"
#include "GarbageCollector.h"
#include "types.h"

#include <map>
#include <vector>

namespace RPGML {

class Scope;

class Map : public Collectable
{
public:
  static const index_t unknown = index_t(-1);

  explicit
  Map( GarbageCollector *gc, Map *parent=0, const String &path = String() );
  virtual ~Map( void );

  Map *getParent( void ) const;
  const String &getPath( void ) const;

  Map &setIdentifier( const String &identifier );
  const String &getIdentifier( void ) const;

  Map &setThisMap( bool is_this_map=true );
  bool isThisMap( void ) const;

  index_t getDepth( void ) const;

  index_t getIndex( const Value &key ) const;
  index_t getCreateIndex( const Value &key, bool *existed_p=0 );

  Value       *get( const Value &key );
  const Value *get( const Value &key ) const;
  //! Creates entry, if not existing
  Value       *set( const Value &key, const Value &value );

  Value *load( const Value &key, const Scope *scope );

  Value &operator[]( const Value &key );
  const Value &operator[]( const Value &key ) const;

  Value &operator[]( index_t index );
  const Value &operator[]( index_t index ) const;

  index_t size( void ) const { return index_t( m_values.size() ); }

  void reserve( index_t n ) { m_values.reserve( n ); }

  struct map_less_than { bool operator()( const Value &x, const Value &y ) const; };
  typedef std::vector< Value > values_t;
  typedef std::map< Value, index_t, map_less_than > map_t;

  class Iterator : public RPGML::Iterator< std::pair< Value, Value > >
  {
    friend class Map;
  private:
    Iterator( const values_t *values, const map_t::const_iterator &begin, const map_t::const_iterator &end )
    : m_values( values ), m_begin( begin ), m_end( end )
    {}
  public:
    typedef RPGML::Iterator< std::pair< Value, Value > > Base;
    virtual ~Iterator( void ) {}
    virtual bool done( void ) { return ( m_begin == m_end ); }
    virtual void next( void ) { ++m_begin; }
    virtual Type get( void ) { return std::make_pair( m_begin->first, (*m_values)[ m_begin->second ] ); }
    virtual CountPtr< Base > clone( void ) const { return new Iterator( (*this) ); }
  private:
    const values_t *const m_values;
    map_t::const_iterator m_begin;
    const map_t::const_iterator m_end;
  };

  CountPtr< Iterator > getIterator( void ) const
  {
    return new Iterator( &m_values, m_map.begin(), m_map.end() );
  }

  //! Returns index for that key
  index_t push( const Value &key );
  //! Makes the last n values invalid, indeces in the map are not cleared
  index_t pop( index_t n );

  class PushPopGuard
  {
  public:
    explicit
    PushPopGuard( Map *m )
    : m_m( m )
    , m_old_size( m ? m->size() : 0 )
    {}
    virtual ~PushPopGuard( void )
    {
      if( m_m )
      {
        const index_t new_size = m_m->size();
        if( m_old_size < new_size )
        {
          m_m->pop( new_size - m_old_size );
        }
      }
    }
  private:
    CountPtr< Map > m_m;
    index_t m_old_size;
  };

  virtual void gc_clear( void );
  virtual void getChildren( Children &children ) const;

private:
  typedef CountPtr< Function > (*function_creator_f)( GarbageCollector *gc, const Refcounted *ref );

  Value *load_local ( const String &identifier, const Scope *scope );
  Value *load_global( const String &identifier, const Scope *scope );
  Value *load( const String &path, const String &identifier, const Scope *scope );

  values_t m_values;
  map_t m_map;
  CountPtr< Map > m_parent; // might create a closure, points to where the lookup continues
  String m_identifier; // might be null for anonymous Maps
  String m_path; // if this map corresponds to a directory
  index_t m_depth; // determined at setParent(), checked there
  bool m_is_this_map; // whether lookup for "this" returns this Map
};

} // namespace RPGML

#endif


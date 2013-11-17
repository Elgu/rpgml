#ifndef RPGML_Map_h
#define RPGML_Map_h

#include "Value.h"
#include "GarbageCollector.h"
#include "types.h"

#include <map>
#include <vector>

namespace RPGML {

class Map : public Collectable
{
public:
  static const index_t unknown = index_t(-1);

  explicit
  Map( GarbageCollector *gc, Map *parent=0 );
  virtual ~Map( void );

  Map *getParent( void ) const;

  Map &setIdentifier( const String *identifier );
  const String *getIdentifier( void ) const;

  Map &setThisMap( bool is_this_map=true );
  bool isThisMap( void ) const;

  index_t getDepth( void ) const;

  index_t getIndex( const Value &key ) const;
  index_t getCreateIndex( const Value &key, bool *existed_p=0 );

  Value       *get( const Value &key );
  const Value *get( const Value &key ) const;
  //! Creates entry, if not existing
  Value       *set( const Value &key, const Value &value );

  Value &operator[]( const Value &key );
  const Value &operator[]( const Value &key ) const;

  Value &operator[]( index_t index );
  const Value &operator[]( index_t index ) const;

  index_t size( void ) const { return index_t( m_values.size() ); }

  void reserve( index_t n ) { m_values.reserve( n ); }

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
  virtual CountPtr< Children > getChildren( void ) const;

private:
  struct map_less_than
  {
    bool operator()( const Value &x, const Value &y ) const;
  };
  typedef std::vector< Value > values_t;
  typedef std::map< Value, index_t, map_less_than > map_t;
  friend class _Children;

  class _Children : public Children
  {
  public:
    explicit
    _Children( const Map *_m, const Map *_parent, index_t _i=0 );
    virtual ~_Children( void );

    virtual bool done( void );
    virtual void next( void );
    virtual const Collectable *get( void );
    virtual CountPtr< Children > clone( void ) const;

  private:
    CountPtr< const Map > m;
    CountPtr< const Map > parent;
    index_t i;
  };

  values_t m_values;
  map_t m_map;
  CountPtr< Map > m_parent; // might create a closure, points to where the lookup continues
  CountPtr< const String > m_identifier; // might be null for anonymous Maps
  index_t m_depth; // determined at setParent(), checked there
  bool m_is_this_map; // whether lookup for "this" returns this Map
};

} // namespace RPGML

#endif


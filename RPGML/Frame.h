#ifndef RPGML_Map_h
#define RPGML_Map_h

#include "Value.h"
#include "String.h"
#include "GarbageCollector.h"
#include "types.h"
#include "Exception.h"

#include <map>
#include <vector>

namespace RPGML {

class Scope;
class SharedObject;

class Frame : public Collectable
{
public:
  typedef Collectable Base;
  EXCEPTION_BASE( Exception );

  explicit
  Frame( GarbageCollector *gc, Frame *parent=0, const String &path = String() );
  virtual ~Frame( void );

  Frame *getParent( void ) const;
  const String &getPath( void ) const;

  Frame &setIdentifier( const String &identifier );
  const String &getIdentifier( void ) const;

  Frame &setThis( bool is_this=true );
  bool isThis( void ) const;

  index_t getDepth( void ) const;

  void reserve( index_t size );

  index_t getIndex( const char *identifier ) const;
  index_t getCreateIndex( const String &identifier, bool *existed_p=0 );

  index_t set( const String &identifier, const Value &value );

  Value       *getVariable( const char *identifier );
  const Value *getVariable( const char *identifier ) const;
  Value       *getStack( index_t index );
  const Value *getStack( index_t index ) const;

  //! like 'foo.bar.identifier'
  String genGlobalName( const String &identifier ) const;

  Value *push_back( const String &identifier, const Value &value );
  void   pop_back( void );

  class PushPopGuard
  {
  public:
    PushPopGuard( Frame *frame, const String &identifier, const Value &value=Value() );
    ~PushPopGuard( void );
    Value *get( void ) const;
  private:
    Frame *const m_frame;
    Value *const m_value;
  };

  // Load from the directory associated with this Frame
  Value *load( const String &identifier, const Scope *scope );

  index_t getSize( void ) const { return index_t( m_values.size() ); }

  typedef std::vector< Value > values_t;
  typedef std::vector< String > identifiers_t;

  class Iterator : public RPGML::Iterator< std::pair< String, Value > >
  {
    friend class Frame;
  private:
    Iterator( const values_t *values, const identifiers_t *identifiers )
    : m_values( values ), m_identifiers( identifiers ), m_i( 0 )
    {
      assert( m_values );
      assert( m_identifiers );
      assert( m_values->size() == m_identifiers->size() );
    }
  public:
    typedef std::pair< String, Value > Type;
    typedef RPGML::Iterator< Type > Base;
    virtual ~Iterator( void ) {}
    virtual bool done( void ) { return ( m_i >= m_values->size() ); }
    virtual void next( void ) { ++m_i; }
    virtual Type get( void ) { return std::make_pair( (*m_identifiers)[ m_i ], (*m_values)[ m_i ] ); }
    virtual CountPtr< Base > clone( void ) const { return new Iterator( (*this) ); }
  private:
    const values_t *const m_values;
    const identifiers_t *const m_identifiers;
    index_t m_i;
  };

  CountPtr< Iterator > getIterator( void ) const
  {
    return new Iterator( &m_values, &m_identifiers );
  }

  virtual void gc_clear( void );
  virtual void gc_getChildren( Children &children ) const;

private:

  Value *load_local ( const String &identifier, const Scope *scope );
  Value *load_global( const String &identifier, const Scope *scope );
  Value *load( const String &path, const String &identifier, const Scope *scope );

  values_t m_values;
  identifiers_t m_identifiers;
  CountPtr< Frame > m_parent; // might create a closure, points to where the lookup continues
  String m_identifier; // might be null for anonymous Maps
  String m_path; // if this map corresponds to a directory
  index_t m_depth; // determined at setParent(), checked there
  bool m_is_this; // whether lookup for "this" returns this Frame
};

} // namespace RPGML

#endif


#ifndef RPGML_Function_h
#define RPGML_Function_h

#include "GarbageCollector.h"
#include "Refcounted.h"
#include "Value.h"
#include "String.h"

namespace RPGML {

class Scope;
class Map;
class String;

class Function : public Collectable
{
public:
  class Arg;
  class Args;

  Function( GarbageCollector *gc, Map *parent, const Args *decl_args );
  virtual ~Function( void );

  Map *getParent( void ) const;
  const Args *getDecl( void ) const;

  //! Sets up call and calls call_impl()
  bool call( Scope *scope, Value &ret, const Args *call_args, index_t recursion_depth );

  //! "Bare" Function call, use on own risk directly
  virtual bool call_impl( Scope *scope, Value &ret, index_t n_args, const Value *args, index_t recursion_depth ) = 0;

  class Arg
  {
  public:
    Arg( void )
    {}

    Arg( const String &_identifier )
    : identifier( _identifier )
    {}

    Arg( const String &_identifier, const Value &_value )
    : identifier( _identifier )
    , value( _value )
    {}

    String identifier;
    Value value;
  };

  class Args : public Refcounted, public std::vector< Arg >
  {
  public:
    Args( size_t n=0 )
    : std::vector< Arg >( n )
    {}
    ~Args( void ) {}
  };

  virtual void gc_clear( void );
  virtual void getChildren( Children &children ) const;

private:
  void fill_args( Map &args, const Args &call_args );

  CountPtr< Map > m_parent;
  CountPtr< const Args > m_decl;
};

} // namespace RPGML

#endif


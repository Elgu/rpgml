#ifndef RPGML_Array_h
#define RPGML_Array_h

#include "GarbageCollector.h"
#include "Value.h"
#include "types.h"

namespace RPGML {

class Array : public Collectable
{
public:
  Array( GarbageCollector *gc );
  virtual ~Array( void );

  index_t size( void ) const;
  Array &resize( index_t new_size );

  Value       *set( index_t i, Value value );
  Value       *get( index_t i );
  const Value *get( index_t i ) const;
  Value       *append( Value value );

  void swap( Array &other );

  virtual void gc_clear( void );
  virtual CountPtr< Children > getChildren( void ) const;

  typedef Iterator< Value* > Elements;
  typedef Iterator< const Value* > ConstElements;
  CountPtr< Elements      > getElements( void );
  CountPtr< ConstElements > getElements( void ) const;
  CountPtr< ConstElements > getElementsConst( void ) const { return getElements(); }

private:
  typedef std::vector< Value > values_t;

  class _Children;
  class _Elements;
  class _ConstElements;
  friend class _Children;
  friend class _Elements;
  friend class _ConstElements;

  class _Children : public Children
  {
  public:
    _Children( const Array *array, index_t i=0 );
    virtual ~_Children( void );

    virtual bool done( void );
    virtual void next( void );
    virtual Type get( void );
    virtual CountPtr< Children > clone( void ) const;

  private:
    CountPtr< const Array > m_array;
    index_t m_i;
  };

  class _ConstElements : public ConstElements
  {
  public:
    _ConstElements( const Array *array, index_t i=0 );
    virtual ~_ConstElements( void );

    virtual bool done( void );
    virtual void next( void );
    virtual const Value *get( void );
    virtual CountPtr< ConstElements > clone( void ) const;

  private:
    CountPtr< const Array > m_array;
    index_t m_i;
  };

  class _Elements : public Elements
  {
  public:
    _Elements( Array *array, index_t i=0 );
    virtual ~_Elements( void );

    virtual bool done( void );
    virtual void next( void );
    virtual Value *get( void );
    virtual CountPtr< Elements > clone( void ) const;

  private:
    CountPtr< Array > m_array;
    index_t m_i;
  };

  values_t m_values;
};

} // namespace RPGML

namespace std {

inline
void swap( RPGML::Array &x1, RPGML::Array &x2 )
{
  x1.swap( x2 );
}

} // namespace std


#endif


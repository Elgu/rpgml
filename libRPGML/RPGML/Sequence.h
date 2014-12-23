#ifndef RPGML_Sequence_h
#define RPGML_Sequence_h

#include "GarbageCollector.h"
#include "Iterator.h"
#include "Value.h"

#include <ostream>

namespace RPGML {

class Sequence : public Collectable
{
public:
  Sequence( GarbageCollector *gc );
  virtual ~Sequence( void );

  virtual std::ostream &print( std::ostream &o ) const = 0;

  typedef Iterator< Value > Iter;

  virtual CountPtr< Sequence > clone( void ) const = 0;
  virtual CountPtr< Iter > getIter( void ) const = 0;
private:
};

CountPtr< Sequence > genSequenceFromToStep( GarbageCollector *gc, int from, int to, int step );
CountPtr< Sequence > genSequenceFromToStep( GarbageCollector *gc, float from, float to, float step );

class SequenceValueArray : public Sequence
{
public:
  SequenceValueArray( GarbageCollector *gc, const ArrayBase *array );
  virtual ~SequenceValueArray( void );

  virtual std::ostream &print( std::ostream &o ) const;

  virtual void gc_clear( void );
  virtual void gc_getChildren( Children &children ) const;

  virtual CountPtr< Sequence > clone( void ) const;
  virtual CountPtr< Iter > getIter( void ) const;

private:
  class IteratorValueArray : public Iter
  {
  public:
    IteratorValueArray( const ArrayBase *array, index_t i=0 );
    virtual ~IteratorValueArray( void );
    virtual bool done( void ); // not const
    virtual void next( void );
    virtual Value get( void );
    virtual CountPtr< Iter > clone( void ) const;
  private:
    CountPtr< const ArrayBase > m_array;
    index_t m_i;
  };

  CountPtr< const ArrayBase > m_array;
};

} // namespace RPGML

#endif


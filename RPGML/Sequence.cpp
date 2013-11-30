#include "Sequence.h"

#include "Value.h"
#include "Array.h"

// #include <iostream>

#include <cassert>

namespace RPGML {

Sequence::Sequence( GarbageCollector *_gc )
: Collectable( _gc )
{}

Sequence::~Sequence( void )
{}

template< class _scalar_t >
class SequenceFromToStep : public Sequence
{
public:
  typedef _scalar_t scalar_t;

  SequenceFromToStep( GarbageCollector *_gc, scalar_t from, scalar_t to, scalar_t step )
  : Sequence( _gc )
  , m_from( from )
  , m_to( to )
  , m_step( step )
  {}

  virtual ~SequenceFromToStep( void )
  {}

  virtual std::ostream &print( std::ostream &o ) const
  {
    o
      << "( " << m_from
      << " to " << m_to
      ;

    if( m_step != scalar_t( 1 ) )
    {
      o << " step " << m_step;
    }

    o << " )";

    return o;
  }

  virtual void gc_clear( void )
  {}

  virtual void getChildren( Children & ) const
  {
    return;
  }

  virtual CountPtr< Sequence > clone( void ) const
  {
    return new SequenceFromToStep( getGC(), m_from, m_to, m_step );
  }

  virtual CountPtr< Iter > getIter( void ) const
  {
    return new IteratorFromToStep( m_from, m_to, m_step );
  }

private:
  class IteratorFromToStep : public Iter
  {
  public:
    IteratorFromToStep( scalar_t curr, scalar_t to, scalar_t step )
    : m_curr( curr )
    , m_to( to )
    , m_step( step )
    {
//      std::cerr << "IteratorFromToStep( " << curr << ", " << to << ", " << step << " )" << std::endl;
      assert( step != 0 );
    }

    virtual ~IteratorFromToStep( void )
    {}

    virtual bool done( void )
    {
      if( m_step > scalar_t( 0 ) )
      {
        return m_curr > m_to;
      }
      else
      {
        return m_curr < m_to;
      }
    }

    virtual void next( void )
    {
      m_curr += m_step;
    }

    virtual const Value *get( void )
    {
      m_value.set( m_curr );
      return &m_value;
    }

    virtual CountPtr< Iter > clone( void ) const
    {
      return new IteratorFromToStep( m_curr, m_to, m_step );
    }

  private:
    Value m_value;
    scalar_t m_curr;
    scalar_t m_to;
    scalar_t m_step;
  };

  scalar_t m_from;
  scalar_t m_to;
  scalar_t m_step;
};

CountPtr< Sequence > genSequenceFromToStep( GarbageCollector *gc, int from, int to, int step )
{
  return new SequenceFromToStep< int >( gc, from, to, step );
}

CountPtr< Sequence > genSequenceFromToStep( GarbageCollector *gc, float from, float to, float step )
{
  return new SequenceFromToStep< float >( gc, from, to, step );
}

SequenceValueArray::SequenceValueArray( GarbageCollector *_gc, const Array *array )
: Sequence( _gc )
, m_array( array )
{}

SequenceValueArray::~SequenceValueArray( void )
{}

std::ostream &SequenceValueArray::print( std::ostream &o ) const
{
  o << "[ ";

  bool first = true;
  for( CountPtr< Iter > i( getIter() ); !i->done(); i->next() )
  {
    if( !first ) o << ", "; else first = false;
    const Value *const v = i->get();
    o << (*v);
  }

  o << " ]";

  return o;
}

void SequenceValueArray::gc_clear( void )
{}

void SequenceValueArray::getChildren( Children &children ) const
{
  children.push_back( m_array );
}

CountPtr< Sequence > SequenceValueArray::clone( void ) const
{
  return new SequenceValueArray( (*this) );
}

CountPtr< Sequence::Iter > SequenceValueArray::getIter( void ) const
{
  return m_array->getElementsConst();
}

SequenceValueArray::IteratorValueArray
::IteratorValueArray( const Array *array, index_t i )
: m_array( array )
, m_i( i )
{}

SequenceValueArray::IteratorValueArray
::~IteratorValueArray( void )
{}

bool SequenceValueArray::IteratorValueArray
::done( void )
{
  return m_i >= m_array->size();
}

void SequenceValueArray::IteratorValueArray
::next( void )
{
  ++m_i;
}

const Value *SequenceValueArray::IteratorValueArray
::get( void )
{
  return m_array->get( m_i );
}

CountPtr< Sequence::Iter > SequenceValueArray::IteratorValueArray
::clone( void ) const
{
  return new IteratorValueArray( (*this) );
}

} // namespace RPGML


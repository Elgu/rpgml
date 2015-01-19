/* This file is part of RPGML.
 *
 * Copyright (c) 2014, Gunnar Payer, All rights reserved.
 *
 * RPGML is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 3.0 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library.
 */
#include "Sequence.h"

#include "Value.h"
#include "Array.h"

// #include <iostream>

#include <cassert>

namespace RPGML {

namespace Sequence_impl {

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

    virtual void gc_getChildren( Children & ) const
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

    index_t length( void ) const
    {
      // dum version
      index_t ret = 0;
      for( CountPtr< Iter > i( getIter() ); !i->done(); i->next() )
      {
        ++ret;
      }
      return ret;
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
        if( step == 0 ) throw Exception() << "step must not be 0";
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
        // TODO: check, id m_step is in range, so no overflow can occur
        m_curr = scalar_t( m_curr + m_step );
      }

      virtual Value get( void )
      {
        return Value( m_curr );
      }

      virtual CountPtr< Iter > clone( void ) const
      {
        return new IteratorFromToStep( m_curr, m_to, m_step );
      }

    private:
      scalar_t m_curr;
      scalar_t m_to;
      scalar_t m_step;
    };

    scalar_t m_from;
    scalar_t m_to;
    scalar_t m_step;
  };

} //  namespace Sequence_impl

using namespace Sequence_impl;

Sequence::Sequence( GarbageCollector *_gc )
: Collectable( _gc )
{}

Sequence::~Sequence( void )
{}

SequenceValueArray::SequenceValueArray( GarbageCollector *_gc, const ArrayBase *array )
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
    o << i->get();
  }

  o << " ]";

  return o;
}

void SequenceValueArray::gc_clear( void )
{}

void SequenceValueArray::gc_getChildren( Children &children ) const
{
  children.add( m_array.get() );
}

CountPtr< Sequence > SequenceValueArray::clone( void ) const
{
  return new SequenceValueArray( (*this) );
}

CountPtr< Sequence::Iter > SequenceValueArray::getIter( void ) const
{
  return m_array->getValueIterator();
}

index_t SequenceValueArray::length( void ) const
{
  return m_array->size();
}

SequenceValueArray::IteratorValueArray
::IteratorValueArray( const ArrayBase *array, index_t i )
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

Value SequenceValueArray::IteratorValueArray
::get( void )
{
  return m_array->getValue( m_i );
}

CountPtr< Sequence::Iter > SequenceValueArray::IteratorValueArray
::clone( void ) const
{
  return new IteratorValueArray( (*this) );
}

CountPtr< Sequence > genSequenceFromToStep( GarbageCollector *gc, const Value &from, const Value &to, const Value &step )
{
  const Type type = Type::Ret( Type::Ret( from.getType(), to.getType() ), step.getType() );

  const Value f = from.to( type );
  const Value t = to  .to( type );
  const Value s = step.to( type );

  switch( type.getEnum() )
  {
    case Type::BOOL  : return new SequenceFromToStep< bool     >( gc, f.get< bool     >(), t.get< bool     >(), s.get< bool     >() );
    case Type::UINT8 : return new SequenceFromToStep< uint8_t  >( gc, f.get< uint8_t  >(), t.get< uint8_t  >(), s.get< uint8_t  >() );
    case Type::INT8  : return new SequenceFromToStep< int8_t   >( gc, f.get< int8_t   >(), t.get< int8_t   >(), s.get< int8_t   >() );
    case Type::UINT16: return new SequenceFromToStep< uint16_t >( gc, f.get< uint16_t >(), t.get< uint16_t >(), s.get< uint16_t >() );
    case Type::INT16 : return new SequenceFromToStep< int16_t  >( gc, f.get< int16_t  >(), t.get< int16_t  >(), s.get< int16_t  >() );
    case Type::UINT32: return new SequenceFromToStep< uint32_t >( gc, f.get< uint32_t >(), t.get< uint32_t >(), s.get< uint32_t >() );
    case Type::INT32 : return new SequenceFromToStep< int32_t  >( gc, f.get< int32_t  >(), t.get< int32_t  >(), s.get< int32_t  >() );
    case Type::UINT64: return new SequenceFromToStep< uint64_t >( gc, f.get< uint64_t >(), t.get< uint64_t >(), s.get< uint64_t >() );
    case Type::INT64 : return new SequenceFromToStep< int64_t  >( gc, f.get< int64_t  >(), t.get< int64_t  >(), s.get< int64_t  >() );
    case Type::FLOAT : return new SequenceFromToStep< float    >( gc, f.get< float    >(), t.get< float    >(), s.get< float    >() );
    case Type::DOUBLE: return new SequenceFromToStep< double   >( gc, f.get< double   >(), t.get< double   >(), s.get< double   >() );
    default:
      throw Exception()
        << "Types for getSequenceFromToStep() must be scalar"
        ;
  }
}

} // namespace RPGML


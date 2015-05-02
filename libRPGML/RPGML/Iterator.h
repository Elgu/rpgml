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
#ifndef RPGML_Iterator_h
#define RPGML_Iterator_h

#include "Refcounted.h"
#include "GarbageCollector.h"
#include "types.h"

#include <vector>
#include <cassert>

namespace RPGML {

template< class _Type >
class IteratorBase
{
public:
  typedef _Type Type;

  IteratorBase( void ) {}
  virtual ~IteratorBase( void ) {}

  virtual bool done( void ) = 0; // not const
  virtual void next( void ) = 0;
  virtual Type get( void ) = 0;
};

template< class Iterator >
class PseudoContainer
{
public:
   class iterator : public std::iterator< std::input_iterator_tag, typename Iterator::Type >
   {
     typedef std::iterator< std::input_iterator_tag, typename Iterator::Type > Base;
   public:
     typedef typename Base::value_type value_type;
     typedef typename Base::reference reference;
     typedef typename Base::pointer pointer;

     iterator( void )
     : m_p( nullptr )
     {}

   private:
     friend class PseudoContainer;

     explicit
     iterator( const PseudoContainer *p )
     : m_p( p )
     {}

     iterator( const PseudoContainer *p, const CountPtr< Iterator > &iter )
     : m_p( p )
     , m_iter( iter->clone() )
     {}

   public:
     iterator( const iterator &other )
     : m_p( other.m_p )
     , m_iter( !other.m_iter.isNull() ? other.m_iter->clone() : CountPtr< Iterator >() )
     {}

     ~iterator( void ) {}

     void swap( iterator &other )
     {
       std::swap( m_p, other.m_p );
       m_iter.swap( other.m_iter );
     }

     iterator &operator=( const iterator &other )
     {
       iterator tmp( other );
       swap( tmp );
       return (*this);
     }

     iterator &operator++( void )
     {
       if( m_iter.isNull() ) throw Exception() << "Cannot increment an end-iterator.";
       m_iter->next();
       return (*this);
     }

     iterator operator++( int )
     {
       if( m_iter.isNull() ) throw Exception() << "Cannot increment an end-iterator.";
       iterator ret( m_p, m_iter->clone() );
       ++(*this);
       return ret;
     }

     bool operator==( const iterator &other ) const
     {
       if( m_p != other.m_p )
       {
         throw Exception() << "Iterators do not belong to the same container.";
       }

       if( !m_iter.isNull() )
       {
         return m_iter->done();
       }

       if( !other.m_iter.isNull() )
       {
         return other.m_iter->done();
       }

       return true; // both end-iterators
     }

     bool operator!=( const iterator &other ) const
     {
       return !( (*this) == other );
     }

     value_type operator*( void ) const
     {
       return m_iter->get();
     }

     pointer operator->( void ) const
     {
       return &m_iter->get();
     }

   private:
     const PseudoContainer *m_p;
     CountPtr< Iterator > m_iter;
   };

   explicit
   PseudoContainer( const CountPtr< const Iterator > &iter )
   : m_iter( iter )
   {}

   iterator begin( void ) const
   {
     return iterator( this, m_iter->clone() );
   }

   iterator end( void ) const
   {
     return iterator( this );
   }

private:
   CountPtr< const Iterator > m_iter;
};

template< class _Type >
class Iterator : public IteratorBase< _Type >, public Refcounted
{
  typedef IteratorBase< _Type > Base;
public:
  typedef typename Base::Type Type;

  Iterator( void ) {}
  virtual ~Iterator( void ) {}

  virtual CountPtr< Iterator > clone( void ) const = 0;

  PseudoContainer< Iterator > in( void ) const
  {
    return PseudoContainer< Iterator >( this );
  }
};

template< class _Type >
class GCIterator : public IteratorBase< _Type >, public Collectable
{
  typedef IteratorBase< _Type > Base;
public:
  typedef typename Base::Type Type;

  explicit
  GCIterator( GarbageCollector *_gc ) : Collectable( _gc ) {}
  virtual ~GCIterator( void ) {}

  virtual CountPtr< GCIterator > clone( void ) const = 0;

  PseudoContainer< GCIterator > in( void ) const
  {
    return PseudoContainer< GCIterator >( this );
  }
};

template< class BaseIterator >
class NullIterator : public BaseIterator
{
public:
  typedef typename BaseIterator::Type Type;
  NullIterator( void ) {}
  virtual ~NullIterator( void ) {}
  virtual bool done( void ) { return true; }
  virtual void next( void ) {}
  virtual Type get( void ) { return Type(); }
  virtual CountPtr< BaseIterator > clone( void ) const { return new NullIterator(); }
};

template< class BaseIterator >
class MultiIterator : public BaseIterator
{
public:
  typedef typename BaseIterator::Type Type;
  MultiIterator( void )
  : i( 0 )
  {}

  explicit
  MultiIterator( BaseIterator *i1, BaseIterator *i2=0, BaseIterator *i3=0, BaseIterator *i4=0 )
  : i( 0 )
  {
    if( i1 ) iters.push_back( i1 );
    if( i2 ) iters.push_back( i2 );
    if( i3 ) iters.push_back( i3 );
    if( i4 ) iters.push_back( i4 );
    find_beginning();
  }

  explicit
  MultiIterator( const BaseIterator *i1, const BaseIterator *i2=0, const BaseIterator *i3=0, const BaseIterator *i4=0 )
  : i( 0 )
  {
    if( i1 ) iters.push_back( i1->clone() );
    if( i2 ) iters.push_back( i2->clone() );
    if( i3 ) iters.push_back( i3->clone() );
    if( i4 ) iters.push_back( i4->clone() );
    find_beginning();
  }

  template< class _BeginEndIterator >
  MultiIterator( _BeginEndIterator begin, const _BeginEndIterator &end, index_t _i=0 )
  : i( _i )
  {
    for( ; begin != end; ++begin )
    {
      const BaseIterator *const iter = (*begin);
      if( iter ) iters.push_back( iter->clone() );
    }
    find_beginning();
  }

  MultiIterator( const Iterator< const BaseIterator* > *_iteriter )
  {
    CountPtr< Iterator< const BaseIterator* > > iteriter( _iteriter->clone() );
    for( ; !iteriter->done(); iteriter->next() )
    {
      const BaseIterator *const iter = iteriter->get();
      if( iter ) iters.push_back( iter->clone() );
    }
    find_beginning();
  }

  virtual ~MultiIterator( void ) {}

  virtual bool done( void )
  {
    return i >= iters.size();
  }

  virtual void next( void )
  {
    if( i < iters.size() )
    {
      iters[ i ]->next();
      find_beginning();
    }
  }

  virtual Type get( void )
  {
    return iters[ i ]->get();
  }

  virtual CountPtr< BaseIterator > clone( void ) const
  {
    if( i < iters.size() )
    {
      return new MultiIterator( &iters[ i ], &iters[ iters.size() ], 0 );
    }
    else
    {
      return new MultiIterator();
    }
  }

private:
  void find_beginning( void )
  {
    while( i < iters.size() && iters[ i ]->done() ) ++i;
  }

  std::vector< CountPtr< BaseIterator > > iters;
  index_t i;
};

template< class BaseIterator >
class ItemIterator : public BaseIterator
{
public:
  typedef typename BaseIterator::Type Type;
  ItemIterator( void )
  : m_i( 0 )
  {}

  virtual ~ItemIterator( void ) {}

  explicit
  ItemIterator( const Type &item1 )
  : m_i( 0 )
  {
    m_items.reserve( 1 );
    m_items.push_back( item1 );
  }

  explicit
  ItemIterator( const Type &item1, const Type &item2 )
  : m_i( 0 )
  {
    m_items.reserve( 2 );
    m_items.push_back( item1 );
    m_items.push_back( item2 );
  }

  explicit
  ItemIterator( const Type &item1, const Type &item2, const Type &item3 )
  : m_i( 0 )
  {
    m_items.reserve( 3 );
    m_items.push_back( item1 );
    m_items.push_back( item2 );
    m_items.push_back( item3 );
  }

  explicit
  ItemIterator( const Type &item1, const Type &item2, const Type &item3, const Type &item4 )
  : m_i( 0 )
  {
    m_items.reserve( 4 );
    m_items.push_back( item1 );
    m_items.push_back( item2 );
    m_items.push_back( item3 );
    m_items.push_back( item4 );
  }

  template< class item_iterator >
  ItemIterator( const item_iterator &items_begin, const item_iterator &items_end, size_t reserve=0 )
  : m_i( 0 )
  {
    m_items.reserve( reserve );
    for( item_iterator i( items_begin ); i != items_end; ++i )
    {
      m_items.push_back( (*i) );
    }
  }

  virtual bool done( void ) { return m_i >= m_items.size(); }
  virtual void next( void ) { ++m_i; }
  virtual Type get( void ) { return m_items[ m_i ]; }
  virtual CountPtr< BaseIterator > clone( void ) const
  {
    if( m_i < m_items.size() )
    {
      const size_t n = m_items.size() - m_i;
      const Type *const items_begin = &m_items[ m_i ];
      const Type *const items_end = items_begin + n;
      return new ItemIterator( items_begin, items_end, n );
    }
    else
    {
      return new NullIterator< BaseIterator >();
    }
  }

private:
  std::vector< Type > m_items;
  index_t m_i;
};

template< class BaseIterator, class iterator >
class BeginEndIterator : public BaseIterator
{
public:
  typedef typename BaseIterator::Type Type;
  BeginEndIterator( const iterator &begin, const iterator &end )
  : m_curr( begin )
  , m_end( end )
  {}
  virtual bool done( void ) { return !( m_curr != m_end ); }
  virtual void next( void ) { ++m_curr; }
  virtual Type get( void ) { return (*m_curr); }
  virtual CountPtr< BaseIterator > clone( void ) const { return new BeginEndIterator( (*this) ); }
private:
  iterator m_curr;
  const iterator m_end;
};

} // namespace RPGML

#endif


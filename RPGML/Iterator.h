#ifndef RPGML_Iterator_h
#define RPGML_Iterator_h

#include "Refcounted.h"
#include "types.h"

#include <vector>
#include <cassert>

namespace RPGML {

template< class _Type >
class Iterator : public Refcounted
{
public:
  typedef _Type Type;

  Iterator( void ) {}
  virtual ~Iterator( void ) {}

  virtual bool done( void ) = 0; // not const
  virtual void next( void ) = 0;
  virtual Type get( void ) = 0;

  virtual CountPtr< Iterator > clone( void ) const = 0;

  class NullIterator : public Iterator
  {
  public:
    NullIterator( void ) {}
    virtual ~NullIterator( void ) {}
    virtual bool done( void ) { return true; }
    virtual void next( void ) {}
    virtual Type get( void ) { return Type(); }
    virtual CountPtr< Iterator > clone( void ) const { return new NullIterator(); }
  };

  class MultiIterator : public Iterator
  {
  public:
    MultiIterator( void )
    : i( 0 )
    {}

    explicit
    MultiIterator( Iterator *i1, Iterator *i2=0, Iterator *i3=0, Iterator *i4=0 )
    : i( 0 )
    {
      if( i1 ) iters.push_back( i1 );
      if( i2 ) iters.push_back( i2 );
      if( i3 ) iters.push_back( i3 );
      if( i4 ) iters.push_back( i4 );
      find_beginning();
    }

    explicit
    MultiIterator( const Iterator *i1, const Iterator *i2=0, const Iterator *i3=0, const Iterator *i4=0 )
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
        const Iterator *const iter = (*begin);
        if( iter ) iters.push_back( iter->clone() );
      }
      find_beginning();
    }

    MultiIterator( const Iterator< const Iterator* > *_iteriter )
    {
      CountPtr< Iterator< const Iterator* > > iteriter( _iteriter->clone() );
      for( ; !iteriter->done(); iteriter->next() )
      {
        const Iterator *const iter = iteriter->get();
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

    virtual CountPtr< Iterator > clone( void ) const
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

    std::vector< CountPtr< Iterator > > iters;
    index_t i;
  };

  template< int N >
  class ItemIterator : public Iterator
  {
  public:
    ItemIterator( void )
    : m_i( 0 )
    {
      rpgml_static_assert( N == 0 );
    }

    virtual ~ItemIterator( void ) {}

    explicit
    ItemIterator( const Type &item1 )
    : m_i( 0 )
    {
      rpgml_static_assert( N == 1 );
      m_items[ 0 ] = item1;
    }

    explicit
    ItemIterator( const Type &item1, const Type &item2 )
    : m_i( 0 )
    {
      rpgml_static_assert( N == 2 );
      m_items[ 0 ] = item1;
      m_items[ 1 ] = item2;
    }

    explicit
    ItemIterator( const Type &item1, const Type &item2, const Type &item3 )
    : m_i( 0 )
    {
      rpgml_static_assert( N == 3 );
      m_items[ 0 ] = item1;
      m_items[ 1 ] = item2;
      m_items[ 2 ] = item3;
    }

    explicit
    ItemIterator( const Type &item1, const Type &item2, const Type &item3, const Type &item4 )
    : m_i( 0 )
    {
      rpgml_static_assert( N == 4 );
      m_items[ 0 ] = item1;
      m_items[ 1 ] = item2;
      m_items[ 2 ] = item3;
      m_items[ 3 ] = item4;
    }

//    explicit
//    ItemIterator( const std::vector< Type > &items, index_t i=0 )
//    : m_items( items )
//    , m_i( i )
//    {
//      assert( items.size() == N );
//      for( index_t j=i; j<N; ++j )
//      {
//        m_items[ j ] = items[ j ];
//      }
//    }

    virtual bool done( void ) { return m_i >= N; }
    virtual void next( void ) { ++m_i; }
    virtual Type get( void ) { return m_items[ m_i ]; }
    virtual CountPtr< Iterator > clone( void ) const { return new ItemIterator( *this ); }

  private:
    Type m_items[ N ];
    index_t m_i;
  };

  template< class iterator >
  class BeginEndIterator : public Iterator
  {
  public:
    BeginEndIterator( const iterator &begin, const iterator &end )
    : m_curr( begin )
    , m_end( end )
    {}
    virtual bool done( void ) { return !( m_curr != m_end ); }
    virtual void next( void ) { ++m_curr; }
    virtual Type get( void ) { return (*m_curr); }
    virtual CountPtr< Iterator > clone( void ) const { return new BeginEndIterator( (*this) ); }
  private:
    iterator m_curr;
    const iterator m_end;
  };

};

} // namespace RPGML

#endif


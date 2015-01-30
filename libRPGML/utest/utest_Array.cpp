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
#include <cppunit/extensions/HelperMacros.h>

#include <RPGML/Array.h>
#include <RPGML/Array2.h>

#include <iostream>
#include <cstdlib>

using namespace RPGML;
using namespace std;

class utest_Array : public CppUnit::TestFixture
{
  CPPUNIT_TEST_SUITE( utest_Array );

  CPPUNIT_TEST( test_Coordinates );
  CPPUNIT_TEST( test_isCollectable );
  CPPUNIT_TEST( test_gc );
  CPPUNIT_TEST( test_getType );
  CPPUNIT_TEST( test_resize );
  CPPUNIT_TEST( test_CoordinatesIterator );
  CPPUNIT_TEST( test_getAs );
  CPPUNIT_TEST( test_iterator );
  CPPUNIT_TEST( test_getROI );
  CPPUNIT_TEST( test_reserve );
  CPPUNIT_TEST( test_mirror_sparse_rotate );
  CPPUNIT_TEST( test_BoolContainer );
  CPPUNIT_TEST( test_Array2_bool );

  CPPUNIT_TEST_SUITE_END();

public:
  void setUp() {}
  void tearDown() {}

  void test_Coordinates( void )
  {
    index_t const c1[ 4 ] = { 3, 4, 5, 6 };
    index_t const c2[ 4 ] = { 2, 3, 4, 5 };
    index_t const c3[ 4 ] = { 4, 5, 6, 7 };

    {
      ArrayBase::Coordinates coord1( 0, c1 );
      CPPUNIT_ASSERT_EQUAL( 0, coord1.getDims() );
      CPPUNIT_ASSERT_EQUAL( (const index_t*)0, coord1.getCoords() );
    }

    {
      ArrayBase::Coordinates coord1( 3, c1 );
      ArrayBase::Coordinates coord2( 3, c2+1 );
      ArrayBase::Coordinates coord3( 4, c1 );
      ArrayBase::Coordinates coord4( 3, c1+1 );
      ArrayBase::Coordinates coord5( 4, c3   );

      CPPUNIT_ASSERT_EQUAL( 3, coord1.getDims() );
      CPPUNIT_ASSERT_EQUAL( 3, coord2.getDims() );
      CPPUNIT_ASSERT_EQUAL( &c1[ 0 ], coord1.getCoords() );
      CPPUNIT_ASSERT_EQUAL( &c2[ 1 ], coord2.getCoords() );
      CPPUNIT_ASSERT_EQUAL( &c1[ 0 ], static_cast< const index_t* >( coord1 ) );
      CPPUNIT_ASSERT_EQUAL( &c2[ 1 ], static_cast< const index_t* >( coord2 ) );

      CPPUNIT_ASSERT_EQUAL( index_t( 3 ), coord1[ 0 ] );
      CPPUNIT_ASSERT_EQUAL( index_t( 4 ), coord1[ 1 ] );
      CPPUNIT_ASSERT_EQUAL( index_t( 5 ), coord1[ 2 ] );

      CPPUNIT_ASSERT_EQUAL( true , coord1 == coord2 );
      CPPUNIT_ASSERT_EQUAL( false, coord1 != coord2 );

      CPPUNIT_ASSERT_EQUAL( false, coord1 == coord3 );
      CPPUNIT_ASSERT_EQUAL( true , coord1 != coord3 );

      CPPUNIT_ASSERT_EQUAL( false, coord1 == coord4 );
      CPPUNIT_ASSERT_EQUAL( true , coord1 != coord4 );

      CPPUNIT_ASSERT_EQUAL( false, coord1 == coord5 );
      CPPUNIT_ASSERT_EQUAL( true , coord1 != coord5 );

      CPPUNIT_ASSERT_EQUAL( false, coord2 == coord3 );
      CPPUNIT_ASSERT_EQUAL( true , coord2 != coord3 );

      CPPUNIT_ASSERT_EQUAL( false, coord2 == coord4 );
      CPPUNIT_ASSERT_EQUAL( true , coord2 != coord4 );

      CPPUNIT_ASSERT_EQUAL( false, coord2 == coord5 );
      CPPUNIT_ASSERT_EQUAL( true , coord2 != coord5 );

      CPPUNIT_ASSERT_EQUAL( false, coord3 == coord4 );
      CPPUNIT_ASSERT_EQUAL( true , coord3 != coord4 );

      CPPUNIT_ASSERT_EQUAL( false, coord3 == coord5 );
      CPPUNIT_ASSERT_EQUAL( true , coord3 != coord5 );

      CPPUNIT_ASSERT_EQUAL( false, coord4 == coord5 );
      CPPUNIT_ASSERT_EQUAL( true , coord4 != coord5 );
    }
  }

  void test_isCollectable( void )
  {
    Collectable *dummy1 = 0;
    ArrayBase *dummy2 = 0;
    Array< int, 0 > *dummy3 = 0;
    Array2< int > *dummy4 = 0;
    CPPUNIT_ASSERT_EQUAL( true, isCollectable( dummy1 ) );
    CPPUNIT_ASSERT_EQUAL( true, isCollectable( dummy2 ) );
    CPPUNIT_ASSERT_EQUAL( true, isCollectable( dummy3 ) );
    CPPUNIT_ASSERT_EQUAL( true, isCollectable( dummy4 ) );
  }

  void test_getType( void )
  {
    GarbageCollector *gc = 0;
    {
    Array< int, 2 > i2( gc );
    Array< float, 1 > f1( gc );
    Array< String, 0 > s0( gc );
    CPPUNIT_ASSERT_EQUAL( index_t( 2 ), i2.getDims() );
    CPPUNIT_ASSERT_EQUAL( index_t( 1 ), f1.getDims() );
    CPPUNIT_ASSERT_EQUAL( index_t( 0 ), s0.getDims() );

    CPPUNIT_ASSERT_EQUAL( Type::Int(), i2.getType() );
    CPPUNIT_ASSERT_EQUAL( Type::Float(), f1.getType() );
    CPPUNIT_ASSERT_EQUAL( Type::String(), s0.getType() );
    }

    {
    Array2< int > i2( gc, 2 );
    Array2< float > f1( gc, 1 );
    Array2< String > s0( gc, 0 );
    CPPUNIT_ASSERT_EQUAL( index_t( 2 ), i2.getDims() );
    CPPUNIT_ASSERT_EQUAL( index_t( 1 ), f1.getDims() );
    CPPUNIT_ASSERT_EQUAL( index_t( 0 ), s0.getDims() );

    CPPUNIT_ASSERT_EQUAL( Type::Int(), i2.getType() );
    CPPUNIT_ASSERT_EQUAL( Type::Float(), f1.getType() );
    CPPUNIT_ASSERT_EQUAL( Type::String(), s0.getType() );
    }
  }

  void test_CoordinatesIterator( void )
  {
    {
    GarbageCollector gc;
    CountPtr< Array< int, 2 > > a = new Array< int, 2 >( &gc, 2, 3 );

    CountPtr< ArrayBase::CoordinatesIterator > coord( a->getCoordinatesIterator() );

    CPPUNIT_ASSERT_EQUAL( false, coord->done() );
    CPPUNIT_ASSERT_EQUAL( index_t( 0 ), coord->get()[ 0 ] );
    CPPUNIT_ASSERT_EQUAL( index_t( 0 ), coord->get()[ 1 ] );

    CPPUNIT_ASSERT_NO_THROW( coord->next() );

    CPPUNIT_ASSERT_EQUAL( false, coord->done() );
    CPPUNIT_ASSERT_EQUAL( index_t( 1 ), coord->get()[ 0 ] );
    CPPUNIT_ASSERT_EQUAL( index_t( 0 ), coord->get()[ 1 ] );

    CPPUNIT_ASSERT_NO_THROW( coord->next() );

    CPPUNIT_ASSERT_EQUAL( false, coord->done() );
    CPPUNIT_ASSERT_EQUAL( index_t( 0 ), coord->get()[ 0 ] );
    CPPUNIT_ASSERT_EQUAL( index_t( 1 ), coord->get()[ 1 ] );

    CPPUNIT_ASSERT_NO_THROW( coord->next() );

    CPPUNIT_ASSERT_EQUAL( false, coord->done() );
    CPPUNIT_ASSERT_EQUAL( index_t( 1 ), coord->get()[ 0 ] );
    CPPUNIT_ASSERT_EQUAL( index_t( 1 ), coord->get()[ 1 ] );

    CPPUNIT_ASSERT_NO_THROW( coord->next() );

    CPPUNIT_ASSERT_EQUAL( false, coord->done() );
    CPPUNIT_ASSERT_EQUAL( index_t( 0 ), coord->get()[ 0 ] );
    CPPUNIT_ASSERT_EQUAL( index_t( 2 ), coord->get()[ 1 ] );

    CPPUNIT_ASSERT_NO_THROW( coord->next() );

    CPPUNIT_ASSERT_EQUAL( false, coord->done() );
    CPPUNIT_ASSERT_EQUAL( index_t( 1 ), coord->get()[ 0 ] );
    CPPUNIT_ASSERT_EQUAL( index_t( 2 ), coord->get()[ 1 ] );

    CPPUNIT_ASSERT_NO_THROW( coord->next() );
    CPPUNIT_ASSERT_EQUAL( true, coord->done() );
    }

    {
    GarbageCollector gc;
    CountPtr< Array2< int > > a = new Array2< int >( &gc, 2, 2, 3 );

    CountPtr< ArrayBase::CoordinatesIterator > coord( a->getCoordinatesIterator() );

    CPPUNIT_ASSERT_EQUAL( false, coord->done() );
    CPPUNIT_ASSERT_EQUAL( index_t( 0 ), coord->get()[ 0 ] );
    CPPUNIT_ASSERT_EQUAL( index_t( 0 ), coord->get()[ 1 ] );

    CPPUNIT_ASSERT_NO_THROW( coord->next() );

    CPPUNIT_ASSERT_EQUAL( false, coord->done() );
    CPPUNIT_ASSERT_EQUAL( index_t( 1 ), coord->get()[ 0 ] );
    CPPUNIT_ASSERT_EQUAL( index_t( 0 ), coord->get()[ 1 ] );

    CPPUNIT_ASSERT_NO_THROW( coord->next() );

    CPPUNIT_ASSERT_EQUAL( false, coord->done() );
    CPPUNIT_ASSERT_EQUAL( index_t( 0 ), coord->get()[ 0 ] );
    CPPUNIT_ASSERT_EQUAL( index_t( 1 ), coord->get()[ 1 ] );

    CPPUNIT_ASSERT_NO_THROW( coord->next() );

    CPPUNIT_ASSERT_EQUAL( false, coord->done() );
    CPPUNIT_ASSERT_EQUAL( index_t( 1 ), coord->get()[ 0 ] );
    CPPUNIT_ASSERT_EQUAL( index_t( 1 ), coord->get()[ 1 ] );

    CPPUNIT_ASSERT_NO_THROW( coord->next() );

    CPPUNIT_ASSERT_EQUAL( false, coord->done() );
    CPPUNIT_ASSERT_EQUAL( index_t( 0 ), coord->get()[ 0 ] );
    CPPUNIT_ASSERT_EQUAL( index_t( 2 ), coord->get()[ 1 ] );

    CPPUNIT_ASSERT_NO_THROW( coord->next() );

    CPPUNIT_ASSERT_EQUAL( false, coord->done() );
    CPPUNIT_ASSERT_EQUAL( index_t( 1 ), coord->get()[ 0 ] );
    CPPUNIT_ASSERT_EQUAL( index_t( 2 ), coord->get()[ 1 ] );

    CPPUNIT_ASSERT_NO_THROW( coord->next() );
    CPPUNIT_ASSERT_EQUAL( true, coord->done() );
    }
  }

  void test_resize( void )
  {
    GarbageCollector *gc = 0;
    {
    Array< int, 2 > i2( gc );

    CPPUNIT_ASSERT_EQUAL( index_t( 0 ), i2.getSize()[ 0 ] );
    CPPUNIT_ASSERT_EQUAL( index_t( 0 ), i2.getSize()[ 1 ] );
    CPPUNIT_ASSERT_EQUAL( index_t( 0 ), i2.size() );

    i2.resize( 2, 3 );
    CPPUNIT_ASSERT_EQUAL( index_t( 2 ), i2.getSize()[ 0 ] );
    CPPUNIT_ASSERT_EQUAL( index_t( 3 ), i2.getSize()[ 1 ] );
    CPPUNIT_ASSERT_EQUAL( index_t( 6 ), i2.size() );

    const index_t size[] = { 5, 7 };
    i2.resize_v( 2, size );
    CPPUNIT_ASSERT_EQUAL( index_t( 5 ), i2.getSize()[ 0 ] );
    CPPUNIT_ASSERT_EQUAL( index_t( 7 ), i2.getSize()[ 1 ] );
    CPPUNIT_ASSERT_EQUAL( index_t( 35 ), i2.size() );

    const index_t size2[] = { 23, 42 };
    i2.resize( ArrayBase::Size( 2, size2 ) );
    CPPUNIT_ASSERT_EQUAL( index_t( 23 ), i2.getSize()[ 0 ] );
    CPPUNIT_ASSERT_EQUAL( index_t( 42 ), i2.getSize()[ 1 ] );
    CPPUNIT_ASSERT_EQUAL( index_t( 23*42 ), i2.size() );
    }

    {
    Array2< int > i2( gc, 2 );

    CPPUNIT_ASSERT_EQUAL( index_t( 0 ), i2.getSize()[ 0 ] );
    CPPUNIT_ASSERT_EQUAL( index_t( 0 ), i2.getSize()[ 1 ] );
    CPPUNIT_ASSERT_EQUAL( index_t( 0 ), i2.size() );

    i2.resize( 2, 3 );
    CPPUNIT_ASSERT_EQUAL( index_t( 2 ), i2.getSize()[ 0 ] );
    CPPUNIT_ASSERT_EQUAL( index_t( 3 ), i2.getSize()[ 1 ] );
    CPPUNIT_ASSERT_EQUAL( index_t( 6 ), i2.size() );

    const index_t size[] = { 5, 7 };
    i2.resize_v( 2, size );
    CPPUNIT_ASSERT_EQUAL( index_t( 5 ), i2.getSize()[ 0 ] );
    CPPUNIT_ASSERT_EQUAL( index_t( 7 ), i2.getSize()[ 1 ] );
    CPPUNIT_ASSERT_EQUAL( index_t( 35 ), i2.size() );

    const index_t size2[] = { 23, 42 };
    i2.resize( ArrayBase::Size( 2, size2 ) );
    CPPUNIT_ASSERT_EQUAL( index_t( 23 ), i2.getSize()[ 0 ] );
    CPPUNIT_ASSERT_EQUAL( index_t( 42 ), i2.getSize()[ 1 ] );
    CPPUNIT_ASSERT_EQUAL( index_t( 23*42 ), i2.size() );
    }
  }

  class Marker : public Collectable
  {
    typedef Collectable Base;
  public:
    explicit Marker( GarbageCollector *_gc, bool *_mark )
    : Collectable( _gc )
    , m_mark( _mark )
    {
      (*m_mark) = false;
    }
    virtual ~Marker( void )
    {
      (*m_mark) = true;
    }
    virtual void gc_clear( void ) {}
    virtual void gc_getChildren( Children & ) const {}
  private:
    bool *m_mark;
  };

  void test_gc( void )
  {
    {
    GarbageCollector gc;

    typedef Array< CountPtr< ArrayBase >, 1 > root_array_t;
    typedef Array< CountPtr< Marker >, 1 > marker_array_t;
    typedef Array< bool, 1 > bool_array_t;

    CountPtr< root_array_t > root( new root_array_t( &gc, 4 ) );
    CountPtr< marker_array_t > markers( new marker_array_t( &gc, 5 ) );
    CountPtr< bool_array_t > bool_array( new bool_array_t( &gc, 3 ) );

    bool m0 = false;
    bool m1 = false;
    bool m2 = false;

    markers->at( 0 ) = new Marker( &gc, &m0 );
    markers->at( 1 ) = new Marker( &gc, &m1 );
    markers->at( 2 ) = new Marker( &gc, &m2 );

    root->at( 1 ) = markers;
    root->at( 2 ) = markers;
    root->at( 3 ) = bool_array;

    gc.run();
    CPPUNIT_ASSERT_EQUAL( false, m0 );
    CPPUNIT_ASSERT_EQUAL( false, m1 );
    CPPUNIT_ASSERT_EQUAL( false, m2 );

    markers->at( 1 ).clear();
    gc.run();
    CPPUNIT_ASSERT_EQUAL( false, m0 );
    CPPUNIT_ASSERT_EQUAL( true , m1 );
    CPPUNIT_ASSERT_EQUAL( false, m2 );

    markers.clear();
    gc.run();
    CPPUNIT_ASSERT_EQUAL( false, m0 );
    CPPUNIT_ASSERT_EQUAL( true , m1 );
    CPPUNIT_ASSERT_EQUAL( false, m2 );

    root->at( 1 ).clear();
    gc.run();
    CPPUNIT_ASSERT_EQUAL( false, m0 );
    CPPUNIT_ASSERT_EQUAL( true , m1 );
    CPPUNIT_ASSERT_EQUAL( false, m2 );

    root->at( 2 ).clear();
    gc.run();
    CPPUNIT_ASSERT_EQUAL( true, m0 );
    CPPUNIT_ASSERT_EQUAL( true , m1 );
    CPPUNIT_ASSERT_EQUAL( true, m2 );
    }

    {
    GarbageCollector gc;

    typedef Array2< CountPtr< ArrayBase > > root_array_t;
    typedef Array2< CountPtr< Marker > > marker_array_t;
    typedef Array2< int8_t > bool_array_t;

    CountPtr< root_array_t > root( new root_array_t( &gc, 1, 4 ) );
    CountPtr< marker_array_t > markers( new marker_array_t( &gc, 1, 5 ) );
    CountPtr< bool_array_t > bool_array( new bool_array_t( &gc, 1, 3 ) );

    bool m0 = false;
    bool m1 = false;
    bool m2 = false;

    markers->at( 0 ) = new Marker( &gc, &m0 );
    markers->at( 1 ) = new Marker( &gc, &m1 );
    markers->at( 2 ) = new Marker( &gc, &m2 );

    root->at( 1 ) = markers;
    root->at( 2 ) = markers;
    root->at( 3 ) = bool_array;

    gc.run();
    CPPUNIT_ASSERT_EQUAL( false, m0 );
    CPPUNIT_ASSERT_EQUAL( false, m1 );
    CPPUNIT_ASSERT_EQUAL( false, m2 );

    markers->at( 1 ).clear();
    gc.run();
    CPPUNIT_ASSERT_EQUAL( false, m0 );
    CPPUNIT_ASSERT_EQUAL( true , m1 );
    CPPUNIT_ASSERT_EQUAL( false, m2 );

    markers.clear();
    gc.run();
    CPPUNIT_ASSERT_EQUAL( false, m0 );
    CPPUNIT_ASSERT_EQUAL( true , m1 );
    CPPUNIT_ASSERT_EQUAL( false, m2 );

    root->at( 1 ).clear();
    gc.run();
    CPPUNIT_ASSERT_EQUAL( false, m0 );
    CPPUNIT_ASSERT_EQUAL( true , m1 );
    CPPUNIT_ASSERT_EQUAL( false, m2 );

    root->at( 2 ).clear();
    gc.run();
    CPPUNIT_ASSERT_EQUAL( true, m0 );
    CPPUNIT_ASSERT_EQUAL( true , m1 );
    CPPUNIT_ASSERT_EQUAL( true, m2 );
    }
  }

  class Other : public Collectable
  {
    typedef Collectable Base;
  public:
    Other( GarbageCollector *_gc=0, int _i=0 )
    : Collectable( _gc )
    , i( _i )
    {}
    int i;
    virtual void gc_clear( void ) {}
    virtual void gc_getChildren( Children & ) const {}
  };

  template< class T >
  void test_getAs_t( void )
  {
    {
    GarbageCollector gc;

    typedef Array< T, 2 > TArray2D;
    typedef ArrayElements< T > TArrayElements;

    {
      CountPtr< TArray2D       > a = new TArray2D( &gc );
      CountPtr< TArrayElements > a_elements = a.get();
      CountPtr< ArrayBase      > a_base     = a.get();
      CountPtr< const TArray2D       > ac          = a.get();
      CountPtr< const TArrayElements > ac_elements = a.get();
      CountPtr< const ArrayBase      > ac_base     = a.get();

      {
        TArray2D *p = 0;
        CPPUNIT_ASSERT_EQUAL( a.get(), a         ->getAs( p ) );
        CPPUNIT_ASSERT_EQUAL( a.get(), a_elements->getAs( p ) );
        CPPUNIT_ASSERT_EQUAL( a.get(), a_base    ->getAs( p ) );

        const TArray2D *pc = 0;
        CPPUNIT_ASSERT_EQUAL( ac.get(), a         ->getAs( pc ) );
        CPPUNIT_ASSERT_EQUAL( ac.get(), a_elements->getAs( pc ) );
        CPPUNIT_ASSERT_EQUAL( ac.get(), a_base    ->getAs( pc ) );
      }

      {
        TArrayElements *p = 0;
        CPPUNIT_ASSERT_EQUAL( a_elements.get(), a         ->getAs( p ) );
        CPPUNIT_ASSERT_EQUAL( a_elements.get(), a_elements->getAs( p ) );
        CPPUNIT_ASSERT_EQUAL( a_elements.get(), a_base    ->getAs( p ) );

        const TArrayElements *pc = 0;
        CPPUNIT_ASSERT_EQUAL( ac_elements.get(), a         ->getAs( pc ) );
        CPPUNIT_ASSERT_EQUAL( ac_elements.get(), a_elements->getAs( pc ) );
        CPPUNIT_ASSERT_EQUAL( ac_elements.get(), a_base    ->getAs( pc ) );
      }

      {
        ArrayBase *p = 0;
        CPPUNIT_ASSERT_EQUAL( a_base.get(), a         ->getAs( p ) );
        CPPUNIT_ASSERT_EQUAL( a_base.get(), a_elements->getAs( p ) );
        CPPUNIT_ASSERT_EQUAL( a_base.get(), a_base    ->getAs( p ) );

        const ArrayBase *pc = 0;
        CPPUNIT_ASSERT_EQUAL( ac_base.get(), a         ->getAs( pc ) );
        CPPUNIT_ASSERT_EQUAL( ac_base.get(), a_elements->getAs( pc ) );
        CPPUNIT_ASSERT_EQUAL( ac_base.get(), a_base    ->getAs( pc ) );
      }
    }
    }

    {
    GarbageCollector gc;

    typedef Array2< T > TArray;

    {
      CountPtr< TArray         > a = new TArray( &gc, 2 );
      CountPtr< ArrayBase      > a_base     = a.get();
      CountPtr< const TArray       > ac          = a.get();
      CountPtr< const ArrayBase    > ac_base     = a.get();

      {
        TArray *p = 0;
        CPPUNIT_ASSERT_EQUAL( a.get(), a         ->getAs( p ) );
        CPPUNIT_ASSERT_EQUAL( a.get(), a_base    ->getAs( p ) );

        const TArray *pc = 0;
        CPPUNIT_ASSERT_EQUAL( ac.get(), a         ->getAs( pc ) );
        CPPUNIT_ASSERT_EQUAL( ac.get(), a_base    ->getAs( pc ) );
      }

      {
        ArrayBase *p = 0;
        CPPUNIT_ASSERT_EQUAL( a_base.get(), a         ->getAs( p ) );
        CPPUNIT_ASSERT_EQUAL( a_base.get(), a_base    ->getAs( p ) );

        const ArrayBase *pc = 0;
        CPPUNIT_ASSERT_EQUAL( ac_base.get(), a         ->getAs( pc ) );
        CPPUNIT_ASSERT_EQUAL( ac_base.get(), a_base    ->getAs( pc ) );
      }
    }
    }
  }

  void test_getAs( void )
  {
    test_getAs_t< int >();
    test_getAs_t< String >();
    test_getAs_t< CountPtr< ArrayBase > >();
    test_getAs_t< CountPtr< Other > >();
  }

  void test_iterator( void )
  {
    GarbageCollector *gc = 0;

    {
      Array2< int > a( gc, 0 );

      (*a) = 42;

      {
        Array2< int >::iterator iter = a.begin();
        Array2< int >::iterator end  = a.end();

        CPPUNIT_ASSERT( iter != end );
        CPPUNIT_ASSERT_EQUAL( 42, *iter );

        (*iter) += 100;

        ++iter;
        CPPUNIT_ASSERT( iter == end );
      }

      {
        Array2< int >::const_iterator iter = a.begin();
        Array2< int >::const_iterator end  = a.end();

        CPPUNIT_ASSERT( iter != end );
        CPPUNIT_ASSERT_EQUAL( 142, *iter );

        ++iter;
        CPPUNIT_ASSERT( iter == end );
      }

    }

    {
      Array2< int > a( gc, 2, 3, 5 );

      for( index_t y=0, i=0; y<5; ++y )
      for( index_t x=0     ; x<3; ++x, ++i )
      {
        a.at( x, y ) = i;
      }

      {
        Array2< int >::iterator iter = a.begin();
        Array2< int >::iterator end  = a.end();

        for( index_t y=0, i=0; y<5; ++y )
        for( index_t x=0     ; x<3; ++x, ++i, ++iter )
        {
          CPPUNIT_ASSERT( iter != end );
          CPPUNIT_ASSERT_EQUAL( int( i ), *iter );
          CPPUNIT_ASSERT_EQUAL( x, iter.getPos()[ 0 ] );
          CPPUNIT_ASSERT_EQUAL( y, iter.getPos()[ 1 ] );

          (*iter) += 100;
        }

        CPPUNIT_ASSERT( iter == end );
      }

      {
        Array2< int >::const_iterator iter = a.begin();
        Array2< int >::const_iterator end  = a.end();

        for( index_t y=0, i=0; y<5; ++y )
        for( index_t x=0     ; x<3; ++x, ++i, ++iter )
        {
          CPPUNIT_ASSERT( iter != end );
          CPPUNIT_ASSERT_EQUAL( int( i + 100 ), *iter );
          CPPUNIT_ASSERT_EQUAL( x, iter.getPos()[ 0 ] );
          CPPUNIT_ASSERT_EQUAL( y, iter.getPos()[ 1 ] );
        }

        CPPUNIT_ASSERT( iter == end );
      }
    }
  }

  void test_getROI( void )
  {
    GarbageCollector gc;
    typedef Array2< int > IntArray2;

    {
      CountPtr< IntArray2 > a = new IntArray2( &gc, 1, 5 );

      for( index_t x=0; x<5; ++x )
      {
        a->at( x ) = x;
      }

      // Whole array
      {
        const index_t x[ 1 ] = { 0 };
        const index_t s[ 1 ] = { 5 };
        CountPtr< const IntArray2 > roi = static_cast< const IntArray2* >( a.get() )->getROI( 1, x, s );

        CPPUNIT_ASSERT_EQUAL( a->getSize(), roi->getSize() );

        IntArray2::const_iterator a_i   = a->begin();
        IntArray2::const_iterator a_end = a->end();
        IntArray2::const_iterator roi_i   = roi->begin();
        IntArray2::const_iterator roi_end = roi->end();

        CPPUNIT_ASSERT( a_end == roi_end );

        for( ; a_i != a_end && roi_i == roi_end; ++a_i, ++roi_i )
        {
          CPPUNIT_ASSERT( a_i == roi_i );
        }
      }

      // Write partial array
      {
        const index_t x[ 1 ] = { 2 };
        const index_t s[ 1 ] = { 2 };
        CountPtr< IntArray2 > roi = a->getROI( 1, x, s );

        IntArray2::iterator a_i   = a->begin();
        IntArray2::iterator a_end = a->end();
        IntArray2::iterator roi_i   = roi->begin();
        IntArray2::iterator roi_end = roi->end();

        // Move a_i to the beginning of the roi
        for( int i=0; i<2; ++i ) { ++a_i; }
        CPPUNIT_ASSERT( a_i == roi_i );

        for( ; a_i != a_end && roi_i != roi_end; ++a_i, ++roi_i )
        {
          CPPUNIT_ASSERT( a_i == roi_i );
          (*roi_i) += 100;
        }
        CPPUNIT_ASSERT( roi_i == roi_end );
        CPPUNIT_ASSERT( a_i == roi_end );

        IntArray2::const_iterator a_j = a->begin();
        CPPUNIT_ASSERT_EQUAL(   0, *a_j++ ); CPPUNIT_ASSERT( a_j != a_end );
        CPPUNIT_ASSERT_EQUAL(   1, *a_j++ ); CPPUNIT_ASSERT( a_j != a_end );
        CPPUNIT_ASSERT_EQUAL( 102, *a_j++ ); CPPUNIT_ASSERT( a_j != a_end );
        CPPUNIT_ASSERT_EQUAL( 103, *a_j++ ); CPPUNIT_ASSERT( a_j != a_end );
        CPPUNIT_ASSERT_EQUAL(   4, *a_j++ ); CPPUNIT_ASSERT( a_j == a_end );
      }
    }

    {
      CountPtr< IntArray2 > a = new IntArray2( &gc, 2, 3, 5 );

      for( index_t y=0; y<5; ++y )
      for( index_t x=0; x<3; ++x )
      {
        a->at( x, y ) = x+y*3;
      }

      // Whole array
      {
        const index_t x[ 2 ] = { 0, 0 };
        const index_t s[ 2 ] = { 3, 5 };
        CountPtr< const IntArray2 > roi = static_cast< const IntArray2* >( a.get() )->getROI( 2, x, s );

        CPPUNIT_ASSERT_EQUAL( a->getSize(), roi->getSize() );

        IntArray2::const_iterator a_i   = a->begin();
        IntArray2::const_iterator a_end = a->end();
        IntArray2::const_iterator roi_i   = roi->begin();
        IntArray2::const_iterator roi_end = roi->end();

        CPPUNIT_ASSERT( a_end == roi_end );

        for( ; a_i != a_end && roi_i == roi_end; ++a_i, ++roi_i )
        {
          CPPUNIT_ASSERT( a_i == roi_i );
        }
      }

      // Partial write
      {
        const index_t x[ 2 ] = { 1, 2 };
        const index_t s[ 2 ] = { 2, 2 };
        CountPtr< IntArray2 > roi = a->getROI( 2, x, s );

        IntArray2::iterator roi_i   = roi->begin();
        IntArray2::iterator roi_end = roi->end();

        for( ; roi_i != roi_end; ++roi_i )
        {
          (*roi_i) += 100;
        }

        for( index_t y=0; y<5; ++y )
        for( index_t x=0; x<3; ++x )
        {
          if( x >= 1 && x < 3 && y >= 2 && y < 4 )
          {
            CPPUNIT_ASSERT_EQUAL( int( x+y*3 + 100 ), a->at( x, y ) );
          }
          else
          {
            CPPUNIT_ASSERT_EQUAL( int( x+y*3 ), a->at( x, y ) );
          }
        }
      }
    }
  }

  void test_reserve( void )
  {
    GarbageCollector gc;
    typedef Array2< int > IntArray2;

    {
      CountPtr< IntArray2 > a = new IntArray2( &gc, 1, 5 );

      for( index_t x=0; x<5; ++x )
      {
        a->at( x ) = x;
      }

      const IntArray2::const_pointer old_elements = a->elements();
      a->reserve( 8 );
      const IntArray2::const_pointer new_elements = a->elements();

      CPPUNIT_ASSERT( old_elements != new_elements );

      for( index_t x=0; x<5; ++x )
      {
        CPPUNIT_ASSERT_EQUAL( int( x ), a->at( x ) );
      }
    }

    {
      CountPtr< IntArray2 > a = new IntArray2( &gc, 2, 5, 7 );
      CountPtr< IntArray2 > b = new IntArray2( *a );

      CPPUNIT_ASSERT_EQUAL( a->elements(), b->elements() );

      a->fill( 23 );

      for( index_t y=0; y<7; ++y )
      for( index_t x=0; x<5; ++x )
      {
        CPPUNIT_ASSERT_EQUAL( 23, a->at( x, y ) );
        CPPUNIT_ASSERT_EQUAL( 23, b->at( x, y ) );
      }

      a->reserve( 11, 13 );

      CPPUNIT_ASSERT_EQUAL( a->getSize(), b->getSize() );
      CPPUNIT_ASSERT( a->elements() != b->elements() );

      for( index_t y=0; y<7; ++y )
      for( index_t x=0; x<5; ++x )
      {
        CPPUNIT_ASSERT_EQUAL( 23, a->at( x, y ) );
      }

      {
        const IntArray2::const_pointer old_elements = a->elements();
        a->resize( 11, 13 );
        const IntArray2::const_pointer new_elements = a->elements();
        CPPUNIT_ASSERT_EQUAL( old_elements, new_elements );
      }

      a->fill( 43 );
      for( index_t y=0; y<13; ++y )
      for( index_t x=0; x<11; ++x )
      {
        CPPUNIT_ASSERT_EQUAL( 43, a->at( x, y ) );
      }
      for( index_t y=0; y<7; ++y )
      for( index_t x=0; x<5; ++x )
      {
        CPPUNIT_ASSERT_EQUAL( 23, b->at( x, y ) );
      }

      {
        const IntArray2::const_pointer old_elements = a->elements();
        a->resize( 5, 6 );
        const IntArray2::const_pointer new_elements = a->elements();
        CPPUNIT_ASSERT_EQUAL( old_elements, new_elements );
      }

      a->fill( 71 );
      {
        const IntArray2::const_pointer old_elements = a->elements();
        a->resize( 11, 13 );
        const IntArray2::const_pointer new_elements = a->elements();
        CPPUNIT_ASSERT_EQUAL( old_elements, new_elements );
      }

      for( index_t y=0; y<13; ++y )
      for( index_t x=0; x<11; ++x )
      {
        if( x < 5 && y < 6 )
        {
          CPPUNIT_ASSERT_EQUAL( 71, a->at( x, y ) );
        }
        else
        {
          CPPUNIT_ASSERT_EQUAL( 43, a->at( x, y ) );
        }
      }
    }
  }

  void test_mirror_sparse_rotate( void )
  {
    GarbageCollector gc;
    typedef Array2< int > IntArray2;

    // mirror
    {
      CountPtr< IntArray2 > a = new IntArray2( &gc, 2, 5, 7 );

      for( index_t y=0; y<7; ++y )
      for( index_t x=0; x<5; ++x )
      {
        a->at( x, y ) = x+y*5;
      }
      for( index_t y=0; y<7; ++y )
      for( index_t x=0; x<5; ++x )
      {
        CPPUNIT_ASSERT_EQUAL( int( x+y*5 ), a->at( x, y ) );
      }

      CPPUNIT_ASSERT_THROW( a->setMirrored( -1 ), ArrayBase::Exception );
      CPPUNIT_ASSERT_THROW( a->setMirrored( 2 ) , ArrayBase::Exception );

      CountPtr< IntArray2 > b = new IntArray2( (*a) );
      CPPUNIT_ASSERT_EQUAL( &a->at( 0, 0 ), &b->at( 0, 0 ) );
      CPPUNIT_ASSERT_EQUAL( &a->at( 4, 6 ), &b->at( 4, 6 ) );

      CPPUNIT_ASSERT_NO_THROW( b->setMirrored( 0 ) );
      CPPUNIT_ASSERT_EQUAL( &a->at( 0, 0 ), &b->at( 4, 0 ) );
      CPPUNIT_ASSERT_EQUAL( &a->at( 4, 0 ), &b->at( 0, 0 ) );

      for( index_t y=0; y<7; ++y )
      for( index_t x=0; x<5; ++x )
      {
        CPPUNIT_ASSERT_EQUAL( int( (4-x)+y*5 ), b->at( x, y ) );
      }

      CPPUNIT_ASSERT_NO_THROW( b->setMirrored( 1 ) );
      CPPUNIT_ASSERT_EQUAL( &a->at( 0, 0 ), &b->at( 4, 6 ) );
      CPPUNIT_ASSERT_EQUAL( &a->at( 4, 6 ), &b->at( 0, 0 ) );

      for( index_t y=0; y<7; ++y )
      for( index_t x=0; x<5; ++x )
      {
        CPPUNIT_ASSERT_EQUAL( int( (4-x)+(6-y)*5 ), b->at( x, y ) );
      }

      {
        CountPtr< IntArray2::ConstIterator > iter = b->getConstIterator();
        for( int i = 5*7-1; i >= 0; --i )
        {
          CPPUNIT_ASSERT( !iter->done() );
          CPPUNIT_ASSERT_EQUAL( i, iter->get() );
          iter->next();
        }
        CPPUNIT_ASSERT( iter->done() );
      }
    }

    // sparse
    {
      CountPtr< IntArray2 > a = new IntArray2( &gc, 2, 5, 7 );

      for( index_t y=0; y<7; ++y )
      for( index_t x=0; x<5; ++x )
      {
        a->at( x, y ) = x+y*5;
      }

      CPPUNIT_ASSERT_THROW( a->setSparse( -1, 1 ), ArrayBase::Exception );
      CPPUNIT_ASSERT_THROW( a->setSparse( 2, 1 ), ArrayBase::Exception );
      CPPUNIT_ASSERT_THROW( a->setSparse( 0, 0 ), ArrayBase::Exception );
      CPPUNIT_ASSERT_THROW( a->setSparse( 0, 1, 1 ), ArrayBase::Exception );

      CountPtr< IntArray2 > b = new IntArray2( (*a) );
      CPPUNIT_ASSERT_EQUAL( &a->at( 0, 0 ), &b->at( 0, 0 ) );
      CPPUNIT_ASSERT_EQUAL( &a->at( 4, 6 ), &b->at( 4, 6 ) );

      CPPUNIT_ASSERT_NO_THROW( b->setSparse( 0, 2 ) );
      CPPUNIT_ASSERT_EQUAL( index_t( 2 ), b->getSize()[ 0 ] );
      CPPUNIT_ASSERT_EQUAL( index_t( 7 ), b->getSize()[ 1 ] );
      CPPUNIT_ASSERT_EQUAL( &a->at( 0, 0 ), &b->at( 0, 0 ) );
      CPPUNIT_ASSERT_EQUAL( &a->at( 2, 6 ), &b->at( 1, 6 ) );

      for( index_t y=0; y<7; ++y )
      for( index_t x=0; x<2; ++x )
      {
        CPPUNIT_ASSERT_EQUAL( int( (x*2)+y*5 ), b->at( x, y ) );
      }

      CPPUNIT_ASSERT_NO_THROW( b->setSparse( 1, 3 ) );
      CPPUNIT_ASSERT_EQUAL( index_t( 2 ), b->getSize()[ 0 ] );
      CPPUNIT_ASSERT_EQUAL( index_t( 2 ), b->getSize()[ 1 ] );
      CPPUNIT_ASSERT_EQUAL( &a->at( 0, 0 ), &b->at( 0, 0 ) );
      CPPUNIT_ASSERT_EQUAL( &a->at( 2, 3 ), &b->at( 1, 1 ) );

      for( index_t y=0; y<2; ++y )
      for( index_t x=0; x<2; ++x )
      {
        CPPUNIT_ASSERT_EQUAL( int( (x*2)+(y*3)*5 ), b->at( x, y ) );
      }

      {
        CountPtr< IntArray2::ConstIterator > iter = b->getConstIterator();
        CPPUNIT_ASSERT( !iter->done() ); CPPUNIT_ASSERT_EQUAL( a->at( 0, 0 ), iter->get() ); iter->next();
        CPPUNIT_ASSERT( !iter->done() ); CPPUNIT_ASSERT_EQUAL( a->at( 2, 0 ), iter->get() ); iter->next();
        CPPUNIT_ASSERT( !iter->done() ); CPPUNIT_ASSERT_EQUAL( a->at( 0, 3 ), iter->get() ); iter->next();
        CPPUNIT_ASSERT( !iter->done() ); CPPUNIT_ASSERT_EQUAL( a->at( 2, 3 ), iter->get() ); iter->next();
        CPPUNIT_ASSERT( iter->done() );
      }

      CountPtr< IntArray2 > c = new IntArray2( (*a) );

      CPPUNIT_ASSERT_NO_THROW( c->setSparse( 0, 2, 1 ) );
      CPPUNIT_ASSERT_EQUAL( index_t( 2 ), c->getSize()[ 0 ] );
      CPPUNIT_ASSERT_EQUAL( index_t( 7 ), c->getSize()[ 1 ] );
      CPPUNIT_ASSERT_EQUAL( &a->at( 1, 0 ), &c->at( 0, 0 ) );
      CPPUNIT_ASSERT_EQUAL( &a->at( 3, 6 ), &c->at( 1, 6 ) );

      for( index_t y=0; y<7; ++y )
      for( index_t x=0; x<2; ++x )
      {
        CPPUNIT_ASSERT_EQUAL( int( ((x*2)+1)+y*5 ), c->at( x, y ) );
      }

      CPPUNIT_ASSERT_NO_THROW( c->setSparse( 1, 3, 2 ) );
      CPPUNIT_ASSERT_EQUAL( index_t( 2 ), c->getSize()[ 0 ] );
      CPPUNIT_ASSERT_EQUAL( index_t( 2 ), c->getSize()[ 1 ] );
      CPPUNIT_ASSERT_EQUAL( &a->at( 1, 2 ), &c->at( 0, 0 ) );
      CPPUNIT_ASSERT_EQUAL( &a->at( 3, 5 ), &c->at( 1, 1 ) );

      for( index_t y=0; y<2; ++y )
      for( index_t x=0; x<2; ++x )
      {
        CPPUNIT_ASSERT_EQUAL( int( ((x*2)+1)+((y*3)+2)*5 ), c->at( x, y ) );
      }

      {
        CountPtr< IntArray2::ConstIterator > iter = c->getConstIterator();
        CPPUNIT_ASSERT( !iter->done() ); CPPUNIT_ASSERT_EQUAL( a->at( 1, 2 ), iter->get() ); iter->next();
        CPPUNIT_ASSERT( !iter->done() ); CPPUNIT_ASSERT_EQUAL( a->at( 3, 2 ), iter->get() ); iter->next();
        CPPUNIT_ASSERT( !iter->done() ); CPPUNIT_ASSERT_EQUAL( a->at( 1, 5 ), iter->get() ); iter->next();
        CPPUNIT_ASSERT( !iter->done() ); CPPUNIT_ASSERT_EQUAL( a->at( 3, 5 ), iter->get() ); iter->next();
        CPPUNIT_ASSERT( iter->done() );
      }

    }

    // sparse mirror
    {
      CountPtr< IntArray2 > a = new IntArray2( &gc, 2, 5, 7 );
      for( index_t y=0; y<7; ++y )
      for( index_t x=0; x<5; ++x )
      {
        a->at( x, y ) = x+y*5;
      }

      CountPtr< IntArray2 > b[ 4 ];

      b[ 0 ] = new IntArray2( (*a) );
      b[ 0 ]->
         setSparse( 0, 2 )
        .setSparse( 1, 3 )
        .setMirrored( 0 )
        .setMirrored( 1 )
        ;
      b[ 1 ] = new IntArray2( (*a) );
      b[ 1 ]->
         setSparse( 1, 3 )
        .setSparse( 0, 2 )
        .setMirrored( 0 )
        .setMirrored( 1 )
        ;
      b[ 2 ] = new IntArray2( (*a) );
      b[ 2 ]->
         setSparse( 0, 2 )
        .setSparse( 1, 3 )
        .setMirrored( 1 )
        .setMirrored( 0 )
        ;
      b[ 3 ] = new IntArray2( (*a) );
      b[ 3 ]->
         setSparse( 1, 3 )
        .setSparse( 0, 2 )
        .setMirrored( 1 )
        .setMirrored( 0 )
        ;

      for( index_t y=0; y<2; ++y )
      for( index_t x=0; x<2; ++x )
      for( int i=0; i<4; ++i )
      {
        CPPUNIT_ASSERT_EQUAL( int( ((1-x)*2)+((1-y)*3)*5 ), b[ i ]->at( x, y ) );
      }
    }

    // rotate
    {
      CountPtr< IntArray2 > a = new IntArray2( &gc, 2, 5, 7 );
      for( index_t y=0; y<7; ++y )
      for( index_t x=0; x<5; ++x )
      {
        a->at( x, y ) = x+y*5;
      }

      CountPtr< IntArray2 > b = new IntArray2( (*a) );

      CPPUNIT_ASSERT_THROW( b->setRotated( 0, -1,  0 ), ArrayBase::Exception );
      CPPUNIT_ASSERT_THROW( b->setRotated( 0,  2,  0 ), ArrayBase::Exception );
      CPPUNIT_ASSERT_THROW( b->setRotated( 0,  0, -1 ), ArrayBase::Exception );
      CPPUNIT_ASSERT_THROW( b->setRotated( 0,  0,  2 ), ArrayBase::Exception );
      CPPUNIT_ASSERT_THROW( b->setRotated( 0,  0,  0 ), ArrayBase::Exception );

      CPPUNIT_ASSERT_NO_THROW( b->setRotated( 0 ) );
      {
        CountPtr< IntArray2 > c = new IntArray2( (*a) );
        CPPUNIT_ASSERT_NO_THROW( c->setRotated( 4 ) );
        for( index_t y=0; y<7; ++y )
        for( index_t x=0; x<5; ++x )
        {
          CPPUNIT_ASSERT_EQUAL( a->at( x, y ), b->at( x, y ) );
          CPPUNIT_ASSERT_EQUAL( b->at( x, y ), c->at( x, y ) );
        }
      }

      CPPUNIT_ASSERT_NO_THROW( b->setRotated( -3 ) );
      {
        CountPtr< IntArray2 > c = new IntArray2( (*a) );
        CPPUNIT_ASSERT_NO_THROW( c->setRotated( 1 ) );
        for( index_t y=0; y<5; ++y )
        for( index_t x=0; x<7; ++x )
        {
          CPPUNIT_ASSERT_EQUAL( a->at( (4-y), x ), b->at( x, y ) );
          CPPUNIT_ASSERT_EQUAL( b->at( x, y ), c->at( x, y ) );
        }
      }

      CPPUNIT_ASSERT_NO_THROW( b->setRotated( 1 ) );
      {
        CountPtr< IntArray2 > c = new IntArray2( (*a) );
        CPPUNIT_ASSERT_NO_THROW( c->setRotated( 2 ) );
        for( index_t y=0; y<7; ++y )
        for( index_t x=0; x<5; ++x )
        {
          CPPUNIT_ASSERT_EQUAL( a->at( (4-x), (6-y) ), b->at( x, y ) );
          CPPUNIT_ASSERT_EQUAL( b->at( x, y ), c->at( x, y ) );
        }
      }

      CPPUNIT_ASSERT_NO_THROW( b->setRotated( 1 ) );
      {
        CountPtr< IntArray2 > c = new IntArray2( (*a) );
        CPPUNIT_ASSERT_NO_THROW( c->setRotated( 3 ) );
        for( index_t y=0; y<5; ++y )
        for( index_t x=0; x<7; ++x )
        {
          CPPUNIT_ASSERT_EQUAL( a->at( y, (6-x) ), b->at( x, y ) );
          CPPUNIT_ASSERT_EQUAL( b->at( x, y ), c->at( x, y ) );
        }
      }
    }

    // transform resize
    {
      CountPtr< IntArray2 > a = new IntArray2( &gc, 2, 10, 10 );
      for( index_t y=0; y<10; ++y )
      for( index_t x=0; x<10; ++x )
      {
        a->at( x, y ) = x+y*10;
      }

      const int *e_first = &a->at( 0, 0 );
      const int *e_last  = &a->at( 9, 9 );

      {
        CountPtr< IntArray2 > b = a->getROI( 3, 3, 5, 5 );

        // Original memory suffices
        CPPUNIT_ASSERT_NO_THROW( b->resize( 7, 7 ) );
        CPPUNIT_ASSERT( b->elements() >= e_first );
        CPPUNIT_ASSERT( b->elements() <= e_last  );

        for( index_t y=0; y<7; ++y )
        for( index_t x=0; x<7; ++x )
        {
          CPPUNIT_ASSERT_EQUAL( int( (x+3)+(y+3)*10 ), b->at( x, y ) );
        }

        // Reallocated
        CPPUNIT_ASSERT_NO_THROW( b->resize( 8, 8 ) );
        CPPUNIT_ASSERT( !( b->elements() >= e_first && b->elements() <= e_last ) );
      }

      {
        CountPtr< IntArray2 > b = a->getROI( 3, 3, 5, 5 );

        CPPUNIT_ASSERT_NO_THROW( b->setRotated( 1 ) );
        // Original memory suffices
        CPPUNIT_ASSERT_NO_THROW( b->resize( 7, 8 ) );
        CPPUNIT_ASSERT( b->elements() >= e_first );
        CPPUNIT_ASSERT( b->elements() <= e_last  );

        for( index_t y=0; y<8; ++y )
        for( index_t x=0; x<7; ++x )
        {
          CPPUNIT_ASSERT_EQUAL( int( (7-y)+(3+x)*10 ), b->at( x, y ) );
        }

        // Reallocated
        CPPUNIT_ASSERT_NO_THROW( b->resize( 8, 9 ) );
        CPPUNIT_ASSERT( !( b->elements() >= e_first && b->elements() <= e_last ) );
      }

      {
        CountPtr< IntArray2 > b = a->getROI( 3, 3, 5, 5 );

        CPPUNIT_ASSERT_NO_THROW( b->setRotated( 2 ) );
        // Original memory suffices
        CPPUNIT_ASSERT_NO_THROW( b->resize( 8, 8 ) );
        CPPUNIT_ASSERT( b->elements() >= e_first );
        CPPUNIT_ASSERT( b->elements() <= e_last  );

        for( index_t y=0; y<8; ++y )
        for( index_t x=0; x<8; ++x )
        {
          CPPUNIT_ASSERT_EQUAL( int( (7-x)+(7-y)*10 ), b->at( x, y ) );
        }

        // Reallocated
        CPPUNIT_ASSERT_NO_THROW( b->resize( 9, 9 ) );
        CPPUNIT_ASSERT( !( b->elements() >= e_first && b->elements() <= e_last ) );
      }

      {
        CountPtr< IntArray2 > b = a->getROI( 3, 3, 5, 5 );

        CPPUNIT_ASSERT_NO_THROW( b->setRotated( 3 ) );
        // Original memory suffices
        CPPUNIT_ASSERT_NO_THROW( b->resize( 8, 7 ) );
        CPPUNIT_ASSERT( b->elements() >= e_first );
        CPPUNIT_ASSERT( b->elements() <= e_last  );

        for( index_t y=0; y<7; ++y )
        for( index_t x=0; x<8; ++x )
        {
          CPPUNIT_ASSERT_EQUAL( int( (3+y)+(7-x)*10 ), b->at( x, y ) );
        }

        // Reallocated
        CPPUNIT_ASSERT_NO_THROW( b->resize( 9, 8 ) );
        CPPUNIT_ASSERT( !( b->elements() >= e_first && b->elements() <= e_last ) );
      }

      {
        CountPtr< IntArray2 > b = a->getROI( 4, 4, 4, 4 );

        CPPUNIT_ASSERT_NO_THROW( b->setRotated( 2 ) );
        CPPUNIT_ASSERT_NO_THROW( b->setSparse( 0, 2 ) );
        CPPUNIT_ASSERT_NO_THROW( b->setSparse( 1, 2 ) );
        // Original memory suffices
        CPPUNIT_ASSERT_NO_THROW( b->resize( 4, 4 ) );
        CPPUNIT_ASSERT( b->elements() >= e_first );
        CPPUNIT_ASSERT( b->elements() <= e_last  );

        for( index_t y=0; y<4; ++y )
        for( index_t x=0; x<4; ++x )
        {
          CPPUNIT_ASSERT_EQUAL( int( (7-2*x)+(7-2*y)*10 ), b->at( x, y ) );
        }

        // Reallocated
        CPPUNIT_ASSERT_NO_THROW( b->resize( 5, 5 ) );
        CPPUNIT_ASSERT( !( b->elements() >= e_first && b->elements() <= e_last ) );
      }
    }

  }

  void test_BoolContainer( void )
  {
    GarbageCollector gc;
    typedef ArrayContainer< bool > BoolContainer;
    typedef Array2< bool > BoolArray2;

    // operator[] / reference
    {
      BoolContainer a( 21 );
      CPPUNIT_ASSERT_EQUAL( index_t( 21 ), a.size() );
      CPPUNIT_ASSERT_EQUAL( index_t( 24 ), a.capacity() );

      // reference write
      const uint32_t bits = 0x17a2945a;
      for( index_t i=0; i<21; ++i )
      {
        CPPUNIT_ASSERT_NO_THROW( a[ i ] = bool( bits & ( 1 << i ) ) );
      }

      // reference read
      for( index_t i=0; i<21; ++i )
      {
        CPPUNIT_ASSERT_EQUAL( bool( bits & ( 1 << i ) ), bool( a[ i ] ) );
      }

      // const_reference read
      for( index_t i=0; i<21; ++i )
      {
        CPPUNIT_ASSERT_EQUAL( bool( bits & ( 1 << i ) ), bool( static_cast< const BoolContainer* >( &a )->operator[]( i ) ) );
      }
    }

    // pointer
    {
      BoolContainer a( 21 );
      const uint32_t bits = 0x17a2945a;
      for( index_t i=0; i<21; ++i )
      {
        CPPUNIT_ASSERT_NO_THROW( a[ i ] = bool( bits & ( 1 << i ) ) );
      }

      // operator=
      // operator++(), operator++(int), operator+=
      // operator==
      // operator-
      BoolContainer::pointer p1 = a.first();
      BoolContainer::pointer p2 = a.first();
      BoolContainer::pointer p3 = a.first();
      BoolContainer::pointer p4 = a.first();
      for( int i=0; i<21; ++i, ++p1, p2++, p3 += 1 )
      {
        CPPUNIT_ASSERT_EQUAL( bool( bits & ( 1 << i ) ), bool( *p1 ) );
        CPPUNIT_ASSERT_EQUAL( bool( bits & ( 1 << i ) ), bool( p2[ 0 ] ) );
        CPPUNIT_ASSERT_EQUAL( bool( bits & ( 1 << i ) ), bool( p4[ i ] ) );
        CPPUNIT_ASSERT_EQUAL( bool( *p3 ), bool( p2[ 0 ] ) );
        CPPUNIT_ASSERT_EQUAL( p1, p2 );
        CPPUNIT_ASSERT_EQUAL( p2, p3 );
        CPPUNIT_ASSERT_EQUAL( p3, p1 );
        CPPUNIT_ASSERT_EQUAL( p4+i, p1 );
        CPPUNIT_ASSERT_EQUAL( ptrdiff_t( i ), p1 - a.first() );
      }

      // operator--(), operator--(int), operator-=
      p4 += 20;
      --p1; p2--; p3 -= 1;
      for( int i=20; i>=0; --i, --p1, p2--, p3 -= 1 )
      {
        CPPUNIT_ASSERT_EQUAL( p1, p2 );
        CPPUNIT_ASSERT_EQUAL( p2, p3 );
        CPPUNIT_ASSERT_EQUAL( p3, p1 );
        CPPUNIT_ASSERT_EQUAL( p4-(20-i), p1 );
        CPPUNIT_ASSERT_EQUAL( bool( bits & ( 1 << i ) ), bool( p4[ -(20-i) ] ) );
        CPPUNIT_ASSERT_EQUAL( ptrdiff_t( i ), p1 - a.first() );
      }

      BoolContainer::pointer p5 = p1;
      CPPUNIT_ASSERT( p5 == p1 );
      ++p5;
      CPPUNIT_ASSERT( p5 != p1 );

      BoolContainer::pointer p6 = a.first();
      const uint32_t bits2 = 0x945a17a2;
      for( index_t i=0; i<21; ++i, ++p6 )
      {
        CPPUNIT_ASSERT_NO_THROW( (*p6) = bool( bits2 & ( 1 << i ) ) );
        CPPUNIT_ASSERT_EQUAL( bool( bits2 & ( 1 << i ) ), bool( a.first()[ i ] ) );
      }
    }

    // const_pointer
    {
      BoolContainer a( 21 );
      const uint32_t bits = 0x17a2945a;
      for( index_t i=0; i<21; ++i )
      {
        CPPUNIT_ASSERT_NO_THROW( a[ i ] = bool( bits & ( 1 << i ) ) );
      }

      // operator=
      // operator++(), operator++(int), operator+=
      // operator==
      // operator-
      BoolContainer::const_pointer p1 = a.first();
      BoolContainer::const_pointer p2 = a.first();
      BoolContainer::const_pointer p3 = a.first();
      BoolContainer::const_pointer p4 = a.first();
      for( int i=0; i<21; ++i, ++p1, p2++, p3 += 1 )
      {
        CPPUNIT_ASSERT_EQUAL( bool( bits & ( 1 << i ) ), bool( *p1 ) );
        CPPUNIT_ASSERT_EQUAL( bool( bits & ( 1 << i ) ), bool( p2[ 0 ] ) );
        CPPUNIT_ASSERT_EQUAL( bool( bits & ( 1 << i ) ), bool( p4[ i ] ) );
        CPPUNIT_ASSERT_EQUAL( bool( *p3 ), bool( p2[ 0 ] ) );
        CPPUNIT_ASSERT_EQUAL( p1, p2 );
        CPPUNIT_ASSERT_EQUAL( p2, p3 );
        CPPUNIT_ASSERT_EQUAL( p3, p1 );
        CPPUNIT_ASSERT_EQUAL( p4+i, p1 );
        CPPUNIT_ASSERT_EQUAL( ptrdiff_t( i ), p1 - a.first() );
      }

      // operator--(), operator--(int), operator-=
      p4 += 20;
      --p1; p2--; p3 -= 1;
      for( int i=20; i>=0; --i, --p1, p2--, p3 -= 1 )
      {
        CPPUNIT_ASSERT_EQUAL( p1, p2 );
        CPPUNIT_ASSERT_EQUAL( p2, p3 );
        CPPUNIT_ASSERT_EQUAL( p3, p1 );
        CPPUNIT_ASSERT_EQUAL( p4-(20-i), p1 );
        CPPUNIT_ASSERT_EQUAL( bool( bits & ( 1 << i ) ), bool( p4[ -(20-i) ] ) );
        CPPUNIT_ASSERT_EQUAL( ptrdiff_t( i ), p1 - a.first() );
      }

      BoolContainer::const_pointer p5 = p1;
      CPPUNIT_ASSERT( p5 == p1 );
      ++p5;
      CPPUNIT_ASSERT( p5 != p1 );

    }
  }

  void test_Array2_bool( void )
  {
    GarbageCollector gc;

    typedef Array2< bool > BoolArray2;

    {
      CountPtr< BoolArray2 > a = new BoolArray2( &gc, 0 );

      (**a) = true;

      {
        BoolArray2::iterator iter = a->begin();
        BoolArray2::iterator end  = a->end();

        CPPUNIT_ASSERT( iter != end );
        CPPUNIT_ASSERT_EQUAL( true, bool(*iter) );

        (*iter) = false;

        ++iter;
        CPPUNIT_ASSERT( iter == end );
      }

      {
        BoolArray2::const_iterator iter = a->begin();
        BoolArray2::const_iterator end  = a->end();

        CPPUNIT_ASSERT( iter != end );
        CPPUNIT_ASSERT_EQUAL( false, bool(*iter) );

        ++iter;
        CPPUNIT_ASSERT( iter == end );
      }

    }

    {
      CountPtr< BoolArray2 > a = new BoolArray2( &gc, 2, 3, 5 );

      const uint32_t bits  = 0x519ea375;
      const uint32_t bits2 = 0xa375519e;

      for( index_t y=0, i=0; y<5; ++y )
      for( index_t x=0     ; x<3; ++x, ++i )
      {
        a->at( x, y ) = bool( ( bits >> i ) & 1 );
      }

      {
        BoolArray2::iterator iter = a->begin();
        BoolArray2::iterator end  = a->end();

        for( index_t y=0, i=0; y<5; ++y )
        for( index_t x=0     ; x<3; ++x, ++i, ++iter )
        {
          CPPUNIT_ASSERT( iter != end );
          CPPUNIT_ASSERT_EQUAL( bool( ( bits >> i ) & 1 ), bool(*iter) );
          CPPUNIT_ASSERT_EQUAL( x, iter.getPos()[ 0 ] );
          CPPUNIT_ASSERT_EQUAL( y, iter.getPos()[ 1 ] );

          (*iter) = bool( ( bits2 >> i ) & 1 );
        }

        CPPUNIT_ASSERT( iter == end );
      }

      {
        BoolArray2::const_iterator iter = a->begin();
        BoolArray2::const_iterator end  = a->end();

        for( index_t y=0, i=0; y<5; ++y )
        for( index_t x=0     ; x<3; ++x, ++i, ++iter )
        {
          CPPUNIT_ASSERT( iter != end );
          CPPUNIT_ASSERT_EQUAL( bool( ( bits2 >> i ) & 1 ), bool(*iter) );
          CPPUNIT_ASSERT_EQUAL( x, iter.getPos()[ 0 ] );
          CPPUNIT_ASSERT_EQUAL( y, iter.getPos()[ 1 ] );
        }

        CPPUNIT_ASSERT( iter == end );
      }
    }

    // transform resize
    {
      bool bits[ 100 ];
      unsigned int seed = 23;
      for( int i=0; i<100; ++i )
      {
        bits[ i ] = bool( rand_r( &seed ) & 1 );
      }

      CountPtr< BoolArray2 > a = new BoolArray2( &gc, 2, 10, 10 );
      for( index_t y=0; y<10; ++y )
      for( index_t x=0; x<10; ++x )
      {
        a->at( x, y ) = bits[ x+y*10 ];
      }

      BoolArray2::const_pointer e_first = &a->at( 0, 0 );
      BoolArray2::const_pointer e_last  = &a->at( 9, 9 );

      {
        CountPtr< BoolArray2 > b = a->getROI( 3, 3, 5, 5 );

        // Original memory suffices
        CPPUNIT_ASSERT_NO_THROW( b->resize( 7, 7 ) );
        CPPUNIT_ASSERT( b->elements() >= e_first );
        CPPUNIT_ASSERT( b->elements() <= e_last  );

        for( index_t y=0; y<7; ++y )
        for( index_t x=0; x<7; ++x )
        {
          CPPUNIT_ASSERT_EQUAL( bits[ (x+3)+(y+3)*10 ], bool( b->at( x, y ) ) );
        }

        // Reallocated
        CPPUNIT_ASSERT_NO_THROW( b->resize( 8, 8 ) );
        CPPUNIT_ASSERT( !( b->elements() >= e_first && b->elements() <= e_last ) );
      }

      {
        CountPtr< BoolArray2 > b = a->getROI( 3, 3, 5, 5 );

        CPPUNIT_ASSERT_NO_THROW( b->setRotated( 1 ) );
        // Original memory suffices
        CPPUNIT_ASSERT_NO_THROW( b->resize( 7, 8 ) );
        CPPUNIT_ASSERT( b->elements() >= e_first );
        CPPUNIT_ASSERT( b->elements() <= e_last  );

        for( index_t y=0; y<8; ++y )
        for( index_t x=0; x<7; ++x )
        {
          CPPUNIT_ASSERT_EQUAL( bits[ (7-y)+(3+x)*10 ], bool( b->at( x, y ) ) );
        }

        // Reallocated
        CPPUNIT_ASSERT_NO_THROW( b->resize( 8, 9 ) );
        CPPUNIT_ASSERT( !( b->elements() >= e_first && b->elements() <= e_last ) );
      }

      {
        CountPtr< BoolArray2 > b = a->getROI( 3, 3, 5, 5 );

        CPPUNIT_ASSERT_NO_THROW( b->setRotated( 2 ) );
        // Original memory suffices
        CPPUNIT_ASSERT_NO_THROW( b->resize( 8, 8 ) );
        CPPUNIT_ASSERT( b->elements() >= e_first );
        CPPUNIT_ASSERT( b->elements() <= e_last  );

        for( index_t y=0; y<8; ++y )
        for( index_t x=0; x<8; ++x )
        {
          CPPUNIT_ASSERT_EQUAL( bits[ (7-x)+(7-y)*10 ], bool( b->at( x, y ) ) );
        }

        // Reallocated
        CPPUNIT_ASSERT_NO_THROW( b->resize( 9, 9 ) );
        CPPUNIT_ASSERT( !( b->elements() >= e_first && b->elements() <= e_last ) );
      }

      {
        CountPtr< BoolArray2 > b = a->getROI( 3, 3, 5, 5 );

        CPPUNIT_ASSERT_NO_THROW( b->setRotated( 3 ) );
        // Original memory suffices
        CPPUNIT_ASSERT_NO_THROW( b->resize( 8, 7 ) );
        CPPUNIT_ASSERT( b->elements() >= e_first );
        CPPUNIT_ASSERT( b->elements() <= e_last  );

        for( index_t y=0; y<7; ++y )
        for( index_t x=0; x<8; ++x )
        {
          CPPUNIT_ASSERT_EQUAL( bits[ (3+y)+(7-x)*10 ], bool( b->at( x, y ) ) );
        }

        // Reallocated
        CPPUNIT_ASSERT_NO_THROW( b->resize( 9, 8 ) );
        CPPUNIT_ASSERT( !( b->elements() >= e_first && b->elements() <= e_last ) );
      }

      {
        CountPtr< BoolArray2 > b = a->getROI( 4, 4, 4, 4 );

        CPPUNIT_ASSERT_NO_THROW( b->setRotated( 2 ) );
        CPPUNIT_ASSERT_NO_THROW( b->setSparse( 0, 2 ) );
        CPPUNIT_ASSERT_NO_THROW( b->setSparse( 1, 2 ) );
        // Original memory suffices
        CPPUNIT_ASSERT_NO_THROW( b->resize( 4, 4 ) );
        CPPUNIT_ASSERT( b->elements() >= e_first );
        CPPUNIT_ASSERT( b->elements() <= e_last  );

        for( index_t y=0; y<4; ++y )
        for( index_t x=0; x<4; ++x )
        {
          CPPUNIT_ASSERT_EQUAL( bits[ (7-2*x)+(7-2*y)*10 ], bool( b->at( x, y ) ) );
        }

        // Reallocated
        CPPUNIT_ASSERT_NO_THROW( b->resize( 5, 5 ) );
        CPPUNIT_ASSERT( !( b->elements() >= e_first && b->elements() <= e_last ) );
      }
    }
  }
};

CPPUNIT_TEST_SUITE_REGISTRATION( utest_Array );


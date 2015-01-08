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

#include <iostream>

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
    CPPUNIT_ASSERT_EQUAL( true, isCollectable( dummy1 ) );
    CPPUNIT_ASSERT_EQUAL( true, isCollectable( dummy2 ) );
    CPPUNIT_ASSERT_EQUAL( true, isCollectable( dummy3 ) );
  }

  void test_getType( void )
  {
    Array< int, 2 > i2( 0 );
    Array< float, 1 > f1( 0 );
    Array< String, 0 > s0( 0 );
    CPPUNIT_ASSERT_EQUAL( index_t( 2 ), i2.getDims() );
    CPPUNIT_ASSERT_EQUAL( index_t( 1 ), f1.getDims() );
    CPPUNIT_ASSERT_EQUAL( index_t( 0 ), s0.getDims() );

    CPPUNIT_ASSERT_EQUAL( Type::Int(), i2.getType() );
    CPPUNIT_ASSERT_EQUAL( Type::Float(), f1.getType() );
    CPPUNIT_ASSERT_EQUAL( Type::String(), s0.getType() );
  }

  void test_CoordinatesIterator( void )
  {
    CountPtr< Array< int, 2 > > a = new Array< int, 2 >( 0, 2, 3 );

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

  void test_resize( void )
  {
    cerr << "test_resize" << endl;
    Array< int, 2 > i2( 0 );

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

  class Marker : public Collectable
  {
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
    cerr << "test_gc" << endl;
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

  class Other : public Collectable
  {
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

  void test_getAs( void )
  {
    test_getAs_t< int >();
    test_getAs_t< String >();
    test_getAs_t< CountPtr< ArrayBase > >();
    test_getAs_t< Other >();
    test_getAs_t< CountPtr< Other > >();
  }

};

CPPUNIT_TEST_SUITE_REGISTRATION( utest_Array );


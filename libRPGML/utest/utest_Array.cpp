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
  CPPUNIT_TEST( test_getLine );
  CPPUNIT_TEST( test_random_access_iterator );
  CPPUNIT_TEST( test_reserve );
  CPPUNIT_TEST( test_mirror_sparse_rotate );
  CPPUNIT_TEST( test_BoolContainer );
  CPPUNIT_TEST( test_Array_bool );
  CPPUNIT_TEST( test_wrapper );
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
    Array< int > *dummy4 = 0;
    CPPUNIT_ASSERT_EQUAL( true, isCollectable( dummy1 ) );
    CPPUNIT_ASSERT_EQUAL( true, isCollectable( dummy2 ) );
    CPPUNIT_ASSERT_EQUAL( true, isCollectable( dummy4 ) );
  }

  void test_getType( void )
  {
    GarbageCollector *gc = 0;

    Array< int > i2( gc, 2 );
    Array< float > f1( gc, 1 );
    Array< String > s0( gc, 0 );
    CPPUNIT_ASSERT_EQUAL( 2, i2.getDims() );
    CPPUNIT_ASSERT_EQUAL( 1, f1.getDims() );
    CPPUNIT_ASSERT_EQUAL( 0, s0.getDims() );

    CPPUNIT_ASSERT_EQUAL( Type::Int(), i2.getType() );
    CPPUNIT_ASSERT_EQUAL( Type::Float(), f1.getType() );
    CPPUNIT_ASSERT_EQUAL( Type::String(), s0.getType() );
  }

  void test_CoordinatesIterator( void )
  {
    GarbageCollector gc;
    CountPtr< Array< int > > a = new Array< int >( &gc, 2, 2, 3 );

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
    GarbageCollector *gc = 0;
    Array< int > i2( gc, 2 );

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
    GarbageCollector gc;

    typedef Array< CountPtr< ArrayBase > > root_array_t;
    typedef Array< CountPtr< Marker > > marker_array_t;
    typedef Array< int8_t > bool_array_t;

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
    GarbageCollector gc;

    typedef Array< T > TArray;

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
      Array< int > a( gc, 0 );

      (*a) = 42;

      {
        Array< int >::iterator iter = a.begin();
        Array< int >::iterator end  = a.end();

        CPPUNIT_ASSERT( iter != end );
        CPPUNIT_ASSERT_EQUAL( 42, *iter );

        (*iter) += 100;

        ++iter;
        CPPUNIT_ASSERT( iter == end );
      }

      {
        Array< int >::const_iterator iter = a.begin();
        Array< int >::const_iterator end  = a.end();

        CPPUNIT_ASSERT( iter != end );
        CPPUNIT_ASSERT_EQUAL( 142, *iter );

        ++iter;
        CPPUNIT_ASSERT( iter == end );
      }

    }

    {
      Array< int > a( gc, 2, 3, 5 );

      for( index_t y=0, i=0; y<5; ++y )
      for( index_t x=0     ; x<3; ++x, ++i )
      {
        a.at( x, y ) = i;
      }

      {
        Array< int >::iterator iter = a.begin();
        Array< int >::iterator end  = a.end();

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
        Array< int >::const_iterator iter = a.begin();
        Array< int >::const_iterator end  = a.end();

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

    {
      CountPtr< IntArray > a = new IntArray( &gc, 1, 5 );

      for( index_t x=0; x<5; ++x )
      {
        a->at( x ) = x;
      }

      // Whole array
      {
        const index_t x[ 1 ] = { 0 };
        const index_t s[ 1 ] = { 5 };
        CountPtr< const IntArray > roi = static_cast< const IntArray* >( a.get() )->getROI( 1, x, s );

        CPPUNIT_ASSERT_EQUAL( a->getSize(), roi->getSize() );

        IntArray::const_iterator a_i   = a->begin();
        IntArray::const_iterator a_end = a->end();
        IntArray::const_iterator roi_i   = roi->begin();
        IntArray::const_iterator roi_end = roi->end();

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
        CountPtr< IntArray > roi = a->getROI( 1, x, s );

        IntArray::iterator a_i   = a->begin();
        IntArray::iterator a_end = a->end();
        IntArray::iterator roi_i   = roi->begin();
        IntArray::iterator roi_end = roi->end();

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

        IntArray::const_iterator a_j = a->begin();
        CPPUNIT_ASSERT_EQUAL(   0, *a_j++ ); CPPUNIT_ASSERT( a_j != a_end );
        CPPUNIT_ASSERT_EQUAL(   1, *a_j++ ); CPPUNIT_ASSERT( a_j != a_end );
        CPPUNIT_ASSERT_EQUAL( 102, *a_j++ ); CPPUNIT_ASSERT( a_j != a_end );
        CPPUNIT_ASSERT_EQUAL( 103, *a_j++ ); CPPUNIT_ASSERT( a_j != a_end );
        CPPUNIT_ASSERT_EQUAL(   4, *a_j++ ); CPPUNIT_ASSERT( a_j == a_end );
      }
    }

    {
      CountPtr< IntArray > a = new IntArray( &gc, 2, 3, 5 );

      for( index_t y=0; y<5; ++y )
      for( index_t x=0; x<3; ++x )
      {
        a->at( x, y ) = x+y*3;
      }

      // Whole array
      {
        const index_t x[ 2 ] = { 0, 0 };
        const index_t s[ 2 ] = { 3, 5 };
        CountPtr< const IntArray > roi = static_cast< const IntArray* >( a.get() )->getROI( 2, x, s );

        CPPUNIT_ASSERT_EQUAL( a->getSize(), roi->getSize() );

        IntArray::const_iterator a_i   = a->begin();
        IntArray::const_iterator a_end = a->end();
        IntArray::const_iterator roi_i   = roi->begin();
        IntArray::const_iterator roi_end = roi->end();

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
        CountPtr< IntArray > roi = a->getROI( 2, x, s );

        IntArray::iterator roi_i   = roi->begin();
        IntArray::iterator roi_end = roi->end();

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

  void test_getLine( void )
  {
    GarbageCollector gc;

    {
      CountPtr< IntArray > a = new IntArray( &gc, 2, 3, 5 );

      for( index_t y=0, i=0; y<5; ++y )
      for( index_t x=0     ; x<3; ++x, ++i )
      {
        a->at( x, y ) = i;
      }

      {
        index_t pos[ 2 ] = { 0, 0 };
        CountPtr< IntArray > line;

        CPPUNIT_ASSERT_THROW( line = a->getLine( 1, pos ), ArrayBase::Exception );
        CPPUNIT_ASSERT( line.isNull() );
        CPPUNIT_ASSERT_THROW( line = a->getLine( 3, pos ), ArrayBase::Exception );
        CPPUNIT_ASSERT( line.isNull() );
        CPPUNIT_ASSERT_THROW( line = a->getLine( 2, pos,  2 ), ArrayBase::Exception );
        CPPUNIT_ASSERT( line.isNull() );
        CPPUNIT_ASSERT_THROW( line = a->getLine( 2, pos, -1 ), ArrayBase::Exception );
        CPPUNIT_ASSERT( line.isNull() );

        CPPUNIT_ASSERT_NO_THROW( line = a->getLine( 2, pos ) );
        CPPUNIT_ASSERT( !line.isNull() );
        CPPUNIT_ASSERT_EQUAL( 1, line->getDims() );
        CPPUNIT_ASSERT_EQUAL( index_t( 3 ), line->getSizeX() );
        CPPUNIT_ASSERT_EQUAL( 0, (*line)[ 0 ] );
        CPPUNIT_ASSERT_EQUAL( 1, (*line)[ 1 ] );
        CPPUNIT_ASSERT_EQUAL( 2, (*line)[ 2 ] );

        CPPUNIT_ASSERT_NO_THROW( line = a->getLine( 2, pos, 1 ) );
        CPPUNIT_ASSERT( !line.isNull() );
        CPPUNIT_ASSERT_EQUAL( 1, line->getDims() );
        CPPUNIT_ASSERT_EQUAL( index_t( 5 ), line->getSizeX() );
        CPPUNIT_ASSERT_EQUAL(  0, (*line)[ 0 ] );
        CPPUNIT_ASSERT_EQUAL(  3, (*line)[ 1 ] );
        CPPUNIT_ASSERT_EQUAL(  6, (*line)[ 2 ] );
        CPPUNIT_ASSERT_EQUAL(  9, (*line)[ 3 ] );
        CPPUNIT_ASSERT_EQUAL( 12, (*line)[ 4 ] );

        pos[ 0 ] = 1;

        CPPUNIT_ASSERT_NO_THROW( line = a->getLine( 2, pos ) );
        CPPUNIT_ASSERT( !line.isNull() );
        CPPUNIT_ASSERT_EQUAL( 1, line->getDims() );
        CPPUNIT_ASSERT_EQUAL( index_t( 2 ), line->getSizeX() );
        CPPUNIT_ASSERT_EQUAL( 1, (*line)[ 0 ] );
        CPPUNIT_ASSERT_EQUAL( 2, (*line)[ 1 ] );

        CPPUNIT_ASSERT_NO_THROW( line = a->getLine( 2, pos, 1 ) );
        CPPUNIT_ASSERT( !line.isNull() );
        CPPUNIT_ASSERT_EQUAL( 1, line->getDims() );
        CPPUNIT_ASSERT_EQUAL( index_t( 5 ), line->getSizeX() );
        CPPUNIT_ASSERT_EQUAL(  1, (*line)[ 0 ] );
        CPPUNIT_ASSERT_EQUAL(  4, (*line)[ 1 ] );
        CPPUNIT_ASSERT_EQUAL(  7, (*line)[ 2 ] );
        CPPUNIT_ASSERT_EQUAL( 10, (*line)[ 3 ] );
        CPPUNIT_ASSERT_EQUAL( 13, (*line)[ 4 ] );

        pos[ 0 ] = 1;
        pos[ 1 ] = 1;

        CPPUNIT_ASSERT_NO_THROW( line = a->getLine( 2, pos ) );
        CPPUNIT_ASSERT( !line.isNull() );
        CPPUNIT_ASSERT_EQUAL( 1, line->getDims() );
        CPPUNIT_ASSERT_EQUAL( index_t( 2 ), line->getSizeX() );
        CPPUNIT_ASSERT_EQUAL( 4, (*line)[ 0 ] );
        CPPUNIT_ASSERT_EQUAL( 5, (*line)[ 1 ] );

        CPPUNIT_ASSERT_NO_THROW( line = a->getLine( 2, pos, 1 ) );
        CPPUNIT_ASSERT( !line.isNull() );
        CPPUNIT_ASSERT_EQUAL( 1, line->getDims() );
        CPPUNIT_ASSERT_EQUAL( index_t( 4 ), line->getSizeX() );
        CPPUNIT_ASSERT_EQUAL(  4, (*line)[ 0 ] );
        CPPUNIT_ASSERT_EQUAL(  7, (*line)[ 1 ] );
        CPPUNIT_ASSERT_EQUAL( 10, (*line)[ 2 ] );
        CPPUNIT_ASSERT_EQUAL( 13, (*line)[ 3 ] );

        CountPtr< IntArray > line2 = new IntArray( &gc, 1 );

        CPPUNIT_ASSERT_NO_THROW( a->getLine( 2, pos, 0, line2 ) );
        CPPUNIT_ASSERT( !line2.isNull() );
        CPPUNIT_ASSERT_EQUAL( 1, line2->getDims() );
        CPPUNIT_ASSERT_EQUAL( index_t( 2 ), line2->getSizeX() );
        CPPUNIT_ASSERT_EQUAL( 4, (*line2)[ 0 ] );
        CPPUNIT_ASSERT_EQUAL( 5, (*line2)[ 1 ] );

        CPPUNIT_ASSERT_NO_THROW( a->getLine( 2, pos, 1, line2 ) );
        CPPUNIT_ASSERT( !line2.isNull() );
        CPPUNIT_ASSERT_EQUAL( 1, line2->getDims() );
        CPPUNIT_ASSERT_EQUAL( index_t( 4 ), line2->getSizeX() );
        CPPUNIT_ASSERT_EQUAL(  4, (*line2)[ 0 ] );
        CPPUNIT_ASSERT_EQUAL(  7, (*line2)[ 1 ] );
        CPPUNIT_ASSERT_EQUAL( 10, (*line2)[ 2 ] );
        CPPUNIT_ASSERT_EQUAL( 13, (*line2)[ 3 ] );

      }
    }
  }

  void test_reserve( void )
  {
    GarbageCollector gc;
    typedef Array< int > IntArray;

    {
      CountPtr< IntArray > a = new IntArray( &gc, 1, 5 );

      for( index_t x=0; x<5; ++x )
      {
        a->at( x ) = x;
      }

      const IntArray::const_pointer old_elements = a->elements();
      a->reserve( 8 );
      const IntArray::const_pointer new_elements = a->elements();

      CPPUNIT_ASSERT( old_elements != new_elements );

      for( index_t x=0; x<5; ++x )
      {
        CPPUNIT_ASSERT_EQUAL( int( x ), a->at( x ) );
      }
    }

    {
      CountPtr< IntArray > a = new IntArray( &gc, 2, 5, 7 );
      CountPtr< IntArray > b = new IntArray( *a );

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
        const IntArray::const_pointer old_elements = a->elements();
        a->resize( 11, 13 );
        const IntArray::const_pointer new_elements = a->elements();
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
        const IntArray::const_pointer old_elements = a->elements();
        a->resize( 5, 6 );
        const IntArray::const_pointer new_elements = a->elements();
        CPPUNIT_ASSERT_EQUAL( old_elements, new_elements );
      }

      a->fill( 71 );
      {
        const IntArray::const_pointer old_elements = a->elements();
        a->resize( 11, 13 );
        const IntArray::const_pointer new_elements = a->elements();
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
    typedef Array< int > IntArray;

    // mirror
    {
      CountPtr< IntArray > a = new IntArray( &gc, 2, 5, 7 );

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

      CountPtr< IntArray > b = new IntArray( (*a) );
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
        CountPtr< IntArray::ConstIterator > iter = b->getConstIterator();
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
      CountPtr< IntArray > a = new IntArray( &gc, 2, 5, 7 );

      for( index_t y=0; y<7; ++y )
      for( index_t x=0; x<5; ++x )
      {
        a->at( x, y ) = x+y*5;
      }

      CPPUNIT_ASSERT_THROW( a->setSparse( -1, 1 ), ArrayBase::Exception );
      CPPUNIT_ASSERT_THROW( a->setSparse( 2, 1 ), ArrayBase::Exception );
      CPPUNIT_ASSERT_THROW( a->setSparse( 0, 0 ), ArrayBase::Exception );
      CPPUNIT_ASSERT_THROW( a->setSparse( 0, 1, 1 ), ArrayBase::Exception );

      CountPtr< IntArray > b = new IntArray( (*a) );
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
        CountPtr< IntArray::ConstIterator > iter = b->getConstIterator();
        CPPUNIT_ASSERT( !iter->done() ); CPPUNIT_ASSERT_EQUAL( a->at( 0, 0 ), iter->get() ); iter->next();
        CPPUNIT_ASSERT( !iter->done() ); CPPUNIT_ASSERT_EQUAL( a->at( 2, 0 ), iter->get() ); iter->next();
        CPPUNIT_ASSERT( !iter->done() ); CPPUNIT_ASSERT_EQUAL( a->at( 0, 3 ), iter->get() ); iter->next();
        CPPUNIT_ASSERT( !iter->done() ); CPPUNIT_ASSERT_EQUAL( a->at( 2, 3 ), iter->get() ); iter->next();
        CPPUNIT_ASSERT( iter->done() );
      }

      CountPtr< IntArray > c = new IntArray( (*a) );

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
        CountPtr< IntArray::ConstIterator > iter = c->getConstIterator();
        CPPUNIT_ASSERT( !iter->done() ); CPPUNIT_ASSERT_EQUAL( a->at( 1, 2 ), iter->get() ); iter->next();
        CPPUNIT_ASSERT( !iter->done() ); CPPUNIT_ASSERT_EQUAL( a->at( 3, 2 ), iter->get() ); iter->next();
        CPPUNIT_ASSERT( !iter->done() ); CPPUNIT_ASSERT_EQUAL( a->at( 1, 5 ), iter->get() ); iter->next();
        CPPUNIT_ASSERT( !iter->done() ); CPPUNIT_ASSERT_EQUAL( a->at( 3, 5 ), iter->get() ); iter->next();
        CPPUNIT_ASSERT( iter->done() );
      }

    }

    // sparse mirror
    {
      CountPtr< IntArray > a = new IntArray( &gc, 2, 5, 7 );
      for( index_t y=0; y<7; ++y )
      for( index_t x=0; x<5; ++x )
      {
        a->at( x, y ) = x+y*5;
      }

      CountPtr< IntArray > b[ 4 ];

      b[ 0 ] = new IntArray( (*a) );
      b[ 0 ]->
         setSparse( 0, 2 )
        .setSparse( 1, 3 )
        .setMirrored( 0 )
        .setMirrored( 1 )
        ;
      b[ 1 ] = new IntArray( (*a) );
      b[ 1 ]->
         setSparse( 1, 3 )
        .setSparse( 0, 2 )
        .setMirrored( 0 )
        .setMirrored( 1 )
        ;
      b[ 2 ] = new IntArray( (*a) );
      b[ 2 ]->
         setSparse( 0, 2 )
        .setSparse( 1, 3 )
        .setMirrored( 1 )
        .setMirrored( 0 )
        ;
      b[ 3 ] = new IntArray( (*a) );
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
      CountPtr< IntArray > a = new IntArray( &gc, 2, 5, 7 );
      for( index_t y=0; y<7; ++y )
      for( index_t x=0; x<5; ++x )
      {
        a->at( x, y ) = x+y*5;
      }

      CountPtr< IntArray > b = new IntArray( (*a) );

      CPPUNIT_ASSERT_THROW( b->setRotated( 0, -1,  0 ), ArrayBase::Exception );
      CPPUNIT_ASSERT_THROW( b->setRotated( 0,  2,  0 ), ArrayBase::Exception );
      CPPUNIT_ASSERT_THROW( b->setRotated( 0,  0, -1 ), ArrayBase::Exception );
      CPPUNIT_ASSERT_THROW( b->setRotated( 0,  0,  2 ), ArrayBase::Exception );
      CPPUNIT_ASSERT_THROW( b->setRotated( 0,  0,  0 ), ArrayBase::Exception );

      CPPUNIT_ASSERT_NO_THROW( b->setRotated( 0 ) );
      {
        CountPtr< IntArray > c = new IntArray( (*a) );
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
        CountPtr< IntArray > c = new IntArray( (*a) );
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
        CountPtr< IntArray > c = new IntArray( (*a) );
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
        CountPtr< IntArray > c = new IntArray( (*a) );
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
      CountPtr< IntArray > a = new IntArray( &gc, 2, 10, 10 );
      for( index_t y=0; y<10; ++y )
      for( index_t x=0; x<10; ++x )
      {
        a->at( x, y ) = x+y*10;
      }

      const int *e_first = &a->at( 0, 0 );
      const int *e_last  = &a->at( 9, 9 );

      {
        CountPtr< IntArray > b = a->getROI( 3, 3, 5, 5 );

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
        CountPtr< IntArray > b = a->getROI( 3, 3, 5, 5 );

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
        CountPtr< IntArray > b = a->getROI( 3, 3, 5, 5 );

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
        CountPtr< IntArray > b = a->getROI( 3, 3, 5, 5 );

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
        CountPtr< IntArray > b = a->getROI( 4, 4, 4, 4 );

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
    typedef Array< bool > BoolArray;

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

  void test_Array_bool( void )
  {
    GarbageCollector gc;

    typedef Array< bool > BoolArray;

    {
      CountPtr< BoolArray > a = new BoolArray( &gc, 0 );

      (**a) = true;

      {
        BoolArray::iterator iter = a->begin();
        BoolArray::iterator end  = a->end();

        CPPUNIT_ASSERT( iter != end );
        CPPUNIT_ASSERT_EQUAL( true, bool(*iter) );

        (*iter) = false;

        ++iter;
        CPPUNIT_ASSERT( iter == end );
      }

      {
        BoolArray::const_iterator iter = a->begin();
        BoolArray::const_iterator end  = a->end();

        CPPUNIT_ASSERT( iter != end );
        CPPUNIT_ASSERT_EQUAL( false, bool(*iter) );

        ++iter;
        CPPUNIT_ASSERT( iter == end );
      }

    }

    {
      CountPtr< BoolArray > a = new BoolArray( &gc, 2, 3, 5 );

      const uint32_t bits  = 0x519ea375;
      const uint32_t bits2 = 0xa375519e;

      for( index_t y=0, i=0; y<5; ++y )
      for( index_t x=0     ; x<3; ++x, ++i )
      {
        a->at( x, y ) = bool( ( bits >> i ) & 1 );
      }

      {
        BoolArray::iterator iter = a->begin();
        BoolArray::iterator end  = a->end();

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
        BoolArray::const_iterator iter = a->begin();
        BoolArray::const_iterator end  = a->end();

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

      CountPtr< BoolArray > a = new BoolArray( &gc, 2, 10, 10 );
      for( index_t y=0; y<10; ++y )
      for( index_t x=0; x<10; ++x )
      {
        a->at( x, y ) = bits[ x+y*10 ];
      }

      BoolArray::const_pointer e_first = &a->at( 0, 0 );
      BoolArray::const_pointer e_last  = &a->at( 9, 9 );

      {
        CountPtr< BoolArray > b = a->getROI( 3, 3, 5, 5 );

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
        CountPtr< BoolArray > b = a->getROI( 3, 3, 5, 5 );

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
        CountPtr< BoolArray > b = a->getROI( 3, 3, 5, 5 );

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
        CountPtr< BoolArray > b = a->getROI( 3, 3, 5, 5 );

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
        CountPtr< BoolArray > b = a->getROI( 4, 4, 4, 4 );

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

  void test_random_access_iterator( void )
  {
    GarbageCollector gc;

    {
      CountPtr< IntArray > b = new IntArray( &gc, 2, 13, 17 );
      CountPtr< IntArray > a = b->getROI( 2, 3, 10, 10 );

      for( index_t y=0; y<10; ++y )
      for( index_t x=0; x<10; ++x )
      {
        a->at( x, y ) = x+y*10;
      }

      {
        Array< int >::const_iterator iter1 = a->begin();
        Array< int >::const_iterator iter2 = a->begin();
        Array< int >::const_iterator iter3 = a->end();
        Array< int >::const_iterator iter4 = a->end();
        for( int i=0; i<7; ++i )
        {
          CPPUNIT_ASSERT_EQUAL( i*7, iter1[ i*7 ] );
          CPPUNIT_ASSERT_EQUAL( i*11, iter1[ i*11 ] );

          CPPUNIT_ASSERT_EQUAL( i* 7, *( iter1 + i* 7 ) );
          CPPUNIT_ASSERT_EQUAL( i*11, *( iter1 + i*11 ) );
          CPPUNIT_ASSERT_EQUAL( i* 7, *( i* 7 + iter1 ) );
          CPPUNIT_ASSERT_EQUAL( i*11, *( i*11 + iter1 ) );

          CPPUNIT_ASSERT_EQUAL( i*5, *iter2 );
          iter2 += 5;

          CPPUNIT_ASSERT_EQUAL( 100 - (i+1)*11, iter3[ -(i+1)*11 ] );
          CPPUNIT_ASSERT_EQUAL( 100 - (i+1)*7, iter3[ -(i+1)*7 ] );
          CPPUNIT_ASSERT_EQUAL( 100 - (i+1)*11, *( iter3 - (i+1)*11 ) );
          CPPUNIT_ASSERT_EQUAL( 100 - (i+1)*7, *( iter3 - (i+1)*7 ) );

          iter4 -= 13;
          CPPUNIT_ASSERT_EQUAL( 100 - (i+1)*13, *iter4 );
        }
      }

      {
        Array< int >::iterator iter1 = a->begin();
        Array< int >::iterator iter2 = a->begin();
        Array< int >::iterator iter3 = a->end();
        Array< int >::iterator iter4 = a->end();
        for( int i=0; i<7; ++i )
        {
          CPPUNIT_ASSERT_EQUAL( i*7, iter1[ i*7 ] );
          CPPUNIT_ASSERT_EQUAL( i*11, iter1[ i*11 ] );

          CPPUNIT_ASSERT_EQUAL( i* 7, *( iter1 + i* 7 ) );
          CPPUNIT_ASSERT_EQUAL( i*11, *( iter1 + i*11 ) );
          CPPUNIT_ASSERT_EQUAL( i* 7, *( i* 7 + iter1 ) );
          CPPUNIT_ASSERT_EQUAL( i*11, *( i*11 + iter1 ) );

          CPPUNIT_ASSERT_EQUAL( i*5, *iter2 );
          iter2 += 5;

          CPPUNIT_ASSERT_EQUAL( 100 - (i+1)*11, iter3[ -(i+1)*11 ] );
          CPPUNIT_ASSERT_EQUAL( 100 - (i+1)*7, iter3[ -(i+1)*7 ] );
          CPPUNIT_ASSERT_EQUAL( 100 - (i+1)*11, *( iter3 - (i+1)*11 ) );
          CPPUNIT_ASSERT_EQUAL( 100 - (i+1)*7, *( iter3 - (i+1)*7 ) );

          iter4 -= 13;
          CPPUNIT_ASSERT_EQUAL( 100 - (i+1)*13, *iter4 );
        }
      }
    }

    {
      bool bits[ 100 ];
      unsigned int seed = 42;
      for( int i=0; i<100; ++i )
      {
        bits[ i ] = bool( rand_r( &seed ) & 1 );
      }

      CountPtr< BoolArray > b = new BoolArray( &gc, 2, 13, 17 );
      CountPtr< BoolArray > a = b->getROI( 2, 3, 10, 10 );

      for( index_t y=0; y<10; ++y )
      for( index_t x=0; x<10; ++x )
      {
        a->at( x, y ) = bits[ x+y*10 ];
      }

      {
        BoolArray::const_iterator iter1 = a->begin();
        BoolArray::const_iterator iter2 = a->begin();
        BoolArray::const_iterator iter3 = a->end();
        BoolArray::const_iterator iter4 = a->end();
        for( int i=0; i<7; ++i )
        {
          CPPUNIT_ASSERT_EQUAL( bits[ i*7 ], bool( iter1[ i*7 ] ) );
          CPPUNIT_ASSERT_EQUAL( bits[ i*11 ], bool( iter1[ i*11 ] ) );

          CPPUNIT_ASSERT_EQUAL( bits[ i* 7 ], bool( *( iter1 + i* 7 ) ) );
          CPPUNIT_ASSERT_EQUAL( bits[ i*11 ], bool( *( iter1 + i*11 ) ) );
          CPPUNIT_ASSERT_EQUAL( bits[ i* 7 ], bool( *( i* 7 + iter1 ) ) );
          CPPUNIT_ASSERT_EQUAL( bits[ i*11 ], bool( *( i*11 + iter1 ) ) );

          CPPUNIT_ASSERT_EQUAL( bits[ i*5 ], bool( *iter2 ) );
          iter2 += 5;

          CPPUNIT_ASSERT_EQUAL( bits[ 100 - (i+1)*11 ], bool( iter3[ -(i+1)*11 ] ) );
          CPPUNIT_ASSERT_EQUAL( bits[ 100 - (i+1)* 7 ], bool( iter3[ -(i+1)*7 ] ) );
          CPPUNIT_ASSERT_EQUAL( bits[ 100 - (i+1)*11 ], bool( *( iter3 - (i+1)*11 ) ) );
          CPPUNIT_ASSERT_EQUAL( bits[ 100 - (i+1)* 7 ], bool( *( iter3 - (i+1)*7 ) ) );

          iter4 -= 13;
          CPPUNIT_ASSERT_EQUAL( bits[ 100 - (i+1)*13 ], bool(*iter4 ) );
        }
      }

      {
        BoolArray::iterator iter1 = a->begin();
        BoolArray::iterator iter2 = a->begin();
        BoolArray::iterator iter3 = a->end();
        BoolArray::iterator iter4 = a->end();
        for( int i=0; i<7; ++i )
        {
          CPPUNIT_ASSERT_EQUAL( bits[ i*7 ], bool( iter1[ i*7 ] ) );
          CPPUNIT_ASSERT_EQUAL( bits[ i*11 ], bool( iter1[ i*11 ] ) );

          CPPUNIT_ASSERT_EQUAL( bits[ i* 7 ], bool( *( iter1 + i* 7 ) ) );
          CPPUNIT_ASSERT_EQUAL( bits[ i*11 ], bool( *( iter1 + i*11 ) ) );
          CPPUNIT_ASSERT_EQUAL( bits[ i* 7 ], bool( *( i* 7 + iter1 ) ) );
          CPPUNIT_ASSERT_EQUAL( bits[ i*11 ], bool( *( i*11 + iter1 ) ) );

          CPPUNIT_ASSERT_EQUAL( bits[ i*5 ], bool( *iter2 ) );
          iter2 += 5;

          CPPUNIT_ASSERT_EQUAL( bits[ 100 - (i+1)*11 ], bool( iter3[ -(i+1)*11 ] ) );
          CPPUNIT_ASSERT_EQUAL( bits[ 100 - (i+1)* 7 ], bool( iter3[ -(i+1)*7 ] ) );
          CPPUNIT_ASSERT_EQUAL( bits[ 100 - (i+1)*11 ], bool( *( iter3 - (i+1)*11 ) ) );
          CPPUNIT_ASSERT_EQUAL( bits[ 100 - (i+1)* 7 ], bool( *( iter3 - (i+1)*7 ) ) );

          iter4 -= 13;
          CPPUNIT_ASSERT_EQUAL( bits[ 100 - (i+1)*13 ], bool(*iter4 ) );
        }
      }
    }
  }

  void test_wrapper( void )
  {
    GarbageCollector gc;

    int data[ 8*4 ] =
    {
       0,  1,  2,  3,  4,  5,  6,  7,
       8,  9, 10, 11, 12, 13, 14, 15,
      16, 17, 18, 19, 20, 21, 22, 23,
      24, 25, 26, 27, 28, 29, 30, 31
    };

    {
      const index_t size[ 1 ] = { 4 };
      CountPtr< IntArray > a = new IntArray( &gc, data, 1, size );

      CPPUNIT_ASSERT_EQUAL( 1, a->getDims() );
      CPPUNIT_ASSERT_EQUAL( index_t( 4 ), a->getSizeX() );
      CPPUNIT_ASSERT_EQUAL( 0, (*a)[ 0 ] );
      CPPUNIT_ASSERT_EQUAL( 1, (*a)[ 1 ] );
      CPPUNIT_ASSERT_EQUAL( 2, (*a)[ 2 ] );
      CPPUNIT_ASSERT_EQUAL( 3, (*a)[ 3 ] );
    }

    {
      const index_t size  [ 2 ] = { 3, 2 };
      const index_t stride[ 2 ] = { 3, 16 };
      CountPtr< IntArray > a = new IntArray( &gc, data, 2, size, stride );

      CPPUNIT_ASSERT_EQUAL( 2, a->getDims() );
      CPPUNIT_ASSERT_EQUAL( index_t( 3 ), a->getSizeX() );
      CPPUNIT_ASSERT_EQUAL( index_t( 2 ), a->getSizeY() );
      CPPUNIT_ASSERT_EQUAL( 0, a->at( 0, 0 ) );
      CPPUNIT_ASSERT_EQUAL( 3, a->at( 1, 0 ) );
      CPPUNIT_ASSERT_EQUAL( 6, a->at( 2, 0 ) );
      CPPUNIT_ASSERT_EQUAL( 16, a->at( 0, 1 ) );
      CPPUNIT_ASSERT_EQUAL( 19, a->at( 1, 1 ) );
      CPPUNIT_ASSERT_EQUAL( 22, a->at( 2, 1 ) );
    }

    {
      const index_t size[ 1 ] = { 4 };
      CountPtr< IntArray > a = new IntArray( &gc, 1 );
      a->wrap( data, 1, size );

      CPPUNIT_ASSERT_EQUAL( 1, a->getDims() );
      CPPUNIT_ASSERT_EQUAL( index_t( 4 ), a->getSizeX() );
      CPPUNIT_ASSERT_EQUAL( 0, (*a)[ 0 ] );
      CPPUNIT_ASSERT_EQUAL( 1, (*a)[ 1 ] );
      CPPUNIT_ASSERT_EQUAL( 2, (*a)[ 2 ] );
      CPPUNIT_ASSERT_EQUAL( 3, (*a)[ 3 ] );
    }

    {
      const index_t size  [ 2 ] = { 3, 2 };
      const index_t stride[ 2 ] = { 3, 16 };
      CountPtr< IntArray > a = new IntArray( &gc, 2 );
      a->wrap( data, 2, size, stride );

      CPPUNIT_ASSERT_EQUAL( 2, a->getDims() );
      CPPUNIT_ASSERT_EQUAL( index_t( 3 ), a->getSizeX() );
      CPPUNIT_ASSERT_EQUAL( index_t( 2 ), a->getSizeY() );
      CPPUNIT_ASSERT_EQUAL( 0, a->at( 0, 0 ) );
      CPPUNIT_ASSERT_EQUAL( 3, a->at( 1, 0 ) );
      CPPUNIT_ASSERT_EQUAL( 6, a->at( 2, 0 ) );
      CPPUNIT_ASSERT_EQUAL( 16, a->at( 0, 1 ) );
      CPPUNIT_ASSERT_EQUAL( 19, a->at( 1, 1 ) );
      CPPUNIT_ASSERT_EQUAL( 22, a->at( 2, 1 ) );
    }
  }
};

CPPUNIT_TEST_SUITE_REGISTRATION( utest_Array );


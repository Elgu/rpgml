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

#include <RPGML/iterator_base.h>

#include <iostream>
#include <sstream>

using namespace RPGML;
using namespace std;

class utest_iterator_base : public CppUnit::TestFixture
{
  CPPUNIT_TEST_SUITE( utest_iterator_base );

  CPPUNIT_TEST( test_ctor );
  CPPUNIT_TEST( test_inc );
  CPPUNIT_TEST( test_sort_operations );

  CPPUNIT_TEST_SUITE_END();

public:
  void setUp() {}
  void tearDown() {}

  void test_ctor( void )
  {
    typedef iterator_base< int > int_iterator;

    // default
    {
      int_iterator iter;

      ostringstream o;
      o << iter;
      CPPUNIT_ASSERT_EQUAL( string( "0 :[  ] @[  ] +[  ]" ), o.str() );
    }

    static const index_t  size  [ 4 ] = { 3, 7,  2,  5 };
    static const stride_t stride[ 4 ] = { 1, 3, 21, 42 };

    // 0d
    {
      static const index_t pos[ 4 ] = { 0, 0, 0, 0 };

      int_iterator iter( 0, size, stride, (int*)23 );

      ostringstream o;
      o << iter;
      CPPUNIT_ASSERT_EQUAL( string( "0x17 :[  ] @[  ] +[  ]" ), o.str() );
    }

    // 1d, origin
    {
      static const index_t pos[ 4 ] = { 0, 0, 0, 0 };

      int_iterator iter( 1, size, stride, (int*)23, pos );

      ostringstream o;
      o << iter;
      CPPUNIT_ASSERT_EQUAL( string( "0x17 :[ 3 ] @[ 0 ] +[ 1 ]" ), o.str() );
    }

    // 1d, pos
    {
      static const index_t pos[ 4 ] = { 2, 0, 0, 0 };

      int_iterator iter( 1, size, stride, (int*)23, pos );

      ostringstream o;
      o << iter;
      CPPUNIT_ASSERT_EQUAL( string( "0x1f :[ 3 ] @[ 2 ] +[ 1 ]" ), o.str() );
    }

    // 2d, origin
    {
      static const index_t pos[ 4 ] = { 0, 0, 0, 0 };

      int_iterator iter( 2, size, stride, (int*)23, pos );

      ostringstream o;
      o << iter;
      CPPUNIT_ASSERT_EQUAL( string( "0x17 :[ 3, 7 ] @[ 0, 0 ] +[ 1, 3 ]" ), o.str() );
    }

    // 2d, pos
    {
      static const index_t pos[ 4 ] = { 1, 2, 0, 0 };

      int_iterator iter( 2, size, stride, (int*)23, pos );

      ostringstream o;
      o << iter;
      CPPUNIT_ASSERT_EQUAL( string( "0x33 :[ 3, 7 ] @[ 1, 2 ] +[ 1, 3 ]" ), o.str() );
    }

    // 3d, pos
    {
      static const index_t pos[ 4 ] = { 0, 3, 1, 0 };

      int_iterator iter( 3, size, stride, (int*)23, pos );

      ostringstream o;
      o << iter;
      CPPUNIT_ASSERT_EQUAL( string( "0x8f :[ 3, 7, 2 ] @[ 0, 3, 1 ] +[ 1, 3, 21 ]" ), o.str() );
    }

    // 4d, pos
    {
      static const index_t pos[ 4 ] = { 0, 3, 1, 2 };

      int_iterator iter( 4, size, stride, (int*)23, pos );

      ostringstream o;
      o << iter;
      CPPUNIT_ASSERT_EQUAL( string( "0x1df :[ 3, 7, 2, 5 ] @[ 0, 3, 1, 2 ] +[ 1, 3, 21, 42 ]" ), o.str() );
    }
  }

  void test_inc( void )
  {
    typedef iterator_base< int > int_iterator;

    static const index_t  size  [ 4 ] = {  3,  7,   2,  5 };
    static const stride_t stride[ 4 ] = { -2,  6, -42, 84 };

    // 0d
    {
      int_iterator iter;
      CPPUNIT_ASSERT_EQUAL( (int*)0, iter.get() );
      CPPUNIT_ASSERT_NO_THROW( iter.inc() );
      CPPUNIT_ASSERT_EQUAL( (int*)4, iter.get() );
    }

    // 1d
    {
      static const index_t pos[ 4 ] = { 1, 0, 0, 0 };

      int_iterator iter( 1, size, stride, (int*)1000, pos );

      CPPUNIT_ASSERT_EQUAL( index_t( 1 ), iter.getPos( 0 ) );
      CPPUNIT_ASSERT_EQUAL( (int*)(1000+4*(1*-2)), iter.get() );
      CPPUNIT_ASSERT_NO_THROW( iter.inc() );
      CPPUNIT_ASSERT_EQUAL( index_t( 2 ), iter.getPos( 0 ) );
      CPPUNIT_ASSERT_EQUAL( (int*)(1000+4*(2*-2)), iter.get() );
    }

    // 2d
    {
      static const index_t pos1[ 4 ] = { 1, 3, 0, 0 };
      static const index_t pos2[ 4 ] = { 2, 3, 0, 0 };
      static const index_t pos3[ 4 ] = { 2, 6, 0, 0 };

      int_iterator iter1( 2, size, stride, (int*)1000, pos1 );
      int_iterator iter2( 2, size, stride, (int*)1000, pos2 );
      int_iterator iter3( 2, size, stride, (int*)1000, pos3 );

      CPPUNIT_ASSERT_EQUAL( index_t( 1 ), iter1.getPos( 0 ) );
      CPPUNIT_ASSERT_EQUAL( index_t( 3 ), iter1.getPos( 1 ) );
      CPPUNIT_ASSERT_EQUAL( index_t( 2 ), iter2.getPos( 0 ) );
      CPPUNIT_ASSERT_EQUAL( index_t( 3 ), iter2.getPos( 1 ) );
      CPPUNIT_ASSERT_EQUAL( index_t( 2 ), iter3.getPos( 0 ) );
      CPPUNIT_ASSERT_EQUAL( index_t( 6 ), iter3.getPos( 1 ) );
      CPPUNIT_ASSERT_EQUAL( (int*)(1000+4*(1*-2 + 3*6)), iter1.get() );
      CPPUNIT_ASSERT_EQUAL( (int*)(1000+4*(2*-2 + 3*6)), iter2.get() );
      CPPUNIT_ASSERT_EQUAL( (int*)(1000+4*(2*-2 + 6*6)), iter3.get() );

      CPPUNIT_ASSERT_NO_THROW( iter1.inc() );
      CPPUNIT_ASSERT_NO_THROW( iter2.inc() );
      CPPUNIT_ASSERT_NO_THROW( iter3.inc() );

      CPPUNIT_ASSERT_EQUAL( index_t( 2 ), iter1.getPos( 0 ) );
      CPPUNIT_ASSERT_EQUAL( index_t( 3 ), iter1.getPos( 1 ) );
      CPPUNIT_ASSERT_EQUAL( index_t( 0 ), iter2.getPos( 0 ) );
      CPPUNIT_ASSERT_EQUAL( index_t( 4 ), iter2.getPos( 1 ) );
      CPPUNIT_ASSERT_EQUAL( index_t( 0 ), iter3.getPos( 0 ) );
      CPPUNIT_ASSERT_EQUAL( index_t( 7 ), iter3.getPos( 1 ) );
      CPPUNIT_ASSERT_EQUAL( (int*)(1000+4*(2*-2 + 3*6)), iter1.get() );
      CPPUNIT_ASSERT_EQUAL( (int*)(1000+4*(0*-2 + 4*6)), iter2.get() );
      CPPUNIT_ASSERT_EQUAL( (int*)(1000+4*(0*-2 + 7*6)), iter3.get() );
    }
  }

  void test_sort_operations( void )
  {
    typedef iterator_base< int > int_iterator;

    vector< int > elements( 4 );
    const index_t size = 4;
    const stride_t stride = 1;

    {
      elements[ 0 ] = 4;
      elements[ 1 ] = 3;
      elements[ 2 ] = 2;
      elements[ 3 ] = 1;

      int_iterator a( 1, &size, &stride, &elements[ 0 ] );
      int_iterator b( a );

      CPPUNIT_ASSERT_EQUAL( a, b );
      CPPUNIT_ASSERT_EQUAL( 4, (*a) );

      CPPUNIT_ASSERT_NO_THROW( a = a + 1 );
      CPPUNIT_ASSERT_EQUAL( 3, (*a) );
      CPPUNIT_ASSERT_EQUAL( b+1, a );

      CPPUNIT_ASSERT_NO_THROW( swap( a, b ) );
      CPPUNIT_ASSERT_EQUAL( b, a+1 );
      CPPUNIT_ASSERT_EQUAL( 4, (*a) );
      CPPUNIT_ASSERT_EQUAL( 3, (*b) );
      CPPUNIT_ASSERT_EQUAL( 4, elements[ 0 ] );
      CPPUNIT_ASSERT_EQUAL( 3, elements[ 1 ] );

      CPPUNIT_ASSERT_NO_THROW( swap( (*a), (*b) ) );
      CPPUNIT_ASSERT_EQUAL( b, a+1 );
      CPPUNIT_ASSERT_EQUAL( 3, (*a) );
      CPPUNIT_ASSERT_EQUAL( 4, (*b) );
      CPPUNIT_ASSERT_EQUAL( 3, elements[ 0 ] );
      CPPUNIT_ASSERT_EQUAL( 4, elements[ 1 ] );






    }



    {
      elements[ 0 ] = 4;
      elements[ 1 ] = 3;
      elements[ 2 ] = 2;
      elements[ 3 ] = 1;

      const index_t end_pos = 4;
      int_iterator begin( 1, &size, &stride, &elements[ 0 ] );
      int_iterator end  ( 1, &size, &stride, &elements[ 0 ], &end_pos );

      sort( begin, end );

      CPPUNIT_ASSERT_EQUAL( 1, elements[ 0 ] );
      CPPUNIT_ASSERT_EQUAL( 2, elements[ 1 ] );
      CPPUNIT_ASSERT_EQUAL( 3, elements[ 2 ] );
      CPPUNIT_ASSERT_EQUAL( 4, elements[ 3 ] );
    }
  }
};

CPPUNIT_TEST_SUITE_REGISTRATION( utest_iterator_base );


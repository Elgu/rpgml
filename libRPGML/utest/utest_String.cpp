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

#include <RPGML/String.h>

#include <iostream>

using namespace RPGML;
using namespace std;

class utest_String : public CppUnit::TestFixture
{
  CPPUNIT_TEST_SUITE( utest_String );

  CPPUNIT_TEST( test_StringData );
  CPPUNIT_TEST( test_MallocString );
  CPPUNIT_TEST( test_StdString );
  CPPUNIT_TEST( test_String );

  CPPUNIT_TEST_SUITE_END();

public:
  void setUp() {}
  void tearDown() {}

  void test_StringData( void )
  {
    {
      StringData s;
      CPPUNIT_ASSERT_EQUAL( (const char*)0, s.get() );
    }

    {
      const char *const foo = "foo";
      StringData s( foo );
      CPPUNIT_ASSERT_EQUAL( foo, s.get() );
    }
  }

  void test_MallocString ( void )
  {
    const char *const foo = "foo";
    const char *const yolo = "yolo";

    {
      CountPtr< const StringData > s = MallocString::create( (const char*)0, 0 );
      CPPUNIT_ASSERT_EQUAL( '\0', s->get()[ 0 ] );
    }

    {
      CountPtr< const StringData > s = MallocString::create( foo, 3 );
      CPPUNIT_ASSERT_EQUAL( 0, strncmp( "foo", s->get(), 3 ) );
      CPPUNIT_ASSERT_EQUAL( '\0', s->get()[ 3 ] );
      CPPUNIT_ASSERT( foo != s->get() );
    }

    {
      CountPtr< const StringData > s = MallocString::create( foo, 2 );
      CPPUNIT_ASSERT_EQUAL( 0, strncmp( "foo", s->get(), 2 ) );
      CPPUNIT_ASSERT_EQUAL( '\0', s->get()[ 2 ] );
    }

    {
      CountPtr< const StringData > s = MallocString::create( foo, 3, yolo, 4 );
      CPPUNIT_ASSERT_EQUAL( 0, strncmp( "fooyolo", s->get(), 7 ) );
      CPPUNIT_ASSERT_EQUAL( '\0', s->get()[ 7 ] );
    }

    {
      CountPtr< const StringData > s = MallocString::create( foo, 1, yolo, 3 );
      CPPUNIT_ASSERT_EQUAL( 0, strncmp( "fyol", s->get(), 4 ) );
      CPPUNIT_ASSERT_EQUAL( '\0', s->get()[ 4 ] );
    }
  }

  void test_StdString ( void )
  {
    const char *const foo = "foo";
//    string yolo( "yolo" ); this test does not work with this because of std::string static-string optimization
    string yolo( "xxxx" );
    yolo[ 0 ] = 'y';
    yolo[ 1 ] = 'o';
    yolo[ 2 ] = 'l';
    yolo[ 3 ] = 'o';

    {
      CountPtr< const StringData > s = new StdString();
      CPPUNIT_ASSERT_EQUAL( '\0', s->get()[ 0 ] );
    }

    {
      CountPtr< const StringData > s = new StdString( foo );
      CPPUNIT_ASSERT_EQUAL( 0, strncmp( "foo", s->get(), 3 ) );
      CPPUNIT_ASSERT_EQUAL( '\0', s->get()[ 3 ] );
      CPPUNIT_ASSERT( foo != s->get() );
    }

    {
      CountPtr< const StringData > s = new StdString( yolo );
      CPPUNIT_ASSERT_EQUAL( 0, strncmp( "yolo", s->get(), 4 ) );
      CPPUNIT_ASSERT_EQUAL( '\0', s->get()[ 4 ] );
//      cerr << "yolo.c_str() = " << (void*)yolo.c_str() << ", s->get() = " << (void*)s->get() << endl;
      CPPUNIT_ASSERT( yolo.c_str() != s->get() );
    }

    // must be last, destroys yolo
    {
      CountPtr< StdString > s = new StdString();
      CPPUNIT_ASSERT_EQUAL( '\0', s->get()[ 0 ] );

      const char *const yolo_p = yolo.c_str();

      s->moveFrom( yolo );

      CPPUNIT_ASSERT( yolo.empty() );

      CPPUNIT_ASSERT_EQUAL( 0, strncmp( "yolo", s->get(), 4 ) );
      CPPUNIT_ASSERT_EQUAL( '\0', s->get()[ 4 ] );
      CPPUNIT_ASSERT_EQUAL( yolo_p, s->get() );
    }
  }

  void test_String ( void )
  {
    {
      String s;
      CPPUNIT_ASSERT_EQUAL( size_t(0), s.length() );
      CPPUNIT_ASSERT_EQUAL( size_t(0), s.size() );
      CPPUNIT_ASSERT( s.empty() );
      CPPUNIT_ASSERT_EQUAL( '\0', s.get()[ 0 ] );
      CPPUNIT_ASSERT( !s.getData().isNull() );
      CPPUNIT_ASSERT_EQUAL( 0, s.compare( "" ) );
      CPPUNIT_ASSERT( s.equals( "" ) );
    }

    {
      CountPtr< const StringData > foo = MallocString::create( "foo", 3 );

      String s( foo );

      CPPUNIT_ASSERT_EQUAL( size_t(3), s.length() );
      CPPUNIT_ASSERT_EQUAL( size_t(3), s.size() );
      CPPUNIT_ASSERT( !s.empty() );
      CPPUNIT_ASSERT_EQUAL( '\0', s.get()[ 3 ] );
      CPPUNIT_ASSERT_EQUAL( foo->get(), s.get() );
      CPPUNIT_ASSERT_EQUAL( foo.get(), s.getData().get() );
      CPPUNIT_ASSERT_EQUAL( 0, s.compare( "foo" ) );
      CPPUNIT_ASSERT( s.equals( "foo" ) );

      String s2( s );

      CPPUNIT_ASSERT_EQUAL( size_t(3), s2.length() );
      CPPUNIT_ASSERT_EQUAL( size_t(3), s2.size() );
      CPPUNIT_ASSERT( !s2.empty() );
      CPPUNIT_ASSERT_EQUAL( '\0', s2.get()[ 3 ] );
      CPPUNIT_ASSERT_EQUAL( foo->get(), s2.get() );
      CPPUNIT_ASSERT_EQUAL( foo.get(), s2.getData().get() );
      CPPUNIT_ASSERT_EQUAL( 0, s2.compare( "foo" ) );
      CPPUNIT_ASSERT( s2.equals( "foo" ) );
    }

    {
      const char *const foo = "foo";
      const char *const yolo = "yolo";

      {
        String s( foo, 3 );

        CPPUNIT_ASSERT_EQUAL( size_t(3), s.length() );
        CPPUNIT_ASSERT_EQUAL( size_t(3), s.size() );
        CPPUNIT_ASSERT( !s.empty() );
        CPPUNIT_ASSERT_EQUAL( '\0', s.get()[ 3 ] );
        CPPUNIT_ASSERT( foo != s.get() );
        CPPUNIT_ASSERT_EQUAL( 0, s.compare( "foo" ) );
        CPPUNIT_ASSERT( s.equals( "foo" ) );
      }

      {
        String s( foo, 2 );

        CPPUNIT_ASSERT_EQUAL( size_t(2), s.length() );
        CPPUNIT_ASSERT_EQUAL( size_t(2), s.size() );
        CPPUNIT_ASSERT( !s.empty() );
        CPPUNIT_ASSERT_EQUAL( '\0', s.get()[ 2 ] );
        CPPUNIT_ASSERT( foo != s.get() );
        CPPUNIT_ASSERT_EQUAL( 0, s.compare( "fo" ) );
        CPPUNIT_ASSERT( s.equals( "fo" ) );
      }

      {
        String s( foo, 2, yolo, 1 );

        CPPUNIT_ASSERT_EQUAL( size_t(3), s.length() );
        CPPUNIT_ASSERT_EQUAL( size_t(3), s.size() );
        CPPUNIT_ASSERT( !s.empty() );
        CPPUNIT_ASSERT_EQUAL( '\0', s.get()[ 3 ] );
        CPPUNIT_ASSERT( foo != s.get() );
        CPPUNIT_ASSERT_EQUAL( 0, s.compare( "foy" ) );
        CPPUNIT_ASSERT( s.equals( "foy" ) );
      }

      {
        String s( foo, yolo );

        CPPUNIT_ASSERT_EQUAL( size_t(7), s.length() );
        CPPUNIT_ASSERT_EQUAL( size_t(7), s.size() );
        CPPUNIT_ASSERT( !s.empty() );
        CPPUNIT_ASSERT_EQUAL( '\0', s.get()[ 7 ] );
        CPPUNIT_ASSERT( foo != s.get() );
        CPPUNIT_ASSERT_EQUAL( 0, s.compare( "fooyolo" ) );
        CPPUNIT_ASSERT( s.equals( "fooyolo" ) );
      }
    }

    String ABC( "abcdefghijklmnopqrstuvwxyz" );
    CPPUNIT_ASSERT_EQUAL( size_t(26), ABC.length() );

    // String::mid()
    {
      CPPUNIT_ASSERT_EQUAL( String(), ABC.mid( 4, 3 ) );
      CPPUNIT_ASSERT_EQUAL( String(), ABC.mid( 30, 31 ) );

      String abcd = ABC.mid( 0, 3 );
      CPPUNIT_ASSERT_EQUAL( size_t(4), abcd.length() );
      CPPUNIT_ASSERT_EQUAL( String( "abcd" ), abcd );

      String e = ABC.mid( 4, 4 );
      CPPUNIT_ASSERT_EQUAL( size_t(1), e.length() );
      CPPUNIT_ASSERT_EQUAL( String( "e" ), e );

      String xyz = ABC.mid( 23 );
      CPPUNIT_ASSERT_EQUAL( size_t(3), xyz.length() );
      CPPUNIT_ASSERT_EQUAL( String( "xyz" ), xyz );
    }

    // String::find()
    {
      CPPUNIT_ASSERT_EQUAL( String::npos, ABC.find( '?' ) );
      CPPUNIT_ASSERT_EQUAL( size_t(0), ABC.find( 'a' ) );
      CPPUNIT_ASSERT_EQUAL( size_t(25), ABC.find( 'z' ) );

      CPPUNIT_ASSERT_EQUAL( String::npos, ABC.find( 'a', 1 ) );
    }
  }
};

CPPUNIT_TEST_SUITE_REGISTRATION( utest_String );


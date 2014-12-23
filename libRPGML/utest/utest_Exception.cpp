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

#include <RPGML/Exception.h>

#include <iostream>

using namespace RPGML;
using namespace std;

class utest_Exception : public CppUnit::TestFixture
{
  EXCEPTION_BASE( Exception );
  EXCEPTION_DERIVED( DerivedException, Exception );
  EXCEPTION_DERIVED_FIXED_TEXT( DerivedExceptionFixedText, Exception );

  CPPUNIT_TEST_SUITE( utest_Exception );

  CPPUNIT_TEST( test_what );
  CPPUNIT_TEST( test_append );
  CPPUNIT_TEST( test_stream );
  CPPUNIT_TEST( test_derive_stream );

  CPPUNIT_TEST_SUITE_END();

public:
  void setUp() {}
  void tearDown() {}

  void test_what( void )
  {
    Exception e( "what" );
    CPPUNIT_ASSERT( std::string( "what" ) == e.what() );
  }

  void test_append( void )
  {
    Exception e( "what" );
    e.append( "tf" );
    CPPUNIT_ASSERT( std::string( "whattf" ) == e.what() );
  }

  void test_stream( void )
  {
    {
      Exception e;
      e << "what" << 23;

      CPPUNIT_ASSERT( std::string( "what23" ) == e.what() );
    }

    try
    {
      throw Exception() << "foo" << 42 << "bar";
    }
    catch( const Exception &e )
    {
      CPPUNIT_ASSERT( std::string( "foo42bar" ) == e.what() );
    }
    catch( const std::exception &e )
    {
      CPPUNIT_FAIL( "Should not be reached: std::exception" );
    }
    catch( ... )
    {
      CPPUNIT_FAIL( "Should not be reached: ..." );
    }

  }

  void test_derive_stream( void )
  {
    try
    {
      throw DerivedException() << "bar" << 23;
    }
    catch( const DerivedException &e )
    {
      CPPUNIT_ASSERT( std::string( "bar23" ) == e.what() );
    }
    catch( const Exception &e )
    {
      CPPUNIT_ASSERT( std::string( "bar23" ) == e.what() );
    }
    catch( ... )
    {
      CPPUNIT_FAIL( "Should not be reached" );
    }

    try
    {
      throw DerivedExceptionFixedText() << 42;
    }
    catch( const Exception &e )
    {
      CPPUNIT_ASSERT( std::string( "fixed text42" ) == e.what() );
    }
    catch( ... )
    {
      CPPUNIT_FAIL( "Should not be reached" );
    }


  }
};

EXCEPTION_DERIVED_DEFINE_FIXED_TEXT( utest_Exception, DerivedExceptionFixedText, "fixed text" );

CPPUNIT_TEST_SUITE_REGISTRATION( utest_Exception );


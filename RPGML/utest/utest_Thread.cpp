#include <cppunit/extensions/HelperMacros.h>

#include <Thread.h>

#include <iostream>

using namespace RPGML;
using namespace std;

class utest_Thread : public CppUnit::TestFixture
{
  CPPUNIT_TEST_SUITE( utest_Thread );

  CPPUNIT_TEST( test_basics );
  CPPUNIT_TEST( test_start );

  CPPUNIT_TEST_SUITE_END();

public:
  void setUp() {}
  void tearDown() {}

  void test_basics( void )
  {
    Thread thread( 0 );
    CPPUNIT_ASSERT_EQUAL( false, thread.isRunning() );
    CPPUNIT_ASSERT_NO_THROW( thread.start() );
    CPPUNIT_ASSERT_EQUAL( true, thread.isRunning() );
    CPPUNIT_ASSERT_NO_THROW( thread.join() );
    CPPUNIT_ASSERT_EQUAL( false, thread.isRunning() );
  }

  class TestObject
  {
  public:
    TestObject( int *marker, int value )
    : m_marker( marker )
    , m_value( value )
    {}

    size_t method1( void )
    {
      (*m_marker) = 1*m_value;
      return 1;
    }

    size_t method2( void )
    {
      (*m_marker) = 2*m_value;
      return 2;
    }

  private:
    int *m_marker;
    int m_value;
  };

  void test_start( void )
  {
    int marker = 0;

    TestObject obj( &marker, 1 );

    Thread thread( 0 );

    CPPUNIT_ASSERT_NO_THROW( thread.start( &obj, &TestObject::method1 ) );
    size_t join_ret1 = 0;
    CPPUNIT_ASSERT_NO_THROW( thread.join( &join_ret1 ) );
    CPPUNIT_ASSERT_EQUAL( size_t( 1 ), join_ret1 );
    CPPUNIT_ASSERT_EQUAL( 1, marker );

    CPPUNIT_ASSERT_NO_THROW( thread.start( &obj, &TestObject::method2 ) );
    size_t join_ret2 = 0;
    CPPUNIT_ASSERT_NO_THROW( thread.join( &join_ret2 ) );
    CPPUNIT_ASSERT_EQUAL( size_t( 2 ), join_ret2 );
    CPPUNIT_ASSERT_EQUAL( 2, marker );
  }
};

CPPUNIT_TEST_SUITE_REGISTRATION( utest_Thread );


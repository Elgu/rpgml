#include <cppunit/extensions/HelperMacros.h>

#include <GarbageCollector.h>

#include <iostream>

using namespace RPGML;
using namespace std;

class utest_GarbageCollector : public CppUnit::TestFixture
{
  CPPUNIT_TEST_SUITE( utest_GarbageCollector );

  CPPUNIT_TEST( test_isCollectable );
  CPPUNIT_TEST( test_ring );

  CPPUNIT_TEST_SUITE_END();

public:
  void setUp() {}
  void tearDown() {}

  class TestNode : public Collectable
  {
  public:
    TestNode( GarbageCollector *_gc, bool *_deleted )
    : Collectable( _gc )
    , deleted( _deleted )
    {
      (*deleted) = false;
    }

    virtual ~TestNode( void )
    {
#ifdef GC_DEBUG
      cerr << "~TestNode " << (void*)this << ", deleted " << (void*)deleted << endl;
#endif
      (*deleted) = true;
      CPPUNIT_ASSERT_EQUAL( true, (*deleted) );
    }

    void connectTo( TestNode *other )
    {
      connections.push_back( other );
    }

    void disconnect( TestNode *other )
    {
      if( !other ) return;
      for( size_t i=0; i<connections.size(); ++i )
      {
        if( connections[ i ].get() == other )
        {
          connections[ i ].clear();
        }
      }
    }

    void disconnect( void )
    {
      connections.clear();
    }

    virtual void gc_clear( void )
    {
      connections.clear();
    }

    virtual void gc_getChildren( Children &children ) const
    {
      for( size_t i=0; i<connections.size(); ++i )
      {
        if( !connections[ i ].isNull() )
        {
          children.push_back( connections[ i ] );
        }
      }
    }

  private:
    std::vector< CountPtr< TestNode > > connections;
    bool *const deleted;
  };

  void test_isCollectable( void )
  {
    Collectable *dummy1 = 0;
    TestNode *dummy2 = 0;
    int a = 0;
    CPPUNIT_ASSERT_EQUAL( true, isCollectable( dummy1 ) );
    CPPUNIT_ASSERT_EQUAL( true, isCollectable( dummy2 ) );
    CPPUNIT_ASSERT_EQUAL( true, isCollectable( (Collectable*)0 ) );
    CPPUNIT_ASSERT_EQUAL( true, isCollectable( (TestNode*)0 ) );
    CPPUNIT_ASSERT_EQUAL( false, isCollectable( &a ) );
    CPPUNIT_ASSERT_EQUAL( false, isCollectable( "" ) );
    CPPUNIT_ASSERT_EQUAL( false, isCollectable( (int*)0 ) );
  }

  void test_ring( void )
  {
    GarbageCollector gc;

    bool deleted1;
    bool deleted2;
    bool deleted3;
    bool deleted4;

    CountPtr< TestNode > node1( new TestNode( &gc, &deleted1 ) ); // root
    CountPtr< TestNode > node2( new TestNode( &gc, &deleted2 ) );
    CountPtr< TestNode > node3( new TestNode( &gc, &deleted3 ) );
    CountPtr< TestNode > node4( new TestNode( &gc, &deleted4 ) );

    CPPUNIT_ASSERT_EQUAL( false, deleted1 );
    CPPUNIT_ASSERT_EQUAL( false, deleted2 );
    CPPUNIT_ASSERT_EQUAL( false, deleted3 );
    CPPUNIT_ASSERT_EQUAL( false, deleted4 );

    // root
    node1->connectTo( node2 );
    node1->connectTo( node3 );
    node1->connectTo( node4 );

    // ring
    node2->connectTo( node3 );
    node3->connectTo( node4 );
    node4->connectTo( node2 );

    node2.clear();
    node3.clear();
    node4.clear();

    CPPUNIT_ASSERT_EQUAL( false, deleted1 );
    CPPUNIT_ASSERT_EQUAL( false, deleted2 );
    CPPUNIT_ASSERT_EQUAL( false, deleted3 );
    CPPUNIT_ASSERT_EQUAL( false, deleted4 );

    node1->disconnect();
    CPPUNIT_ASSERT_EQUAL( false, deleted1 );
    CPPUNIT_ASSERT_EQUAL( false, deleted2 );
    CPPUNIT_ASSERT_EQUAL( false, deleted3 );
    CPPUNIT_ASSERT_EQUAL( false, deleted4 );

    gc.run();

    /*
    std::cerr
      << "deleted1 (" << &deleted1 << ") = " << deleted1 << endl
      << "deleted2 (" << &deleted2 << ") = " << deleted2 << endl
      << "deleted3 (" << &deleted3 << ") = " << deleted3 << endl
      << "deleted4 (" << &deleted4 << ") = " << deleted4 << endl
      ;
      */

    CPPUNIT_ASSERT_EQUAL( false, deleted1 );
    CPPUNIT_ASSERT_EQUAL( true, deleted2 );
    CPPUNIT_ASSERT_EQUAL( true, deleted3 );
    CPPUNIT_ASSERT_EQUAL( true, deleted4 );

    node1.clear();
    CPPUNIT_ASSERT_EQUAL( true, deleted1 );
  }
};

CPPUNIT_TEST_SUITE_REGISTRATION( utest_GarbageCollector );


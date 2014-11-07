#include <cppunit/extensions/HelperMacros.h>

#include <Array.h>

#include <iostream>

using namespace RPGML;
using namespace std;

class utest_Array : public CppUnit::TestFixture
{
  CPPUNIT_TEST_SUITE( utest_Array );

  CPPUNIT_TEST( test_isCollectable );
  CPPUNIT_TEST( test_properties );
  CPPUNIT_TEST( test_resize );
  CPPUNIT_TEST( test_gc );

  CPPUNIT_TEST_SUITE_END();

public:
  void setUp() {}
  void tearDown() {}

  void test_isCollectable( void )
  {
    Collectable *dummy1 = 0;
    ArrayBase *dummy2 = 0;
    Array< int, 0 > *dummy3 = 0;
    CPPUNIT_ASSERT_EQUAL( true, isCollectable( dummy1 ) );
    CPPUNIT_ASSERT_EQUAL( true, isCollectable( dummy2 ) );
    CPPUNIT_ASSERT_EQUAL( true, isCollectable( dummy3 ) );
  }

  void test_properties( void )
  {
    Array< int, 2 > i2( 0 );
    Array< float, 1 > f1( 0 );
    Array< String, 0 > s0( 0 );

    CPPUNIT_ASSERT_EQUAL( Type::Int(), i2.getType() );
    CPPUNIT_ASSERT_EQUAL( Type::Float(), f1.getType() );
    CPPUNIT_ASSERT_EQUAL( Type::String(), s0.getType() );

    CPPUNIT_ASSERT_EQUAL( index_t( 2 ), i2.getDims() );
    CPPUNIT_ASSERT_EQUAL( index_t( 1 ), f1.getDims() );
    CPPUNIT_ASSERT_EQUAL( index_t( 0 ), s0.getDims() );
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
    i2.resize( 2, size );
    CPPUNIT_ASSERT_EQUAL( index_t( 5 ), i2.getSize()[ 0 ] );
    CPPUNIT_ASSERT_EQUAL( index_t( 7 ), i2.getSize()[ 1 ] );
    CPPUNIT_ASSERT_EQUAL( index_t( 35 ), i2.size() );
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
};

CPPUNIT_TEST_SUITE_REGISTRATION( utest_Array );


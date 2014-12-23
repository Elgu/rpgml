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

#include <RPGML/Node.h>

#include <iostream>

using namespace RPGML;
using namespace std;

class utest_Node : public CppUnit::TestFixture
{
  CPPUNIT_TEST_SUITE( utest_Node );

  CPPUNIT_TEST( test_Port );
  CPPUNIT_TEST( test_Input_Output );
  CPPUNIT_TEST( test_Output_Data );

  CPPUNIT_TEST_SUITE_END();

public:
  void setUp() {}
  void tearDown() {}

  class TestNode : public Node
  {
  public:
    TestNode(
        GarbageCollector *_gc
      , const String &identifier
      , const SharedObject *so
      , index_t num_inputs = 0
      , index_t num_outputs = 0
      , index_t num_params = 0
      )
    : Node( _gc, identifier, so, num_inputs, num_outputs, num_params )
    {}

    virtual ~TestNode( void ) {}

    virtual const char *getName( void ) const
    {
      return "TestNode";
    }
  };

  void test_Port( void )
  {
    GarbageCollector gc;
    CountPtr< Node > node( new TestNode( &gc, String::Static( "node" ), 0 ) );

    CountPtr< Port > port( new Port( &gc, node ) );

    CPPUNIT_ASSERT_EQUAL( &gc, port->getGC() );
    CPPUNIT_ASSERT_EQUAL( node.get(), port->getParent() );

    CPPUNIT_ASSERT_NO_THROW( port->setIdentifier( String::Static( "foo" ) ) );
    CPPUNIT_ASSERT_EQUAL( String::Static( "foo" ), port->getIdentifier() );

    Collectable::Children children;
    port->gc_getChildren( children );
    CPPUNIT_ASSERT( children.contains( node.get() ) );
  }

  void test_Input_Output( void )
  {
    GarbageCollector gc;
    CountPtr< Node > node( new TestNode( &gc, String::Static( "node" ), 0 ) );

    // in, out, in2, out2

    CountPtr< Input > in( new Input( &gc, node ) );
    CPPUNIT_ASSERT_EQUAL( &gc, in->getGC() );
    CPPUNIT_ASSERT_EQUAL( node.get(), in->getParent() );
    CPPUNIT_ASSERT_NO_THROW( in->setIdentifier( String::Static( "in" ) ) );
    CPPUNIT_ASSERT_EQUAL( String::Static( "in" ), in->getIdentifier() );
    CPPUNIT_ASSERT_EQUAL( false, in->isConnected() );
    CPPUNIT_ASSERT_EQUAL( (const Output*)0, in->getOutput() );
    CPPUNIT_ASSERT_EQUAL( (const ArrayBase*)0, in->getData() );

    CountPtr< Input > in2( new Input( &gc, node ) );
    CPPUNIT_ASSERT_NO_THROW( in2->setIdentifier( String::Static( "in2" ) ) );

    CountPtr< Output > out( new Output( &gc, node ) );
    CPPUNIT_ASSERT_EQUAL( &gc, out->getGC() );
    CPPUNIT_ASSERT_EQUAL( node.get(), out->getParent() );
    CPPUNIT_ASSERT_NO_THROW( out->setIdentifier( String::Static( "out" ) ) );
    CPPUNIT_ASSERT_EQUAL( String::Static( "out" ), out->getIdentifier() );
    CPPUNIT_ASSERT_EQUAL( false, out->isConnected() );
    CPPUNIT_ASSERT_EQUAL( (ArrayBase*)0, out->getData() );

    CountPtr< Output > out2( new Output( &gc, node ) );
    CPPUNIT_ASSERT_NO_THROW( out2->setIdentifier( String::Static( "out2" ) ) );

    // in <- out

    CPPUNIT_ASSERT_NO_THROW( in->connect( out ) );
    CPPUNIT_ASSERT_EQUAL( true, in->isConnected() );
    CPPUNIT_ASSERT_EQUAL( true, out->isConnected() );
    CPPUNIT_ASSERT_EQUAL( static_cast< const Output* >( out.get() ), in->getOutput() );

    {
      Collectable::Children children;
      in->gc_getChildren( children );
      CPPUNIT_ASSERT_EQUAL( true, children.contains( out ) );
    }

    {
      Collectable::Children children;
      out->gc_getChildren( children );
      CPPUNIT_ASSERT_EQUAL( true, children.contains( in ) );
    }

    // in2 <- out

    CPPUNIT_ASSERT_NO_THROW( in2->connect( out ) );
    CPPUNIT_ASSERT_EQUAL( true, in->isConnected() );
    CPPUNIT_ASSERT_EQUAL( true, in2->isConnected() );
    CPPUNIT_ASSERT_EQUAL( true, out->isConnected() );
    CPPUNIT_ASSERT_EQUAL( static_cast< const Output* >( out.get() ), in->getOutput() );
    CPPUNIT_ASSERT_EQUAL( static_cast< const Output* >( out.get() ), in2->getOutput() );

    {
      Collectable::Children children;
      out->gc_getChildren( children );
      CPPUNIT_ASSERT_EQUAL( true, children.contains( in ) );
      CPPUNIT_ASSERT_EQUAL( true, children.contains( in2 ) );
    }

    // out -/> in

    CPPUNIT_ASSERT_NO_THROW( in->disconnect() );
    CPPUNIT_ASSERT_EQUAL( false, in->isConnected() );
    CPPUNIT_ASSERT_EQUAL( true, in2->isConnected() );
    CPPUNIT_ASSERT_EQUAL( true, out->isConnected() );
    CPPUNIT_ASSERT_EQUAL( static_cast< const Output* >( 0 ), in->getOutput() );
    CPPUNIT_ASSERT_EQUAL( static_cast< const Output* >( out.get() ), in2->getOutput() );

    {
      Collectable::Children children;
      out->gc_getChildren( children );
      CPPUNIT_ASSERT_EQUAL( false, children.contains( in ) );
      CPPUNIT_ASSERT_EQUAL( true, children.contains( in2 ) );
    }

    // out2 -> in2

    CPPUNIT_ASSERT_NO_THROW( out2->connect( in2 ) );
    CPPUNIT_ASSERT_EQUAL( true, in2->isConnected() );
    CPPUNIT_ASSERT_EQUAL( false, out->isConnected() );
    CPPUNIT_ASSERT_EQUAL( true, out2->isConnected() );
    CPPUNIT_ASSERT_EQUAL( static_cast< const Output* >( out2.get() ), in2->getOutput() );

    {
      Collectable::Children children;
      out->gc_getChildren( children );
      CPPUNIT_ASSERT_EQUAL( false, children.contains( in ) );
      CPPUNIT_ASSERT_EQUAL( false, children.contains( in2 ) );
    }
    {
      Collectable::Children children;
      out2->gc_getChildren( children );
      CPPUNIT_ASSERT_EQUAL( true, children.contains( in2 ) );
    }

    // out2 -/> in2

    CPPUNIT_ASSERT_NO_THROW( out2->disconnect() );
    CPPUNIT_ASSERT_EQUAL( false, in2->isConnected() );
    CPPUNIT_ASSERT_EQUAL( false, out->isConnected() );
    CPPUNIT_ASSERT_EQUAL( false, out2->isConnected() );
    CPPUNIT_ASSERT_EQUAL( static_cast< const Output* >( false ), in2->getOutput() );
    {
      Collectable::Children children;
      out2->gc_getChildren( children );
      CPPUNIT_ASSERT_EQUAL( false, children.contains( in2 ) );
    }
  }

  void test_Output_Data( void )
  {
    GarbageCollector gc;
    CountPtr< Node > node( new TestNode( &gc, String::Static( "node" ), 0 ) );

    CountPtr< Input > in( new Input( &gc, node ) );
    CountPtr< Output > out( new Output( &gc, node ) );

    in->connect( out );

    CountPtr< Array< int, 1 > > data( new Array< int, 1 >( &gc ) );
    CPPUNIT_ASSERT_NO_THROW( out->setData( data ) );
    CPPUNIT_ASSERT_EQUAL( static_cast<       ArrayBase* >( data.get() ), out->getData() );
    CPPUNIT_ASSERT_EQUAL( static_cast< const ArrayBase* >( data.get() ), in->getData() );

    typedef Array< int, 1 > Array_int;
    typedef Array< float, 1 > Array_float;

    Array_int       *int_p = 0;
    const Array_int *int_p_const = 0;
    CPPUNIT_ASSERT_EQUAL( data.get(), out->getAs( int_p ) );
    CPPUNIT_ASSERT_EQUAL( static_cast< const Array_int* >( data.get() ), in->getOutput()->getAs( int_p_const ) );

    Array_float       *float_p = 0;
    const Array_float *float_p_const = 0;
    CPPUNIT_ASSERT_EQUAL( static_cast<       Array_float* >( 0 ), out->getAs( float_p ) );
    CPPUNIT_ASSERT_EQUAL( static_cast< const Array_float* >( 0 ), in->getOutput()->getAs( float_p_const ) );

    ArrayElements< int >       *int_e = 0;
    const ArrayElements< int > *int_e_const = 0;
    CPPUNIT_ASSERT_EQUAL( static_cast<       ArrayElements< int >* >( data.get() ), out->getAs( int_e ) );
    CPPUNIT_ASSERT_EQUAL( static_cast< const ArrayElements< int >* >( data.get() ), in->getOutput()->getAs( int_e_const ) );

    ArrayElements< float >       *float_e = 0;
    const ArrayElements< float > *float_e_const = 0;
    CPPUNIT_ASSERT_EQUAL( static_cast<       ArrayElements< float >* >( 0 ), out->getAs( float_e ) );
    CPPUNIT_ASSERT_EQUAL( static_cast< const ArrayElements< float >* >( 0 ), in->getOutput()->getAs( float_e_const ) );

    ArrayBase       *base = 0;
    const ArrayBase *base_const = 0;
    CPPUNIT_ASSERT_EQUAL( static_cast<       ArrayBase* >( data.get() ), out->getAs( base ) );
    CPPUNIT_ASSERT_EQUAL( static_cast< const ArrayBase* >( data.get() ), in->getOutput()->getAs( base_const ) );
  }
};

CPPUNIT_TEST_SUITE_REGISTRATION( utest_Node );


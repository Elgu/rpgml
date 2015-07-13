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

#include <RPGML/Graph.h>
#include <RPGML/Thread.h>
#include <RPGML/Array.h>

#include <iostream>

using namespace RPGML;
using namespace std;

class utest_Graph : public CppUnit::TestFixture
{
  CPPUNIT_TEST_SUITE( utest_Graph );

  CPPUNIT_TEST( test_addNode );

  CPPUNIT_TEST_SUITE_END();

public:
  void setUp() {}
  void tearDown() {}

  class ConstNode : public Node
  {
  public:
    ConstNode(
        GarbageCollector *_gc
      , const String &identifier
      , int value
      )
    : Node( _gc, identifier, 0, 0, 1, 0 )
    , m_value( value )
    {
      DEFINE_OUTPUT_INIT( 0, "out", int, 0 );
    }

    bool set_value( const Value &value, index_t )
    {
      CPPUNIT_ASSERT_NO_THROW( m_value = value.getInt() );
      return true;
    }

    virtual ~ConstNode( void ) {}
    virtual const char *getName( void ) const { return "ConstNode"; }

    virtual bool tick( void )
    {
//      std::cerr << "ConstNode '" << getIdentifier() << "'" << std::endl;
      Array< int > *out = 0;
      CPPUNIT_ASSERT( 0 != getOutput( 0 )->getAs( out ) );
      (**out) = m_value;
      return true;
    }

  private:
    int m_value;
  };

  class AddNode : public Node
  {
  public:
    AddNode(
        GarbageCollector *_gc
      , const String &identifier
      )
    : Node( _gc, identifier, 0, 2, 1, 0 )
    {
      DEFINE_INPUT( 0, "in1" );
      DEFINE_INPUT( 1, "in2" );
      DEFINE_OUTPUT_INIT( 0, "out", int, 0 );
    }

    virtual ~AddNode( void ) {}
    virtual const char *getName( void ) const { return "AddNode"; }

    virtual bool tick( void )
    {
//      std::cerr << "AddNode '" << getIdentifier() << "'" << std::endl;

      const Array< int > *in1 = 0;
      const Array< int > *in2 = 0;

      CPPUNIT_ASSERT( getInput( 0 )->isConnected() );
      CPPUNIT_ASSERT( getInput( 1 )->isConnected() );

      CPPUNIT_ASSERT( 0 != getInput( 0 )->getOutput()->getAs( in1 ) );
      CPPUNIT_ASSERT( 0 != getInput( 1 )->getOutput()->getAs( in2 ) );

      Array< int > *out = 0;
      CPPUNIT_ASSERT( 0 != getOutput( 0 )->getAs( out ) );

      (**out) = (**in1) + (**in2);
      return true;
    }
  };

  class ExitNode : public Node
  {
  public:
    explicit
    ExitNode( GarbageCollector *_gc )
    : Node( _gc, String::Static( "exit" ), 0, 0, 0, 0 )
    {}

    virtual ~ExitNode( void ) {}
    virtual const char *getName( void ) const { return "ExitNode"; }

    virtual bool tick( void )
    {
      throw ExitRequest();
    }
  };

  class EndWorker : public JobQueue::Job
  {
  public:
    EndWorker( GarbageCollector *_gc )
    : JobQueue::Job( _gc )
    {
      setPriority( 0 );
    }

    virtual ~EndWorker( void )
    {}

  protected:
    virtual size_t doit( CountPtr< JobQueue > queue )
    {
      return size_t( -1 );
    }
  };

  class Worker : public Thread
  {
  public:
    explicit
    Worker( GarbageCollector *_gc, JobQueue *queue=0 )
    : Thread( _gc, false )
    , q( queue )
    {
      if( q ) start();
    }

    virtual ~Worker( void )
    {}

    JobQueue *q;

    virtual size_t run( void )
    {
      for(;;)
      {
        CountPtr< JobQueue::Job > job = q->getJob();
        if( size_t(-1) == job->work( q ) ) return 0;
        Thread::yield();
      }
    }
  };

  void test_addNode( void )
  {
    CountPtr< GarbageCollector > gc( newGenerationalGarbageCollector() );

    CountPtr< ConstNode > c1( new ConstNode( gc, String::Static( "c1" ), 1 ) );
    CountPtr< ConstNode > c2( new ConstNode( gc, String::Static( "c2" ), 2 ) );
    CountPtr< AddNode   > add( new AddNode( gc, String::Static( "add" ) ) );
    CountPtr< ExitNode  > ex ( new ExitNode( gc ) );

    c1->getOutput( "out" )->connect( add->getInput( "in1" ) );
    c2->getOutput( "out" )->connect( add->getInput( "in2" ) );

    const Array< int > *out = 0;
    CPPUNIT_ASSERT( 0 != add->getOutput( "out" )->getAs( out ) );

    // Run "by hand"
    CPPUNIT_ASSERT_EQUAL( true, c1->tick() );
    CPPUNIT_ASSERT_EQUAL( true, c2->tick() );
    CPPUNIT_ASSERT_EQUAL( true, add->tick() );
    CPPUNIT_ASSERT_EQUAL( 3, (**out) );

    // Run with Graph execution
    CountPtr< Graph > graph( new Graph( gc ) );
    CountPtr< JobQueue > queue( new JobQueue( gc ) );

    // Create thread pool
    const index_t num_workers = 16;
    typedef Array< CountPtr< Worker > > WorkerArray;
    CountPtr< WorkerArray > workers( new WorkerArray( gc, 1, num_workers ) );
    for( index_t i=0; i<num_workers; ++i ) (*workers)[ i ] = new Worker( gc, queue );

    CPPUNIT_ASSERT_EQUAL( true, c1->set_value( Value( 23 ), 0 ) );
    CPPUNIT_ASSERT_EQUAL( true, c2->set_value( Value( 42 ), 0 ) );

    // Add to Graph,
    CPPUNIT_ASSERT_NO_THROW( graph->addNode( add ) );
    CPPUNIT_ASSERT_EQUAL( true, graph->alreadyAdded( c1 ) );
    CPPUNIT_ASSERT_EQUAL( true, graph->alreadyAdded( c2 ) );
    CPPUNIT_ASSERT_EQUAL( true, graph->alreadyAdded( add ) );

    CPPUNIT_ASSERT_NO_THROW( graph->addNode( ex ) );
    CPPUNIT_ASSERT_EQUAL( true, graph->alreadyAdded( ex ) );

    CPPUNIT_ASSERT_NO_THROW( graph->execute( queue ) );

    // Clear thread pool
    CountPtr< EndWorker > end( new EndWorker( gc ) );
    for( index_t i=0; i<num_workers; ++i ) queue->addJob( end );
    for( index_t i=0; i<num_workers; ++i ) (*workers)[ i ]->join();
  }
};

CPPUNIT_TEST_SUITE_REGISTRATION( utest_Graph );


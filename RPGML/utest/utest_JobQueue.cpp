#include <cppunit/extensions/HelperMacros.h>

#include <JobQueue.h>
#include <Thread.h>

#include <iostream>

using namespace RPGML;
using namespace std;

class utest_JobQueue : public CppUnit::TestFixture
{
  CPPUNIT_TEST_SUITE( utest_JobQueue );

  CPPUNIT_TEST( test_push_pop_priority );
  CPPUNIT_TEST( test_multiple_workers );

  CPPUNIT_TEST_SUITE_END();

public:
  void setUp() {}
  void tearDown() {}

  class NrJob : public JobQueue::Job
  {
  public:
    NrJob( void )
    : Job( 0 )
    , nr( 0 )
    {}

    virtual ~NrJob( void )
    {}

    virtual size_t doit( CountPtr< JobQueue > )
    {
      return size_t( nr );
    }

    int nr;
  };

  class IncJob : public JobQueue::Job
  {
  public:
    IncJob( void )
    : Job( 0 )
    , p( 0 )
    {}

    virtual ~IncJob( void )
    {}

    virtual size_t doit( CountPtr< JobQueue > queue )
    {
      ++(*p);
      if( getPriority() > 1 )
      {
        setPriority( getPriority()-1 );
        queue->addJob( this );
      }
      return 0;
    }

    int *p;
  };

  class EndWorker : public JobQueue::Job
  {
  public:
    EndWorker( void )
    : Job( 0 )
    {
      setPriority( 0 );
    }

    virtual ~EndWorker( void )
    {}

    virtual size_t doit( CountPtr< JobQueue > )
    {
      return size_t( -1 );
    }
  };

  class Worker : public Thread
  {
  public:
    Worker( void )
    : Thread( 0, false )
    {}

    virtual ~Worker( void )
    {}

    JobQueue *q;

    virtual size_t run( void )
    {
      for(;;)
      {
        CountPtr< JobQueue::Job > job = q->getJob();
        if( size_t(-1) == job->doit( q ) ) return 0;
        Thread::yield();
      }
    }
  };

  void test_push_pop_priority( void )
  {
    CountPtr< JobQueue > q = new JobQueue( 0 );

    const size_t num_jobs = 16;
    std::vector< CountPtr< NrJob > > jobs( num_jobs );
    for( size_t i=0; i<num_jobs; ++i )
    {
      jobs[ i ] = new NrJob;
      jobs[ i ]->nr = i;
      jobs[ i ]->setPriority( i );
      CPPUNIT_ASSERT_EQUAL( i, jobs[ i ]->getPriority() );
      q->addJob( jobs[ i ].get() );
//      std::cerr << "q.addJob( " << (void*)&jobs[ i ] << " ), nr = " << i << std::endl;
    }

    for( size_t i=0; i<num_jobs; ++i )
    {
      CountPtr< JobQueue::Job > job = q->getJob();
//      std::cerr << "utest_JobQueue: job = " << (void*)job.get() << ", nr = " << job->doit() << std::endl;
      CPPUNIT_ASSERT_EQUAL( num_jobs-i-1, job->doit( q ) );
      CPPUNIT_ASSERT_EQUAL( num_jobs-i-1, job->getPriority() );
    }
  }

  void test_multiple_workers( void )
  {
    CountPtr< JobQueue > q = new JobQueue( 0 );

    const size_t num_workers = 32;
    const size_t num_jobs = 64;
    const size_t times = 512;

    std::vector< int > a( num_jobs );
    std::vector< CountPtr< IncJob > > j( num_jobs );
    for( size_t i=0; i<num_jobs; ++i )
    {
      j[ i ] = new IncJob;
      j[ i ]->p = &a[ i ];
      j[ i ]->setPriority( times );
      q->addJob( j[ i ].get() );
    }

    std::vector< CountPtr< Worker > > w( num_workers );
    for( size_t i=0; i<num_workers; ++i )
    {
      w[ i ] = new Worker;
      w[ i ]->q = q;
      CPPUNIT_ASSERT_NO_THROW( w[ i ]->start() );
    }

    // Has lower priority than the IncJobs
    CountPtr< EndWorker > e( new EndWorker );
    for( size_t i=0; i<num_workers; ++i )
    {
      CPPUNIT_ASSERT_NO_THROW( q->addJob( e.get() ) );
    }

    for( size_t i=0; i<num_workers; ++i )
    {
      CPPUNIT_ASSERT_NO_THROW( w[ i ]->join() );
      w[ i ]->unref(); // TODO: Fix Refcounted for Collectable with NULL GarbageCollector
    }

    for( size_t i=0; i<num_jobs; ++i )
    {
      CPPUNIT_ASSERT_EQUAL( int( times ), a[ i ] );
    }
  }
};

CPPUNIT_TEST_SUITE_REGISTRATION( utest_JobQueue );


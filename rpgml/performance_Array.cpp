#include <RPGML/Array.h>
#include <RPGML/Array2.h>

#include <cstdlib>
#include <ctime>
#include <cerrno>
#include <algorithm>
#include <iostream>

// RPGML_LDFLAGS =
// RPGML_CXXFLAGS =-O3 -DNDEBUG

using namespace RPGML;
using namespace std;

template< class A >
static
void access_random( A &a, int dims )
{
  unsigned int seed = 0;

  index_t x1[ 8 ][ 4 ];
  index_t x2[ 8 ][ 4 ];
  const index_t *size = a.getSize();

  for( int i=0; i<8; ++i )
  {
    for( int d=0; d<dims; ++d )
    {
      x1[ i ][ d ] = index_t( rand_r( &seed ) ) % size[ d ];
      x2[ i ][ d ] = index_t( rand_r( &seed ) ) % size[ d ];
    }
  }

  for( int i=0; i<10000000; ++i )
  {
    swap( a.at_v( dims, x1[ i % 8 ] ), a.at_v( dims, x2[ i % 8 ] ) );
  }
}

static
uint64_t getNanoSeconds( void )
{
  struct timespec tp;
  if( -1 == clock_gettime( CLOCK_THREAD_CPUTIME_ID, &tp ) )
  {
    switch( errno )
    {
      case EFAULT: throw Exception() << "tp points outside the accessible address space";
      case EINVAL: throw Exception() << "The clk_id specified is not supported on this system";
      default    : throw Exception() << "clock_gettime failed for some reason";
    }
  }

  return uint64_t( tp.tv_sec ) * 1000000000ull + uint64_t( tp.tv_nsec );
}

template< int Dims >
static
void compare_random( void )
{
  GarbageCollector *gc = 0;
  const index_t size[ 4 ] = { 123, 67, 31, 51 };
  Array< int, Dims > a1( gc, Dims, size );
  Array2< int > a2( gc, Dims, size );

  const uint64_t t1 = getNanoSeconds();
  access_random( a1, Dims );
  const uint64_t t2 = getNanoSeconds();
  access_random( a2, Dims );
  const uint64_t t3 = getNanoSeconds();

  std::cerr
    << "access_random Array < int  , " << Dims << " > took " << double( t2-t1 )/1000000 << "ms" << endl
    << "access_random Array2< int >( " << Dims << " ) took " << double( t3-t2 )/1000000 << "ms" << endl
    ;
}

int main( int argc, char **argv )
{
  (void)argc;
  (void)argv;

  compare_random< 1 >();
  compare_random< 2 >();
  compare_random< 3 >();
  compare_random< 4 >();

  return 0;
}


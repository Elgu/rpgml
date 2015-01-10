#ifndef RPGML_cv_h
#define RPGML_cv_h

#include <RPGML/Array.h>
#include <RPGML/Type.h>
#include <RPGML/Refcounted.h>

#include <opencv2/core/core.hpp>

#include <algorithm>

namespace RPGML {
namespace cv {

template< class Element >
CountPtr< Array< Element, 2 > > createArray_t( GarbageCollector *gc, const ::cv::Mat &mat )
{
  if( mat.depth() != ::cv::DataDepth< Element >::value )
  {
    throw Exception() << "RPGML.cv.createArray( Mat ): Element type does not match the Mat type";
  }

  typedef Array< Element, 2 > ArrayType;

  CountPtr< ArrayType > ret = new ArrayType( gc, mat.cols, mat.rows );

  ::cv::MatConstIterator_< Element >       m     = mat.begin< Element >();
  ::cv::MatConstIterator_< Element > const m_end = mat.end  < Element >();
  typename ArrayType::iterator       a     = ret->begin();
  typename ArrayType::iterator const a_end = ret->end();

  for( ; m != m_end && a != a_end; ++m, ++a )
  {
    (*a) = (*m);
  }

  if( m != m_end || a != a_end )
  {
    throw Exception() << "RPGML.cv.createArray_t( Mat ): Iterators do not match";
  }

  return ret;
}

CountPtr< ArrayBase > createArray( GarbageCollector *gc, const ::cv::Mat &mat )
{
  if( 1 != mat.channels() )
  {
    throw Exception() << "RPGML.cv.createArray( Mat ) expects Mat to have only one channel";
  }

  switch( mat.depth() )
  {
    case ::cv::DataDepth< uint8_t  >::value: return createArray_t< uint8_t  >( gc, mat );
    case ::cv::DataDepth< int8_t   >::value: return createArray_t< int8_t   >( gc, mat );
    case ::cv::DataDepth< uint16_t >::value: return createArray_t< uint16_t >( gc, mat );
    case ::cv::DataDepth< int16_t  >::value: return createArray_t< int16_t  >( gc, mat );
    case ::cv::DataDepth< int      >::value: return createArray_t< int      >( gc, mat );
    case ::cv::DataDepth< float    >::value: return createArray_t< float    >( gc, mat );
    case ::cv::DataDepth< double   >::value: return createArray_t< double   >( gc, mat );
    default:
      throw Exception() << "RPGML.cv.createArray( Mat ): Unsupported Mat type";
  }
}





template< class Element >
CountPtr< ArrayArray1D > createArrays_t( GarbageCollector *gc, const ::cv::Mat &mat )
{
  if( mat.depth() != ::cv::DataDepth< Element >::value )
  {
    throw Exception() << "RPGML.cv.createArray_t( Mat ): Element type does not match the Mat type";
  }
  if( mat.channels() < 1 )
  {
    throw Exception() << "RPGML.cv.createArray_t( Mat ) expects Mat to have at least one channel";
  }

  typedef Array< Element, 2 > ArrayType;
  typedef Array< CountPtr< ArrayType >, 1 > ArrayArrayType;

  const int channels = mat.channels();
  const int cols     = mat.cols;
  const int rows     = mat.rows;

  CountPtr< ArrayArrayType > ret_aa = new ArrayArrayType( gc, channels );

  typename ArrayType::iterator a    [ channels ];
  typename ArrayType::iterator a_end[ channels ];
  for( int c=0; c<channels; ++c )
  {
    ret_aa->at( c ) = new ArrayType( gc, cols, rows );
    a    [ c ] = ret_aa->at( c )->begin();
    a_end[ c ] = ret_aa->at( c )->end();
  }

  for( int y=0; y<rows; ++y )
  {
    const Element *m = mat.ptr< Element >( y );
    for( int x=0; x<cols; ++x, m += channels )
    {
      for( int c=0; c<channels; ++c )
      {
        if( a[ c ] == a_end[ c ] )
        {
          throw Exception() << "RPGML.cv.createArray_t( Mat ): iterator a[ " << c << " ] do not match iterator m";
        }
        (*a[ c ]) = m[ c ];
        ++a[ c ];
      }
    }
  }

  CountPtr< ArrayArray1D > ret = new ArrayArray1D( gc, channels );
  for( int c=0; c<channels; ++c )
  {
    ret->at( c ) = ret_aa->at( c );
  }

  return ret;
}

CountPtr< ArrayArray1D > createArrays( GarbageCollector *gc, const ::cv::Mat &mat )
{
  switch( mat.depth() )
  {
    case ::cv::DataDepth< uint8_t  >::value: return createArrays_t< uint8_t  >( gc, mat );
    case ::cv::DataDepth< int8_t   >::value: return createArrays_t< int8_t   >( gc, mat );
    case ::cv::DataDepth< uint16_t >::value: return createArrays_t< uint16_t >( gc, mat );
    case ::cv::DataDepth< int16_t  >::value: return createArrays_t< int16_t  >( gc, mat );
    case ::cv::DataDepth< int      >::value: return createArrays_t< int      >( gc, mat );
    case ::cv::DataDepth< float    >::value: return createArrays_t< float    >( gc, mat );
    case ::cv::DataDepth< double   >::value: return createArrays_t< double   >( gc, mat );
    default:
      throw Exception() << "RPGML.cv.createArray( Mat ): Unsupported Mat type";
  }
}
} // namespace cv
} // namespace RPGML

#endif


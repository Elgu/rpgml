/* This file is part of RPGML.
 *
 * Copyright (c) 2015, Gunnar Payer, All rights reserved.
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
#ifndef RPGML_math_MapOp_h
#define RPGML_math_MapOp_h

#include <RPGML/Array.h>
#include <RPGML/Value.h>

namespace RPGML {
namespace math {

template< class _T, class _Op >
struct MapOp
{
  typedef _T T;
  typedef typename _Op:: template Other< T >::Op Op;
  typedef typename Op::Ret Ret;

  Ret operator()( const T &x ) const
  {
    return Op()( x );
  }
};

template< class Op > struct MapOp< Value, Op >;
template< class Op > struct MapOp< const ArrayBase*, Op >;
template< class Op > struct MapOp< CountPtr< ArrayBase >, Op >;
template< class Op > struct MapOp< CountPtr< const ArrayBase >, Op >;
template< class Op > struct MapOp< const ArrayElements< Value >*, Op >;
template< class Op > struct MapOp< CountPtr< ArrayElements< Value > >, Op >;
template< class Op > struct MapOp< CountPtr< const ArrayElements< Value > >, Op >;
template< class Op, class Element > struct MapOp< const ArrayElements< Element >*, Op >;
template< class Op, class Element > struct MapOp< CountPtr< ArrayElements< Element > >, Op >;
template< class Op, class Element > struct MapOp< CountPtr< const ArrayElements< Element > >, Op >;

template< class Op >
struct MapOp< Value, Op >
{
  typedef Value T;
  typedef Value Ret;

  Ret operator()( const T &x ) const
  {
    switch( x.getType().getEnum() )
    {
      case Type::BOOL  : return Value( MapOp< bool    , Op >()( x.getBool  () ) );
      case Type::UINT8 : return Value( MapOp< uint8_t , Op >()( x.getUInt8 () ) );
      case Type::INT8  : return Value( MapOp< int8_t  , Op >()( x.getInt8  () ) );
      case Type::UINT16: return Value( MapOp< uint16_t, Op >()( x.getUInt16() ) );
      case Type::INT16 : return Value( MapOp< int16_t , Op >()( x.getInt16 () ) );
      case Type::UINT32: return Value( MapOp< uint32_t, Op >()( x.getUInt32() ) );
      case Type::INT32 : return Value( MapOp< int32_t , Op >()( x.getInt32 () ) );
      case Type::UINT64: return Value( MapOp< uint64_t, Op >()( x.getUInt64() ) );
      case Type::INT64 : return Value( MapOp< int64_t , Op >()( x.getInt64 () ) );
      case Type::FLOAT : return Value( MapOp< float   , Op >()( x.getFloat () ) );
      case Type::DOUBLE: return Value( MapOp< double  , Op >()( x.getDouble() ) );
      case Type::ARRAY : return Value( MapOp< const ArrayBase*, Op >()( x.getArray() ) );
      default:
        throw Exception() << "Unsupported Type";
    }
  }
};

template< class Op >
struct MapOp< const ArrayBase*, Op >
{
  typedef const ArrayBase *T;
  typedef CountPtr< ArrayBase > Ret;

  Ret operator()( const T &x ) const
  {
    if( x->isValue() )
    {
      return MapOp< CountPtr< ArrayElements< Value > >, Op >()( x );
    }
    switch( x->getType().getEnum() )
    {
      case Type::BOOL  : return Value( MapOp< CountPtr< ArrayElements< bool     > >, Op >()( x ) );
      case Type::UINT8 : return Value( MapOp< CountPtr< ArrayElements< uint8_t  > >, Op >()( x ) );
      case Type::INT8  : return Value( MapOp< CountPtr< ArrayElements< int8_t   > >, Op >()( x ) );
      case Type::UINT16: return Value( MapOp< CountPtr< ArrayElements< uint16_t > >, Op >()( x ) );
      case Type::INT16 : return Value( MapOp< CountPtr< ArrayElements< int16_t  > >, Op >()( x ) );
      case Type::UINT32: return Value( MapOp< CountPtr< ArrayElements< uint32_t > >, Op >()( x ) );
      case Type::INT32 : return Value( MapOp< CountPtr< ArrayElements< int32_t  > >, Op >()( x ) );
      case Type::UINT64: return Value( MapOp< CountPtr< ArrayElements< uint64_t > >, Op >()( x ) );
      case Type::INT64 : return Value( MapOp< CountPtr< ArrayElements< int64_t  > >, Op >()( x ) );
      case Type::FLOAT : return Value( MapOp< CountPtr< ArrayElements< float    > >, Op >()( x ) );
      case Type::DOUBLE: return Value( MapOp< CountPtr< ArrayElements< double   > >, Op >()( x ) );
      case Type::ARRAY : return Value( MapOp< CountPtr< ArrayElements< CountPtr< ArrayBase > > >, Op >()( x ) );
      default:
        throw Exception() << "Unsupported Array Type";
    }
  }
};

template< class Op >
struct MapOp< CountPtr< ArrayBase >, Op >
{
  typedef const ArrayBase *T;
  typedef CountPtr< ArrayBase > Ret;

  Ret operator()( const T &x ) const
  {
    return MapOp< const ArrayBase*, Op >()( x );
  }
};

template< class Op >
struct MapOp< CountPtr< const ArrayBase >, Op >
{
  typedef const ArrayBase *T;
  typedef CountPtr< ArrayBase > Ret;

  Ret operator()( const T &x ) const
  {
    return MapOp< const ArrayBase*, Op >()( x );
  }
};

template< class _Op >
struct MapOp< const ArrayElements< Value >*, _Op >
{
  typedef const ArrayBase *T;
  typedef CountPtr< ArrayElements< Value > > Ret;
  typedef typename _Op:: template Other< Value >::Op Op;

  Ret operator()( const T &x ) const
  {
    typedef ArrayElements< Value > ValueArrayElements;
    const ValueArrayElements *e = 0;
    if( !x->getAs( e ) ) throw Exception() << "x is not a Value Array";

    const ArrayBase::Size size = x->getSize();
    CountPtr< ValueArrayElements > ret( new_Array< Value >( x->getGC(), size.getDims() ) );
    ret->resize( size );

    ValueArrayElements::const_iterator e_iter = e->begin();
    ValueArrayElements::const_iterator e_end  = e->end();
    ValueArrayElements::iterator ret_iter = ret->begin();
    for( ; e_iter != e_end; ++e_iter, ++ret_iter )
    {
      (*ret_iter ) = MapOp< Value, Op >()( (*e_iter) );
    }

    return ret;
  }
};

template< class _Op >
struct MapOp< CountPtr< ArrayElements< Value > >, _Op >
{
  typedef const ArrayBase *T;
  typedef CountPtr< ArrayElements< Value > > Ret;
  typedef typename _Op:: template Other< Value >::Op Op;

  Ret operator()( const T &x ) const
  {
    return MapOp< const ArrayElements< Value >*, Op >()( x );
  }
};

template< class _Op >
struct MapOp< CountPtr< const ArrayElements< Value > >, _Op >
{
  typedef const ArrayBase *T;
  typedef CountPtr< ArrayElements< Value > > Ret;
  typedef typename _Op:: template Other< Value >::Op Op;

  Ret operator()( const T &x ) const
  {
    return MapOp< const ArrayElements< Value >*, Op >()( x );
  }
};

template< class _Op, class Element >
struct MapOp< const ArrayElements< Element >*, _Op >
{
  typedef const ArrayBase *T;
  typedef typename _Op:: template Other< Element >::Op Op;
  typedef MapOp< Element, Op > ElementOp;
  typedef typename ElementOp::Ret RetElement;
  typedef CountPtr< ArrayElements< RetElement > > Ret;

  Ret operator()( const T &x ) const
  {
    typedef ArrayElements< Element > InElements;
    typedef ArrayElements< RetElement > RetElements;

    const InElements *e = 0;
    if( !x->getAs( e ) )
    {
      throw Exception()
        << "x is not of specified element type " << getTypeName< T >()
        << ", is " << x->getTypeName()
        ;
    }

    const ArrayBase::Size size = x->getSize();
    CountPtr< RetElements > ret( new_Array< RetElement >( x->getGC(), size.getDims() ) );
    ret->resize( size );

    typename InElements::const_iterator e_iter = e->begin();
    typename InElements::const_iterator e_end  = e->end();
    typename RetElements::iterator ret_iter = ret->begin();
    for( ; e_iter != e_end; ++e_iter, ++ret_iter )
    {
      (*ret_iter ) = ElementOp()( Element(*e_iter) );
    }

    return ret;
  }
};

template< class _Op, class Element >
struct MapOp< CountPtr< ArrayElements< Element > >, _Op >
{
  typedef const ArrayBase *T;
  typedef typename _Op:: template Other< Element >::Op Op;
  typedef MapOp< Element, Op > ElementOp;
  typedef typename ElementOp::Ret RetElement;
  typedef CountPtr< ArrayElements< RetElement > > Ret;

  Ret operator()( const T &x ) const
  {
    return MapOp< const ArrayElements< Element >*, Op >()( x );
  }
};

template< class _Op, class Element >
struct MapOp< CountPtr< const ArrayElements< Element > >, _Op >
{
  typedef const ArrayBase *T;
  typedef typename _Op:: template Other< Element >::Op Op;
  typedef MapOp< Element, Op > ElementOp;
  typedef typename ElementOp::Ret RetElement;
  typedef CountPtr< ArrayElements< RetElement > > Ret;

  Ret operator()( const T &x ) const
  {
    return MapOp< const ArrayElements< Element >*, Op >()( x );
  }
};

} // namespace math
} // namespace RPGML

#endif
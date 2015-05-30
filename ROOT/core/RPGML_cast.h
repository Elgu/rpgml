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
#ifndef RPGML_core_cast_h
#define RPGML_core_cast_h

#include "../math/RPGML_MapOp.h"

#include <RPGML/String.h>

namespace RPGML {
namespace core {

template< class _T, Type::Enum _to >
struct CastTo_op
{
  static const Type::Enum to = _to;
  template< class OtherT >
  struct Other
  {
    typedef CastTo_op< OtherT, to > Op;
  };
  typedef _T T;
  typedef typename EnumType< to >::T Ret;
  Ret operator()( const T &x ) const
  {
    return Ret( x );
  }
};

template< class _T >
struct CastTo_op< _T, Type::STRING >
{
  static const Type::Enum to = Type::STRING;
  template< class OtherT >
  struct Other
  {
    typedef CastTo_op< OtherT, to > Op;
  };
  typedef _T T;
  typedef typename EnumType< to >::T Ret;
  Ret operator()( const T &x ) const
  {
    return toString( x );
  }
};

template< Type::Enum _to >
struct CastTo_op< String, _to >
{
  static const Type::Enum to = _to;
  template< class OtherT >
  struct Other
  {
    typedef CastTo_op< OtherT, to > Op;
  };
  typedef String T;
  typedef typename EnumType< to >::T Ret;
  Ret operator()( const T &x ) const
  {
    Ret ret;
    x.parse( ret );
    return ret;
  }
};

template<>
struct CastTo_op< String, Type::STRING >
{
  static const Type::Enum to = Type::STRING;
  template< class OtherT >
  struct Other
  {
    typedef CastTo_op< OtherT, to > Op;
  };
  typedef String T;
  typedef String Ret;
  Ret operator()( const T &x ) const
  {
    return x;
  }
};

template< Type::Enum _to >
struct CastTo_op< Value, _to >
{
  static const Type::Enum to = _to;
  template< class OtherT >
  struct Other
  {
    typedef CastTo_op< OtherT, to > Op;
  };
  typedef Value T;
  typedef Value Ret;
  Ret operator()( const T &x ) const
  {
    return x.to( to );
  }
};

template<>
struct CastTo_op< Value, Type::STRING >
{
  static const Type::Enum to = Type::STRING;
  template< class OtherT >
  struct Other
  {
    typedef CastTo_op< OtherT, to > Op;
  };
  typedef Value T;
  typedef Value Ret;
  Ret operator()( const T &x ) const
  {
    return x.to( to );
  }
};

static inline
CountPtr< ArrayBase > castTo( Type::Enum to, const ArrayBase *x );
static inline
CountPtr< ArrayBase > castTo( Type::Enum to, ArrayBase *x );

template< class T >
static
Value castTo( Type::Enum to, const T &x )
{
  switch( to )
  {
    case Type::BOOL  : return Value( math::MapOp< T, CastTo_op< T, Type::BOOL   > >()( x ) );
    case Type::UINT8 : return Value( math::MapOp< T, CastTo_op< T, Type::UINT8  > >()( x ) );
    case Type::INT8  : return Value( math::MapOp< T, CastTo_op< T, Type::INT8   > >()( x ) );
    case Type::UINT16: return Value( math::MapOp< T, CastTo_op< T, Type::UINT16 > >()( x ) );
    case Type::INT16 : return Value( math::MapOp< T, CastTo_op< T, Type::INT16  > >()( x ) );
    case Type::UINT32: return Value( math::MapOp< T, CastTo_op< T, Type::UINT32 > >()( x ) );
    case Type::INT32 : return Value( math::MapOp< T, CastTo_op< T, Type::INT32  > >()( x ) );
    case Type::UINT64: return Value( math::MapOp< T, CastTo_op< T, Type::UINT64 > >()( x ) );
    case Type::INT64 : return Value( math::MapOp< T, CastTo_op< T, Type::INT64  > >()( x ) );
    case Type::FLOAT : return Value( math::MapOp< T, CastTo_op< T, Type::FLOAT  > >()( x ) );
    case Type::DOUBLE: return Value( math::MapOp< T, CastTo_op< T, Type::DOUBLE > >()( x ) );
    case Type::STRING: return Value( math::MapOp< T, CastTo_op< T, Type::STRING > >()( x ) );
    default:
      throw Exception()
        << "Cannot cast type '" << getTypeName< T >() << "'"
        << " to type '" << Type( to ) << "'"
        ;
  }
}

static inline
CountPtr< ArrayBase > castTo( Type::Enum to, const ArrayBase *x )
{
  typedef const ArrayBase *T;
  switch( to )
  {
    case Type::BOOL  : return math::MapOp< T, CastTo_op< T, Type::BOOL   > >()( x );
    case Type::UINT8 : return math::MapOp< T, CastTo_op< T, Type::UINT8  > >()( x );
    case Type::INT8  : return math::MapOp< T, CastTo_op< T, Type::INT8   > >()( x );
    case Type::UINT16: return math::MapOp< T, CastTo_op< T, Type::UINT16 > >()( x );
    case Type::INT16 : return math::MapOp< T, CastTo_op< T, Type::INT16  > >()( x );
    case Type::UINT32: return math::MapOp< T, CastTo_op< T, Type::UINT32 > >()( x );
    case Type::INT32 : return math::MapOp< T, CastTo_op< T, Type::INT32  > >()( x );
    case Type::UINT64: return math::MapOp< T, CastTo_op< T, Type::UINT64 > >()( x );
    case Type::INT64 : return math::MapOp< T, CastTo_op< T, Type::INT64  > >()( x );
    case Type::FLOAT : return math::MapOp< T, CastTo_op< T, Type::FLOAT  > >()( x );
    case Type::DOUBLE: return math::MapOp< T, CastTo_op< T, Type::DOUBLE > >()( x );
    case Type::STRING: return math::MapOp< T, CastTo_op< T, Type::STRING > >()( x );
    default:
      throw Exception()
        << "Cannot cast to type '" << Type( to ) << "'"
        ;
  }
}

static inline
CountPtr< ArrayBase > castTo( Type::Enum to, ArrayBase *x )
{
  return castTo( to, static_cast< const ArrayBase* >( x ) );
}

} // namespace core

namespace math {

template< class SomeT, Type::Enum _to >
struct MapOp< const Array< Value >*, core::CastTo_op< SomeT, _to > >
{
  typedef const ArrayBase *T;
  typedef core::CastTo_op< SomeT, _to > _Op;
  typedef typename _Op:: template Other< Value >::Op Op;
  typedef CountPtr< ArrayBase > Ret;

  Ret operator()( const T &x ) const
  {
    const ValueArray *e = 0;
    if( !x->getAs( e ) ) throw Exception() << "x is not a Value Array";

    bool is_primitive = true;
    for( ValueArray::const_iterator i( e->begin() ), end( e->end() ); i != end; ++i )
    {
      if( !i->isPrimitive() )
      {
        is_primitive = false;
        break;
      }
    }

    const ArrayBase::Size size = x->getSize();

    if( is_primitive )
    {
      typedef typename EnumType< Op::to >::T array_type;

      CountPtr< Array< array_type > > ret( new Array< array_type >( x->getGC(), size.getDims() ) );
      ret->resize( size );

      ValueArray::const_iterator e_iter = e->begin();
      ValueArray::const_iterator e_end  = e->end();
      typename Array< array_type >::iterator ret_iter = ret->begin();
      for( ; e_iter != e_end; ++e_iter, ++ret_iter )
      {
        (*ret_iter ) = MapOp< Value, Op >()( (*e_iter) );
      }

      return ret;
    }
    else
    {
      typedef Value array_type;

      CountPtr< Array< array_type > > ret( new Array< array_type >( x->getGC(), size.getDims() ) );
      ret->resize( size );

      ValueArray::const_iterator e_iter = e->begin();
      ValueArray::const_iterator e_end  = e->end();
      typename Array< array_type >::iterator ret_iter = ret->begin();
      for( ; e_iter != e_end; ++e_iter, ++ret_iter )
      {
        (*ret_iter ) = MapOp< Value, Op >()( (*e_iter) );
      }

      return ret;
    }
  }
};

} // namespace math

} // namespace RPGML

#endif

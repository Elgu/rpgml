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
#include "RPGML_Node_BinaryOp.h"

namespace RPGML {

BinaryOp::BinaryOp( GarbageCollector *_gc, const String &identifier, const RPGML::SharedObject *so )
: Node( _gc, identifier, so, NUM_INPUTS, NUM_OUTPUTS, NUM_PARAMS )
, m_op( BOP_UNDEFINED )
{
  DEFINE_INPUT( INPUT_IN1, "in1" );
  DEFINE_INPUT( INPUT_IN2, "in2" );
  DEFINE_OUTPUT( OUTPUT_OUT, "out" );
  DEFINE_PARAM ( PARAM_OP, "op", BinaryOp::set_op );
}

BinaryOp::~BinaryOp( void )
{}

const char *BinaryOp::getName( void ) const
{
  return "BinaryOp";
}

void BinaryOp::set_op( const Value &value, index_t, int, const index_t* )
{
  if( value.getType() != Type::String() )
  {
    throw Exception() << "Param 'op' must be set with string, is " << value.getTypeName();
  }

  try
  {
    m_op = getBOP( value.getString() );
  }
  catch( const char * )
  {
    throw Exception() << "Invalid op '" << value << "'";
  }
}

namespace BinaryOp_impl {

template< class X, class Y, BOP OP > struct op_impl {};
template< class X, class Y > struct op_impl< X, Y, BOP_LEFT    > { typedef X                           Ret; static Ret doit( const X &x, const Y &y ) { return Ret( x << y ); } };
template< class X, class Y > struct op_impl< X, Y, BOP_RIGHT   > { typedef X                           Ret; static Ret doit( const X &x, const Y &y ) { return Ret( x >> y ); } };
template< class X, class Y > struct op_impl< X, Y, BOP_LT      > { typedef bool                        Ret; static Ret doit( const X &x, const Y &y ) { return x <  y; } };
template< class X, class Y > struct op_impl< X, Y, BOP_LE      > { typedef bool                        Ret; static Ret doit( const X &x, const Y &y ) { return x <= y; } };
template< class X, class Y > struct op_impl< X, Y, BOP_GT      > { typedef bool                        Ret; static Ret doit( const X &x, const Y &y ) { return x >  y; } };
template< class X, class Y > struct op_impl< X, Y, BOP_GE      > { typedef bool                        Ret; static Ret doit( const X &x, const Y &y ) { return x >= y; } };
template< class X, class Y > struct op_impl< X, Y, BOP_EQ      > { typedef bool                        Ret; static Ret doit( const X &x, const Y &y ) { return x == y; } };
template< class X, class Y > struct op_impl< X, Y, BOP_NE      > { typedef bool                        Ret; static Ret doit( const X &x, const Y &y ) { return x != y; } };
template< class X, class Y > struct op_impl< X, Y, BOP_LOG_AND > { typedef bool                        Ret; static Ret doit( const X &x, const Y &y ) { return x && y; } };
template< class X, class Y > struct op_impl< X, Y, BOP_LOG_OR  > { typedef bool                        Ret; static Ret doit( const X &x, const Y &y ) { return x || y; } };
template< class X, class Y > struct op_impl< X, Y, BOP_LOG_XOR > { typedef bool                        Ret; static Ret doit( const X &x, const Y &y ) { return bool( x ) ^ bool( y ); } };
template< class X, class Y > struct op_impl< X, Y, BOP_BIT_AND > { typedef typename RetType< X, Y >::T Ret; static Ret doit( const X &x, const Y &y ) { return Ret( x & y ); } };
template< class X, class Y > struct op_impl< X, Y, BOP_BIT_OR  > { typedef typename RetType< X, Y >::T Ret; static Ret doit( const X &x, const Y &y ) { return Ret( x | y ); } };
template< class X, class Y > struct op_impl< X, Y, BOP_BIT_XOR > { typedef typename RetType< X, Y >::T Ret; static Ret doit( const X &x, const Y &y ) { return Ret( x ^ y ); } };
template< class X, class Y > struct op_impl< X, Y, BOP_MUL     > { typedef typename RetType< X, Y >::T Ret; static Ret doit( const X &x, const Y &y ) { return Ret( x * y ); } };
template< class X, class Y > struct op_impl< X, Y, BOP_DIV     > { typedef typename RetType< X, Y >::T Ret; static Ret doit( const X &x, const Y &y ) { return Ret( x / y ); } };
template< class X, class Y > struct op_impl< X, Y, BOP_ADD     > { typedef typename RetType< X, Y >::T Ret; static Ret doit( const X &x, const Y &y ) { return Ret( x + y ); } };
template< class X, class Y > struct op_impl< X, Y, BOP_SUB     > { typedef typename RetType< X, Y >::T Ret; static Ret doit( const X &x, const Y &y ) { return Ret( x - y ); } };
template< class X, class Y > struct op_impl< X, Y, BOP_MOD     > { typedef typename RetType< X, Y >::T Ret; static Ret doit( const X &x, const Y &y ) { return Ret( x % y ); } };

template< class T1, class T2, BOP OP >
struct impl
{
  typedef op_impl< T1, T2, OP > Op;
  typedef typename Op::Ret Ret;

  static
  bool doit( const Array< T1 > *in1, const Array< T2 > *in2, Array< Ret > *out )
  {
    typename Array< Ret >::iterator o_iter = out->begin();
    typename Array< T1 >::const_iterator i1_iter = in1->begin();
    typename Array< T2 >::const_iterator i2_iter = in2->begin();
    const index_t n = in1->size();
    for( index_t i=0; i<n; ++i, ++i1_iter, ++i2_iter, ++o_iter )
    {
      (*o_iter) = Op::doit( (*i1_iter), (*i2_iter) );
    }

    return true;
  }

  static
  bool doit_scalar1( const T1 &in1, const Array< T2 > *in2, Array< Ret > *out )
  {
    typename Array< Ret >::iterator o_iter = out->begin();
    typename Array< T2 >::const_iterator i2_iter = in2->begin();
    const index_t n = in2->size();
    for( index_t i=0; i<n; ++i, ++i2_iter, ++o_iter )
    {
      (*o_iter) = Op::doit( in1, (*i2_iter) );
    }

    return true;
  }

  static
  bool doit_scalar2( const Array< T1 > *in1, const T2 &in2, Array< Ret > *out )
  {
    typename Array< Ret >::iterator o_iter = out->begin();
    typename Array< T1 >::const_iterator i1_iter = in1->begin();
    const index_t n = in1->size();
    for( index_t i=0; i<n; ++i, ++i1_iter, ++o_iter )
    {
      (*o_iter) = Op::doit( (*i1_iter), in2 );
    }

    return true;
  }
};

template< BOP OP >
struct impl< String, String, OP >
{
  typedef String T1;
  typedef String T2;
  typedef op_impl< T1, T2, OP > Op;
  typedef typename Op::Ret Ret;

  static
  bool doit( const Array< T1 > *in1, const Array< T2 > *in2, Array< Ret > *out )
  {
    typename Array< Ret >::iterator o_iter = out->begin();
    typename Array< T1 >::const_iterator i1_iter = in1->begin();
    typename Array< T2 >::const_iterator i2_iter = in2->begin();
    const index_t n = in1->size();
    for( index_t i=0; i<n; ++i, ++i1_iter, ++i2_iter, ++o_iter )
    {
      (*o_iter) = Op::doit( (*i1_iter), (*i2_iter) );
    }

    return true;
  }

  static
  bool doit_scalar1( const T1 &in1, const Array< T2 > *in2, Array< Ret > *out )
  {
    typename Array< Ret >::iterator o_iter = out->begin();
    typename Array< T2 >::const_iterator i2_iter = in2->begin();
    const index_t n = in2->size();
    for( index_t i=0; i<n; ++i, ++i2_iter, ++o_iter )
    {
      (*o_iter) = Op::doit( in1, (*i2_iter) );
    }

    return true;
  }

  static
  bool doit_scalar2( const Array< T1 > *in1, const T2 &in2, Array< Ret > *out )
  {
    typename Array< Ret >::iterator o_iter = out->begin();
    typename Array< T1 >::const_iterator i1_iter = in1->begin();
    const index_t n = in1->size();
    for( index_t i=0; i<n; ++i, ++i1_iter, ++o_iter )
    {
      (*o_iter) = Op::doit( (*i1_iter), in2 );
    }

    return true;
  }
};

template< class T1, BOP OP >
struct impl< T1, String, OP >
{
  typedef String T2;
  typedef op_impl< String, String, OP > Op;
  typedef typename Op::Ret Ret;

  static
  bool doit( const Array< T1 > *in1, const Array< T2 > *in2, Array< Ret > *out )
  {
    typename Array< Ret >::iterator o_iter = out->begin();
    typename Array< T1 >::const_iterator i1_iter = in1->begin();
    typename Array< T2 >::const_iterator i2_iter = in2->begin();
    const index_t n = in1->size();
    for( index_t i=0; i<n; ++i, ++i1_iter, ++i2_iter, ++o_iter )
    {
      (*o_iter) = Op::doit( toString(*i1_iter), (*i2_iter) );
    }

    return true;
  }

  static
  bool doit_scalar1( const T1 &in1, const Array< T2 > *in2, Array< Ret > *out )
  {
    typename Array< Ret >::iterator o_iter = out->begin();
    typename Array< T2 >::const_iterator i2_iter = in2->begin();
    const String in1_str = toString( in1 );
    const index_t n = in2->size();
    for( index_t i=0; i<n; ++i, ++i2_iter, ++o_iter )
    {
      (*o_iter) = Op::doit( in1_str, (*i2_iter) );
    }

    return true;
  }

  static
  bool doit_scalar2( const Array< T1 > *in1, const T2 &in2, Array< Ret > *out )
  {
    typename Array< Ret >::iterator o_iter = out->begin();
    typename Array< T1 >::const_iterator i1_iter = in1->begin();
    const index_t n = in1->size();
    for( index_t i=0; i<n; ++i, ++i1_iter, ++o_iter )
    {
      (*o_iter) = Op::doit( toString(*i1_iter), in2 );
    }

    return true;
  }
};

template< class T2, BOP OP >
struct impl< String, T2, OP >
{
  typedef String T1;
  typedef op_impl< String, String, OP > Op;
  typedef typename Op::Ret Ret;

  static
  bool doit( const Array< T1 > *in1, const Array< T2 > *in2, Array< Ret > *out )
  {
    typename Array< Ret >::iterator o_iter = out->begin();
    typename Array< T1 >::const_iterator i1_iter = in1->begin();
    typename Array< T2 >::const_iterator i2_iter = in2->begin();
    const index_t n = in1->size();
    for( index_t i=0; i<n; ++i, ++i1_iter, ++i2_iter, ++o_iter )
    {
      (*o_iter) = Op::doit( (*i1_iter), toString(*i2_iter) );
    }

    return true;
  }

  static
  bool doit_scalar1( const T1 &in1, const Array< T2 > *in2, Array< Ret > *out )
  {
    typename Array< Ret >::iterator o_iter = out->begin();
    typename Array< T2 >::const_iterator i2_iter = in2->begin();
    const index_t n = in2->size();
    for( index_t i=0; i<n; ++i, ++i2_iter, ++o_iter )
    {
      (*o_iter) = Op::doit( in1, toString(*i2_iter) );
    }

    return true;
  }

  static
  bool doit_scalar2( const Array< T1 > *in1, const T2 &in2, Array< Ret > *out )
  {
    typename Array< Ret >::iterator o_iter = out->begin();
    typename Array< T1 >::const_iterator i1_iter = in1->begin();
    const String in2_str = toString( in2 );
    const index_t n = in1->size();
    for( index_t i=0; i<n; ++i, ++i1_iter, ++o_iter )
    {
      (*o_iter) = Op::doit( (*i1_iter), in2_str );
    }

    return true;
  }
};

} // namespace BinaryOp_impl


template< BOP OP, class T1, class T2 >
bool BinaryOp::tick3( const Array< T1 > *in1, const ArrayBase *in2_base )
{
  using namespace BinaryOp_impl;

  typedef impl< T1, T2, OP > Impl;
  typedef typename Impl::Ret Out;

  const Array< T2 > *in2 = 0;
  if( !in2_base->getAs( in2 ) ) throw GetAsFailed( getInput( INPUT_IN2 ), in2 );

  const ArrayBase::Size in1_size = in1->getSize();
  const ArrayBase::Size in2_size = in2->getSize();

  if( in1->getDims() == 0 )
  {
    getOutput( OUTPUT_OUT )->initData< Out >( in2_size );

    Array< Out > *out = 0;
    if( !getOutput( OUTPUT_OUT )->getAs( out ) ) throw GetAsFailed( getOutput( OUTPUT_OUT ), out );

    return Impl::doit_scalar1( *in1->begin(), in2, out );
  }

  if( in2->getDims() == 0 )
  {
    getOutput( OUTPUT_OUT )->initData< Out >( in1_size );

    Array< Out > *out = 0;
    if( !getOutput( OUTPUT_OUT )->getAs( out ) ) throw GetAsFailed( getOutput( OUTPUT_OUT ), out );

    return Impl::doit_scalar2( in1, *in2->begin(), out );
  }

  if( in1_size == in2_size )
  {
    getOutput( OUTPUT_OUT )->initData< Out >( in1_size );

    Array< Out > *out = 0;
    if( !getOutput( OUTPUT_OUT )->getAs( out ) ) throw GetAsFailed( getOutput( OUTPUT_OUT ), out );

    return Impl::doit( in1, in2, out );
  }

  throw Exception()
    << "If none of the operands for '" << getBOPStr( OP ) << "'"
    << " are scalar (0d), the sizes and dimensions must match"
    << ": in1 (left) is " << in1_size
    << ", in2 (right) is " << in2_size
    ;
}


template< BOP OP, class T1 >
bool BinaryOp::tick2( const ArrayBase *in1_base )
{
  const Array< T1 > *in1 = 0;
  if( !in1_base->getAs( in1 ) ) throw GetAsFailed( getInput( INPUT_IN1 ), in1 );

  const ArrayBase *const in2_base = getInput( INPUT_IN2 )->getData();
  if( !in2_base ) throw NotConnected( getInput( INPUT_IN2 ) );

  switch( in2_base->getType() )
  {
    case Type::BOOL  : return tick3< OP, T1, bool     >( in1, in2_base );
    case Type::UINT8 : return tick3< OP, T1, uint8_t  >( in1, in2_base );
    case Type::INT8  : return tick3< OP, T1, int8_t   >( in1, in2_base );
    case Type::UINT16: return tick3< OP, T1, uint16_t >( in1, in2_base );
    case Type::INT16 : return tick3< OP, T1, int16_t  >( in1, in2_base );
    case Type::UINT32: return tick3< OP, T1, uint32_t >( in1, in2_base );
    case Type::INT32 : return tick3< OP, T1, int32_t  >( in1, in2_base );
    case Type::UINT64: return tick3< OP, T1, uint64_t >( in1, in2_base );
    case Type::INT64 : return tick3< OP, T1, int64_t  >( in1, in2_base );
    case Type::FLOAT : return tick3< OP, T1, float    >( in1, in2_base );
    case Type::DOUBLE: return tick3< OP, T1, double   >( in1, in2_base );
    case Type::STRING: return tick3< OP, T1, String   >( in1, in2_base );
    default:
      throw Exception()
        << "unsupported type '" << in2_base->getType().getTypeName() << "' for 'in2'"
        ;
  }

  return true;
}

template< BOP OP, class T1 >
bool BinaryOp::tick2_int( const ArrayBase *in1_base )
{
  const Array< T1 > *in1 = 0;
  if( !in1_base->getAs( in1 ) ) throw GetAsFailed( getInput( INPUT_IN1 ), in1 );

  const ArrayBase *const in2_base = getInput( INPUT_IN2 )->getData();
  if( !in2_base ) throw NotConnected( getInput( INPUT_IN2 ) );

  switch( in2_base->getType() )
  {
    case Type::BOOL  : return tick3< OP, T1, bool     >( in1, in2_base );
    case Type::UINT8 : return tick3< OP, T1, uint8_t  >( in1, in2_base );
    case Type::INT8  : return tick3< OP, T1, int8_t   >( in1, in2_base );
    case Type::UINT16: return tick3< OP, T1, uint16_t >( in1, in2_base );
    case Type::INT16 : return tick3< OP, T1, int16_t  >( in1, in2_base );
    case Type::UINT32: return tick3< OP, T1, uint32_t >( in1, in2_base );
    case Type::INT32 : return tick3< OP, T1, int32_t  >( in1, in2_base );
    case Type::UINT64: return tick3< OP, T1, uint64_t >( in1, in2_base );
    case Type::INT64 : return tick3< OP, T1, int64_t  >( in1, in2_base );
    default:
      throw Exception()
        << "unsupported type '" << in2_base->getType().getTypeName() << "' for 'in2'"
        << ": Only integer types supported for op '" << getBOPStr( m_op ) << "'"
        ;
  }

  return true;
}

template< BOP OP, class T1 >
bool BinaryOp::tick2_no_string( const ArrayBase *in1_base )
{
  const Array< T1 > *in1 = 0;
  if( !in1_base->getAs( in1 ) ) throw GetAsFailed( getInput( INPUT_IN1 ), in1 );

  const ArrayBase *const in2_base = getInput( INPUT_IN2 )->getData();
  if( !in2_base ) throw NotConnected( getInput( INPUT_IN2 ) );

  switch( in2_base->getType() )
  {
    case Type::BOOL  : return tick3< OP, T1, bool     >( in1, in2_base );
    case Type::UINT8 : return tick3< OP, T1, uint8_t  >( in1, in2_base );
    case Type::INT8  : return tick3< OP, T1, int8_t   >( in1, in2_base );
    case Type::UINT16: return tick3< OP, T1, uint16_t >( in1, in2_base );
    case Type::INT16 : return tick3< OP, T1, int16_t  >( in1, in2_base );
    case Type::UINT32: return tick3< OP, T1, uint32_t >( in1, in2_base );
    case Type::INT32 : return tick3< OP, T1, int32_t  >( in1, in2_base );
    case Type::UINT64: return tick3< OP, T1, uint64_t >( in1, in2_base );
    case Type::INT64 : return tick3< OP, T1, int64_t  >( in1, in2_base );
    case Type::FLOAT : return tick3< OP, T1, float    >( in1, in2_base );
    case Type::DOUBLE: return tick3< OP, T1, double   >( in1, in2_base );
    default:
      throw Exception()
        << "unsupported type '" << in2_base->getType().getTypeName() << "' for 'in2'"
        << ": All but string type supported for op '" << getBOPStr( m_op ) << "'"
        ;
  }

  return true;
}

template< BOP OP >
bool BinaryOp::tick1( void )
{
  const ArrayBase *const in1_base = getInput( INPUT_IN1 )->getData();
  if( !in1_base ) throw NotConnected( getInput( INPUT_IN1 ) );

  switch( in1_base->getType() )
  {
    case Type::BOOL  : return tick2< OP, bool     >( in1_base );
    case Type::UINT8 : return tick2< OP, uint8_t  >( in1_base );
    case Type::INT8  : return tick2< OP, int8_t   >( in1_base );
    case Type::UINT16: return tick2< OP, uint16_t >( in1_base );
    case Type::INT16 : return tick2< OP, int16_t  >( in1_base );
    case Type::UINT32: return tick2< OP, uint32_t >( in1_base );
    case Type::INT32 : return tick2< OP, int32_t  >( in1_base );
    case Type::UINT64: return tick2< OP, uint64_t >( in1_base );
    case Type::INT64 : return tick2< OP, int64_t  >( in1_base );
    case Type::FLOAT : return tick2< OP, float    >( in1_base );
    case Type::DOUBLE: return tick2< OP, double   >( in1_base );
    case Type::STRING: return tick2< OP, String   >( in1_base );
    default:
      throw Exception()
        << "unsupported type '" << in1_base->getType().getTypeName() << "' for 'in1'"
        ;
  }

  return true;
}

template< BOP OP >
bool BinaryOp::tick1_int( void )
{
  const ArrayBase *const in1_base = getInput( INPUT_IN1 )->getData();
  if( !in1_base ) throw NotConnected( getInput( INPUT_IN1 ) );

  switch( in1_base->getType() )
  {
    case Type::BOOL  : return tick2_int< OP, bool     >( in1_base );
    case Type::UINT8 : return tick2_int< OP, uint8_t  >( in1_base );
    case Type::INT8  : return tick2_int< OP, int8_t   >( in1_base );
    case Type::UINT16: return tick2_int< OP, uint16_t >( in1_base );
    case Type::INT16 : return tick2_int< OP, int16_t  >( in1_base );
    case Type::UINT32: return tick2_int< OP, uint32_t >( in1_base );
    case Type::INT32 : return tick2_int< OP, int32_t  >( in1_base );
    case Type::UINT64: return tick2_int< OP, uint64_t >( in1_base );
    case Type::INT64 : return tick2_int< OP, int64_t  >( in1_base );
    default:
      throw Exception()
        << "unsupported type '" << in1_base->getType().getTypeName() << "' for 'in1'"
        << ": Only integer types supported for op '" << getBOPStr( m_op ) << "'"
        ;
  }

  return true;
}

template< BOP OP >
bool BinaryOp::tick1_no_string( void )
{
  const ArrayBase *const in1_base = getInput( INPUT_IN1 )->getData();
  if( !in1_base ) throw NotConnected( getInput( INPUT_IN1 ) );

  switch( in1_base->getType() )
  {
    case Type::BOOL  : return tick2_no_string< OP, bool     >( in1_base );
    case Type::UINT8 : return tick2_no_string< OP, uint8_t  >( in1_base );
    case Type::INT8  : return tick2_no_string< OP, int8_t   >( in1_base );
    case Type::UINT16: return tick2_no_string< OP, uint16_t >( in1_base );
    case Type::INT16 : return tick2_no_string< OP, int16_t  >( in1_base );
    case Type::UINT32: return tick2_no_string< OP, uint32_t >( in1_base );
    case Type::INT32 : return tick2_no_string< OP, int32_t  >( in1_base );
    case Type::UINT64: return tick2_no_string< OP, uint64_t >( in1_base );
    case Type::INT64 : return tick2_no_string< OP, int64_t  >( in1_base );
    case Type::FLOAT : return tick2_no_string< OP, float    >( in1_base );
    case Type::DOUBLE: return tick2_no_string< OP, double   >( in1_base );
    default:
      throw Exception()
        << "unsupported type '" << in1_base->getType().getTypeName() << "' for 'in1'"
        << ": All but string type supported for op '" << getBOPStr( m_op ) << "'"
        ;
  }

  return true;
}

bool BinaryOp::tick( void )
{
  if( !hasAnyInputChanged() ) return true;
  setAllOutputChanged();

  switch( m_op )
  {
    case BOP_LEFT   : return tick1_int      < BOP_LEFT    >();
    case BOP_RIGHT  : return tick1_int      < BOP_RIGHT   >();
    case BOP_LT     : return tick1          < BOP_LT      >();
    case BOP_LE     : return tick1          < BOP_LE      >();
    case BOP_GT     : return tick1          < BOP_GT      >();
    case BOP_GE     : return tick1          < BOP_GE      >();
    case BOP_EQ     : return tick1          < BOP_EQ      >();
    case BOP_NE     : return tick1          < BOP_NE      >();
    case BOP_LOG_AND: return tick1_no_string< BOP_LOG_AND >();
    case BOP_LOG_OR : return tick1_no_string< BOP_LOG_OR  >();
    case BOP_LOG_XOR: return tick1_no_string< BOP_LOG_XOR >();
    case BOP_BIT_AND: return tick1_int      < BOP_BIT_AND >();
    case BOP_BIT_OR : return tick1_int      < BOP_BIT_OR  >();
    case BOP_BIT_XOR: return tick1_int      < BOP_BIT_XOR >();
    case BOP_MUL    : return tick1_no_string< BOP_MUL     >();
    case BOP_DIV    : return tick1_no_string< BOP_DIV     >();
    case BOP_ADD    : return tick1          < BOP_ADD     >();
    case BOP_SUB    : return tick1_no_string< BOP_SUB     >();
    case BOP_MOD    : return tick1_int      < BOP_MOD     >();
    case BOP_UNDEFINED:
      throw Exception() << "Param 'op' was not set";
  }

  throw Exception() << "Broken 'op'";
}

void BinaryOp::gc_clear( void )
{
  Node::gc_clear();
}

void BinaryOp::gc_getChildren( Children &children ) const
{
  Base::gc_getChildren( children );
}


} // namespace RPGML

RPGML_CREATE_NODE( BinaryOp,  )


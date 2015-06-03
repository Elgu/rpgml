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
#ifndef RPGML_core_Blocks_h
#define RPGML_core_Blocks_h

#include <RPGML/Type.h>
#include <RPGML/Refcounted.h>
#include <RPGML/Exception.h>

namespace RPGML {
namespace core {

class BlockBase : public Refcounted
{
  typedef Refcounted Base;
public:
  EXCEPTION_BASE( Exception );

  BlockBase( void ) {}
  virtual ~BlockBase( void ) {}

  template< class BlockType >
  BlockType *getAs( void )
  {
    BlockType *as = dynamic_cast< BlockType* >( this );
    if( !as )
    {
      throw Exception()
        << "Could not get Block as expected BlockType"
        ;
    }
    return as;
  }
};

template< class OutType >
class Block : public BlockBase
{
  typedef BlockBase Base;
public:
  Block( void ) {}
  virtual ~Block( void ) {}

  virtual bool next( index_t &n, index_t buffer_n, typename Array< OutType >::pointer buffer_p ) = 0;
};

template< class InType, class OutType >
struct cast_impl
{
public:
  OutType operator()( const InType &in ) const { return OutType( in ); }
};

template< class InType >
struct cast_impl< InType, String >
{
public:
  String operator()( const InType &in ) const { return toString( in ); }
};

template<>
struct cast_impl< char, String >
{
public:
  String operator()( const char &in ) const { return toString( int( in ) ); }
};

template<>
struct cast_impl< unsigned char, String >
{
public:
  String operator()( const unsigned char &in ) const { return toString( int( in ) ); }
};

template< class OutType >
class CopyBlock : public Block< OutType >
{
  typedef Block< OutType > Base;
public:
  typedef Array< OutType  > InArray;

  explicit
  CopyBlock( const ArrayBase *in_base )
  {
    const InArray *in = 0;
    if( !in_base->getAs( in ) )
    {
      throw Exception()
        << "Incompatible Array as Input for CopyBlock"
        << ", expected " << getTypeName< OutType >()
        << ", is " << in_base->getType()
        ;
    }
    m_in = in;
    m_in_i = in->begin();
    m_in_end = in->end();
  }

  virtual ~CopyBlock( void ) {}

  virtual bool next( index_t &n, index_t buffer_n, typename Array< OutType >::pointer buffer_p )
  {
    index_t i = 0;
    for( ; i < buffer_n && m_in_i != m_in_end; ++i, ++m_in_i )
    {
      buffer_p[ i ] = (*m_in_i);
    }
    n = i;
    return ( n > 0 );
  }

private:
  CountPtr< const InArray > m_in;
  typename InArray::const_iterator m_in_i;
  typename InArray::const_iterator m_in_end;
};

template< class InType, class OutType >
class CastBlock : public Block< OutType >
{
  typedef Block< OutType > Base;
public:
  typedef Array< InType  > InArray;

  explicit
  CastBlock( const ArrayBase *in_base )
  {
    const InArray *in = 0;
    if( !in_base->getAs( in ) )
    {
      throw Exception()
        << "Incompatible Array as Input for CastBlock"
        << ", expected " << getTypeName< InType >()
        << ", is " << in_base->getType()
        ;
    }
    m_in = in;
    m_in_i = in->begin();
    m_in_end = in->end();
  }

  virtual ~CastBlock( void ) {}

  virtual bool next( index_t &n, index_t buffer_n, typename Array< OutType >::pointer buffer_p )
  {
    index_t i = 0;
    for( ; i < buffer_n && m_in_i != m_in_end; ++i, ++m_in_i )
    {
      const InType &in_i = (*m_in_i);
      buffer_p[ i ] = cast_impl< InType, OutType >()( in_i );
    }
    n = i;
    return ( n > 0 );
  }

private:
  CountPtr< const InArray > m_in;
  typename InArray::const_iterator m_in_i;
  typename InArray::const_iterator m_in_end;
};

template< class ToType >
static inline
CountPtr< Block< ToType > > createCastBlock( const ArrayBase *from )
{
  if( TypeOf< ToType >::E == from->getType().getEnum() )
  {
    return new CopyBlock< ToType >( from );
  }

  switch( from->getType().getEnum() )
  {
    case Type::BOOL  : return new CastBlock< bool    , ToType >( from );
    case Type::UINT8 : return new CastBlock< uint8_t , ToType >( from );
    case Type::INT8  : return new CastBlock< int8_t  , ToType >( from );
    case Type::UINT16: return new CastBlock< uint16_t, ToType >( from );
    case Type::INT16 : return new CastBlock< int16_t , ToType >( from );
    case Type::UINT32: return new CastBlock< uint32_t, ToType >( from );
    case Type::INT32 : return new CastBlock< int32_t , ToType >( from );
    case Type::UINT64: return new CastBlock< uint64_t, ToType >( from );
    case Type::INT64 : return new CastBlock< int64_t , ToType >( from );
    case Type::FLOAT : return new CastBlock< float   , ToType >( from );
    case Type::DOUBLE: return new CastBlock< double  , ToType >( from );
//      case Type::STRING: return new CastBlock< String  , ToType >( from );
    default:
      throw BlockBase::Exception()
        << "Can only create CastBlocks from and to primitive types"
        << ": from is " << from->getType()
        << ", to is " << getTypeName< ToType >()
        ;
  }
}

static inline
CountPtr< BlockBase > createCastBlock( const ArrayBase *from, const Type &to )
{
  if( from->getType() == to )
  {
    switch( to.getEnum() )
    {
      case Type::BOOL  : return new CopyBlock< bool     >( from );
      case Type::UINT8 : return new CopyBlock< uint8_t  >( from );
      case Type::INT8  : return new CopyBlock< int8_t   >( from );
      case Type::UINT16: return new CopyBlock< uint16_t >( from );
      case Type::INT16 : return new CopyBlock< int16_t  >( from );
      case Type::UINT32: return new CopyBlock< uint32_t >( from );
      case Type::INT32 : return new CopyBlock< int32_t  >( from );
      case Type::UINT64: return new CopyBlock< uint64_t >( from );
      case Type::INT64 : return new CopyBlock< int64_t  >( from );
      case Type::FLOAT : return new CopyBlock< float    >( from );
      case Type::DOUBLE: return new CopyBlock< double   >( from );
      case Type::STRING: return new CopyBlock< String   >( from );
      default:
        throw BlockBase::Exception()
          << "Can only create CastBlocks from and to primitive types"
          << ": from is " << from
          << ", to is " << to
          ;
    }
  }
  else
  {
    switch( to.getEnum() )
    {
      case Type::BOOL  : return createCastBlock< bool     >( from );
      case Type::UINT8 : return createCastBlock< uint8_t  >( from );
      case Type::INT8  : return createCastBlock< int8_t   >( from );
      case Type::UINT16: return createCastBlock< uint16_t >( from );
      case Type::INT16 : return createCastBlock< int16_t  >( from );
      case Type::UINT32: return createCastBlock< uint32_t >( from );
      case Type::INT32 : return createCastBlock< int32_t  >( from );
      case Type::UINT64: return createCastBlock< uint64_t >( from );
      case Type::INT64 : return createCastBlock< int64_t  >( from );
      case Type::FLOAT : return createCastBlock< float    >( from );
      case Type::DOUBLE: return createCastBlock< double   >( from );
      case Type::STRING: return createCastBlock< String   >( from );
      default:
        throw BlockBase::Exception()
          << "Can only create CastBlocks from and to primitive types"
          << ": from is " << from
          << ", to is " << to
          ;
    }
  }
}

template< class OutType, class InType1, class InType2, class Op >
class BinaryOpBlock : public Block< OutType >
{
  typedef Block< OutType > Base;
public:
  explicit
  BinaryOpBlock( const CountPtr< BlockBase > &in1, const CountPtr< BlockBase > &in2, const Op &op = Op() )
  : m_in1( in1->getAs< Block< InType1 > >() )
  , m_in2( in2->getAs< Block< InType2 > >() )
  , m_buffer1( 0, 1 )
  , m_buffer2( 0, 1 )
  , m_op( op )
  {}
  virtual ~BinaryOpBlock( void ) {}

  virtual bool next( index_t &n, index_t buffer_n, typename Array< OutType >::pointer buffer_p )
  {
    index_t n1 = 0;
    index_t n2 = 0;
    m_buffer1.resize( buffer_n );
    m_buffer2.resize( buffer_n );
    const typename Array< InType1 >::pointer in1 = m_buffer1.elements();
    const typename Array< InType2 >::pointer in2 = m_buffer2.elements();

    m_in1->next( n1, buffer_n, in1 );
    m_in2->next( n2, buffer_n, in2 );

    if( n1 != n2 )
    {
      throw Exception()
        << "BinaryOpBlock: in1 and in2 delivered different n"
        << ": n1 = " << n1
        << ", n2 = " << n2
        ;
    }

    if( n1 > buffer_n )
    {
      throw Exception()
        << "BinaryOpBlock: the n that was delivered is bigger than the block size"
        << ": n = " << n1
        << ", block size = " << buffer_n
        ;
    }

    for( index_t i=0; i<n1; ++i )
    {
      buffer_p[ i ] = m_op( in1[ i ], in2[ i ] );
    }

    n = n1;
    return ( n > 0 );
  }

private:
  const CountPtr< Block< InType1 > > m_in1;
  const CountPtr< Block< InType2 > > m_in2;
  Array< InType1 > m_buffer1;
  Array< InType2 > m_buffer2;
  Op m_op;
};

template< class OutType, class InType1, class InType2, class Op >
class BinaryOpBlockScalar1 : public Block< OutType >
{
  typedef Block< OutType > Base;
public:
  explicit
  BinaryOpBlockScalar1( const InType1 &in1, const CountPtr< BlockBase > &in2, const Op &op = Op() )
  : m_in1( in1 )
  , m_in2( in2->getAs< Block< InType2 > >() )
  , m_buffer2( 0, 1 )
  , m_op( op )
  {}
  virtual ~BinaryOpBlockScalar1( void ) {}

  virtual bool next( index_t &n, index_t buffer_n, typename Array< OutType >::pointer buffer_p )
  {
    index_t n2 = 0;

    m_buffer2.resize( buffer_n );
    const typename Array< InType2 >::pointer in2 = m_buffer2.elements();
    m_in2->next( n2, buffer_n, in2 );

    if( n2 > buffer_n )
    {
      throw Exception()
        << "BinaryOpBlock: the n that was delivered is bigger than the block size"
        << ": n = " << n2
        << ", block size = " << buffer_n
        ;
    }

    const InType1 &in1 = m_in1;

    for( index_t i=0; i<n2; ++i )
    {
      buffer_p[ i ] = m_op( in1, in2[ i ] );
    }

    n = n2;
    return ( n > 0 );
  }

private:
  const InType1                      m_in1;
  const CountPtr< Block< InType2 > > m_in2;
  Array< InType2 > m_buffer2;
  Op m_op;
};

template< class OutType, class InType1, class InType2, class Op >
class BinaryOpBlockScalar2 : public Block< OutType >
{
  typedef Block< OutType > Base;
public:
  explicit
  BinaryOpBlockScalar2( const CountPtr< BlockBase > &in1, const InType2 &in2, const Op &op = Op() )
  : m_in1( in1->getAs< Block< InType1 > >() )
  , m_in2( in2 )
  , m_buffer1( 0, 1 )
  , m_op( op )
  {}

  virtual ~BinaryOpBlockScalar2( void ) {}

  virtual bool next( index_t &n, index_t buffer_n, typename Array< OutType >::pointer buffer_p )
  {
    index_t n1 = 0;
    m_buffer1.resize( buffer_n );
    const typename Array< InType1 >::pointer in1 = m_buffer1.elements();
    m_in1->next( n1, buffer_n, in1 );

    if( n1 > buffer_n )
    {
      throw Exception()
        << "BinaryOpBlock: the n that was delivered is bigger than the block size"
        << ": n = " << n1
        << ", block size = " << buffer_n
        ;
    }

    const InType2 &in2 = m_in2;

    for( index_t i=0; i<n1; ++i )
    {
      buffer_p[ i ] = m_op( in1[ i ], in2 );
    }

    n = n1;
    return ( n > 0 );
  }

private:
  const CountPtr< Block< InType1 > > m_in1;
  const InType2                      m_in2;
  Array< InType1 > m_buffer1;
  Op m_op;
};

} // namespace core
} // namespace RPGML

#endif // RPGML_core_Blocks_h

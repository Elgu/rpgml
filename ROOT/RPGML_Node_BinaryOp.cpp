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

#include <iostream>

using namespace std;

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
template< class X, class Y > struct op_impl< X, Y, BOP_LEFT    > { typedef X                           Ret; Ret operator()( const X &x, const Y &y ) const { return Ret( x << y ); } };
template< class X, class Y > struct op_impl< X, Y, BOP_RIGHT   > { typedef X                           Ret; Ret operator()( const X &x, const Y &y ) const { return Ret( x >> y ); } };
template< class X, class Y > struct op_impl< X, Y, BOP_LT      > { typedef bool                        Ret; Ret operator()( const X &x, const Y &y ) const { return x <  y; } };
template< class X, class Y > struct op_impl< X, Y, BOP_LE      > { typedef bool                        Ret; Ret operator()( const X &x, const Y &y ) const { return x <= y; } };
template< class X, class Y > struct op_impl< X, Y, BOP_GT      > { typedef bool                        Ret; Ret operator()( const X &x, const Y &y ) const { return x >  y; } };
template< class X, class Y > struct op_impl< X, Y, BOP_GE      > { typedef bool                        Ret; Ret operator()( const X &x, const Y &y ) const { return x >= y; } };
template< class X, class Y > struct op_impl< X, Y, BOP_EQ      > { typedef bool                        Ret; Ret operator()( const X &x, const Y &y ) const { return x == y; } };
template< class X, class Y > struct op_impl< X, Y, BOP_NE      > { typedef bool                        Ret; Ret operator()( const X &x, const Y &y ) const { return x != y; } };
template< class X, class Y > struct op_impl< X, Y, BOP_LOG_AND > { typedef bool                        Ret; Ret operator()( const X &x, const Y &y ) const { return x && y; } };
template< class X, class Y > struct op_impl< X, Y, BOP_LOG_OR  > { typedef bool                        Ret; Ret operator()( const X &x, const Y &y ) const { return x || y; } };
template< class X, class Y > struct op_impl< X, Y, BOP_LOG_XOR > { typedef bool                        Ret; Ret operator()( const X &x, const Y &y ) const { return bool( x ) ^ bool( y ); } };
template< class X, class Y > struct op_impl< X, Y, BOP_BIT_AND > { typedef typename RetType< X, Y >::T Ret; Ret operator()( const X &x, const Y &y ) const { return Ret( x & y ); } };
template< class X, class Y > struct op_impl< X, Y, BOP_BIT_OR  > { typedef typename RetType< X, Y >::T Ret; Ret operator()( const X &x, const Y &y ) const { return Ret( x | y ); } };
template< class X, class Y > struct op_impl< X, Y, BOP_BIT_XOR > { typedef typename RetType< X, Y >::T Ret; Ret operator()( const X &x, const Y &y ) const { return Ret( x ^ y ); } };
template< class X, class Y > struct op_impl< X, Y, BOP_MUL     > { typedef typename RetType< X, Y >::T Ret; Ret operator()( const X &x, const Y &y ) const { return Ret( x * y ); } };
template< class X, class Y > struct op_impl< X, Y, BOP_DIV     > { typedef typename RetType< X, Y >::T Ret; Ret operator()( const X &x, const Y &y ) const { return Ret( x / y ); } };
template< class X, class Y > struct op_impl< X, Y, BOP_ADD     > { typedef typename RetType< X, Y >::T Ret; Ret operator()( const X &x, const Y &y ) const { return Ret( x + y ); } };
template< class X, class Y > struct op_impl< X, Y, BOP_SUB     > { typedef typename RetType< X, Y >::T Ret; Ret operator()( const X &x, const Y &y ) const { return Ret( x - y ); } };
template< class X, class Y > struct op_impl< X, Y, BOP_MOD     > { typedef typename RetType< X, Y >::T Ret; Ret operator()( const X &x, const Y &y ) const { return Ret( x % y ); } };
template< class X, class Y > struct op_impl< X, Y, BOP_MIN     > { typedef typename RetType< X, Y >::T Ret; Ret operator()( const X &x, const Y &y ) const { return std::min( Ret(x), Ret(y) ); } };
template< class X, class Y > struct op_impl< X, Y, BOP_MAX     > { typedef typename RetType< X, Y >::T Ret; Ret operator()( const X &x, const Y &y ) const { return std::max( Ret(x), Ret(y) ); } };

static
Type getRetType( Type in1_type, Type in2_type, BOP op )
{
  switch( op )
  {
    case BOP_UNDEFINED : throw BinaryOp::Exception() << "Undefined BOP";
    case BOP_LEFT   : return in1_type;
    case BOP_RIGHT  : return in1_type;
    case BOP_LT     : return Type::Bool();
    case BOP_LE     : return Type::Bool();
    case BOP_GT     : return Type::Bool();
    case BOP_GE     : return Type::Bool();
    case BOP_EQ     : return Type::Bool();
    case BOP_NE     : return Type::Bool();
    case BOP_LOG_AND: return Type::Bool();
    case BOP_LOG_OR : return Type::Bool();
    case BOP_LOG_XOR: return Type::Bool();
    case BOP_BIT_AND: return Type::Ret( in1_type, in2_type );
    case BOP_BIT_OR : return Type::Ret( in1_type, in2_type );
    case BOP_BIT_XOR: return Type::Ret( in1_type, in2_type );
    case BOP_MUL    : return Type::Ret( in1_type, in2_type );
    case BOP_DIV    : return Type::Ret( in1_type, in2_type );
    case BOP_ADD    : return Type::Ret( in1_type, in2_type );
    case BOP_SUB    : return Type::Ret( in1_type, in2_type );
    case BOP_MOD    : return Type::Ret( in1_type, in2_type );
    case BOP_MIN    : return Type::Ret( in1_type, in2_type );
    case BOP_MAX    : return Type::Ret( in1_type, in2_type );
    // No default, so compiler can issue a warning
  }

  throw BinaryOp::Exception()
    << __FILE__":" << __LINE__ << ": Internal: Invalid or not implemented BOP"
    ;
}



  class BlockBase : public Refcounted
  {
  public:
    BlockBase( void ) {}
    virtual ~BlockBase( void ) {}

    template< class BlockType >
    BlockType *getAs( void )
    {
      BlockType *as = dynamic_cast< BlockType* >( this );
      if( !as )
      {
        throw BinaryOp::Exception()
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
        throw BinaryOp::Exception()
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
        throw BinaryOp::Exception()
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
  static
  CountPtr< BlockBase > createCastBlock( const ArrayBase *from )
  {
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
        throw BinaryOp::Exception()
          << "Can only create CastBlocks from and to primitive types"
          << ": from is " << from->getType()
          << ", to is " << getTypeName< ToType >()
          ;
    }
  }

  static
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
          throw BinaryOp::Exception()
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
          throw BinaryOp::Exception()
            << "Can only create CastBlocks from and to primitive types"
            << ": from is " << from
            << ", to is " << to
            ;
      }
    }
  }

  template< class OutType, class InType1, class InType2, BOP OP >
  class BinaryOpBlock : public Block< OutType >
  {
    typedef Block< OutType > Base;
  public:
    explicit
    BinaryOpBlock( const CountPtr< BlockBase > &in1, const CountPtr< BlockBase > &in2 )
    : m_in1( in1->getAs< Block< InType1 > >() )
    , m_in2( in2->getAs< Block< InType2 > >() )
    , m_buffer1( 0, 1 )
    , m_buffer2( 0, 1 )
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
        throw BinaryOp::Exception()
          << "BinaryOpBlock: in1 and in2 delivered different n"
          << ": n1 = " << n1
          << ", n2 = " << n2
          ;
      }

      if( n1 > buffer_n )
      {
        throw BinaryOp::Exception()
          << "BinaryOpBlock: the n that was delivered is bigger than the block size"
          << ": n = " << n1
          << ", block size = " << buffer_n
          ;
      }

      const op_impl< InType1, InType2, OP > op;

      for( index_t i=0; i<n1; ++i )
      {
        buffer_p[ i ] = op( in1[ i ], in2[ i ] );
      }

      n = n1;
      return ( n > 0 );
    }

  private:
    const CountPtr< Block< InType1 > > m_in1;
    const CountPtr< Block< InType2 > > m_in2;
    Array< InType1 > m_buffer1;
    Array< InType2 > m_buffer2;
  };

  template< class OutType, class InType1, class InType2, BOP OP >
  class BinaryOpBlockScalar1 : public Block< OutType >
  {
    typedef Block< OutType > Base;
  public:
    explicit
    BinaryOpBlockScalar1( const InType1 &in1, const CountPtr< BlockBase > &in2 )
    : m_in1( in1 )
    , m_in2( in2->getAs< Block< InType2 > >() )
    , m_buffer2( 0, 1 )
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
        throw BinaryOp::Exception()
          << "BinaryOpBlock: the n that was delivered is bigger than the block size"
          << ": n = " << n2
          << ", block size = " << buffer_n
          ;
      }

      const InType1 &in1 = m_in1;

      const op_impl< InType1, InType2, OP > op;

      for( index_t i=0; i<n2; ++i )
      {
        buffer_p[ i ] = op( in1, in2[ i ] );
      }

      n = n2;
      return ( n > 0 );
    }

  private:
    const InType1                      m_in1;
    const CountPtr< Block< InType2 > > m_in2;
    Array< InType2 > m_buffer2;
  };

  template< class OutType, class InType1, class InType2, BOP OP >
  class BinaryOpBlockScalar2 : public Block< OutType >
  {
    typedef Block< OutType > Base;
  public:
    explicit
    BinaryOpBlockScalar2( const CountPtr< BlockBase > &in1, const InType2 &in2 )
    : m_in1( in1->getAs< Block< InType1 > >() )
    , m_in2( in2 )
    , m_buffer1( 0, 1 )
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
        throw BinaryOp::Exception()
          << "BinaryOpBlock: the n that was delivered is bigger than the block size"
          << ": n = " << n1
          << ", block size = " << buffer_n
          ;
      }

      const InType2 &in2 = m_in2;

      const op_impl< InType1, InType2, OP > op;

      for( index_t i=0; i<n1; ++i )
      {
        buffer_p[ i ] = op( in1[ i ], in2 );
      }

      n = n1;
      return ( n > 0 );
    }

  private:
    const CountPtr< Block< InType1 > > m_in1;
    const InType2                      m_in2;
    Array< InType1 > m_buffer1;
  };

} // namespace BinaryOp_impl

using namespace BinaryOp_impl;

template< BOP OP, class T1, class T2 >
bool BinaryOp::tick3( const ArrayBase *in1_base, const ArrayBase *in2_base )
{
  using namespace BinaryOp_impl;

  typedef op_impl< T1, T2, OP > Impl;
  typedef typename Impl::Ret Out;

  const ArrayBase::Size in1_size = in1_base->getSize();
  const ArrayBase::Size in2_size = in2_base->getSize();

  CountPtr< Block< Out > > bop;
  CountPtr< Array< Out > > out;

  if( in1_size.getDims() == 0 )
  {
    out = getOutput( OUTPUT_OUT )->initData< Out >( in2_size );

    typedef BinaryOpBlockScalar1< Out, T1, T2, OP > BOP_Block;
    bop = new BOP_Block(
              in1_base->getValue().to< T1 >()
            , createCastBlock( in2_base, TypeOf< T2 >::E )
            );

  }
  else if( in2_size.getDims() == 0 )
  {
    out = getOutput( OUTPUT_OUT )->initData< Out >( in1_size );

    typedef BinaryOpBlockScalar2< Out, T1, T2, OP > BOP_Block;
    bop = new BOP_Block(
              createCastBlock( in1_base, TypeOf< T1 >::E )
            , in2_base->getValue().to< T2 >()
            );
  }
  else if( in1_size == in2_size )
  {
    out = getOutput( OUTPUT_OUT )->initData< Out >( in1_size );

    typedef BinaryOpBlock< Out, T1, T2, OP > BOP_Block;
    bop = new BOP_Block(
              createCastBlock( in1_base, TypeOf< T1 >::E )
            , createCastBlock( in2_base, TypeOf< T2 >::E )
            );
  }
  else
  {
    throw Exception()
      << "If none of the operands for '" << getBOPStr( OP ) << "'"
      << " are scalar (0d), the sizes and dimensions must match"
      << ": in1 (left) is " << in1_size
      << ", in2 (right) is " << in2_size
      ;
  }

  static const index_t buffer_n = 4096;
  Array< Out > buffer( 0, 1 );
  typename Array< Out >::iterator o;
//  Out *out_p = 0;

  typedef typename Array< Out >::pointer out_pointer_t;
  out_pointer_t buffer_p = out_pointer_t();
  index_t remaining = buffer_n;

//  const bool is_dense = out->isDense();
//  if( is_dense )
//  {
//    buffer_p = out_p;
//    out_p = out->elements();
//    remaining = out->size();
//  }
//  else
  {
    buffer.resize( buffer_n );
    buffer_p = &buffer[ 0 ];
    o = out->begin();
  }

  index_t n = 0;
  while( bop->next( n, min( remaining, buffer_n ), buffer_p ) )
  {
//    if( is_dense )
//    {
//      remaining -= n;
//      out_p
//
//    }
//    else
    {
      for( index_t i=0; i<n; ++i, ++o )
      {
        (*o) = buffer_p[ i ];
      }
    }
  }

  return true;
}

template< BOP OP >
bool BinaryOp::tick1( const ArrayBase *in1_base, const ArrayBase *in2_base )
{
  const Type in1_type = in1_base->getType();
  const Type in2_type = in2_base->getType();
  const Type ret_type = getRetType( in1_type, in2_type, OP );

  switch( ret_type.getEnum() )
  {
    case Type::BOOL  : return tick3< OP, bool    , bool     >( in1_base, in2_base );
    case Type::UINT8 : return tick3< OP, uint8_t , uint8_t  >( in1_base, in2_base );
    case Type::INT8  : return tick3< OP, int8_t  , int8_t   >( in1_base, in2_base );
    case Type::UINT16: return tick3< OP, uint16_t, uint16_t >( in1_base, in2_base );
    case Type::INT16 : return tick3< OP, int16_t , int16_t  >( in1_base, in2_base );
    case Type::UINT32: return tick3< OP, uint32_t, uint32_t >( in1_base, in2_base );
    case Type::INT32 : return tick3< OP, int32_t , int32_t  >( in1_base, in2_base );
    case Type::UINT64: return tick3< OP, uint64_t, uint64_t >( in1_base, in2_base );
    case Type::INT64 : return tick3< OP, int64_t , int64_t  >( in1_base, in2_base );
    case Type::FLOAT : return tick3< OP, float   , float    >( in1_base, in2_base );
    case Type::DOUBLE: return tick3< OP, double  , double   >( in1_base, in2_base );
    case Type::STRING: return tick3< OP, String  , String   >( in1_base, in2_base );
    default:
      throw Exception()
        << "Unsupported type combination '" << in1_type << "' for 'in1'"
        << " and '" << in2_type << "' for 'in2'"
        << " for '" << getBOPStr( m_op ) << "'"
        ;
  }

  return true;
}

template< BOP OP >
bool BinaryOp::tick1_bool_ret( const ArrayBase *in1_base, const ArrayBase *in2_base )
{
  const Type in1_type = in1_base->getType();
  const Type in2_type = in2_base->getType();

  const Type common_type = Type::Ret( in1_type, in2_type );

  switch( common_type.getEnum() )
  {
    case Type::BOOL  : return tick3< OP, bool    , bool     >( in1_base, in2_base );
    case Type::UINT8 : return tick3< OP, uint8_t , uint8_t  >( in1_base, in2_base );
    case Type::INT8  : return tick3< OP, int8_t  , int8_t   >( in1_base, in2_base );
    case Type::UINT16: return tick3< OP, uint16_t, uint16_t >( in1_base, in2_base );
    case Type::INT16 : return tick3< OP, int16_t , int16_t  >( in1_base, in2_base );
    case Type::UINT32: return tick3< OP, uint32_t, uint32_t >( in1_base, in2_base );
    case Type::INT32 : return tick3< OP, int32_t , int32_t  >( in1_base, in2_base );
    case Type::UINT64: return tick3< OP, uint64_t, uint64_t >( in1_base, in2_base );
    case Type::INT64 : return tick3< OP, int64_t , int64_t  >( in1_base, in2_base );
    case Type::FLOAT : return tick3< OP, float   , float    >( in1_base, in2_base );
    case Type::DOUBLE: return tick3< OP, double  , double   >( in1_base, in2_base );
    case Type::STRING: return tick3< OP, String  , String   >( in1_base, in2_base );
    default:
      throw Exception()
        << "Unsupported type combination '" << in1_type << "' for 'in1'"
        << " and '" << in2_type << "' for 'in2'"
        << " for '" << getBOPStr( m_op ) << "'"
        ;
  }

  return true;
}

template< BOP OP >
bool BinaryOp::tick1_shift( const ArrayBase *in1_base, const ArrayBase *in2_base )
{
  const Type in1_type = in1_base->getType();
  const Type in2_type = in2_base->getType();

  switch( in1_type )
  {
    case Type::BOOL  : return tick3< OP, bool    , int8_t >( in1_base, in2_base );
    case Type::UINT8 : return tick3< OP, uint8_t , int8_t >( in1_base, in2_base );
    case Type::INT8  : return tick3< OP, int8_t  , int8_t >( in1_base, in2_base );
    case Type::UINT16: return tick3< OP, uint16_t, int8_t >( in1_base, in2_base );
    case Type::INT16 : return tick3< OP, int16_t , int8_t >( in1_base, in2_base );
    case Type::UINT32: return tick3< OP, uint32_t, int8_t >( in1_base, in2_base );
    case Type::INT32 : return tick3< OP, int32_t , int8_t >( in1_base, in2_base );
    case Type::UINT64: return tick3< OP, uint64_t, int8_t >( in1_base, in2_base );
    case Type::INT64 : return tick3< OP, int64_t , int8_t >( in1_base, in2_base );
    default:
      throw Exception()
        << "Unsupported type combination '" << in1_type << "' for 'in1'"
        << " and '" << in2_type << "' for 'in2'"
        << " for '" << getBOPStr( m_op ) << "'"
        ;
  }

  return true;
}

template< BOP OP >
bool BinaryOp::tick1_int( const ArrayBase *in1_base, const ArrayBase *in2_base )
{
  const Type in1_type = in1_base->getType();
  const Type in2_type = in2_base->getType();
  const Type ret_type = getRetType( in1_type, in2_type, OP );

  switch( ret_type.getEnum() )
  {
    case Type::BOOL  : return tick3< OP, bool    , bool     >( in1_base, in2_base );
    case Type::UINT8 : return tick3< OP, uint8_t , uint8_t  >( in1_base, in2_base );
    case Type::INT8  : return tick3< OP, int8_t  , int8_t   >( in1_base, in2_base );
    case Type::UINT16: return tick3< OP, uint16_t, uint16_t >( in1_base, in2_base );
    case Type::INT16 : return tick3< OP, int16_t , int16_t  >( in1_base, in2_base );
    case Type::UINT32: return tick3< OP, uint32_t, uint32_t >( in1_base, in2_base );
    case Type::INT32 : return tick3< OP, int32_t , int32_t  >( in1_base, in2_base );
    case Type::UINT64: return tick3< OP, uint64_t, uint64_t >( in1_base, in2_base );
    case Type::INT64 : return tick3< OP, int64_t , int64_t  >( in1_base, in2_base );
    default:
      throw Exception()
        << "Unsupported type combination '" << in1_type << "' for 'in1'"
        << " and '" << in2_type << "' for 'in2'"
        << " for '" << getBOPStr( m_op ) << "'"
        ;
  }

  return true;
}

template< BOP OP >
bool BinaryOp::tick1_no_string( const ArrayBase *in1_base, const ArrayBase *in2_base )
{
  const Type in1_type = in1_base->getType();
  const Type in2_type = in2_base->getType();

  if( in1_type.isString() || in2_type.isString() )
  {
    throw Exception()
      << "Unsupported type combination '" << in1_type << "' for 'in1'"
      << " and '" << in1_type << "' for 'in2'"
      << " for '" << getBOPStr( m_op ) << "'"
      ;
  }

  const Type ret_type = getRetType( in1_type, in2_type, OP );

  switch( ret_type.getEnum() )
  {
    case Type::BOOL  : return tick3< OP, bool    , bool     >( in1_base, in2_base );
    case Type::UINT8 : return tick3< OP, uint8_t , uint8_t  >( in1_base, in2_base );
    case Type::INT8  : return tick3< OP, int8_t  , int8_t   >( in1_base, in2_base );
    case Type::UINT16: return tick3< OP, uint16_t, uint16_t >( in1_base, in2_base );
    case Type::INT16 : return tick3< OP, int16_t , int16_t  >( in1_base, in2_base );
    case Type::UINT32: return tick3< OP, uint32_t, uint32_t >( in1_base, in2_base );
    case Type::INT32 : return tick3< OP, int32_t , int32_t  >( in1_base, in2_base );
    case Type::UINT64: return tick3< OP, uint64_t, uint64_t >( in1_base, in2_base );
    case Type::INT64 : return tick3< OP, int64_t , int64_t  >( in1_base, in2_base );
    case Type::FLOAT : return tick3< OP, float   , float    >( in1_base, in2_base );
    case Type::DOUBLE: return tick3< OP, double  , double   >( in1_base, in2_base );
    default:
      throw Exception()
        << "Unsupported type combination '" << in1_type << "' for 'in1'"
        << " and '" << in2_type << "' for 'in2'"
        << " for '" << getBOPStr( m_op ) << "'"
        ;
  }

  return true;
}

bool BinaryOp::tick( void )
{
  if( !hasAnyInputChanged() ) return true;
  setAllOutputChanged();

  GET_INPUT_BASE( INPUT_IN1, in1_base );
  GET_INPUT_BASE( INPUT_IN2, in2_base );

  switch( m_op )
  {
    case BOP_UNDEFINED: break;
    case BOP_LEFT   : return tick1_shift    < BOP_LEFT    >( in1_base, in2_base );
    case BOP_RIGHT  : return tick1_shift    < BOP_RIGHT   >( in1_base, in2_base );
    case BOP_LT     : return tick1_bool_ret < BOP_LT      >( in1_base, in2_base );
    case BOP_LE     : return tick1_bool_ret < BOP_LE      >( in1_base, in2_base );
    case BOP_GT     : return tick1_bool_ret < BOP_GT      >( in1_base, in2_base );
    case BOP_GE     : return tick1_bool_ret < BOP_GE      >( in1_base, in2_base );
    case BOP_EQ     : return tick1_bool_ret < BOP_EQ      >( in1_base, in2_base );
    case BOP_NE     : return tick1_bool_ret < BOP_NE      >( in1_base, in2_base );
    case BOP_LOG_AND: return tick3< BOP_LOG_AND, bool, bool >( in1_base, in2_base );
    case BOP_LOG_OR : return tick3< BOP_LOG_OR , bool, bool >( in1_base, in2_base );
    case BOP_LOG_XOR: return tick3< BOP_LOG_XOR, bool, bool >( in1_base, in2_base );
    case BOP_BIT_AND: return tick1_int      < BOP_BIT_AND >( in1_base, in2_base );
    case BOP_BIT_OR : return tick1_int      < BOP_BIT_OR  >( in1_base, in2_base );
    case BOP_BIT_XOR: return tick1_int      < BOP_BIT_XOR >( in1_base, in2_base );
    case BOP_MUL    : return tick1_no_string< BOP_MUL     >( in1_base, in2_base );
    case BOP_DIV    : return tick1_no_string< BOP_DIV     >( in1_base, in2_base );
    case BOP_ADD    : return tick1          < BOP_ADD     >( in1_base, in2_base );
    case BOP_SUB    : return tick1_no_string< BOP_SUB     >( in1_base, in2_base );
    case BOP_MOD    : return tick1_int      < BOP_MOD     >( in1_base, in2_base );
    case BOP_MIN    : return tick1          < BOP_MIN     >( in1_base, in2_base );
    case BOP_MAX    : return tick1          < BOP_MAX     >( in1_base, in2_base );
    // no default, so compiler will find a missing case
  }

  throw Exception() << "Param 'op' was not set or is broken.";
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


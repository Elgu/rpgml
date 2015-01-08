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

#include <RPGML/Type.h>
#include <RPGML/Refcounted.h>
#include <RPGML/Frame.h>
#include <RPGML/Function.h>
#include <RPGML/Node.h>
#include <RPGML/Sequence.h>
#include <RPGML/Array.h>

#include <iostream>

using namespace RPGML;
using namespace std;

class utest_Type : public CppUnit::TestFixture
{
  CPPUNIT_TEST_SUITE( utest_Type );

  CPPUNIT_TEST( test_ctor );
  CPPUNIT_TEST( test_getTypeName );
  CPPUNIT_TEST( test_getTypeEnum );
  CPPUNIT_TEST( test_cmp );
  CPPUNIT_TEST( test_isScalar );
  CPPUNIT_TEST( test_isInteger );
  CPPUNIT_TEST( test_isPrimitive );
  CPPUNIT_TEST( test_isFoo );
  CPPUNIT_TEST( test_typeOf );

  CPPUNIT_TEST_SUITE_END();

public:
  void setUp() {}
  void tearDown() {}

  void test_ctor( void )
  {
    CPPUNIT_ASSERT( Type( Type::NIL  ).getEnum() == Type::NIL  );
    CPPUNIT_ASSERT( Type( Type::BOOL     ).getEnum() == Type::BOOL     );
    CPPUNIT_ASSERT( Type( Type::UINT8    ).getEnum() == Type::UINT8    );
    CPPUNIT_ASSERT( Type( Type::INT8     ).getEnum() == Type::INT8     );
    CPPUNIT_ASSERT( Type( Type::UINT16   ).getEnum() == Type::UINT16   );
    CPPUNIT_ASSERT( Type( Type::INT16    ).getEnum() == Type::INT16    );
    CPPUNIT_ASSERT( Type( Type::UINT32   ).getEnum() == Type::UINT32   );
    CPPUNIT_ASSERT( Type( Type::INT32    ).getEnum() == Type::INT32    );
    CPPUNIT_ASSERT( Type( Type::UINT64   ).getEnum() == Type::UINT64   );
    CPPUNIT_ASSERT( Type( Type::INT64    ).getEnum() == Type::INT64    );
    CPPUNIT_ASSERT( Type( Type::FLOAT    ).getEnum() == Type::FLOAT    );
    CPPUNIT_ASSERT( Type( Type::DOUBLE   ).getEnum() == Type::DOUBLE   );
    CPPUNIT_ASSERT( Type( Type::STRING   ).getEnum() == Type::STRING   );
    CPPUNIT_ASSERT( Type( Type::FRAME    ).getEnum() == Type::FRAME    );
    CPPUNIT_ASSERT( Type( Type::FUNCTION ).getEnum() == Type::FUNCTION );
    CPPUNIT_ASSERT( Type( Type::NODE     ).getEnum() == Type::NODE     );
    CPPUNIT_ASSERT( Type( Type::OUTPUT   ).getEnum() == Type::OUTPUT   );
    CPPUNIT_ASSERT( Type( Type::INPUT    ).getEnum() == Type::INPUT    );
    CPPUNIT_ASSERT( Type( Type::PARAM    ).getEnum() == Type::PARAM    );
    CPPUNIT_ASSERT( Type( Type::SEQUENCE ).getEnum() == Type::SEQUENCE );
    CPPUNIT_ASSERT( Type( Type::ARRAY    ).getEnum() == Type::ARRAY    );
    CPPUNIT_ASSERT( Type( Type::OTHER    ).getEnum() == Type::OTHER    );

    Type t;
    CPPUNIT_ASSERT( t == Type::Nil() );
    t = Type::DOUBLE;
    CPPUNIT_ASSERT( t == Type::Double() );
  }

  void test_getTypeName( void )
  {
    CPPUNIT_ASSERT( std::string( "nil"      ) == Type( Type::NIL      ).getTypeName() );
    CPPUNIT_ASSERT( std::string( "bool"     ) == Type( Type::BOOL     ).getTypeName() );
    CPPUNIT_ASSERT( std::string( "uint8"    ) == Type( Type::UINT8    ).getTypeName() );
    CPPUNIT_ASSERT( std::string( "int8"     ) == Type( Type::INT8     ).getTypeName() );
    CPPUNIT_ASSERT( std::string( "uint16"   ) == Type( Type::UINT16   ).getTypeName() );
    CPPUNIT_ASSERT( std::string( "int16"    ) == Type( Type::INT16    ).getTypeName() );
    CPPUNIT_ASSERT( std::string( "uint32"   ) == Type( Type::UINT32   ).getTypeName() );
    CPPUNIT_ASSERT( std::string( "int32"    ) == Type( Type::INT32    ).getTypeName() );
    CPPUNIT_ASSERT( std::string( "uint64"   ) == Type( Type::UINT64   ).getTypeName() );
    CPPUNIT_ASSERT( std::string( "int64"    ) == Type( Type::INT64    ).getTypeName() );
    CPPUNIT_ASSERT( std::string( "float"    ) == Type( Type::FLOAT    ).getTypeName() );
    CPPUNIT_ASSERT( std::string( "double"   ) == Type( Type::DOUBLE   ).getTypeName() );
    CPPUNIT_ASSERT( std::string( "string"   ) == Type( Type::STRING   ).getTypeName() );
    CPPUNIT_ASSERT( std::string( "Frame"    ) == Type( Type::FRAME    ).getTypeName() );
    CPPUNIT_ASSERT( std::string( "Function" ) == Type( Type::FUNCTION ).getTypeName() );
    CPPUNIT_ASSERT( std::string( "Node"     ) == Type( Type::NODE     ).getTypeName() );
    CPPUNIT_ASSERT( std::string( "Output"   ) == Type( Type::OUTPUT   ).getTypeName() );
    CPPUNIT_ASSERT( std::string( "Input"    ) == Type( Type::INPUT    ).getTypeName() );
    CPPUNIT_ASSERT( std::string( "Param"    ) == Type( Type::PARAM    ).getTypeName() );
    CPPUNIT_ASSERT( std::string( "Sequence" ) == Type( Type::SEQUENCE ).getTypeName() );
    CPPUNIT_ASSERT( std::string( "Array"    ) == Type( Type::ARRAY    ).getTypeName() );
    CPPUNIT_ASSERT( std::string( "other"    ) == Type( Type::OTHER    ).getTypeName() );
  }

  void test_getTypeEnum( void )
  {
    CPPUNIT_ASSERT_EQUAL( Type::Nil     (), Type( "nil"      ) );
    CPPUNIT_ASSERT_EQUAL( Type::Bool    (), Type( "bool"     ) );
    CPPUNIT_ASSERT_EQUAL( Type::UInt8   (), Type( "uint8"    ) );
    CPPUNIT_ASSERT_EQUAL( Type::Int8    (), Type( "int8"     ) );
    CPPUNIT_ASSERT_EQUAL( Type::UInt16  (), Type( "uint16"   ) );
    CPPUNIT_ASSERT_EQUAL( Type::Int16   (), Type( "int16"    ) );
    CPPUNIT_ASSERT_EQUAL( Type::UInt32  (), Type( "uint32"   ) );
    CPPUNIT_ASSERT_EQUAL( Type::Int32   (), Type( "int32"    ) );
    CPPUNIT_ASSERT_EQUAL( Type::UInt64  (), Type( "uint64"   ) );
    CPPUNIT_ASSERT_EQUAL( Type::Int64   (), Type( "int64"    ) );
    CPPUNIT_ASSERT_EQUAL( Type::Float   (), Type( "float"    ) );
    CPPUNIT_ASSERT_EQUAL( Type::Double  (), Type( "double"   ) );
    CPPUNIT_ASSERT_EQUAL( Type::String  (), Type( "string"   ) );
    CPPUNIT_ASSERT_EQUAL( Type::Frame   (), Type( "Frame"    ) );
    CPPUNIT_ASSERT_EQUAL( Type::Function(), Type( "Function" ) );
    CPPUNIT_ASSERT_EQUAL( Type::Node    (), Type( "Node"     ) );
    CPPUNIT_ASSERT_EQUAL( Type::Output  (), Type( "Output"   ) );
    CPPUNIT_ASSERT_EQUAL( Type::Input   (), Type( "Input"    ) );
    CPPUNIT_ASSERT_EQUAL( Type::Param   (), Type( "Param"    ) );
    CPPUNIT_ASSERT_EQUAL( Type::Sequence(), Type( "Sequence" ) );
    CPPUNIT_ASSERT_EQUAL( Type::Array   (), Type( "Array"    ) );
    CPPUNIT_ASSERT_EQUAL( Type::Other   (), Type( "other"    ) );
  }

  void test_cmp( void )
  {
    CPPUNIT_ASSERT( Type::Int8() == Type( Type::INT8 ) );
    CPPUNIT_ASSERT( Type::Int16() != Type( Type::INT8 ) );

    CPPUNIT_ASSERT( Type::Int8()  < Type::Int16() );
    CPPUNIT_ASSERT( Type::UInt8() < Type::Int8() );

    CPPUNIT_ASSERT( Type::Int16() > Type::Int8() );
    CPPUNIT_ASSERT( Type::Int8()  > Type::UInt8() );

    CPPUNIT_ASSERT( Type::UInt8() <= Type::Int8() );
    CPPUNIT_ASSERT( Type::Int8()  <= Type::Int8() );

    CPPUNIT_ASSERT( Type::Int8()  >= Type::UInt8() );
    CPPUNIT_ASSERT( Type::Int8()  >= Type::Int8() );
  }

  void test_isScalar( void )
  {
    CPPUNIT_ASSERT( !Type::Nil().isScalar() );
    CPPUNIT_ASSERT( Type::Bool    ().isScalar() );
    CPPUNIT_ASSERT( Type::UInt8   ().isScalar() );
    CPPUNIT_ASSERT( Type::Int8    ().isScalar() );
    CPPUNIT_ASSERT( Type::UInt16  ().isScalar() );
    CPPUNIT_ASSERT( Type::Int16   ().isScalar() );
    CPPUNIT_ASSERT( Type::UInt32  ().isScalar() );
    CPPUNIT_ASSERT( Type::Int32   ().isScalar() );
    CPPUNIT_ASSERT( Type::UInt64  ().isScalar() );
    CPPUNIT_ASSERT( Type::Int64   ().isScalar() );
    CPPUNIT_ASSERT( Type::Float   ().isScalar() );
    CPPUNIT_ASSERT( Type::Double  ().isScalar() );
    CPPUNIT_ASSERT( !Type::String  ().isScalar() );
    CPPUNIT_ASSERT( !Type::Frame   ().isScalar() );
    CPPUNIT_ASSERT( !Type::Function().isScalar() );
    CPPUNIT_ASSERT( !Type::Node    ().isScalar() );
    CPPUNIT_ASSERT( !Type::Output  ().isScalar() );
    CPPUNIT_ASSERT( !Type::Input   ().isScalar() );
    CPPUNIT_ASSERT( !Type::Param   ().isScalar() );
    CPPUNIT_ASSERT( !Type::Sequence().isScalar() );
    CPPUNIT_ASSERT( !Type::Array   ().isScalar() );
    CPPUNIT_ASSERT( !Type::Other   ().isScalar() );
  }

  void test_isInteger( void )
  {
    CPPUNIT_ASSERT( !Type::Nil().isInteger() );
    CPPUNIT_ASSERT( Type::Bool    ().isInteger() );
    CPPUNIT_ASSERT( Type::UInt8   ().isInteger() );
    CPPUNIT_ASSERT( Type::Int8    ().isInteger() );
    CPPUNIT_ASSERT( Type::UInt16  ().isInteger() );
    CPPUNIT_ASSERT( Type::Int16   ().isInteger() );
    CPPUNIT_ASSERT( Type::UInt32  ().isInteger() );
    CPPUNIT_ASSERT( Type::Int32   ().isInteger() );
    CPPUNIT_ASSERT( Type::UInt64  ().isInteger() );
    CPPUNIT_ASSERT( Type::Int64   ().isInteger() );
    CPPUNIT_ASSERT( !Type::Float   ().isInteger() );
    CPPUNIT_ASSERT( !Type::Double  ().isInteger() );
    CPPUNIT_ASSERT( !Type::String  ().isInteger() );
    CPPUNIT_ASSERT( !Type::Frame   ().isInteger() );
    CPPUNIT_ASSERT( !Type::Function().isInteger() );
    CPPUNIT_ASSERT( !Type::Node    ().isInteger() );
    CPPUNIT_ASSERT( !Type::Output  ().isInteger() );
    CPPUNIT_ASSERT( !Type::Input   ().isInteger() );
    CPPUNIT_ASSERT( !Type::Param   ().isInteger() );
    CPPUNIT_ASSERT( !Type::Sequence().isInteger() );
    CPPUNIT_ASSERT( !Type::Array   ().isInteger() );
    CPPUNIT_ASSERT( !Type::Other   ().isInteger() );
  }

  void test_isPrimitive( void )
  {
    CPPUNIT_ASSERT( !Type::Nil().isPrimitive() );
    CPPUNIT_ASSERT( Type::Bool    ().isPrimitive() );
    CPPUNIT_ASSERT( Type::UInt8   ().isPrimitive() );
    CPPUNIT_ASSERT( Type::Int8    ().isPrimitive() );
    CPPUNIT_ASSERT( Type::UInt16  ().isPrimitive() );
    CPPUNIT_ASSERT( Type::Int16   ().isPrimitive() );
    CPPUNIT_ASSERT( Type::UInt32  ().isPrimitive() );
    CPPUNIT_ASSERT( Type::Int32   ().isPrimitive() );
    CPPUNIT_ASSERT( Type::UInt64  ().isPrimitive() );
    CPPUNIT_ASSERT( Type::Int64   ().isPrimitive() );
    CPPUNIT_ASSERT( Type::Float   ().isPrimitive() );
    CPPUNIT_ASSERT( Type::Double  ().isPrimitive() );
    CPPUNIT_ASSERT( Type::String  ().isPrimitive() );
    CPPUNIT_ASSERT( !Type::Frame   ().isPrimitive() );
    CPPUNIT_ASSERT( !Type::Function().isPrimitive() );
    CPPUNIT_ASSERT( !Type::Node    ().isPrimitive() );
    CPPUNIT_ASSERT( !Type::Output  ().isPrimitive() );
    CPPUNIT_ASSERT( !Type::Input   ().isPrimitive() );
    CPPUNIT_ASSERT( !Type::Param   ().isPrimitive() );
    CPPUNIT_ASSERT( !Type::Sequence().isPrimitive() );
    CPPUNIT_ASSERT( !Type::Array   ().isPrimitive() );
    CPPUNIT_ASSERT( !Type::Other   ().isPrimitive() );
  }

  void test_isFoo( void )
  {
    CPPUNIT_ASSERT( Type::Nil ().isNil () );
    CPPUNIT_ASSERT( Type::Bool    ().isBool    () );
    CPPUNIT_ASSERT( Type::UInt8   ().isUInt8   () );
    CPPUNIT_ASSERT( Type::Int8    ().isInt8    () );
    CPPUNIT_ASSERT( Type::UInt16  ().isUInt16  () );
    CPPUNIT_ASSERT( Type::Int16   ().isInt16   () );
    CPPUNIT_ASSERT( Type::UInt32  ().isUInt32  () );
    CPPUNIT_ASSERT( Type::Int32   ().isInt32   () );
    CPPUNIT_ASSERT( Type::UInt64  ().isUInt64  () );
    CPPUNIT_ASSERT( Type::Int64   ().isInt64   () );
    CPPUNIT_ASSERT( Type::Float   ().isFloat   () );
    CPPUNIT_ASSERT( Type::Double  ().isDouble  () );
    CPPUNIT_ASSERT( Type::String  ().isString  () );
    CPPUNIT_ASSERT( Type::Frame   ().isFrame   () );
    CPPUNIT_ASSERT( Type::Function().isFunction() );
    CPPUNIT_ASSERT( Type::Node    ().isNode    () );
    CPPUNIT_ASSERT( Type::Output  ().isOutput  () );
    CPPUNIT_ASSERT( Type::Input   ().isInput   () );
    CPPUNIT_ASSERT( Type::Param   ().isParam   () );
    CPPUNIT_ASSERT( Type::Sequence().isSequence() );
    CPPUNIT_ASSERT( Type::Array   ().isArray   () );
    CPPUNIT_ASSERT( Type::Other   ().isOther   () );
  }

  void test_typeOf( void )
  {
    CPPUNIT_ASSERT( Type::Bool    () == typeOf( bool      () ) );
    CPPUNIT_ASSERT( Type::UInt8   () == typeOf( uint8_t   () ) );
    CPPUNIT_ASSERT( Type::Int8    () == typeOf( int8_t    () ) );
    CPPUNIT_ASSERT( Type::UInt16  () == typeOf( uint16_t  () ) );
    CPPUNIT_ASSERT( Type::Int16   () == typeOf( int16_t   () ) );
    CPPUNIT_ASSERT( Type::UInt32  () == typeOf( uint32_t  () ) );
    CPPUNIT_ASSERT( Type::Int32   () == typeOf( int32_t   () ) );
    CPPUNIT_ASSERT( Type::UInt64  () == typeOf( uint64_t  () ) );
    CPPUNIT_ASSERT( Type::Int64   () == typeOf( int64_t   () ) );
    CPPUNIT_ASSERT( Type::Float   () == typeOf( float     () ) );
    CPPUNIT_ASSERT( Type::Double  () == typeOf( double    () ) );
    CPPUNIT_ASSERT( Type::String  () == typeOf( String    () ) );
    CPPUNIT_ASSERT( Type::Frame   () == typeOf( CountPtr< Frame     > () ) );
    CPPUNIT_ASSERT( Type::Function() == typeOf( CountPtr< Function  > () ) );
    CPPUNIT_ASSERT( Type::Node    () == typeOf( CountPtr< Node      > () ) );
    CPPUNIT_ASSERT( Type::Output  () == typeOf( CountPtr< Output    > () ) );
    CPPUNIT_ASSERT( Type::Input   () == typeOf( CountPtr< Input     > () ) );
    CPPUNIT_ASSERT( Type::Param   () == typeOf( CountPtr< Param     > () ) );
    CPPUNIT_ASSERT( Type::Sequence() == typeOf( CountPtr< const Sequence  > () ) );
    CPPUNIT_ASSERT( Type::Array   () == typeOf( CountPtr< ArrayBase > () ) );

    CPPUNIT_ASSERT( Type::Other   () == typeOf( (const char*)0 ) );
    CPPUNIT_ASSERT( Type::Other   () == typeOf( std::string() ) );
  }
};

CPPUNIT_TEST_SUITE_REGISTRATION( utest_Type );


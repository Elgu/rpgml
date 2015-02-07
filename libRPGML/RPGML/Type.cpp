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
#include "Type.h"

#include <cstring>

namespace RPGML {

const char *Type::getTypeName( void ) const
{
  static
  const char *const names[] =
  {
      "nil"
    , "bool"
    , "uint8"
    , "int8"
    , "uint16"
    , "int16"
    , "uint32"
    , "int32"
    , "uint64"
    , "int64"
    , "float"
    , "double"
    , "string"
    , "Frame"
    , "Function"
    , "Node"
    , "Output"
    , "Input"
    , "InOut"
    , "Param"
    , "Sequence"
    , "Array"
    , "other"
  };

  return names[ m_e ];
}

Type::Enum Type::getTypeEnum( const char *typeName )
{
  switch( typeName[ 0 ] )
  {
    case 'A': return ( ::strcmp( typeName, "Array" ) ? OTHER : ARRAY );
    case 'F':
      if( typeName[ 1 ] == 'r' ) return ( ::strcmp( typeName, "Frame" ) ? OTHER : FRAME );
      if( typeName[ 1 ] == 'u' ) return ( ::strcmp( typeName, "Function" ) ? OTHER : FUNCTION );
      return OTHER;
    case 'I':
       if( 0 == ::strcmp( typeName, "Input" ) ) return INPUT;
       if( 0 == ::strcmp( typeName, "InOut" ) ) return INOUT;
      return OTHER;
    case 'N': return ( ::strcmp( typeName, "Node"     ) ? OTHER : NODE );
    case 'O': return ( ::strcmp( typeName, "Output"   ) ? OTHER : OUTPUT );
    case 'P': return ( ::strcmp( typeName, "Param"    ) ? OTHER : PARAM );
    case 'S': return ( ::strcmp( typeName, "Sequence" ) ? OTHER : SEQUENCE );
    case 'b': return ( ::strcmp( typeName, "bool"     ) ? OTHER : BOOL );
    case 'd': return ( ::strcmp( typeName, "double"   ) ? OTHER : DOUBLE );
    case 'f': return ( ::strcmp( typeName, "float"    ) ? OTHER : FLOAT );
    case 'i':
      if( typeName[ 3 ] == '8' ) return ( typeName[ 4 ] ? OTHER : INT8 );
      if( typeName[ 3 ] == '1' ) return ( ::strcmp( typeName+4, "6"    ) ? OTHER : INT16 );
      if( typeName[ 3 ] == '3' ) return ( ::strcmp( typeName+4, "2"    ) ? OTHER : INT32 );
      if( typeName[ 3 ] == '6' ) return ( ::strcmp( typeName+4, "4"    ) ? OTHER : INT64 );
      if( typeName[ 3 ] == '\0' ) return INT;
      return OTHER;
    case 'n': return ( ::strcmp( typeName, "nil"   ) ? OTHER : NIL );
    case 'o': return ( ::strcmp( typeName, "other"  ) ? OTHER : OTHER );
    case 's': return ( ::strcmp( typeName, "string" ) ? OTHER : STRING );
    case 'u':
      if( typeName[ 4 ] == '8' ) return ( typeName[ 5 ] ? OTHER : UINT8 );
      if( typeName[ 4 ] == '1' ) return ( ::strcmp( typeName+5, "6"    ) ? OTHER : UINT16 );
      if( typeName[ 4 ] == '3' ) return ( ::strcmp( typeName+5, "2"    ) ? OTHER : UINT32 );
      if( typeName[ 4 ] == '6' ) return ( ::strcmp( typeName+5, "4"    ) ? OTHER : UINT64 );
      return OTHER;
    default:
      return OTHER;
  }
}

Type Type::Ret( Type left, Type right )
{
  if( left.isScalar() && right.isScalar() )
  {
    if( left.hasSign() || right.hasSign() )
    {
      return WithSign( std::max( left, right ) );
    }
    else
    {
      return std::max( left, right );
    }
  }
  else if( left.isString() && right.isPrimitive() )
  {
    return String();
  }
  else if( left.isPrimitive() && right.isString() )
  {
    return String();
  }
  else
  {
    return Nil();
  }
}

} // namespace RPGML


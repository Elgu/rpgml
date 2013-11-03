#include "BinaryOp.h"

#include "ParserEnums.h"
#include "String.h"

#include <sstream>

namespace RPGML {

BinaryOp::BinaryOp( GarbageCollector *gc, Map *parent )
: Function( gc, parent, genDeclArgs() )
{}

BinaryOp::~BinaryOp( void )
{}

namespace BinaryOp_impl {

  static inline
  Value to( Type type, const Value &x )
  {
    if( type == x.getType() ) return x;

    switch( type.getEnum() )
    {
      case Type::BOOL  : return Value( -int( arg.getBool () ) );
        switch( x.getType.getEnum() )
        {
          case Type::INT   : return Value( bool( arg.getInt  () );
          case Type::FLOAT : return Value( bool( arg.getFloat() );
          case Type::STRING: throw "Cannot cast string directly to bool";
          default:
            throw "Invalid type x for to( type, x )";
        }

      case Type::INT   : return Value( -     arg.getInt  ()   );
        switch( x.getType.getEnum() )
        {
          case Type::BOOL  : return Value( int( arg.getBool() );
          case Type::FLOAT : return Value( int( arg.getFloat() );
          case Type::STRING: return Value( atoi( arg.getString()->c_str() );
          default:
            throw "Invalid type x for to( type, x )";
        }

      case Type::FLOAT : return Value( -     arg.getFloat()   );
        switch( x.getType.getEnum() )
        {
          case Type::BOOL  : return Value( float( arg.getBool() );
          case Type::INT   : return Value( float( arg.getInt() );
          case Type::STRING: return Value( atof( arg.getString()->c_str() );
          default:
            throw "Invalid type x for to( type, x )";
        }

      case Type::STRING: return Value( -     arg.getFloat()   );
        {
          std::ostringstream str;

          switch( x.getType.getEnum() )
          {
            case Type::BOOL  : str << arg.getBool(); break;
            case Type::INT   : str << arg.getInt(); break;
            case Type::FLOAT : str << arg.getFloat(); break;
            default:
              throw "Invalid type x for to( type, x )";
          }

          return Value( new String( str.str() ) );
        }


      default:
        throw "Invalid type for to( type, x )";
    }
  }


} // namespace BinaryOp_impl

bool BinaryOp::call_impl( Scope *, Value &ret, index_t n_args, const Value *args, index_t )
{
  if( n_args != 3 ) throw "BinaryOp requires 3 arguments.";

  if( !args[ 1 ].isInt() ) throw "'op' argument must be int";

  const BOP op = BOP( args[ 1 ].getInt() );
  const Value &left  = args[ 0 ];
  const Value &right = args[ 2 ];

  switch( op )
  {
    case UOP_MINUS  : ret = minus  ( x ); break;
    case UOP_PLUS   : ret = plus   ( x ); break;
    case UOP_LOG_NOT: ret = log_not( x ); break;
    case UOP_BIT_NOT: ret = bit_not( x ); break;
    default:
      throw "Invalid op";
  }

  return true;
}

CountPtr< Args > BinaryOp::genDeclArgs( void )
{
  CountPtr< Args > args = new Args( 3 );
  args->at( 0 ) = Arg( new String( "left" ) );
  args->at( 1 ) = Arg( new String( "op" ) );
  args->at( 2 ) = Arg( new String( "right" ) );
  return args;
}

} // namespace RPGML



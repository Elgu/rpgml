#include "Value.h"

#include "String.h"
#include "Array.h"
#include "Map.h"
#include "Function.h"
#include "Node.h"
#include "Sequence.h"

#include <sstream>
#include <cmath>

namespace RPGML {

Value::Value( void )
: p( 0 )
{}

Value::~Value( void )
{
  clear();
}

Value::Value( const Value &other )
: p( other.p )
, m_type( other.m_type )
{
  switch( m_type.getEnum() )
  {
    case Type::INVALID : break;
    case Type::BOOL    : break;
    case Type::INT     : break;
    case Type::FLOAT   : break;
    case Type::STRING  : str ->ref(); break;
    case Type::ARRAY   : arr ->ref(); break;
    case Type::MAP     : map ->ref(); break;
    case Type::FUNCTION: func->ref(); break;
    case Type::NODE    : node->ref(); break;
    case Type::OUTPUT  : out ->ref(); break;
    case Type::INPUT   : in  ->ref(); break;
    case Type::PARAM   : para->ref(); break;
    case Type::SEQUENCE: seq ->ref(); break;
  }
}

Value::Value( bool      _b    ) : m_type( Type::BOOL     ) { b    = _b   ; }
Value::Value( int       _i    ) : m_type( Type::INT      ) { i    = _i   ; }
Value::Value( float     _f    ) : m_type( Type::FLOAT    ) { f    = _f   ; }
Value::Value( std::string const &_str  ) : m_type( Type::STRING   ) { str  = String(_str).getData(); str->ref(); }
Value::Value( char        const *_str  ) : m_type( Type::STRING   ) { str  = String(_str).getData(); str->ref(); }
Value::Value( String      const &_str  ) : m_type( Type::STRING   ) { str  = _str.getData(); str->ref(); }
Value::Value( StringData  const *_str  ) : m_type( Type::STRING   ) { str  = _str; str->ref(); }
Value::Value( Array          *_arr  ) : m_type( Type::ARRAY    ) { arr  = _arr ; arr ->ref(); }
Value::Value( Map            *_map  ) : m_type( Type::MAP      ) { map  = _map ; map ->ref(); }
Value::Value( Function       *_func ) : m_type( Type::FUNCTION ) { func = _func; func->ref(); }
Value::Value( Node           *_node ) : m_type( Type::NODE     ) { node = _node; node->ref(); }
Value::Value( Output         *_out  ) : m_type( Type::OUTPUT   ) { out  = _out ; out ->ref(); }
Value::Value( Input          *_in   ) : m_type( Type::INPUT    ) { in   = _in  ; in  ->ref(); }
Value::Value( Param          *_para ) : m_type( Type::PARAM    ) { para = _para; para->ref(); }
Value::Value( Sequence const *_seq  ) : m_type( Type::SEQUENCE ) { seq  = _seq ; seq ->ref(); }

Value &Value::set( bool               _b    ) { return (*this) = Value( _b    ); }
Value &Value::set( int                _i    ) { return (*this) = Value( _i    ); }
Value &Value::set( float              _f    ) { return (*this) = Value( _f    ); }
Value &Value::set( std::string const &_str  ) { return (*this) = Value( _str  ); }
Value &Value::set( char        const *_str  ) { return (*this) = Value( _str  ); }
Value &Value::set( String      const &_str  ) { return (*this) = Value( _str  ); }
Value &Value::set( StringData  const *_str  ) { return (*this) = Value( _str  ); }
Value &Value::set( Array             *_arr  ) { return (*this) = Value( _arr  ); }
Value &Value::set( Map               *_map  ) { return (*this) = Value( _map  ); }
Value &Value::set( Function          *_func ) { return (*this) = Value( _func ); }
Value &Value::set( Node              *_node ) { return (*this) = Value( _node ); }
Value &Value::set( Output            *_out  ) { return (*this) = Value( _out  ); }
Value &Value::set( Input             *_in   ) { return (*this) = Value( _in   ); }
Value &Value::set( Param             *_para ) { return (*this) = Value( _para ); }
Value &Value::set( Sequence const    *_seq  ) { return (*this) = Value( _seq  ); }

Value &Value::operator=( Value other )
{
  this->swap( other );
  return (*this);
}

void Value::clear( void )
{
  switch( m_type.getEnum() )
  {
    case Type::INVALID : break;
    case Type::BOOL    : break;
    case Type::INT     : break;
    case Type::FLOAT   : break;
    case Type::STRING  : if( !str ->unref() ) delete str ; break;
    case Type::ARRAY   : if( !arr ->unref() ) delete arr ; break;
    case Type::MAP     : if( !map ->unref() ) delete map ; break;
    case Type::FUNCTION: if( !func->unref() ) delete func; break;
    case Type::NODE    : if( !node->unref() ) delete node; break;
    case Type::OUTPUT  : if( !out ->unref() ) delete out ; break;
    case Type::INPUT   : if( !in  ->unref() ) delete in  ; break;
    case Type::PARAM   : if( !para->unref() ) delete para; break;
    case Type::SEQUENCE: if( !seq ->unref() ) delete seq ; break;
  }
  p = 0;
  m_type = Type::INVALID;
}

void Value::swap( Value &other )
{
  std::swap( m_type, other.m_type );
  std::swap( p, other.p );
}

bool Value::isCollectable( void ) const
{
  static const int flags =
      ( 0 << Type::INVALID  )
    | ( 0 << Type::BOOL     )
    | ( 0 << Type::INT      )
    | ( 0 << Type::FLOAT    )
    | ( 0 << Type::STRING   )
    | ( 1 << Type::ARRAY    )
    | ( 1 << Type::MAP      )
    | ( 1 << Type::FUNCTION )
    | ( 1 << Type::NODE     )
    | ( 1 << Type::OUTPUT   )
    | ( 1 << Type::INPUT    )
    | ( 1 << Type::PARAM    )
    | ( 1 << Type::SEQUENCE )
    ;
  return flags & ( 1 << m_type.getEnum() );
}

const Collectable *Value::getCollectable( void ) const
{
  switch( m_type.getEnum() )
  {
    case Type::INVALID : return 0;
    case Type::BOOL    : return 0;
    case Type::INT     : return 0;
    case Type::FLOAT   : return 0;
    case Type::STRING  : return 0;
    case Type::ARRAY   : return arr ;
    case Type::MAP     : return map ;
    case Type::FUNCTION: return func;
    case Type::NODE    : return node;
    case Type::OUTPUT  : return out ;
    case Type::INPUT   : return in  ;
    case Type::PARAM   : return para;
    case Type::SEQUENCE: return seq ;
  }
  return 0;
}

Value Value::to( Type type ) const
{
  const Value &x = (*this);

  if( type == x.getType() ) return x;

  switch( type.getEnum() )
  {
    case Type::BOOL  :
      switch( x.getEnum() )
      {
        case Type::INT   : return Value( bool( getInt  () ) );
        case Type::FLOAT : return Value( bool( getFloat() ) );
        case Type::STRING: throw "Cannot cast string directly to bool";
        default:
          throw "Invalid type x for to( type, x )";
      }

    case Type::INT   :
      switch( x.getEnum() )
      {
        case Type::BOOL  : return Value( int( getBool()  ) );
        case Type::FLOAT : return Value( int( getFloat() ) );
        case Type::STRING: return Value( atoi( getString().c_str() ) );
        default:
          throw "Invalid type x for to( type, x )";
      }

    case Type::FLOAT :
      switch( x.getEnum() )
      {
        case Type::BOOL  : return Value( float( getBool() ) );
        case Type::INT   : return Value( float( getInt()  ) );
        case Type::STRING: return Value( float( atof( getString().c_str() ) ) );
        default:
          throw "Invalid type x for to( type, x )";
      }

    case Type::STRING:
      {
        std::ostringstream s;

        switch( x.getEnum() )
        {
          case Type::BOOL  : s << getBool(); break;
          case Type::INT   : s << getInt(); break;
          case Type::FLOAT : s << getFloat(); break;
          default:
            throw "Invalid type x for to( type, x )";
        }

        return Value( String( s.str() ) );
      }


    default:
      throw "Invalid type for Value::to( type )";
  }
}

bool Value::operator< ( const Value &right ) const
{
  const Value &left = (*this);
  if( left.isScalar() && right.isScalar() )
  {
    const Type ret_type = std::max( left.getType(), right.getType() );

    const Value l = left .to( ret_type );
    const Value r = right.to( ret_type );

    switch( ret_type.getEnum() )
    {
      case Type::BOOL : return ( l.getBool()  < r.getBool() );
      case Type::INT  : return ( l.getInt ()  < r.getInt() );
      case Type::FLOAT: return ( l.getFloat() < r.getFloat() );
      default:
        throw "Internal error: unhandled scalar";
    }
  }
  else if( left.isString() && right.isString() )
  {
    return ( left.getString() < right.getString() );
  }
  else if( left.getType() == right.getType() )
  {
    return ( left.getP() < right.getP() );
  }
  else
  {
    throw std::string( "Cannot compare " ) + left.getTypeName() + " and " + right.getTypeName() + " directly";
  }
}

bool Value::operator<=( const Value &right ) const
{
  const Value &left = (*this);
  if( left.isScalar() && right.isScalar() )
  {
    const Type ret_type = std::max( left.getType(), right.getType() );

    const Value l = left .to( ret_type );
    const Value r = right.to( ret_type );

    switch( ret_type.getEnum() )
    {
      case Type::BOOL : return ( l.getBool()  <= r.getBool() );
      case Type::INT  : return ( l.getInt ()  <= r.getInt() );
      case Type::FLOAT: return ( l.getFloat() <= r.getFloat() );
      default:
        throw "Internal error: unhandled scalar";
    }
  }
  else if( left.isString() && right.isString() )
  {
    return ( left.getString() <= right.getString() );
  }
  else if( left.getType() == right.getType() )
  {
    return ( left.getP() <= right.getP() );
  }
  else
  {
    throw std::string( "Cannot compare " ) + left.getTypeName() + " and " + right.getTypeName() + " directly";
  }
}

bool Value::operator> ( const Value &right ) const
{
  const Value &left = (*this);
  if( left.isScalar() && right.isScalar() )
  {
    const Type ret_type = std::max( left.getType(), right.getType() );

    const Value l = left .to( ret_type );
    const Value r = right.to( ret_type );

    switch( ret_type.getEnum() )
    {
      case Type::BOOL : return ( l.getBool()  > r.getBool() );
      case Type::INT  : return ( l.getInt ()  > r.getInt() );
      case Type::FLOAT: return ( l.getFloat() > r.getFloat() );
      default:
        throw "Internal error: unhandled scalar";
    }
  }
  else if( left.isString() && right.isString() )
  {
    return ( left.getString() > right.getString() );
  }
  else if( left.getType() == right.getType() )
  {
    return ( left.getP() > right.getP() );
  }
  else
  {
    throw std::string( "Cannot compare " ) + left.getTypeName() + " and " + right.getTypeName() + " directly";
  }
}

bool Value::operator>=( const Value &right ) const
{
  const Value &left = (*this);
  if( left.isScalar() && right.isScalar() )
  {
    const Type ret_type = std::max( left.getType(), right.getType() );

    const Value l = left .to( ret_type );
    const Value r = right.to( ret_type );

    switch( ret_type.getEnum() )
    {
      case Type::BOOL : return ( l.getBool()  >= r.getBool() );
      case Type::INT  : return ( l.getInt ()  >= r.getInt() );
      case Type::FLOAT: return ( l.getFloat() >= r.getFloat() );
      default:
        throw "Internal error: unhandled scalar";
    }
  }
  else if( left.isString() && right.isString() )
  {
    return ( left.getString() >= right.getString() );
  }
  else if( left.getType() == right.getType() )
  {
    return ( left.getP() >= right.getP() );
  }
  else
  {
    throw std::string( "Cannot compare " ) + left.getTypeName() + " and " + right.getTypeName() + " directly";
  }
}

bool Value::operator==( const Value &right ) const
{
  const Value &left = (*this);
  if( left.isScalar() && right.isScalar() )
  {
    const Type ret_type = std::max( left.getType(), right.getType() );

    const Value l = left .to( ret_type );
    const Value r = right.to( ret_type );

    switch( ret_type.getEnum() )
    {
      case Type::BOOL : return ( l.getBool()  == r.getBool() );
      case Type::INT  : return ( l.getInt ()  == r.getInt() );
      case Type::FLOAT: return ( l.getFloat() == r.getFloat() );
      default:
        throw "Internal error: unhandled scalar";
    }
  }
  else if( left.isString() && right.isString() )
  {
    return ( left.getString() == right.getString() );
  }
  else if( left.getType() == right.getType() )
  {
    return ( left.getP() == right.getP() );
  }
  else
  {
    throw std::string( "Cannot compare " ) + left.getTypeName() + " and " + right.getTypeName() + " directly";
  }
}

bool Value::operator!=( const Value &right ) const
{
  const Value &left = (*this);
  if( left.isScalar() && right.isScalar() )
  {
    const Type ret_type = std::max( left.getType(), right.getType() );

    const Value l = left .to( ret_type );
    const Value r = right.to( ret_type );

    switch( ret_type.getEnum() )
    {
      case Type::BOOL : return ( l.getBool()  != r.getBool() );
      case Type::INT  : return ( l.getInt ()  != r.getInt() );
      case Type::FLOAT: return ( l.getFloat() != r.getFloat() );
      default:
        throw "Internal error: unhandled scalar";
    }
  }
  else if( left.isString() && right.isString() )
  {
    return ( left.getString() != right.getString() );
  }
  else if( left.getType() == right.getType() )
  {
    return ( left.getP() != right.getP() );
  }
  else
  {
    throw std::string( "Cannot compare " ) + left.getTypeName() + " and " + right.getTypeName() + " directly";
  }
}

bool Value::operator&&( const Value &right ) const
{
  const Value &left = (*this);
  if( left.isScalar() && right.isScalar() )
  {
    const Value l = left .to( Type::BOOL );
    const Value r = right.to( Type::BOOL );
    return ( l && r );
  }
  else if( !left.isScalar() )
  {
    throw "left must be scalar";
  }
  else if( !right.isScalar() )
  {
    throw "right must be scalar";
  }
  else
  {
    throw "left and right must be scalar";
  }
}

bool Value::operator||( const Value &right ) const
{
  const Value &left = (*this);
  if( left.isScalar() && right.isScalar() )
  {
    const Value l = left .to( Type::BOOL );
    const Value r = right.to( Type::BOOL );
    return ( l || r );
  }
  else if( !left.isScalar() )
  {
    throw "left must be scalar";
  }
  else if( !right.isScalar() )
  {
    throw "right must be scalar";
  }
  else
  {
    throw "left and right must be scalar";
  }
}

bool Value::log_xor   ( const Value &right ) const
{
  const Value &left = (*this);
  if( left.isScalar() && right.isScalar() )
  {
    const Value l = left .to( Type::BOOL );
    const Value r = right.to( Type::BOOL );
    return ( l.getBool() ^ r.getBool() );
  }
  else if( !left.isScalar() )
  {
    throw "left must be scalar";
  }
  else if( !right.isScalar() )
  {
    throw "right must be scalar";
  }
  else
  {
    throw "left and right must be scalar";
  }
}

Value Value::operator<<( const Value &right ) const
{
  const Value &left = (*this);
  if( !left.isInteger() ) throw "left must be integer";
  if( !right.isInteger() ) throw "right must be integer";

  const Value l = left .to( Type::INT );
  const Value r = right.to( Type::INT );

  return Value( l.getInt() << r.getInt() );
}

Value Value::operator>>( const Value &right ) const
{
  const Value &left = (*this);
  if( !left.isInteger() ) throw "left must be integer";
  if( !right.isInteger() ) throw "right must be integer";

  const Value l = left .to( Type::INT );
  const Value r = right.to( Type::INT );

  return Value( l.getInt() >> r.getInt() );
}

Value Value::operator& ( const Value &right ) const
{
  const Value &left = (*this);
  if( !left.isInteger() ) throw "left must be integer";
  if( !right.isInteger() ) throw "right must be integer";

  const Value l = left .to( Type::INT );
  const Value r = right.to( Type::INT );

  return Value( l.getInt() & r.getInt() );
}

Value Value::operator| ( const Value &right ) const
{
  const Value &left = (*this);
  if( !left.isInteger() ) throw "left must be integer";
  if( !right.isInteger() ) throw "right must be integer";

  const Value l = left .to( Type::INT );
  const Value r = right.to( Type::INT );

  return Value( l.getInt() | r.getInt() );
}

Value Value::operator^ ( const Value &right ) const
{
  const Value &left = (*this);
  if( !left.isInteger() ) throw "left must be integer";
  if( !right.isInteger() ) throw "right must be integer";

  const Value l = left .to( Type::INT );
  const Value r = right.to( Type::INT );

  return Value( l.getInt() ^ r.getInt() );
}

Value Value::operator* ( const Value &right ) const
{
  const Value &left = (*this);
  if( left.isScalar() && right.isScalar() )
  {
    const Type ret_type = std::max( left.getType(), right.getType() );

    const Value l = left .to( ret_type );
    const Value r = right.to( ret_type );

    switch( ret_type.getEnum() )
    {
      case Type::BOOL : return Value( l.getBool()  * r.getBool() );
      case Type::INT  : return Value( l.getInt ()  * r.getInt() );
      case Type::FLOAT: return Value( l.getFloat() * r.getFloat() );
      default:
        throw "Internal error: unhandled scalar";
    }
  }
  else
  {
    throw "left and right must be scalar.";
  }
}

Value Value::operator/ ( const Value &right ) const
{
  const Value &left = (*this);
  if( left.isScalar() && right.isScalar() )
  {
    const Type ret_type = std::max( left.getType(), right.getType() );

    const Value l = left .to( ret_type );
    const Value r = right.to( ret_type );

    switch( ret_type.getEnum() )
    {
      case Type::BOOL :
        if( r.getBool()  )
          return Value( l.getBool()  / r.getBool() );
        else
          throw "Division by zero";

      case Type::INT  :
        if( r.getInt()   )
          return Value( l.getInt ()  / r.getInt() );
        else
          throw "Division by zero";

      case Type::FLOAT:
        if( r.getFloat() )
          return Value( l.getFloat() / r.getFloat() );
        else
          throw "Division by zero";

      default:
        throw "Internal error: unhandled scalar";
    }
  }
  else
  {
    throw "left and right must be scalar.";
  }
}

Value Value::operator+ ( const Value &right ) const
{
  const Value &left = (*this);
  if( left.isScalar() && right.isScalar() )
  {
    const Type ret_type = std::max( Type::Int(), std::max( left.getType(), right.getType() ) );

    const Value l = left .to( ret_type );
    const Value r = right.to( ret_type );

    switch( ret_type.getEnum() )
    {
      case Type::BOOL : return Value( l.getBool()  + r.getBool() );
      case Type::INT  : return Value( l.getInt ()  + r.getInt() );
      case Type::FLOAT: return Value( l.getFloat() + r.getFloat() );
      default:
        throw "Internal error: unhandled scalar";
    }
  }
  else if( left.isString() && right.isString() )
  {
    const Value &l = left;
    const Value &r = right;
    return Value( l.getString() + r.getString() );
  }
  else if( left.isString() && right.isScalar() )
  {
    const Value &l = left;
    const Value  r = right.to( Type::String() );
    return Value( l.getString() + r.getString() );
  }
  else if( left.isScalar() && right.isString() )
  {
    const Value  l = left.to( Type::String() );
    const Value &r = right;
    return Value( l.getString() + r.getString() );
  }
  else
  {
    throw "left and right must be scalar or string.";
  }
}

Value Value::operator- ( const Value &right ) const
{
  const Value &left = (*this);
  if( left.isScalar() && right.isScalar() )
  {
    const Type ret_type = std::max( left.getType(), right.getType() );

    const Value l = left .to( ret_type );
    const Value r = right.to( ret_type );

    switch( ret_type.getEnum() )
    {
      case Type::BOOL : return Value( l.getBool()  - r.getBool() );
      case Type::INT  : return Value( l.getInt ()  - r.getInt() );
      case Type::FLOAT: return Value( l.getFloat() - r.getFloat() );
      default:
        throw "Internal error: unhandled scalar";
    }
  }
  else
  {
    throw "left and right must be scalar.";
  }
}

Value Value::operator% ( const Value &right ) const
{
  const Value &left = (*this);
  switch( left.getEnum() )
  {
    case Type::BOOL : return Value( false );

    case Type::INT  :
      switch( right.getEnum() )
      {
        case Type::BOOL : return Value( false );
        case Type::INT  : return Value( left.getInt() % right.getInt() );
        case Type::FLOAT:
          {
            const float l = float( left.getInt() );
            const float r = float( left.getFloat() );
            if( r )
              return Value( l - floorf( l / r ) * r );
            else
              throw "Modulo by zero";
          }
        default:
          throw "left and right must be scalar.";
      }

    case Type::FLOAT:
      {
        float l;
        float r;

        switch( right.getEnum() )
        {
          case Type::BOOL : return Value( false );

          case Type::INT  :
            l = float( left.getFloat() );
            r = float( left.getInt() );
            break;

          case Type::FLOAT:
            l = float( left.getFloat() );
            r = float( left.getFloat() );
            break;

          default:
            throw "left and right must be scalar.";
        }

        if( r )
          return Value( l - floorf( l / r ) * r );
        else
          throw "Modulo by zero";
      }

    default:
      throw "left and right must be scalar.";
  }
}

std::ostream &Value::print( std::ostream &o ) const
{
  switch( getType().getEnum() )
  {
    case Type::INVALID : o << "nil"; break;
    case Type::BOOL    : o << ( getBool() ? "true" : "false" ); break;
    case Type::INT     : o << getInt(); break;
    case Type::FLOAT   : o << getFloat(); break;
    case Type::STRING  : o << "\"" << getString() << "\""; break;

    case Type::ARRAY   :
      {
        const Array &a = *getArray();
        o << "[ ";
        for( index_t j( 0 ), end( a.size() ); j < end; ++j )
        {
          if( j > 0 ) o << ", ";
          o << a[ j ];
        }
        o << " ]";
      }
      break;

    case Type::MAP     :
      {
        const Map *m = getMap();
        o << "{ ";
        for( CountPtr< RPGML::Map::Iterator > j( m->getIterator() ); !j->done(); j->next() )
        {
          const Map::Iterator::Type v( j->get() );
          o << " " << v.second.getTypeName() << " [ " << v.first << " ] = " << v.second << ";";
        }
        o << " }";
      }
      break;

    case Type::FUNCTION: o << "Function( " << (void*)getFunction() << " )"; break;
    case Type::NODE    : o << "Node( " << getNode()->getGlobalName() << " )"; break;

    case Type::OUTPUT  :
      {
        const Output *const output = getOutput();
        o
          << "Output( " << output->getParent()->getGlobalName()
          << ", " << output->getIdentifier() << " )"
          ;
      }
      break;

    case Type::INPUT   :
      {
        const Input *const input = getInput();
        o
          << "Input( " << input->getParent()->getGlobalName()
          << ", " << input->getIdentifier() << " )"
          ;
      }
      break;

    case Type::PARAM   :
      {
        const Param *const param = getParam();
        o
          << "Param( " << param->getParent()->getGlobalName()
          << ", " << param->getIdentifier() << " )"
          ;
      }
      break;

    case Type::SEQUENCE: getSequence()->print( o ); break;
  }

  return o;
}


} // namespace RPGML

namespace std {

std::ostream &operator<<( std::ostream &o, const RPGML::Value &v )
{
  return v.print( o );
}

} // namespace std


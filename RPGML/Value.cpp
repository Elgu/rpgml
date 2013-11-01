#include "Value.h"

#include "String.h"
#include "Array.h"
#include "Map.h"
#include "Function.h"
#include "Node.h"
#include "Sequence.h"

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
Value::Value( String   const *_str  ) : m_type( Type::STRING   ) { str  = _str ; str ->ref(); }
Value::Value( Array          *_arr  ) : m_type( Type::ARRAY    ) { arr  = _arr ; arr ->ref(); }
Value::Value( Map            *_map  ) : m_type( Type::MAP      ) { map  = _map ; map ->ref(); }
Value::Value( Function       *_func ) : m_type( Type::FUNCTION ) { func = _func; func->ref(); }
Value::Value( Node           *_node ) : m_type( Type::NODE     ) { node = _node; node->ref(); }
Value::Value( Output         *_out  ) : m_type( Type::OUTPUT   ) { out  = _out ; out ->ref(); }
Value::Value( Input          *_in   ) : m_type( Type::INPUT    ) { in   = _in  ; in  ->ref(); }
Value::Value( Param          *_para ) : m_type( Type::PARAM    ) { para = _para; para->ref(); }
Value::Value( Sequence const *_seq  ) : m_type( Type::SEQUENCE ) { seq  = _seq ; seq ->ref(); }

Value &Value::set( bool            _b    ) { return (*this) = _b   ; }
Value &Value::set( int             _i    ) { return (*this) = _i   ; }
Value &Value::set( float           _f    ) { return (*this) = _f   ; }
Value &Value::set( String   const *_str  ) { return (*this) = _str ; }
Value &Value::set( Array          *_arr  ) { return (*this) = _arr ; }
Value &Value::set( Map            *_map  ) { return (*this) = _map ; }
Value &Value::set( Function       *_func ) { return (*this) = _func; }
Value &Value::set( Node           *_node ) { return (*this) = _node; }
Value &Value::set( Output         *_out  ) { return (*this) = _out ; }
Value &Value::set( Input          *_in   ) { return (*this) = _in  ; }
Value &Value::set( Param          *_para ) { return (*this) = _para; }
Value &Value::set( Sequence const *_seq  ) { return (*this) = _seq ; }

Value &Value::operator=( Value other )
{
  this->swap( other );
  return (*this);
}

template< class PrimitiveType >
int Value::primitive_compare( PrimitiveType x, PrimitiveType y )
{
  return ( x < y ? -1 : ( x == y ? 0 : 1 ) );
}

int Value::compare( const Value &other ) const
{
  const Type::Enum e1 =       getType().getEnum();
  const Type::Enum e2 = other.getType().getEnum();
  if( e1 < e2 ) return -1;
  if( e1 > e2 ) return  1;

  if( isString() )
  {
    return str->get().compare( other.str->get() );
  }
  else
  {
    return primitive_compare( p, other.p );
  }
}

/*
int Value::deep_compare( const Value &other ) const
{
  const Type::Enum e1 =       getType().getEnum();
  const Type::Enum e2 = other.getType().getEnum();
  if( e1 < e2 ) return -1;
  if( e1 > e2 ) return  1;

  switch( e1 )
  {
    case Type::INVALID : return 0;
    case Type::BOOL    : return primitive_compare( b, other.b );
    case Type::INT     : return primitive_compare( i, other.i );
    case Type::FLOAT   : return primitive_compare( f, other.f );
    case Type::STRING  : return str->get().compare( other.str->get() );
    case Type::ARRAY   : break;
    case Type::MAP     : break;
    case Type::FUNCTION: break;
    case Type::NODE    : break;
    case Type::OUTPUT  : break;
    case Type::INPUT   : break;
    case Type::PARAM   : break;
    case Type::SEQUENCE: break;
  }

  return 0;
}
*/

bool Value::operator<( const Value &other ) const
{
  return ( compare( other ) < 0 );
}

bool Value::operator==( const Value &other ) const
{
  return ( 0 == compare( other ) );
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

} // namespace std


#ifndef RPGML_Value_h
#define RPGML_Value_h

#include "Type.h"

#include <cassert>

namespace RPGML {

class String  ;
class Array   ;
class Map     ;
class Function;
class Node    ;
class Output  ;
class Input   ;
class Param   ;
class Sequence;
class Collectable;

class Value
{
public:
  Value( void );
  ~Value( void );

  Value( const Value &other );
  Value &operator=( Value other );

  Value to( Type type ) const;

  bool operator< ( const Value &right ) const;
  bool operator<=( const Value &right ) const;
  bool operator> ( const Value &right ) const;
  bool operator>=( const Value &right ) const;
  bool operator==( const Value &right ) const;
  bool operator!=( const Value &right ) const;
  bool operator&&( const Value &right ) const;
  bool operator||( const Value &right ) const;
  bool log_xor   ( const Value &right ) const;
  Value operator<<( const Value &right ) const;
  Value operator>>( const Value &right ) const;
  Value operator& ( const Value &right ) const;
  Value operator| ( const Value &right ) const;
  Value operator^ ( const Value &right ) const;
  Value operator* ( const Value &right ) const;
  Value operator/ ( const Value &right ) const;
  Value operator+ ( const Value &right ) const;
  Value operator- ( const Value &right ) const;
  Value operator% ( const Value &right ) const;

  void clear( void );
  void swap( Value &other );

  Value ( bool  _b );
  Value ( int   _i );
  Value ( float _f );
  Value ( String   const *_str  );
  Value ( Array          *_arr  );
  Value ( Map            *_map  );
  Value ( Function       *_func );
  Value ( Node           *_node );
  Value ( Output         *_out  );
  Value ( Input          *_in   );
  Value ( Param          *_para );
  Value ( Sequence const *_seq  );

  Value &set( bool  _b );
  Value &set( int   _i );
  Value &set( float _f );
  Value &set( String   const *_str  );
  Value &set( Array          *_arr  );
  Value &set( Map            *_map  );
  Value &set( Function       *_func );
  Value &set( Node           *_node );
  Value &set( Output         *_out  );
  Value &set( Input          *_in   );
  Value &set( Param          *_para );
  Value &set( Sequence const *_seq  );

  Type        getType    ( void ) const { return m_type; }
  Type::Enum  getEnum    ( void ) const { return m_type.getEnum(); }
  const char *getTypeName( void ) const { return m_type.getTypeName(); }

  bool is( Type t       ) const { return ( m_type == t ); }
  bool is( Type::Enum e ) const { return ( m_type == e ); }
  bool isInvalid ( void ) const { return is( Type::INVALID  ); }
  bool isBool    ( void ) const { return is( Type::BOOL     ); }
  bool isInt     ( void ) const { return is( Type::INT      ); }
  bool isFloat   ( void ) const { return is( Type::FLOAT    ); }
  bool isString  ( void ) const { return is( Type::STRING   ); }
  bool isArray   ( void ) const { return is( Type::ARRAY    ); }
  bool isMap     ( void ) const { return is( Type::MAP      ); }
  bool isFunction( void ) const { return is( Type::FUNCTION ); }
  bool isNode    ( void ) const { return is( Type::NODE     ); }
  bool isOutput  ( void ) const { return is( Type::OUTPUT   ); }
  bool isInput   ( void ) const { return is( Type::INPUT    ); }
  bool isParam   ( void ) const { return is( Type::PARAM    ); }
  bool isSequence( void ) const { return is( Type::SEQUENCE ); }

  bool isScalar( void ) const { return getType().isScalar(); }
  bool isInteger( void ) const { return getType().isInteger(); }

  bool      getBool    ( void ) const { assert( isBool    () ); return b   ; }
  int       getInt     ( void ) const { assert( isInt     () ); return i   ; }
  float     getFloat   ( void ) const { assert( isFloat   () ); return f   ; }
  String   const *getString  ( void ) const { assert( isString  () ); return str ; }
  Array          *getArray   ( void ) const { assert( isArray   () ); return arr ; }
  Map            *getMap     ( void ) const { assert( isMap     () ); return map ; }
  Function       *getFunction( void ) const { assert( isFunction() ); return func; }
  Node           *getNode    ( void ) const { assert( isNode    () ); return node; }
  Output         *getOutput  ( void ) const { assert( isOutput  () ); return out ; }
  Input          *getInput   ( void ) const { assert( isInput   () ); return in  ; }
  Param          *getParam   ( void ) const { assert( isParam   () ); return para; }
  Sequence const *getSequence( void ) const { assert( isSequence() ); return seq ; }

  const void *getP( void ) const { return p; }

  bool isCollectable( void ) const;
  const Collectable *getCollectable( void ) const;
private:
  template< class PrimitiveType >
  static int primitive_compare( PrimitiveType x, PrimitiveType y );

  union
  {
    void     *p   ;
    bool      b   ;
    int       i   ;
    float     f   ;
    String   const *str ;
    Array          *arr ;
    Map            *map ;
    Function       *func;
    Node           *node;
    Output         *out ;
    Input          *in  ;
    Param          *para;
    Sequence const *seq ;
  };
  Type m_type;
};


} // namespace RPGML

namespace std {

inline
void swap( RPGML::Value &x1, RPGML::Value &x2 )
{
  x1.swap( x2 );
}

} // namespace std

#endif


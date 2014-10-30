#ifndef RPGML_Value_h
#define RPGML_Value_h

#include "Type.h"
#include "String.h"

#include <cassert>

namespace RPGML {

class String  ;
class StringData;
class ArrayBase   ;
class Frame     ;
class Function;
class Node    ;
class Output  ;
class Input   ;
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

  std::ostream &print( std::ostream &o ) const;

  explicit Value ( bool  _b );
  explicit Value ( int   _i );
  explicit Value ( index_t _i );
  explicit Value ( float _f );
  explicit Value ( char const *_str  );
  explicit Value ( std::string const &_str  );
  explicit Value ( String   const &_str  );
  explicit Value ( StringData const *_str );
  explicit Value ( ArrayBase      *_arr  );
  explicit Value ( Frame          *_frame );
  explicit Value ( Function       *_func );
  explicit Value ( Node           *_node );
  explicit Value ( Output         *_out  );
  explicit Value ( Input          *_in   );
  explicit Value ( Sequence const *_seq  );

//  template< class Invalid >
//  explicit Value( const Invalid & )
//  {}

  Value &set( bool  _b );
  Value &set( int   _i );
  Value &set( index_t _i );
  Value &set( float _f );
  Value &set( std::string const &_str  );
  Value &set( char        const *_str  );
  Value &set( String      const &_str  );
  Value &set( StringData  const *_str );
  Value &set( ArrayBase      *_arr  );
  Value &set( Frame          *_frame );
  Value &set( Function       *_func );
  Value &set( Node           *_node );
  Value &set( Output         *_out  );
  Value &set( Input          *_in   );
  Value &set( Sequence const *_seq  );
  Value &set( const Value &v );

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
  bool isFrame   ( void ) const { return is( Type::FRAME    ); }
  bool isFunction( void ) const { return is( Type::FUNCTION ); }
  bool isNode    ( void ) const { return is( Type::NODE     ); }
  bool isOutput  ( void ) const { return is( Type::OUTPUT   ); }
  bool isInput   ( void ) const { return is( Type::INPUT    ); }
  bool isSequence( void ) const { return is( Type::SEQUENCE ); }

  bool isScalar( void ) const { return getType().isScalar(); }
  bool isInteger( void ) const { return getType().isInteger(); }
  bool isPrimitive( void ) const { return getType().isPrimitive(); }

  bool            getBool    ( void ) const { assert( isBool    () ); return b   ; }
  int             getInt     ( void ) const { assert( isInt     () ); return i   ; }
  float           getFloat   ( void ) const { assert( isFloat   () ); return f   ; }
  String          getString  ( void ) const { assert( isString  () ); return String( str ); }
  ArrayBase      *getArray   ( void ) const { assert( isArray   () ); return arr ; }
  Frame          *getFrame   ( void ) const { assert( isFrame   () ); return frame; }
  Function       *getFunction( void ) const { assert( isFunction() ); return func; }
  Node           *getNode    ( void ) const { assert( isNode    () ); return node; }
  Output         *getOutput  ( void ) const { assert( isOutput  () ); return out ; }
  Input          *getInput   ( void ) const { assert( isInput   () ); return in  ; }
  Sequence const *getSequence( void ) const { assert( isSequence() ); return seq ; }

  operator bool            ( void ) const { return getBool    (); }
  operator int             ( void ) const { return getInt     (); }
  operator float           ( void ) const { return getFloat   (); }
  operator String          ( void ) const { return getString  (); }
  operator ArrayBase      *( void ) const { return getArray   (); }
  operator Frame          *( void ) const { return getFrame   (); }
  operator Function       *( void ) const { return getFunction(); }
  operator Node           *( void ) const { return getNode    (); }
  operator Output         *( void ) const { return getOutput  (); }
  operator Input          *( void ) const { return getInput   (); }
  operator Sequence const *( void ) const { return getSequence(); }

  operator CountPtr< ArrayBase      >( void ) const;
  operator CountPtr< Frame          >( void ) const;
  operator CountPtr< Function       >( void ) const;
  operator CountPtr< Node           >( void ) const;
  operator CountPtr< Output         >( void ) const;
  operator CountPtr< Input          >( void ) const;
  operator CountPtr< Sequence const >( void ) const;

  bool get( bool            &x ) const;
  bool get( int             &x ) const;
  bool get( float           &x ) const;
  bool get( String          &x ) const;
  bool get( ArrayBase      *&x ) const;
  bool get( Frame          *&x ) const;
  bool get( Function       *&x ) const;
  bool get( Node           *&x ) const;
  bool get( Output         *&x ) const;
  bool get( Input          *&x ) const;
  bool get( Sequence const *&x ) const;
  bool get( CountPtr< ArrayBase > &x ) const;
  bool get( CountPtr< Frame     > &x ) const;
  bool get( CountPtr< Function  > &x ) const;
  bool get( CountPtr< Node      > &x ) const;
  bool get( CountPtr< Output    > &x ) const;
  bool get( CountPtr< Input     > &x ) const;
  bool get( CountPtr< const Sequence > &x ) const;
  bool get( Value &x ) const { x = (*this); return true; }
  template< class T > bool get( T & ) const { return false; }

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
    StringData const *str ;
    ArrayBase      *arr ;
    Frame          *frame;
    Function       *func;
    Node           *node;
    Output         *out ;
    Input          *in  ;
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

  std::ostream &operator<<( std::ostream &o, const RPGML::Value &s );

} // namespace std

#endif


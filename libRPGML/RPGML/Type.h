#ifndef RPGML_Type_h
#define RPGML_Type_h

#include "types.h"

#include <ostream>

namespace RPGML {

class String;
class Frame;
class Function;
class Node;
class Output;
class Input;
class Param;
class Sequence;
class ArrayBase;
class Collectable;
template< class RefcountedType > class CountPtr;

class Type
{
public:
  enum Enum
  {
      INVALID  // no type specified
    , BOOL     // bool
    , UINT8    // uint8_t
    , INT8     // int8_t
    , UINT16   // uint16_t
    , INT16    // int16_t
    , UINT32   // uint32_t
    , INT32    // int32_t
    , UINT64   // uint64_t
    , INT64    // int64_t
    , FLOAT    // float
    , DOUBLE   // double
    , STRING   // String
    , FRAME    // Frame
    , FUNCTION // Function
    , NODE     // Node
    , OUTPUT   // Output
    , INPUT    // Input
    , PARAM    // Param
    , SEQUENCE // Sequence
    , ARRAY    // ArrayBase
    , OTHER    // anything else

    , INT = INT32
  };

  Type( Enum e=INVALID ) : m_e( e ) {}
  Type &operator=( Enum e ) { m_e = e; return (*this); }

  const char *getTypeName( void ) const
  {
    static
    const char *const names[] =
    {
        "invalid"
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
      , "Param"
      , "Sequence"
      , "Array"
      , "other"
    };

    return names[ m_e ];
  }

  bool operator==( Enum e ) const { return m_e == e; }
  bool operator!=( Enum e ) const { return m_e != e; }
  bool operator< ( Enum e ) const { return m_e <  e; }
  bool operator> ( Enum e ) const { return m_e >  e; }
  bool operator<=( Enum e ) const { return m_e <= e; }
  bool operator>=( Enum e ) const { return m_e >= e; }

  Enum getEnum( void ) const { return m_e; }
  operator Enum( void ) const { return getEnum(); }

  bool isScalar( void ) const { return m_e >= BOOL && m_e <= DOUBLE; }
  bool isInteger( void ) const { return m_e >= BOOL && m_e <= INT64; }
  bool isPrimitive( void ) const { return m_e >= BOOL && m_e <= STRING; }

  static Type Invalid  ( void ) { return Type( Type::INVALID  ); }
  static Type Bool     ( void ) { return Type( Type::BOOL     ); }
  static Type UInt8    ( void ) { return Type( Type::UINT8    ); }
  static Type Int8     ( void ) { return Type( Type::INT8     ); }
  static Type UInt16   ( void ) { return Type( Type::UINT16   ); }
  static Type Int16    ( void ) { return Type( Type::INT16    ); }
  static Type UInt32   ( void ) { return Type( Type::UINT32   ); }
  static Type Int32    ( void ) { return Type( Type::INT32    ); }
  static Type UInt64   ( void ) { return Type( Type::UINT64   ); }
  static Type Int64    ( void ) { return Type( Type::INT64    ); }
  static Type Float    ( void ) { return Type( Type::FLOAT    ); }
  static Type Double   ( void ) { return Type( Type::DOUBLE   ); }
  static Type String   ( void ) { return Type( Type::STRING   ); }
  static Type Frame    ( void ) { return Type( Type::FRAME    ); }
  static Type Function ( void ) { return Type( Type::FUNCTION ); }
  static Type Node     ( void ) { return Type( Type::NODE     ); }
  static Type Output   ( void ) { return Type( Type::OUTPUT   ); }
  static Type Input    ( void ) { return Type( Type::INPUT    ); }
  static Type Param    ( void ) { return Type( Type::PARAM    ); }
  static Type Sequence ( void ) { return Type( Type::SEQUENCE ); }
  static Type Array    ( void ) { return Type( Type::ARRAY    ); }
  static Type Other    ( void ) { return Type( Type::OTHER    ); }

  static Type Int     ( void ) { return Type::Int32(); }

  explicit Type( const bool               & ) : m_e( Type::BOOL     ) {}
  explicit Type( const uint8_t            & ) : m_e( Type::UINT8    ) {}
  explicit Type( const int8_t             & ) : m_e( Type::INT8     ) {}
  explicit Type( const uint16_t           & ) : m_e( Type::UINT16   ) {}
  explicit Type( const int16_t            & ) : m_e( Type::INT16    ) {}
  explicit Type( const uint32_t           & ) : m_e( Type::UINT32   ) {}
  explicit Type( const int32_t            & ) : m_e( Type::INT32    ) {}
  explicit Type( const uint64_t           & ) : m_e( Type::UINT64   ) {}
  explicit Type( const int64_t            & ) : m_e( Type::INT64    ) {}
  explicit Type( const float              & ) : m_e( Type::FLOAT    ) {}
  explicit Type( const double             & ) : m_e( Type::DOUBLE   ) {}
  explicit Type( const RPGML::String      & ) : m_e( Type::STRING   ) {}
  explicit Type( const RPGML::Frame       & ) : m_e( Type::FRAME    ) {}
  explicit Type( const RPGML::Function    & ) : m_e( Type::FUNCTION ) {}
  explicit Type( const RPGML::Node        & ) : m_e( Type::NODE     ) {}
  explicit Type( const RPGML::Output      & ) : m_e( Type::OUTPUT   ) {}
  explicit Type( const RPGML::Input       & ) : m_e( Type::INPUT    ) {}
  explicit Type( const RPGML::Param       & ) : m_e( Type::PARAM    ) {}
  explicit Type( const RPGML::Sequence    & ) : m_e( Type::SEQUENCE ) {}
  explicit Type( const RPGML::ArrayBase   & ) : m_e( Type::ARRAY    ) {}

  bool isInvalid ( void ) const { return ( m_e == Type::INVALID  ); }
  bool isBool    ( void ) const { return ( m_e == Type::BOOL     ); }
  bool isUInt8   ( void ) const { return ( m_e == Type::UINT8    ); }
  bool isInt8    ( void ) const { return ( m_e == Type::INT8     ); }
  bool isUInt16  ( void ) const { return ( m_e == Type::UINT16   ); }
  bool isInt16   ( void ) const { return ( m_e == Type::INT16    ); }
  bool isUInt32  ( void ) const { return ( m_e == Type::UINT32   ); }
  bool isInt32   ( void ) const { return ( m_e == Type::INT32    ); }
  bool isUInt64  ( void ) const { return ( m_e == Type::UINT64   ); }
  bool isInt64   ( void ) const { return ( m_e == Type::INT64    ); }
  bool isFloat   ( void ) const { return ( m_e == Type::FLOAT    ); }
  bool isDouble  ( void ) const { return ( m_e == Type::DOUBLE   ); }
  bool isString  ( void ) const { return ( m_e == Type::STRING   ); }
  bool isFrame   ( void ) const { return ( m_e == Type::FRAME    ); }
  bool isFunction( void ) const { return ( m_e == Type::FUNCTION ); }
  bool isNode    ( void ) const { return ( m_e == Type::NODE     ); }
  bool isOutput  ( void ) const { return ( m_e == Type::OUTPUT   ); }
  bool isInput   ( void ) const { return ( m_e == Type::INPUT    ); }
  bool isParam   ( void ) const { return ( m_e == Type::PARAM    ); }
  bool isSequence( void ) const { return ( m_e == Type::SEQUENCE ); }
  bool isArray   ( void ) const { return ( m_e == Type::ARRAY    ); }
  bool isOther   ( void ) const { return ( m_e == Type::OTHER    ); }

  bool isInt      ( void ) const { return isInt32(); }
private:
  Enum m_e;
};

template< typename T > struct TypeOf  { static const Type::Enum E = Type::OTHER; };

template<> struct TypeOf< bool      > { static const Type::Enum E = Type::BOOL    ; };
template<> struct TypeOf< uint8_t   > { static const Type::Enum E = Type::UINT8   ; };
template<> struct TypeOf< int8_t    > { static const Type::Enum E = Type::INT8    ; };
template<> struct TypeOf< uint16_t  > { static const Type::Enum E = Type::UINT16  ; };
template<> struct TypeOf< int16_t   > { static const Type::Enum E = Type::INT16   ; };
template<> struct TypeOf< uint32_t  > { static const Type::Enum E = Type::UINT32  ; };
template<> struct TypeOf< int32_t   > { static const Type::Enum E = Type::INT32   ; };
template<> struct TypeOf< uint64_t  > { static const Type::Enum E = Type::UINT64  ; };
template<> struct TypeOf< int64_t   > { static const Type::Enum E = Type::INT64   ; };
template<> struct TypeOf< float     > { static const Type::Enum E = Type::FLOAT   ; };
template<> struct TypeOf< double    > { static const Type::Enum E = Type::DOUBLE  ; };
template<> struct TypeOf< String    > { static const Type::Enum E = Type::STRING  ; };
template<> struct TypeOf< CountPtr< Frame     > > { static const Type::Enum E = Type::FRAME   ; };
template<> struct TypeOf< CountPtr< Function  > > { static const Type::Enum E = Type::FUNCTION; };
template<> struct TypeOf< CountPtr< Node      > > { static const Type::Enum E = Type::NODE    ; };
template<> struct TypeOf< CountPtr< Output    > > { static const Type::Enum E = Type::OUTPUT  ; };
template<> struct TypeOf< CountPtr< Input     > > { static const Type::Enum E = Type::INPUT   ; };
template<> struct TypeOf< CountPtr< Param     > > { static const Type::Enum E = Type::PARAM   ; };
template<> struct TypeOf< CountPtr< Sequence  > > { static const Type::Enum E = Type::SEQUENCE; };
template<> struct TypeOf< CountPtr< ArrayBase > > { static const Type::Enum E = Type::ARRAY   ; };

template< class T >
static inline
Type typeOf( const T & ) { return Type( TypeOf< T >::E  ); }

template< class T >
static inline
const char *getTypeName( void ) { return Type( TypeOf< T >::E  ).getTypeName(); }

} // namespace RPGML

namespace std {

  static inline
  std::ostream &operator<<( std::ostream &o, const RPGML::Type &t )
  {
    return o << t.getTypeName();
  }

} // namespace std
#endif


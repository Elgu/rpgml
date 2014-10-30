#ifndef RPGML_Type_h
#define RPGML_Type_h

namespace RPGML {

class String;
class Frame;
class Function;
class Node;
class Output;
class Input;
class Sequence;
class ArrayBase;
class Collectable;
template< class RefcountedType > class CountPtr;

struct Type
{
  enum Enum
  {
      INVALID
    , BOOL
    , INT
    , FLOAT
    , STRING
    , FRAME
    , FUNCTION
    , NODE
    , OUTPUT
    , INPUT
    , SEQUENCE
    , ARRAY
  };

  const char *getTypeName( void ) const
  {
    static
    const char *const names[] =
    {
        "invalid"
      , "bool"
      , "int"
      , "float"
      , "string"
      , "Frame"
      , "Function"
      , "Node"
      , "Output"
      , "Input"
      , "Sequence"
      , "Array"
    };

    return names[ m_e ];
  }

  Type( Enum e=INVALID ) : m_e( e ) {}
  Type &operator=( Enum e ) { m_e = e; return (*this); }

  bool operator==( Enum e ) { return m_e == e; }
  bool operator!=( Enum e ) { return m_e != e; }
  bool operator< ( Enum e ) { return m_e <  e; }
  bool operator> ( Enum e ) { return m_e >  e; }
  bool operator<=( Enum e ) { return m_e <= e; }
  bool operator>=( Enum e ) { return m_e >= e; }

  Enum getEnum( void ) const { return m_e; }
  operator Enum( void ) const { return getEnum(); }

  bool isScalar( void ) const { return m_e >= BOOL && m_e <= FLOAT; }
  bool isInteger( void ) const { return m_e >= BOOL && m_e <= INT; }
  bool isPrimitive( void ) const { return m_e <= STRING; }

  static Type Invalid  ( void ) { return Type( Type::INVALID  ); }
  static Type Bool     ( void ) { return Type( Type::BOOL     ); }
  static Type Int      ( void ) { return Type( Type::INT      ); }
  static Type Float    ( void ) { return Type( Type::FLOAT    ); }
  static Type String   ( void ) { return Type( Type::STRING   ); }
  static Type Frame    ( void ) { return Type( Type::FRAME    ); }
  static Type Function ( void ) { return Type( Type::FUNCTION ); }
  static Type Node     ( void ) { return Type( Type::NODE     ); }
  static Type Output   ( void ) { return Type( Type::OUTPUT   ); }
  static Type Input    ( void ) { return Type( Type::INPUT    ); }
  static Type Sequence ( void ) { return Type( Type::SEQUENCE ); }
  static Type Array    ( void ) { return Type( Type::ARRAY    ); }

  explicit Type( const bool               & ) : m_e( Type::BOOL     ) {}
  explicit Type( const int                & ) : m_e( Type::INT      ) {}
  explicit Type( const float              & ) : m_e( Type::FLOAT    ) {}
  explicit Type( const RPGML::String      & ) : m_e( Type::STRING   ) {}
  explicit Type( const RPGML::String      * ) : m_e( Type::STRING   ) {}
  explicit Type( const char               * ) : m_e( Type::STRING   ) {}
  explicit Type( const RPGML::Frame       & ) : m_e( Type::FRAME    ) {}
  explicit Type( const RPGML::Frame       * ) : m_e( Type::FRAME    ) {}
  explicit Type( const RPGML::Function    & ) : m_e( Type::FUNCTION ) {}
  explicit Type( const RPGML::Function    * ) : m_e( Type::FUNCTION ) {}
  explicit Type( const RPGML::Node        & ) : m_e( Type::NODE     ) {}
  explicit Type( const RPGML::Node        * ) : m_e( Type::NODE     ) {}
  explicit Type( const RPGML::Output      & ) : m_e( Type::OUTPUT   ) {}
  explicit Type( const RPGML::Output      * ) : m_e( Type::OUTPUT   ) {}
  explicit Type( const RPGML::Input       & ) : m_e( Type::INPUT    ) {}
  explicit Type( const RPGML::Input       * ) : m_e( Type::INPUT    ) {}
  explicit Type( const RPGML::Sequence    & ) : m_e( Type::SEQUENCE ) {}
  explicit Type( const RPGML::Sequence    * ) : m_e( Type::SEQUENCE ) {}
  explicit Type( const RPGML::ArrayBase   & ) : m_e( Type::ARRAY    ) {}
  explicit Type( const RPGML::ArrayBase   * ) : m_e( Type::ARRAY    ) {}

  bool isInvalid  ( void ) const { return ( m_e == Type::INVALID  ); }
  bool isBool     ( void ) const { return ( m_e == Type::BOOL     ); }
  bool isInt      ( void ) const { return ( m_e == Type::INT      ); }
  bool isFloat    ( void ) const { return ( m_e == Type::FLOAT    ); }
  bool isString   ( void ) const { return ( m_e == Type::STRING   ); }
  bool isFrame    ( void ) const { return ( m_e == Type::FRAME    ); }
  bool isFunction ( void ) const { return ( m_e == Type::FUNCTION ); }
  bool isNode     ( void ) const { return ( m_e == Type::NODE     ); }
  bool isOutput   ( void ) const { return ( m_e == Type::OUTPUT   ); }
  bool isInput    ( void ) const { return ( m_e == Type::INPUT    ); }
  bool isSequence ( void ) const { return ( m_e == Type::SEQUENCE ); }
  bool isArray    ( void ) const { return ( m_e == Type::ARRAY    ); }
private:
  Enum m_e;
};

template< class T > static inline Type TypeOf( const T & ) { return Type( Type::INVALID  ); }
template< class T > static inline Type TypeOf( const T * ) { return Type( Type::INVALID  ); }
static inline Type TypeOf( const bool               & ) { return Type( Type::BOOL     ); }
static inline Type TypeOf( const bool               * ) { return Type( Type::BOOL     ); }
static inline Type TypeOf( const int                & ) { return Type( Type::INT      ); }
static inline Type TypeOf( const int                * ) { return Type( Type::INT      ); }
static inline Type TypeOf( const float              & ) { return Type( Type::FLOAT    ); }
static inline Type TypeOf( const float              * ) { return Type( Type::FLOAT    ); }
static inline Type TypeOf( const RPGML::String      & ) { return Type( Type::STRING   ); }
static inline Type TypeOf( const RPGML::String      * ) { return Type( Type::STRING   ); }
static inline Type TypeOf( const char               * ) { return Type( Type::STRING   ); }
static inline Type TypeOf( const RPGML::Frame       & ) { return Type( Type::FRAME    ); }
static inline Type TypeOf( const RPGML::Frame       * ) { return Type( Type::FRAME    ); }
static inline Type TypeOf( const RPGML::Function    & ) { return Type( Type::FUNCTION ); }
static inline Type TypeOf( const RPGML::Function    * ) { return Type( Type::FUNCTION ); }
static inline Type TypeOf( const RPGML::Node        & ) { return Type( Type::NODE     ); }
static inline Type TypeOf( const RPGML::Node        * ) { return Type( Type::NODE     ); }
static inline Type TypeOf( const RPGML::Output      & ) { return Type( Type::OUTPUT   ); }
static inline Type TypeOf( const RPGML::Output      * ) { return Type( Type::OUTPUT   ); }
static inline Type TypeOf( const RPGML::Input       & ) { return Type( Type::INPUT    ); }
static inline Type TypeOf( const RPGML::Input       * ) { return Type( Type::INPUT    ); }
static inline Type TypeOf( const RPGML::Sequence    & ) { return Type( Type::SEQUENCE ); }
static inline Type TypeOf( const RPGML::Sequence    * ) { return Type( Type::SEQUENCE ); }
static inline Type TypeOf( const RPGML::ArrayBase   & ) { return Type( Type::ARRAY    ); }
static inline Type TypeOf( const RPGML::ArrayBase   * ) { return Type( Type::ARRAY    ); }
template< class RefcountedType> static inline Type TypeOf( const CountPtr< RefcountedType       > & ) { return TypeOf( (RefcountedType*)0 ); }
template< class RefcountedType> static inline Type TypeOf( const CountPtr< RefcountedType const > & ) { return TypeOf( (const RefcountedType*)0 ); }
template< class RefcountedType> static inline Type TypeOf( const CountPtr< RefcountedType       > * ) { return TypeOf( (RefcountedType*)0 ); }
template< class RefcountedType> static inline Type TypeOf( const CountPtr< RefcountedType const > * ) { return TypeOf( (const RefcountedType*)0 ); }

} // namespace RPGML

#endif


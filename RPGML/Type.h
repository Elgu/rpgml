#ifndef RPGML_Type_h
#define RPGML_Type_h

namespace RPGML {

struct Type
{
  enum Enum
  {
      INVALID
    , BOOL
    , INT
    , FLOAT
    , STRING
    , ARRAY
    , MAP
    , FUNCTION
    , NODE
    , OUTPUT
    , INPUT
    , PARAM
    , SEQUENCE
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
      , "Array"
      , "Map"
      , "Function"
      , "Node"
      , "Output"
      , "Input"
      , "Param"
      , "Sequence"
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

  bool isScalar( void ) { return m_e >= BOOL && m_e <= FLOAT; }
  bool isInteger( void ) { return m_e >= BOOL && m_e <= INT; }

  static Type Invalid  ( void ) { return Type( Type::INVALID  ); }
  static Type Bool     ( void ) { return Type( Type::BOOL     ); }
  static Type Int      ( void ) { return Type( Type::INT      ); }
  static Type Float    ( void ) { return Type( Type::FLOAT    ); }
  static Type String   ( void ) { return Type( Type::STRING   ); }
  static Type Array    ( void ) { return Type( Type::ARRAY    ); }
  static Type Map      ( void ) { return Type( Type::MAP      ); }
  static Type Function ( void ) { return Type( Type::FUNCTION ); }
  static Type Node     ( void ) { return Type( Type::NODE     ); }
  static Type Output   ( void ) { return Type( Type::OUTPUT   ); }
  static Type Input    ( void ) { return Type( Type::INPUT    ); }
  static Type Param    ( void ) { return Type( Type::PARAM    ); }
  static Type Sequecnce( void ) { return Type( Type::SEQUENCE ); }

private:
  Enum m_e;
};

} // namespace RPGML

#endif


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

private:
  Enum m_e;
};

} // namespace RPGML

#endif


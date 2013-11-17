#ifndef RPGML_ParserEnums_h
#define RPGML_ParserEnums_h

namespace RPGML {

enum BOP
{
    BOP_LEFT
  , BOP_RIGHT
  , BOP_LT
  , BOP_LE
  , BOP_GT
  , BOP_GE
  , BOP_EQ
  , BOP_NE
  , BOP_LOG_AND
  , BOP_LOG_OR
  , BOP_LOG_XOR
  , BOP_BIT_AND
  , BOP_BIT_OR
  , BOP_BIT_XOR
  , BOP_MUL
  , BOP_DIV
  , BOP_ADD
  , BOP_SUB
  , BOP_MOD
};

static inline
const char *getBOP( BOP bop )
{
  static
  const char *const op[] =
  {
      "<<", ">>"
    , "<", "<="
    , ">", ">="
    , "==" , "!="
    , "&&", "||", "^^"
    , "&", "|", "^"
    , "*", "/", "+", "-"
    , "%"
  };
  return op[ bop ];
}

enum UOP
{
    UOP_MINUS
  , UOP_PLUS
  , UOP_LOG_NOT
  , UOP_BIT_NOT
};

static inline
const char *getUOP( UOP uop )
{
  static
  const char *const op[] =
  { "-", "+", "!", "~" };
  return op[ uop ];
}

enum ASSIGN
{
    ID_ASSIGN
  , MUL_ASSIGN
  , DIV_ASSIGN
  , MOD_ASSIGN
  , ADD_ASSIGN
  , SUB_ASSIGN
  , LEFT_ASSIGN
  , RIGHT_ASSIGN
  , AND_ASSIGN
  , XOR_ASSIGN
  , OR_ASSIGN
};

static inline
const char *getAssign( ASSIGN assign )
{
  static
  const char *const op[] =
  {
      "=", "*=", "/="
    , "%=", "+=", "-="
    , "<<=", ">>="
    , "&=", "^=", "|="
  };
  return op[ assign ];
}

static inline
BOP getAssignBOP( ASSIGN assign )
{
  assert( assign >= MUL_ASSIGN );
  assert( assign <= OR_ASSIGN );

  static
  const BOP bop[] =
  {
      BOP_MUL
    , BOP_DIV
    , BOP_MOD
    , BOP_ADD
    , BOP_SUB
    , BOP_LEFT
    , BOP_RIGHT
    , BOP_BIT_AND
    , BOP_BIT_XOR
    , BOP_BIT_OR
  };

  return bop[ assign-1 ];
}

} // namespace RPGML

#endif


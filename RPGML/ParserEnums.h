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
const char *getBOPStr( BOP bop )
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

static inline
BOP getBOP( const char *const bop )
{
  switch( bop[ 0 ] )
  {
    case '<':
      switch( bop[ 1 ] )
      {
        case '\0': return BOP_LT;
        case  '<': return BOP_LEFT;
        case  '=': return BOP_LE;
        default: goto fail;
      }
      break;
    case '>':
      switch( bop[ 1 ] )
      {
        case '\0': return BOP_GT;
        case  '>': return BOP_RIGHT;
        case  '=': return BOP_GE;
        default: goto fail;
      }
      break;
    case '=':
      switch( bop[ 1 ] )
      {
        case  '=': return BOP_EQ;
        default: goto fail;
      }
      break;
    case '!':
      switch( bop[ 1 ] )
      {
        case  '=': return BOP_NE;
        default: goto fail;
      }
      break;
    case '&':
      switch( bop[ 1 ] )
      {
        case '\0': return BOP_BIT_AND;
        case  '&': return BOP_LOG_AND;
        default: goto fail;
      }
      break;
    case '|':
      switch( bop[ 1 ] )
      {
        case '\0': return BOP_BIT_OR;
        case  '|': return BOP_LOG_OR;
        default: goto fail;
      }
      break;
    case '^':
      switch( bop[ 1 ] )
      {
        case '\0': return BOP_BIT_XOR;
        case  '^': return BOP_LOG_XOR;
        default: goto fail;
      }
      break;
    case '*':
      switch( bop[ 1 ] )
      {
        case '\0': return BOP_MUL;
        default: goto fail;
      }
      break;
    case '/':
      switch( bop[ 1 ] )
      {
        case '\0': return BOP_DIV;
        default: goto fail;
      }
      break;
    case '+':
      switch( bop[ 1 ] )
      {
        case '\0': return BOP_ADD;
        default: goto fail;
      }
      break;
    case '-':
      switch( bop[ 1 ] )
      {
        case '\0': return BOP_SUB;
        default: goto fail;
      }
      break;
    case '%':
      switch( bop[ 1 ] )
      {
        case '\0': return BOP_MOD;
        default: goto fail;
      }
      break;
    default:
      goto fail;
  }

fail:
  throw "Invalid bop";
}

enum UOP
{
    UOP_MINUS
  , UOP_PLUS
  , UOP_LOG_NOT
  , UOP_BIT_NOT
};

static inline
const char *getUOPStr( UOP uop )
{
  static
  const char *const op[] =
  { "-", "+", "!", "~" };
  return op[ uop ];
}

static inline
UOP getUOP( const char *uop )
{
  if( !uop[ 0 ] || uop[ 1 ] ) goto fail;

  switch( uop[ 0 ] )
  {
    case '-': return UOP_MINUS;
    case '+': return UOP_PLUS;
    case '!': return UOP_LOG_NOT;
    case '~': return UOP_BIT_NOT;
    default: goto fail;
  }

fail:
  throw "Invalid uop";
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


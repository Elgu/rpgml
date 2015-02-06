%skeleton "lalr1.cc" /* C++ */
%require "2.5"
%defines
%define parser_class_name "_Parser"
%define api.namespace RPGML
%glr-parser
%parse-param { RPGML::Scanner &scanner }
%lex-param { RPGML::Scanner &scanner }
%error-verbose

%code requires
{
#include "AST.h"
#include "String.h"
#include "Value.h"
#include "Array.h"
#include "Function.h"
#include "ParserEnums.h"
#include "toLocation.h"
namespace RPGML { class Scanner; }
using namespace RPGML::AST;
}

%initial-action
{
// Initialize the initial location.
@$.begin.filename = @$.end.filename = scanner.filename;
};

// The code between %code { and } is output in the *.cc file; it needs detailed knowledge about the scanner
%code
{
#include "Scanner.h"
namespace RPGML
{
  int yylex( _Parser::semantic_type *yylval, _Parser::location_type *yylloc, Scanner &scanner )
  {
    return scanner.yylex( yylval, yylloc );
  }
}
#define RPGML_LOC(L) toLocation( scanner.unified_filename, (L) )
}

%union
{
  int64_t      ival;
  uint64_t     uval;
  double       fval;
  Type::Enum   type_enum;
  UOP          uop;
  ASSIGN       assign;
  StringData const                         *str;
  Statement                                *stmt;
  CompoundStatement                        *comp;
  const Expression                         *expr;
  SequenceExpression                       *seq;
  ExpressionSequenceExpression             *expr_seq;
  FunctionDefinitionStatement::ArgDecl     *fad;
  FunctionDefinitionStatement::ArgDeclList *fadl;
  FunctionCallExpression::Arg              *arg;
  FunctionCallExpression::Args             *args;
  TypeExpression                           *type;
  DimensionsExpression                     *dims;
  Array< CountPtr< const SequenceExpression> > *seq_array;
  Array< CountPtr< const ArrayBase > >  *array_array;
}

%destructor {} <ival>
%destructor {} <uval>
%destructor {} <fval>
%destructor {} <type_enum>
%destructor {} <uop>
%destructor {} <assign>
%destructor { /* is unified */ } <str>
%destructor {} <stmt>
%destructor {} <comp>
%destructor {} <expr>
%destructor {} <seq>
%destructor {} <expr_seq>
%destructor {} <fad>
%destructor {} <fadl>
%destructor {} <arg>
%destructor {} <args>
%destructor {} <type>
%destructor {} <dims>
%destructor {} <seq_array>
%destructor {} <array_array>


%token            END          0 "end of file";
%token            FLUSH        "flush evaluaton";
%token  <str>     IDENTIFIER   "identifier";
%token  <ival>    I_CONSTANT   "integer constant";
%token  <uval>    U_CONSTANT   "unsigned integer constant";
%token  <fval>    F_CONSTANT   "floating point constant";
%token  <str>     S_CONSTANT   "string constant";
%token            NIL          "nil";
%token            ARROW        "->";
%token            INC_OP       "++";
%token            DEC_OP       "--";
%token            LEFT_OP      "<<";
%token            RIGHT_OP     ">>";
%token            LE_OP        "<=";
%token            GE_OP        ">=";
%token            EQ_OP        "==";
%token            NE_OP        "!=";
%token            AND_OP       "&&";
%token            OR_OP        "||";
%token            XOR_OP       "^^";
%token  <assign>  MUL_ASSIGN   "*=";
%token  <assign>  DIV_ASSIGN   "/=";
%token  <assign>  MOD_ASSIGN   "%=";
%token  <assign>  ADD_ASSIGN   "+=";
%token  <assign>  SUB_ASSIGN   "-=";
%token  <assign>  LEFT_ASSIGN  "<<=";
%token  <assign>  RIGHT_ASSIGN ">>=";
%token  <assign>  AND_ASSIGN   "&=";
%token  <assign>  XOR_ASSIGN   "^=";
%token  <assign>  OR_ASSIGN    "|=";
%token  <type_enum>    BOOL         "bool";
%token  <type_enum>    INT          "int"   ;
%token  <type_enum>    UINT8        "uint8" ;
%token  <type_enum>    INT8         "int8"  ;
%token  <type_enum>    UINT16       "uint16";
%token  <type_enum>    INT16        "int16" ;
%token  <type_enum>    UINT32       "uint32";
%token  <type_enum>    INT32        "int32" ;
%token  <type_enum>    UINT64       "uint64";
%token  <type_enum>    INT64        "int64" ;
%token  <type_enum>    FLOAT        "float" ;
%token  <type_enum>    DOUBLE       "double";
%token  <type_enum>    STRING       "string";
%token  <type_enum>    ARRAY        "Array";
%token  <type_enum>    FRAME        "Frame";
%token  <type_enum>    FUNCTION     "Function";
%token  <type_enum>    METHOD       "Method";
%token  <type_enum>    OUTPUT       "Output";
%token  <type_enum>    INPUT        "Input";
%token  <type_enum>    NODE         "Node";
%token  <type_enum>    PARAM        "Param";
%token  <type_enum>    SEQUENCE     "Sequence";
%token            IF           "if";
%token            ELSE         "else";
%token            FOR          "for";
%token            IN           "in";
%token            TO           "to";
%token            STEP         "step";
%token            ELLIPSIS     "...";
%token            DOTDOT       "..";
%token            TRUE         "true";
%token            FALSE        "false";
%token            THIS         "this";
%token            RETURN       "return";
%token            DOUBLE_SEMI  ";;";
%token            TRI_SEMI     ";;;";
%token            QUAD_SEMI    ";;;;";

%type <str> identifier
%type <expr> array_constant
%type <seq>  array_constant_line
%type <seq_array> array_constant_slice
%type <array_array> array_constant_volume
%type <array_array> array_constant_hyper
%type <expr> frame_constant
%type <expr> callable_expression
%type <expr> primary_expression
%type <expr> constant
%type <expr> postfix_expression
%type <arg> argument_expression
%type <args> argument_expression_list
%type <expr> unary_expression
%type <uop> unary_operator
%type <expr> multiplicative_expression
%type <expr> additive_expression
%type <expr> shift_expression
%type <expr> relational_expression
%type <expr> equality_expression
%type <expr> and_expression
%type <expr> exclusive_or_expression
%type <expr> inclusive_or_expression
%type <expr> logical_and_expression
%type <expr> logical_exclusive_or_expression
%type <expr> logical_inclusive_or_expression
%type <expr> conditional_expression
%type <expr> expression
%type <stmt> assignment_statement
%type <stmt> connect_statement
%type <assign> assignment_operator
%type <expr> array_dimension
%type <dims> array_dimensions_expression
%type <dims> array_coordinates_expression
%type <type_enum> primitive_type_expression
%type <type_enum> basic_type_expression
%type <type> array_type_expression
%type <type> type_expression
%type <stmt> statement
%type <comp> statements
%type <comp> compound_statement
%type <stmt> selection_statement
%type <expr_seq> expression_sequence
%type <expr> sequence_expression
%type <seq> sequence
%type <stmt> iteration_statement
%type <fad> function_argument_decl
%type <fadl> function_argument_decl_list
%type <stmt> function_definition_statement
%type <stmt> variable_creation_statement
%type <stmt> return_statement
%type <stmt> expression_statement

%left "||"
%left "^^"
%left "&&"
%left '|'
%left '^'
%left '&'
%left "==" "!="
%left '<' '>' "<=" ">="
%left LEFT_OP RIGHT_OP
%left '+' '-'
%left '*' '/' '%'

%right "then" "else"

%start translation_unit;

%%

semicolon
  : ';'
  | DOUBLE_SEMI
  | TRI_SEMI
  | QUAD_SEMI
  ;

identifier
  : IDENTIFIER { ($$) = ($1); }
  | IN         { ($$) = scanner.unify( "in" ); }
  | TO         { ($$) = scanner.unify( "to" ); }
  | STEP       { ($$) = scanner.unify( "step" ); }
  ;

array_constant_line
  : sequence { ($$) = ($1); }
  ;

array_constant_slice
  : array_constant_slice ';' array_constant_line { ($1)->push_back( ($3) ); ($$) = ($1); }
  | array_constant_line { ($$) = new ArrayConstantExpression::SequenceExpressionArray( scanner.getScannerGC(), 1 ); ($$)->push_back( ($1) ); }
  ;

array_constant_volume
  : array_constant_volume DOUBLE_SEMI array_constant_slice { ($1)->push_back( ($3) ); ($$) = ($1); }
  | array_constant_slice { ($$) = new ArrayConstantExpression::ArrayBaseArray( scanner.getScannerGC(), 1 ); ($$)->push_back( ($1) ); }
  ;

array_constant_hyper
  : array_constant_hyper TRI_SEMI array_constant_volume { ($1)->push_back( ($3) ); ($$) = ($1); }
  | array_constant_volume { ($$) = new ArrayConstantExpression::ArrayBaseArray( scanner.getScannerGC(), 1 ); ($$)->push_back( ($1) ); }
  ;

array_constant
  : '[' ']'
    {
      ($$) = new ArrayConstantExpression(
          RPGML_LOC(@$)
        , new ArrayConstantExpression::SequenceExpressionArray( scanner.getScannerGC(), 1 )
        , 1
        );
    }
  | '[' array_constant_hyper  ']'
    {
      int dims = 4;
      CountPtr< const ArrayBase > ret = ($2);
      while( ret->size() == 1 )
      {
        --dims;
        const ArrayConstantExpression::ArrayBaseArray *arr = 0;
        if( !ret->getAs( arr ) ) break;
        ret = arr->at( 0 );
      }
      ($$) = new ArrayConstantExpression( RPGML_LOC(@$), ret, dims );
    }
  ;

frame_constant
  : FRAME compound_statement { ($1); ($2)->own_frame = false; ($$) = new FrameConstantExpression( RPGML_LOC(@$), ($2) ); }
  ;

primary_expression
  : constant           { ($$) = ($1); }
  | sequence_expression { ($$) = ($1); }
  | '(' expression ')' { ($$) = ($2); }
  ;

constant
  : I_CONSTANT { ($$) = new ConstantExpression( RPGML_LOC(@$), RPGML::Value($1) ); }
  | U_CONSTANT { ($$) = new ConstantExpression( RPGML_LOC(@$), RPGML::Value($1) ); }
  | F_CONSTANT { ($$) = new ConstantExpression( RPGML_LOC(@$), RPGML::Value($1) ); }
  | S_CONSTANT { ($$) = new ConstantExpression( RPGML_LOC(@$), RPGML::Value($1) ); }
  | TRUE       { ($$) = new ConstantExpression( RPGML_LOC(@$), RPGML::Value( true ) ); }
  | FALSE      { ($$) = new ConstantExpression( RPGML_LOC(@$), RPGML::Value( false ) ); }
  | NIL        { ($$) = new ConstantExpression( RPGML_LOC(@$), RPGML::Value() ); }
  | THIS       { ($$) = new ThisExpression( RPGML_LOC(@$) ); }
  | array_constant 
  | frame_constant
  ;

// Expressions that can potentially be of type Function
callable_expression
  : postfix_expression '[' array_coordinates_expression ']'
    {
      ($$) = new ArrayAccessExpression( RPGML_LOC(@$), ($1), ($3) );
    }
  | postfix_expression '.' identifier
    {
      ($$) = new FrameAccessExpression( RPGML_LOC(@$), ($1), ($3) );
    }
  | identifier         { ($$) = new LookupVariableExpression( RPGML_LOC(@$), ($1) ); }
  | '.' identifier     { ($$) = new LookupVariableExpression( RPGML_LOC(@$), ($2), true ); }
  ;

postfix_expression
  : callable_expression { ($$) = ($1); }
  | callable_expression '(' ')'
    {
      ($$) = new FunctionCallExpression( RPGML_LOC(@$), ($1), new FunctionCallExpression::Args() );
    } 
  | callable_expression '(' argument_expression_list ')'
    {
      ($$) = new FunctionCallExpression( RPGML_LOC(@$), ($1), ($3) );
    } 
  | primitive_type_expression '(' expression ')'
    {
      ($$) = new CastExpression( RPGML_LOC(@$), ($1), ($3) );
    } 
//  | postfix_expression INC_OP
//  | postfix_expression DEC_OP
  | primary_expression { ($$) = ($1); }
  ;

argument_expression
  : expression
    {
      ($$) = new FunctionCallExpression::Arg( RPGML_LOC(@$), ($1) );
    }
  | identifier '=' expression
    {
      ($$) = new FunctionCallExpression::Arg( RPGML_LOC(@$), ($3), ($1) );
    }
  ;

argument_expression_list
  : argument_expression 
    {
      ($$) = new FunctionCallExpression::Args();
      ($$)->append( ($1) );
    }
  | argument_expression_list ',' argument_expression
    {
      ($$) = ($1);
      ($$)->append( ($3) );
    }
  ;

unary_expression
  : postfix_expression { ($$) = ($1); }
//  | INC_OP unary_expression
//  | DEC_OP unary_expression
  | unary_operator unary_expression { ($$) = new UnaryExpression( RPGML_LOC(@$), ($1), ($2) ); }
  ;

unary_operator
  : '+' { ($$) = UOP_PLUS; }
  | '-' { ($$) = UOP_MINUS; }
  | '~' { ($$) = UOP_BIT_NOT; }
  | '!' { ($$) = UOP_LOG_NOT; }
  ;

multiplicative_expression
  : unary_expression { ($$) = ($1); }
  | multiplicative_expression '*' unary_expression
    {
      ($$) = new BinaryExpression( RPGML_LOC(@$), ($1), BOP_MUL, ($3) );
    }
  | multiplicative_expression '/' unary_expression
    {
      ($$) = new BinaryExpression( RPGML_LOC(@$), ($1), BOP_DIV, ($3) );
    }
  | multiplicative_expression '%' unary_expression
    {
      ($$) = new BinaryExpression( RPGML_LOC(@$), ($1), BOP_MOD, ($3) );
    }
  ;

additive_expression
  : multiplicative_expression  { ($$) = ($1); }
  | additive_expression '+' multiplicative_expression
    {
      ($$) = new BinaryExpression( RPGML_LOC(@$), ($1), BOP_ADD, ($3) );
    }
  | additive_expression '-' multiplicative_expression
    {
      ($$) = new BinaryExpression( RPGML_LOC(@$), ($1), BOP_SUB, ($3) );
    }
  ;

shift_expression
  : additive_expression { ($$) = ($1); }
  | shift_expression LEFT_OP additive_expression
    {
      ($$) = new BinaryExpression( RPGML_LOC(@$), ($1), BOP_LEFT, ($3) );
    }
  | shift_expression RIGHT_OP additive_expression
    {
      ($$) = new BinaryExpression( RPGML_LOC(@$), ($1), BOP_RIGHT, ($3) );
    }
  ;

relational_expression
  : shift_expression { ($$) = ($1); }
  | relational_expression '<' shift_expression
    {
      ($$) = new BinaryExpression( RPGML_LOC(@$), ($1), BOP_LT, ($3) );
    }
  | relational_expression '>' shift_expression
    {
      ($$) = new BinaryExpression( RPGML_LOC(@$), ($1), BOP_GT, ($3) );
    }
  | relational_expression LE_OP shift_expression
    {
      ($$) = new BinaryExpression( RPGML_LOC(@$), ($1), BOP_LE, ($3) );
    }
  | relational_expression GE_OP shift_expression
    {
      ($$) = new BinaryExpression( RPGML_LOC(@$), ($1), BOP_GE, ($3) );
    }
  ;

equality_expression
  : relational_expression { ($$) = ($1); }
  | equality_expression EQ_OP relational_expression
    {
      ($$) = new BinaryExpression( RPGML_LOC(@$), ($1), BOP_EQ, ($3) );
    }
  | equality_expression NE_OP relational_expression
    {
      ($$) = new BinaryExpression( RPGML_LOC(@$), ($1), BOP_NE, ($3) );
    }
  ;

and_expression
  : equality_expression { ($$) = ($1); }
  | and_expression '&' equality_expression
    {
      ($$) = new BinaryExpression( RPGML_LOC(@$), ($1), BOP_BIT_AND, ($3) );
    }
  ;

exclusive_or_expression
  : and_expression { ($$) = ($1); }
  | exclusive_or_expression '^' and_expression
    {
      ($$) = new BinaryExpression( RPGML_LOC(@$), ($1), BOP_BIT_XOR, ($3) );
    }
  ;

inclusive_or_expression
  : exclusive_or_expression { ($$) = ($1); }
  | inclusive_or_expression '|' exclusive_or_expression
    {
      ($$) = new BinaryExpression( RPGML_LOC(@$), ($1), BOP_BIT_OR, ($3) );
    }
  ;

logical_and_expression
  : inclusive_or_expression { ($$) = ($1); }
  | logical_and_expression AND_OP inclusive_or_expression
    {
      ($$) = new BinaryExpression( RPGML_LOC(@$), ($1), BOP_LOG_AND, ($3) );
    }
  ;

logical_exclusive_or_expression
  : logical_and_expression { ($$) = ($1); }
  | logical_exclusive_or_expression XOR_OP logical_and_expression
    {
      ($$) = new BinaryExpression( RPGML_LOC(@$), ($1), BOP_LOG_XOR, ($3) );
    }
  ;

logical_inclusive_or_expression
  : logical_exclusive_or_expression { ($$) = ($1); }
  | logical_inclusive_or_expression OR_OP logical_exclusive_or_expression
    {
      ($$) = new BinaryExpression( RPGML_LOC(@$), ($1), BOP_LOG_OR, ($3) );
    }
  ;

conditional_expression
  : logical_inclusive_or_expression { ($$) = ($1); }
  | logical_inclusive_or_expression '?' expression ':' conditional_expression
    {
      ($$) = new IfThenElseExpression( RPGML_LOC(@$), ($1), ($3), ($5) );
    }
  ;

expression
  : conditional_expression { ($$) = ($1); }
  ;

connect_statement
  : expression ARROW expression semicolon
    {
      ($$) = new ConnectStatement( RPGML_LOC(@$), ($1), ($3) );
    }
  ;

assignment_statement
  : identifier assignment_operator expression semicolon
    {
      ($$) = new AssignIdentifierStatement( RPGML_LOC(@$), ($1), ($2), ($3) );
    }
  | postfix_expression '.' identifier assignment_operator expression semicolon
    {
      ($$) = new AssignDotStatement( RPGML_LOC(@$), ($1), ($3), ($4), ($5) );
    }
  | postfix_expression '[' array_coordinates_expression ']' assignment_operator expression semicolon
    {
      ($$) = new AssignBracketStatement( RPGML_LOC(@$), ($1), ($3), ($5), ($6) );
    }
  ;

assignment_operator
  : '=' { ($$) = ID_ASSIGN; }
  | MUL_ASSIGN
  | DIV_ASSIGN
  | MOD_ASSIGN
  | ADD_ASSIGN
  | SUB_ASSIGN
  | LEFT_ASSIGN
  | RIGHT_ASSIGN
  | AND_ASSIGN
  | XOR_ASSIGN
  | OR_ASSIGN
  ;

primitive_type_expression
  : BOOL
  | INT
  | UINT8 
  | INT8  
  | UINT16
  | INT16 
  | UINT32
  | INT32 
  | UINT64
  | INT64 
  | FLOAT 
  | DOUBLE
  | STRING
  ;

basic_type_expression
  : primitive_type_expression
  | NODE
  | FRAME
  | FUNCTION
  | OUTPUT
  | INPUT
  | PARAM
  | SEQUENCE
//  | ARRAY
  ;

array_dimension
  : expression
  | /*empty*/
    {
      ($$) = 0;
    }
  ;

array_dimensions_expression
  : array_dimensions_expression ',' array_dimension
    {
      ($$) = ($1);
      ($$)->push_back( ($3) );
    }
  | array_dimension
    {
      ($$) = new DimensionsExpression( RPGML_LOC(@$) );
      ($$)->push_back( ($1) );
    }
  ;

array_type_expression
  : type_expression '[' array_dimensions_expression ']'
    {
      ($$) = new TypeExpression( RPGML_LOC(@$), Type::Array(), ($1), ($3) );
    }
  ;

array_coordinates_expression
  : array_dimensions_expression
  ;

type_expression
  : basic_type_expression
    {
      ($$) = new TypeExpression( RPGML_LOC(@$), ($1) );
    }
  | array_type_expression
  ;

statement
  : compound_statement { ($$) = ($1); }
  | selection_statement
  | iteration_statement
  | function_definition_statement
  | assignment_statement
  | connect_statement
  | variable_creation_statement
  | return_statement
  | expression_statement
  | FLUSH { ($$) = new NOPStatement( RPGML_LOC(@$) ); }
  | semicolon   { ($$) = new NOPStatement( RPGML_LOC(@$) ); }
  ;

statements
  : statement
    {
      ($$) = new CompoundStatement( RPGML_LOC(@$) );
      ($$)->append( ($1) );
    }
  | statements statement
    {
      ($$) = ($1);
      ($$)->append( ($2) );
    }
  | statements error semicolon
    {
      ($$) = ($1);
    }
  | error semicolon
    {
      ($$) = new CompoundStatement( RPGML_LOC(@$) );
    }
  ;

compound_statement
  : '{' '}'
    {
      ($$) = new CompoundStatement( RPGML_LOC(@$) );
    }
  | '{' statements '}'
    {
      ($$) = ($2);
    }
  | '{' error '}'
    {
      ($$) = new CompoundStatement( RPGML_LOC(@$) );
    }
  ;

selection_statement
  : IF '(' expression ')' statement ELSE statement
    {
      ($$) = new IfStatement( RPGML_LOC(@$), ($3), ($5), ($7) );
    }
  | IF '(' expression ')' statement %prec "then"
    {
      ($$) = new IfStatement( RPGML_LOC(@$), ($3), ($5) );
    }
  ;

expression_statement
  : expression semicolon
    {
      ($$) = new ExpressionStatement( RPGML_LOC(@$), ($1) );
    }
  ;

expression_sequence
  : expression
    {
      ($$) = new ExpressionSequenceExpression( RPGML_LOC(@$) );
      ($$)->append( ($1) );
    }
  | expression_sequence ',' expression
    {
      ($$) = ($1);
      ($$)->append( ($3) );
    }
  ;

sequence
  : expression TO expression
    {
      ($$) = new FromToStepSequenceExpression( RPGML_LOC(@$), ($1), ($3) );
    }
  | expression TO expression STEP expression
    {
      ($$) = new FromToStepSequenceExpression( RPGML_LOC(@$), ($1), ($3), ($5) );
    }
  | expression_sequence
    {
      ($$) = ($1);
    }
  ;

sequence_expression
  : SEQUENCE '(' sequence ')'
    {
      (void)($1);
      ($$) = new ParenthisSequenceExpression( RPGML_LOC(@$), ($3) );
    }
  | SEQUENCE '(' ')'
    {
      (void)($1);
      ($$) = new ParenthisSequenceExpression( RPGML_LOC(@$) );
    }
  ;

iteration_statement
  : FOR identifier IN expression statement
    {
      ($$) = new ForContainerStatement( RPGML_LOC(@$), ($2), ($4), ($5) );
    }
  | FOR identifier '=' sequence statement
    {
      ($$) = new ForSequenceStatement( RPGML_LOC(@$), ($2), ($4), ($5) );
    }
  ;

function_argument_decl
  : identifier
    {
      ($$) = new FunctionDefinitionStatement::ArgDecl( RPGML_LOC(@$), ($1) );
    }
  | identifier '=' expression
    {
      ($$) = new FunctionDefinitionStatement::ArgDecl( RPGML_LOC(@$), ($1), ($3) );
    }
  ;

function_argument_decl_list
  : function_argument_decl
    {
      ($$) = new FunctionDefinitionStatement::ArgDeclList();
      ($$)->append( ($1) );
    }
  | function_argument_decl_list ',' function_argument_decl
    {
      ($$) = ($1);
      ($$)->append( ($3) );
    }
  ;

function_definition_statement
  : type_expression identifier '(' ')' compound_statement
    {
      ($5)->own_frame = false;
      ($$) = new FunctionDefinitionStatement( RPGML_LOC(@$), ($1), ($2), new FunctionDefinitionStatement::ArgDeclList(), ($5) );
      (void)($1);
    }
  | type_expression identifier '(' function_argument_decl_list ')' compound_statement
    {
      ($6)->own_frame = false;
      ($$) = new FunctionDefinitionStatement( RPGML_LOC(@$), ($1), ($2), ($4), ($6) );
      (void)($1);
    }
  | METHOD identifier '(' ')' compound_statement
    {
      ($5)->own_frame = false;
      ($$) = new FunctionDefinitionStatement( RPGML_LOC(@$), new TypeExpression( RPGML_LOC(@$), ($1) ), ($2), new FunctionDefinitionStatement::ArgDeclList(), ($5), true );
      (void)($1);
    }
  | METHOD identifier '(' function_argument_decl_list ')' compound_statement
    {
      ($6)->own_frame = false;
      ($$) = new FunctionDefinitionStatement( RPGML_LOC(@$), new TypeExpression( RPGML_LOC(@$), ($1) ), ($2), ($4), ($6), true );
      (void)($1);
    }
  ;

variable_creation_statement
  : type_expression identifier '=' expression semicolon
    {
      ($$) = new VariableCreationStatement( RPGML_LOC(@$), ($1), ($2), ($4) );
    }
  | callable_expression identifier '(' ')' semicolon
    {
      ($$) =
        new VariableConstructionStatement(
            RPGML_LOC(@$)
          , ($2)
          , new FunctionCallExpression( RPGML_LOC(@$), ($1), new FunctionCallExpression::Args() )
          );
    }
  | callable_expression identifier '(' argument_expression_list ')' semicolon
    {
      ($$) =
        new VariableConstructionStatement(
            RPGML_LOC(@$)
          , ($2)
          , new FunctionCallExpression( RPGML_LOC(@$), ($1), ($4) )
          );
    }
  ;

return_statement
  : RETURN expression semicolon
    {
      ($$) = new ReturnStatement( RPGML_LOC(@$), ($2) );
    }
  ;

translation_unit_statements
  : statement
    {
      scanner.append( ($1) );
    }
  | translation_unit_statements statement
    {
      scanner.append( ($2) );
    }
  | error semicolon
  ;

translation_unit
  : translation_unit_statements
  | /* empty */
  ;

%%



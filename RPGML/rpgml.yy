%skeleton "lalr1.cc" /* C++ */
%require "2.5"
%defines
%define parser_class_name "_Parser"
%define namespace "RPGML"
%glr-parser
%parse-param { RPGML::Scanner &scanner }
%lex-param { RPGML::Scanner &scanner }
%error-verbose

%code requires
{
#include "AST.h"
#include "String.h"
#include "Value.h"
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

// The code between ‘%code {’ and ‘}’ is output in the *.cc file; it needs detailed knowledge about the scanner
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
  bool         bval;
  int          ival;
  float        fval;
  Type::Enum   type;
  BOP          binop;
  UOP          uop;
  ASSIGN       assign;
  const StringData *str;
  Statement   *stmt;
  CompoundStatement *comp;
  Expression  *expr;
  SequenceExpression *seq;
  ExpressionSequenceExpression *expr_seq;
  FunctionDefinitionStatement::ArgDecl     *fad;
  FunctionDefinitionStatement::ArgDeclList *fadl;
  FunctionCallExpression::Arg  *arg;
  FunctionCallExpression::Args *args;
}

%destructor {} <bval>
%destructor {} <ival>
%destructor {} <fval>
%destructor {} <type>
%destructor {} <uop>
%destructor {} <assign>
%destructor { /*unified*/ } <str>
%destructor { delete ($$); } <stmt>
%destructor { delete ($$); } <expr>
%destructor { delete ($$); } <seq>
%destructor { delete ($$); } <fad>
%destructor { delete ($$); } <fadl>


%token            END          0 "end of file";
%token            FLUSH        "flush evaluaton";
%token  <str>     IDENTIFIER   "identifier";
%token  <ival>    I_CONSTANT   "integer constant";
%token  <fval>    F_CONSTANT   "floating point constant";
%token  <str>     S_CONSTANT   "string constant";
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
%token  <type>    BOOL         "bool";
%token  <type>    INT          "int";
%token  <type>    FLOAT        "float";
%token  <type>    STRING       "string";
%token  <type>    ARRAY        "Array";
%token  <type>    FRAME          "Frame";
%token  <type>    FUNCTION     "Function";
%token  <type>    OUTPUT       "Output";
%token  <type>    INPUT        "Input";
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

%type <str> identifier
%type <expr> array_constant
%type <expr> map_constant
%type <expr> primary_expression
%type <expr> constant
%type <expr> function_call_expression
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
%type <assign> assignment_operator
%type <type> type_specifier
%type <stmt> statement
%type <comp> statements
%type <comp> compound_statement
%type <stmt> selection_statement
%type <expr_seq> expression_sequence_expression
%type <seq> sequence_expression
%type <seq> sequence
%type <stmt> iteration_statement
%type <fad> function_argument_decl
%type <fadl> function_argument_decl_list
%type <stmt> function_definition_statement
%type <stmt> function_call_statement
%type <stmt> variable_creation_statement
%type <stmt> return_statement

%start translation_unit;

%%

identifier
  : IDENTIFIER { ($$) = ($1); }
  | IN         { ($$) = scanner.unify( String::Static( "in" ) ); }
  | TO         { ($$) = scanner.unify( String::Static( "to"  ) ); }
  | STEP       { ($$) = scanner.unify( String::Static( "step" ) ); }
  ;

array_constant
  : '[' ']'          { ($$) = new ArrayConstantExpression( RPGML_LOC(@$) ); }
  | '[' sequence ']' { ($$) = new ArrayConstantExpression( RPGML_LOC(@$), ($2) ); }
  ;

map_constant
  : compound_statement { ($1)->own_map = false; ($$) = new FrameConstantExpression( RPGML_LOC(@$), ($1) ); }
  ;

primary_expression
  : identifier         { ($$) = new LookupVariableExpression( RPGML_LOC(@$), ($1) ); }
  | constant           { ($$) = ($1); }
  | '(' expression ')' { ($$) = ($2); }
  | '(' sequence_expression ')' { ($$) = new ParenthisSequenceExpression( RPGML_LOC(@$), ($2) ); }
  ;

constant
  : I_CONSTANT { ($$) = new ConstantExpression( RPGML_LOC(@$), ($1) ); }
  | F_CONSTANT { ($$) = new ConstantExpression( RPGML_LOC(@$), ($1) ); }
  | S_CONSTANT { ($$) = new ConstantExpression( RPGML_LOC(@$), ($1) ); }
  | TRUE       { ($$) = new ConstantExpression( RPGML_LOC(@$), true ); }
  | FALSE      { ($$) = new ConstantExpression( RPGML_LOC(@$), false ); }
  | array_constant 
  | map_constant
  ;

function_call_expression
  : postfix_expression '(' ')'
    {
      ($$) = new FunctionCallExpression( RPGML_LOC(@$), ($1), new FunctionCallExpression::Args() );
    } 
  | postfix_expression '(' argument_expression_list ')'
    {
      ($$) = new FunctionCallExpression( RPGML_LOC(@$), ($1), ($3) );
    } 
//  | type_specifier '(' expression ')'
  ;

postfix_expression
  : postfix_expression '[' expression ']'
    {
      ($$) = new AccessExpression( RPGML_LOC(@$), ($1), ($3) );
    }
  | postfix_expression '.' identifier
    {
      ($$) = new AccessExpression( RPGML_LOC(@$), ($1), new ConstantExpression( RPGML_LOC(@$), ($3) ) );
    }
  | function_call_expression { ($$) = ($1); }
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

assignment_statement
  : identifier assignment_operator expression ';'
    {
      ($$) = new AssignIdentifierStatement( RPGML_LOC(@$), ($1), ($2), ($3) );
    }
  | postfix_expression '.' identifier assignment_operator expression ';'
    {
      ($$) = new AssignDotStatement( RPGML_LOC(@$), ($1), ($3), ($4), ($5) );
    }
  | postfix_expression '[' expression ']' assignment_operator expression ';'
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

type_specifier
  : BOOL
  | INT
  | FLOAT
  | STRING
  | ARRAY
  | FRAME
//  | FUNCTION
  | OUTPUT
  | INPUT
  ;

statement
  : compound_statement { ($$) = ($1); }
  | selection_statement
  | iteration_statement
  | function_definition_statement
  | function_call_statement
  | assignment_statement
  | variable_creation_statement
  | return_statement
  | FLUSH { ($$) = new NOPStatement( RPGML_LOC(@$) ); }
  | ';'   { ($$) = new NOPStatement( RPGML_LOC(@$) ); }
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
  | statements error ';'
    {
      ($$) = ($1);
    }
  | error ';'
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
  | IF '(' expression ')' statement
    {
      ($$) = new IfStatement( RPGML_LOC(@$), ($3), ($5) );
    }
  ;

expression_sequence_expression
  : expression
    {
      ($$) = new ExpressionSequenceExpression( RPGML_LOC(@$) );
      ($$)->append( ($1) );
    }
  | expression_sequence_expression ',' expression
    {
      ($$) = ($1);
      ($$)->append( ($3) );
    }
  ;

sequence_expression
  : expression TO expression
    {
      ($$) = new FromToStepSequenceExpression( RPGML_LOC(@$), ($1), ($3) );
    }
  | expression TO expression STEP expression
    {
      ($$) = new FromToStepSequenceExpression( RPGML_LOC(@$), ($1), ($3), ($5) );
    }
  | expression_sequence_expression
    {
      ($$) = ($1);
    }
  ;

sequence
  : sequence_expression
    {
      ($$) = ($1);
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
  : FUNCTION identifier '(' ')' compound_statement
    {
      ($$) = new FunctionDefinitionStatement( RPGML_LOC(@$), ($2), new FunctionDefinitionStatement::ArgDeclList(), ($5) );
      (void)($1);
    }
  | FUNCTION identifier '(' function_argument_decl_list ')' compound_statement
    {
      ($$) = new FunctionDefinitionStatement( RPGML_LOC(@$), ($2), ($4), ($6) );
      (void)($1);
    }
  | FUNCTION identifier '=' expression ';'
    {
      ($$) = new VariableCreationStatement( RPGML_LOC(@$), ($1), ($2), ($4) );
    }
  ;

function_call_statement
  : function_call_expression ';'
    {
      ($$) = new FunctionCallStatement( RPGML_LOC(@$), ($1) );
    }
  ;

variable_creation_statement
  : type_specifier identifier '=' expression ';'
    {
      ($$) = new VariableCreationStatement( RPGML_LOC(@$), ($1), ($2), ($4) );
    }
//  | type_specifier postfix_expression '.' identifier '=' expression ';'
//  | type_specifier postfix_expression '[' expression ']' '=' expression ';'
  ;

return_statement
  : RETURN expression ';'
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
  | error ';'
  ;

translation_unit
  : translation_unit_statements
  | /* empty */
  ;

%%



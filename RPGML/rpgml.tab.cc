/* A Bison parser, made by GNU Bison 2.5.  */

/* Skeleton implementation for Bison LALR(1) parsers in C++
   
      Copyright (C) 2002-2011 Free Software Foundation, Inc.
   
   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.
   
   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.
   
   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.
   
   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */


/* First part of user declarations.  */


/* Line 293 of lalr1.cc  */
#line 39 "rpgml.tab.cc"


#include "rpgml.tab.hh"

/* User implementation prologue.  */


/* Line 299 of lalr1.cc  */
#line 48 "rpgml.tab.cc"
/* Unqualified %code blocks.  */

/* Line 300 of lalr1.cc  */
#line 31 "rpgml.yy"

#include "Scanner.h"
namespace RPGML
{
  int yylex( _Parser::semantic_type *yylval, _Parser::location_type *yylloc, Scanner &scanner )
  {
    return scanner.yylex( yylval, yylloc );
  }
}
#define RPGML_LOC(L) toLocation( scanner.unified_filename, (L) )



/* Line 300 of lalr1.cc  */
#line 67 "rpgml.tab.cc"

#ifndef YY_
# if defined YYENABLE_NLS && YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> /* FIXME: INFRINGES ON USER NAME SPACE */
#   define YY_(msgid) dgettext ("bison-runtime", msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(msgid) msgid
# endif
#endif

/* YYLLOC_DEFAULT -- Set CURRENT to span from RHS[1] to RHS[N].
   If N is 0, then set CURRENT to the empty location which ends
   the previous symbol: RHS[0] (always defined).  */

#define YYRHSLOC(Rhs, K) ((Rhs)[K])
#ifndef YYLLOC_DEFAULT
# define YYLLOC_DEFAULT(Current, Rhs, N)                               \
 do                                                                    \
   if (N)                                                              \
     {                                                                 \
       (Current).begin = YYRHSLOC (Rhs, 1).begin;                      \
       (Current).end   = YYRHSLOC (Rhs, N).end;                        \
     }                                                                 \
   else                                                                \
     {                                                                 \
       (Current).begin = (Current).end = YYRHSLOC (Rhs, 0).end;        \
     }                                                                 \
 while (false)
#endif

/* Suppress unused-variable warnings by "using" E.  */
#define YYUSE(e) ((void) (e))

/* Enable debugging if requested.  */
#if YYDEBUG

/* A pseudo ostream that takes yydebug_ into account.  */
# define YYCDEBUG if (yydebug_) (*yycdebug_)

# define YY_SYMBOL_PRINT(Title, Type, Value, Location)	\
do {							\
  if (yydebug_)						\
    {							\
      *yycdebug_ << Title << ' ';			\
      yy_symbol_print_ ((Type), (Value), (Location));	\
      *yycdebug_ << std::endl;				\
    }							\
} while (false)

# define YY_REDUCE_PRINT(Rule)		\
do {					\
  if (yydebug_)				\
    yy_reduce_print_ (Rule);		\
} while (false)

# define YY_STACK_PRINT()		\
do {					\
  if (yydebug_)				\
    yystack_print_ ();			\
} while (false)

#else /* !YYDEBUG */

# define YYCDEBUG if (false) std::cerr
# define YY_SYMBOL_PRINT(Title, Type, Value, Location)
# define YY_REDUCE_PRINT(Rule)
# define YY_STACK_PRINT()

#endif /* !YYDEBUG */

#define yyerrok		(yyerrstatus_ = 0)
#define yyclearin	(yychar = yyempty_)

#define YYACCEPT	goto yyacceptlab
#define YYABORT		goto yyabortlab
#define YYERROR		goto yyerrorlab
#define YYRECOVERING()  (!!yyerrstatus_)


/* Line 382 of lalr1.cc  */
#line 5 "rpgml.yy"
namespace RPGML {

/* Line 382 of lalr1.cc  */
#line 155 "rpgml.tab.cc"

  /* Return YYSTR after stripping away unnecessary quotes and
     backslashes, so that it's suitable for yyerror.  The heuristic is
     that double-quoting is unnecessary unless the string contains an
     apostrophe, a comma, or backslash (other than backslash-backslash).
     YYSTR is taken from yytname.  */
  std::string
  _Parser::yytnamerr_ (const char *yystr)
  {
    if (*yystr == '"')
      {
        std::string yyr = "";
        char const *yyp = yystr;

        for (;;)
          switch (*++yyp)
            {
            case '\'':
            case ',':
              goto do_not_strip_quotes;

            case '\\':
              if (*++yyp != '\\')
                goto do_not_strip_quotes;
              /* Fall through.  */
            default:
              yyr += *yyp;
              break;

            case '"':
              return yyr;
            }
      do_not_strip_quotes: ;
      }

    return yystr;
  }


  /// Build a parser object.
  _Parser::_Parser (RPGML::Scanner &scanner_yyarg)
    :
#if YYDEBUG
      yydebug_ (false),
      yycdebug_ (&std::cerr),
#endif
      scanner (scanner_yyarg)
  {
  }

  _Parser::~_Parser ()
  {
  }

#if YYDEBUG
  /*--------------------------------.
  | Print this symbol on YYOUTPUT.  |
  `--------------------------------*/

  inline void
  _Parser::yy_symbol_value_print_ (int yytype,
			   const semantic_type* yyvaluep, const location_type* yylocationp)
  {
    YYUSE (yylocationp);
    YYUSE (yyvaluep);
    switch (yytype)
      {
         default:
	  break;
      }
  }


  void
  _Parser::yy_symbol_print_ (int yytype,
			   const semantic_type* yyvaluep, const location_type* yylocationp)
  {
    *yycdebug_ << (yytype < yyntokens_ ? "token" : "nterm")
	       << ' ' << yytname_[yytype] << " ("
	       << *yylocationp << ": ";
    yy_symbol_value_print_ (yytype, yyvaluep, yylocationp);
    *yycdebug_ << ')';
  }
#endif

  void
  _Parser::yydestruct_ (const char* yymsg,
			   int yytype, semantic_type* yyvaluep, location_type* yylocationp)
  {
    YYUSE (yylocationp);
    YYUSE (yymsg);
    YYUSE (yyvaluep);

    YY_SYMBOL_PRINT (yymsg, yytype, yyvaluep, yylocationp);

    switch (yytype)
      {
        case 4: /* "\"identifier\"" */

/* Line 480 of lalr1.cc  */
#line 70 "rpgml.yy"
	{ /*unified*/ };

/* Line 480 of lalr1.cc  */
#line 260 "rpgml.tab.cc"
	break;
      case 5: /* "\"integer constant\"" */

/* Line 480 of lalr1.cc  */
#line 65 "rpgml.yy"
	{};

/* Line 480 of lalr1.cc  */
#line 269 "rpgml.tab.cc"
	break;
      case 6: /* "\"floating point constant\"" */

/* Line 480 of lalr1.cc  */
#line 66 "rpgml.yy"
	{};

/* Line 480 of lalr1.cc  */
#line 278 "rpgml.tab.cc"
	break;
      case 7: /* "\"string constant\"" */

/* Line 480 of lalr1.cc  */
#line 70 "rpgml.yy"
	{ /*unified*/ };

/* Line 480 of lalr1.cc  */
#line 287 "rpgml.tab.cc"
	break;
      case 20: /* "\"*=\"" */

/* Line 480 of lalr1.cc  */
#line 69 "rpgml.yy"
	{};

/* Line 480 of lalr1.cc  */
#line 296 "rpgml.tab.cc"
	break;
      case 21: /* "\"/=\"" */

/* Line 480 of lalr1.cc  */
#line 69 "rpgml.yy"
	{};

/* Line 480 of lalr1.cc  */
#line 305 "rpgml.tab.cc"
	break;
      case 22: /* "\"%=\"" */

/* Line 480 of lalr1.cc  */
#line 69 "rpgml.yy"
	{};

/* Line 480 of lalr1.cc  */
#line 314 "rpgml.tab.cc"
	break;
      case 23: /* "\"+=\"" */

/* Line 480 of lalr1.cc  */
#line 69 "rpgml.yy"
	{};

/* Line 480 of lalr1.cc  */
#line 323 "rpgml.tab.cc"
	break;
      case 24: /* "\"-=\"" */

/* Line 480 of lalr1.cc  */
#line 69 "rpgml.yy"
	{};

/* Line 480 of lalr1.cc  */
#line 332 "rpgml.tab.cc"
	break;
      case 25: /* "\"<<=\"" */

/* Line 480 of lalr1.cc  */
#line 69 "rpgml.yy"
	{};

/* Line 480 of lalr1.cc  */
#line 341 "rpgml.tab.cc"
	break;
      case 26: /* "\">>=\"" */

/* Line 480 of lalr1.cc  */
#line 69 "rpgml.yy"
	{};

/* Line 480 of lalr1.cc  */
#line 350 "rpgml.tab.cc"
	break;
      case 27: /* "\"&=\"" */

/* Line 480 of lalr1.cc  */
#line 69 "rpgml.yy"
	{};

/* Line 480 of lalr1.cc  */
#line 359 "rpgml.tab.cc"
	break;
      case 28: /* "\"^=\"" */

/* Line 480 of lalr1.cc  */
#line 69 "rpgml.yy"
	{};

/* Line 480 of lalr1.cc  */
#line 368 "rpgml.tab.cc"
	break;
      case 29: /* "\"|=\"" */

/* Line 480 of lalr1.cc  */
#line 69 "rpgml.yy"
	{};

/* Line 480 of lalr1.cc  */
#line 377 "rpgml.tab.cc"
	break;
      case 30: /* "\"bool\"" */

/* Line 480 of lalr1.cc  */
#line 67 "rpgml.yy"
	{};

/* Line 480 of lalr1.cc  */
#line 386 "rpgml.tab.cc"
	break;
      case 31: /* "\"int\"" */

/* Line 480 of lalr1.cc  */
#line 67 "rpgml.yy"
	{};

/* Line 480 of lalr1.cc  */
#line 395 "rpgml.tab.cc"
	break;
      case 32: /* "\"float\"" */

/* Line 480 of lalr1.cc  */
#line 67 "rpgml.yy"
	{};

/* Line 480 of lalr1.cc  */
#line 404 "rpgml.tab.cc"
	break;
      case 33: /* "\"string\"" */

/* Line 480 of lalr1.cc  */
#line 67 "rpgml.yy"
	{};

/* Line 480 of lalr1.cc  */
#line 413 "rpgml.tab.cc"
	break;
      case 34: /* "\"Array\"" */

/* Line 480 of lalr1.cc  */
#line 67 "rpgml.yy"
	{};

/* Line 480 of lalr1.cc  */
#line 422 "rpgml.tab.cc"
	break;
      case 35: /* "\"Map\"" */

/* Line 480 of lalr1.cc  */
#line 67 "rpgml.yy"
	{};

/* Line 480 of lalr1.cc  */
#line 431 "rpgml.tab.cc"
	break;
      case 36: /* "\"Function\"" */

/* Line 480 of lalr1.cc  */
#line 67 "rpgml.yy"
	{};

/* Line 480 of lalr1.cc  */
#line 440 "rpgml.tab.cc"
	break;
      case 37: /* "\"Output\"" */

/* Line 480 of lalr1.cc  */
#line 67 "rpgml.yy"
	{};

/* Line 480 of lalr1.cc  */
#line 449 "rpgml.tab.cc"
	break;
      case 38: /* "\"Input\"" */

/* Line 480 of lalr1.cc  */
#line 67 "rpgml.yy"
	{};

/* Line 480 of lalr1.cc  */
#line 458 "rpgml.tab.cc"
	break;
      case 39: /* "\"Param\"" */

/* Line 480 of lalr1.cc  */
#line 67 "rpgml.yy"
	{};

/* Line 480 of lalr1.cc  */
#line 467 "rpgml.tab.cc"
	break;
      case 77: /* "identifier" */

/* Line 480 of lalr1.cc  */
#line 70 "rpgml.yy"
	{ /*unified*/ };

/* Line 480 of lalr1.cc  */
#line 476 "rpgml.tab.cc"
	break;
      case 78: /* "array_constant" */

/* Line 480 of lalr1.cc  */
#line 72 "rpgml.yy"
	{ delete ((yyvaluep->expr)); };

/* Line 480 of lalr1.cc  */
#line 485 "rpgml.tab.cc"
	break;
      case 79: /* "map_constant" */

/* Line 480 of lalr1.cc  */
#line 72 "rpgml.yy"
	{ delete ((yyvaluep->expr)); };

/* Line 480 of lalr1.cc  */
#line 494 "rpgml.tab.cc"
	break;
      case 80: /* "primary_expression" */

/* Line 480 of lalr1.cc  */
#line 72 "rpgml.yy"
	{ delete ((yyvaluep->expr)); };

/* Line 480 of lalr1.cc  */
#line 503 "rpgml.tab.cc"
	break;
      case 81: /* "constant" */

/* Line 480 of lalr1.cc  */
#line 72 "rpgml.yy"
	{ delete ((yyvaluep->expr)); };

/* Line 480 of lalr1.cc  */
#line 512 "rpgml.tab.cc"
	break;
      case 82: /* "function_call_expression" */

/* Line 480 of lalr1.cc  */
#line 72 "rpgml.yy"
	{ delete ((yyvaluep->expr)); };

/* Line 480 of lalr1.cc  */
#line 521 "rpgml.tab.cc"
	break;
      case 83: /* "postfix_expression" */

/* Line 480 of lalr1.cc  */
#line 72 "rpgml.yy"
	{ delete ((yyvaluep->expr)); };

/* Line 480 of lalr1.cc  */
#line 530 "rpgml.tab.cc"
	break;
      case 86: /* "unary_expression" */

/* Line 480 of lalr1.cc  */
#line 72 "rpgml.yy"
	{ delete ((yyvaluep->expr)); };

/* Line 480 of lalr1.cc  */
#line 539 "rpgml.tab.cc"
	break;
      case 87: /* "unary_operator" */

/* Line 480 of lalr1.cc  */
#line 68 "rpgml.yy"
	{};

/* Line 480 of lalr1.cc  */
#line 548 "rpgml.tab.cc"
	break;
      case 88: /* "multiplicative_expression" */

/* Line 480 of lalr1.cc  */
#line 72 "rpgml.yy"
	{ delete ((yyvaluep->expr)); };

/* Line 480 of lalr1.cc  */
#line 557 "rpgml.tab.cc"
	break;
      case 89: /* "additive_expression" */

/* Line 480 of lalr1.cc  */
#line 72 "rpgml.yy"
	{ delete ((yyvaluep->expr)); };

/* Line 480 of lalr1.cc  */
#line 566 "rpgml.tab.cc"
	break;
      case 90: /* "shift_expression" */

/* Line 480 of lalr1.cc  */
#line 72 "rpgml.yy"
	{ delete ((yyvaluep->expr)); };

/* Line 480 of lalr1.cc  */
#line 575 "rpgml.tab.cc"
	break;
      case 91: /* "relational_expression" */

/* Line 480 of lalr1.cc  */
#line 72 "rpgml.yy"
	{ delete ((yyvaluep->expr)); };

/* Line 480 of lalr1.cc  */
#line 584 "rpgml.tab.cc"
	break;
      case 92: /* "equality_expression" */

/* Line 480 of lalr1.cc  */
#line 72 "rpgml.yy"
	{ delete ((yyvaluep->expr)); };

/* Line 480 of lalr1.cc  */
#line 593 "rpgml.tab.cc"
	break;
      case 93: /* "and_expression" */

/* Line 480 of lalr1.cc  */
#line 72 "rpgml.yy"
	{ delete ((yyvaluep->expr)); };

/* Line 480 of lalr1.cc  */
#line 602 "rpgml.tab.cc"
	break;
      case 94: /* "exclusive_or_expression" */

/* Line 480 of lalr1.cc  */
#line 72 "rpgml.yy"
	{ delete ((yyvaluep->expr)); };

/* Line 480 of lalr1.cc  */
#line 611 "rpgml.tab.cc"
	break;
      case 95: /* "inclusive_or_expression" */

/* Line 480 of lalr1.cc  */
#line 72 "rpgml.yy"
	{ delete ((yyvaluep->expr)); };

/* Line 480 of lalr1.cc  */
#line 620 "rpgml.tab.cc"
	break;
      case 96: /* "logical_and_expression" */

/* Line 480 of lalr1.cc  */
#line 72 "rpgml.yy"
	{ delete ((yyvaluep->expr)); };

/* Line 480 of lalr1.cc  */
#line 629 "rpgml.tab.cc"
	break;
      case 97: /* "logical_exclusive_or_expression" */

/* Line 480 of lalr1.cc  */
#line 72 "rpgml.yy"
	{ delete ((yyvaluep->expr)); };

/* Line 480 of lalr1.cc  */
#line 638 "rpgml.tab.cc"
	break;
      case 98: /* "logical_inclusive_or_expression" */

/* Line 480 of lalr1.cc  */
#line 72 "rpgml.yy"
	{ delete ((yyvaluep->expr)); };

/* Line 480 of lalr1.cc  */
#line 647 "rpgml.tab.cc"
	break;
      case 99: /* "conditional_expression" */

/* Line 480 of lalr1.cc  */
#line 72 "rpgml.yy"
	{ delete ((yyvaluep->expr)); };

/* Line 480 of lalr1.cc  */
#line 656 "rpgml.tab.cc"
	break;
      case 100: /* "expression" */

/* Line 480 of lalr1.cc  */
#line 72 "rpgml.yy"
	{ delete ((yyvaluep->expr)); };

/* Line 480 of lalr1.cc  */
#line 665 "rpgml.tab.cc"
	break;
      case 101: /* "assignment_statement" */

/* Line 480 of lalr1.cc  */
#line 71 "rpgml.yy"
	{ delete ((yyvaluep->stmt)); };

/* Line 480 of lalr1.cc  */
#line 674 "rpgml.tab.cc"
	break;
      case 102: /* "assignment_operator" */

/* Line 480 of lalr1.cc  */
#line 69 "rpgml.yy"
	{};

/* Line 480 of lalr1.cc  */
#line 683 "rpgml.tab.cc"
	break;
      case 103: /* "type_specifier" */

/* Line 480 of lalr1.cc  */
#line 67 "rpgml.yy"
	{};

/* Line 480 of lalr1.cc  */
#line 692 "rpgml.tab.cc"
	break;
      case 104: /* "statement" */

/* Line 480 of lalr1.cc  */
#line 71 "rpgml.yy"
	{ delete ((yyvaluep->stmt)); };

/* Line 480 of lalr1.cc  */
#line 701 "rpgml.tab.cc"
	break;
      case 107: /* "selection_statement" */

/* Line 480 of lalr1.cc  */
#line 71 "rpgml.yy"
	{ delete ((yyvaluep->stmt)); };

/* Line 480 of lalr1.cc  */
#line 710 "rpgml.tab.cc"
	break;
      case 109: /* "sequence_expression" */

/* Line 480 of lalr1.cc  */
#line 73 "rpgml.yy"
	{ delete ((yyvaluep->seq)); };

/* Line 480 of lalr1.cc  */
#line 719 "rpgml.tab.cc"
	break;
      case 110: /* "sequence" */

/* Line 480 of lalr1.cc  */
#line 73 "rpgml.yy"
	{ delete ((yyvaluep->seq)); };

/* Line 480 of lalr1.cc  */
#line 728 "rpgml.tab.cc"
	break;
      case 111: /* "iteration_statement" */

/* Line 480 of lalr1.cc  */
#line 71 "rpgml.yy"
	{ delete ((yyvaluep->stmt)); };

/* Line 480 of lalr1.cc  */
#line 737 "rpgml.tab.cc"
	break;
      case 112: /* "function_argument_decl" */

/* Line 480 of lalr1.cc  */
#line 74 "rpgml.yy"
	{ delete ((yyvaluep->fad)); };

/* Line 480 of lalr1.cc  */
#line 746 "rpgml.tab.cc"
	break;
      case 113: /* "function_argument_decl_list" */

/* Line 480 of lalr1.cc  */
#line 75 "rpgml.yy"
	{ delete ((yyvaluep->fadl)); };

/* Line 480 of lalr1.cc  */
#line 755 "rpgml.tab.cc"
	break;
      case 114: /* "function_definition_statement" */

/* Line 480 of lalr1.cc  */
#line 71 "rpgml.yy"
	{ delete ((yyvaluep->stmt)); };

/* Line 480 of lalr1.cc  */
#line 764 "rpgml.tab.cc"
	break;
      case 115: /* "function_call_statement" */

/* Line 480 of lalr1.cc  */
#line 71 "rpgml.yy"
	{ delete ((yyvaluep->stmt)); };

/* Line 480 of lalr1.cc  */
#line 773 "rpgml.tab.cc"
	break;
      case 116: /* "variable_creation_statement" */

/* Line 480 of lalr1.cc  */
#line 71 "rpgml.yy"
	{ delete ((yyvaluep->stmt)); };

/* Line 480 of lalr1.cc  */
#line 782 "rpgml.tab.cc"
	break;
      case 117: /* "return_statement" */

/* Line 480 of lalr1.cc  */
#line 71 "rpgml.yy"
	{ delete ((yyvaluep->stmt)); };

/* Line 480 of lalr1.cc  */
#line 791 "rpgml.tab.cc"
	break;

	default:
	  break;
      }
  }

  void
  _Parser::yypop_ (unsigned int n)
  {
    yystate_stack_.pop (n);
    yysemantic_stack_.pop (n);
    yylocation_stack_.pop (n);
  }

#if YYDEBUG
  std::ostream&
  _Parser::debug_stream () const
  {
    return *yycdebug_;
  }

  void
  _Parser::set_debug_stream (std::ostream& o)
  {
    yycdebug_ = &o;
  }


  _Parser::debug_level_type
  _Parser::debug_level () const
  {
    return yydebug_;
  }

  void
  _Parser::set_debug_level (debug_level_type l)
  {
    yydebug_ = l;
  }
#endif

  inline bool
  _Parser::yy_pact_value_is_default_ (int yyvalue)
  {
    return yyvalue == yypact_ninf_;
  }

  inline bool
  _Parser::yy_table_value_is_error_ (int yyvalue)
  {
    return yyvalue == yytable_ninf_;
  }

  int
  _Parser::parse ()
  {
    /// Lookahead and lookahead in internal form.
    int yychar = yyempty_;
    int yytoken = 0;

    /* State.  */
    int yyn;
    int yylen = 0;
    int yystate = 0;

    /* Error handling.  */
    int yynerrs_ = 0;
    int yyerrstatus_ = 0;

    /// Semantic value of the lookahead.
    semantic_type yylval;
    /// Location of the lookahead.
    location_type yylloc;
    /// The locations where the error started and ended.
    location_type yyerror_range[3];

    /// $$.
    semantic_type yyval;
    /// @$.
    location_type yyloc;

    int yyresult;

    YYCDEBUG << "Starting parse" << std::endl;


    /* User initialization code.  */
    
/* Line 565 of lalr1.cc  */
#line 24 "rpgml.yy"
{
// Initialize the initial location.
yylloc.begin.filename = yylloc.end.filename = scanner.filename;
}

/* Line 565 of lalr1.cc  */
#line 889 "rpgml.tab.cc"

    /* Initialize the stacks.  The initial state will be pushed in
       yynewstate, since the latter expects the semantical and the
       location values to have been already stored, initialize these
       stacks with a primary value.  */
    yystate_stack_ = state_stack_type (0);
    yysemantic_stack_ = semantic_stack_type (0);
    yylocation_stack_ = location_stack_type (0);
    yysemantic_stack_.push (yylval);
    yylocation_stack_.push (yylloc);

    /* New state.  */
  yynewstate:
    yystate_stack_.push (yystate);
    YYCDEBUG << "Entering state " << yystate << std::endl;

    /* Accept?  */
    if (yystate == yyfinal_)
      goto yyacceptlab;

    goto yybackup;

    /* Backup.  */
  yybackup:

    /* Try to take a decision without lookahead.  */
    yyn = yypact_[yystate];
    if (yy_pact_value_is_default_ (yyn))
      goto yydefault;

    /* Read a lookahead token.  */
    if (yychar == yyempty_)
      {
	YYCDEBUG << "Reading a token: ";
	yychar = yylex (&yylval, &yylloc, scanner);
      }


    /* Convert token to internal form.  */
    if (yychar <= yyeof_)
      {
	yychar = yytoken = yyeof_;
	YYCDEBUG << "Now at end of input." << std::endl;
      }
    else
      {
	yytoken = yytranslate_ (yychar);
	YY_SYMBOL_PRINT ("Next token is", yytoken, &yylval, &yylloc);
      }

    /* If the proper action on seeing token YYTOKEN is to reduce or to
       detect an error, take that action.  */
    yyn += yytoken;
    if (yyn < 0 || yylast_ < yyn || yycheck_[yyn] != yytoken)
      goto yydefault;

    /* Reduce or error.  */
    yyn = yytable_[yyn];
    if (yyn <= 0)
      {
	if (yy_table_value_is_error_ (yyn))
	  goto yyerrlab;
	yyn = -yyn;
	goto yyreduce;
      }

    /* Shift the lookahead token.  */
    YY_SYMBOL_PRINT ("Shifting", yytoken, &yylval, &yylloc);

    /* Discard the token being shifted.  */
    yychar = yyempty_;

    yysemantic_stack_.push (yylval);
    yylocation_stack_.push (yylloc);

    /* Count tokens shifted since error; after three, turn off error
       status.  */
    if (yyerrstatus_)
      --yyerrstatus_;

    yystate = yyn;
    goto yynewstate;

  /*-----------------------------------------------------------.
  | yydefault -- do the default action for the current state.  |
  `-----------------------------------------------------------*/
  yydefault:
    yyn = yydefact_[yystate];
    if (yyn == 0)
      goto yyerrlab;
    goto yyreduce;

  /*-----------------------------.
  | yyreduce -- Do a reduction.  |
  `-----------------------------*/
  yyreduce:
    yylen = yyr2_[yyn];
    /* If YYLEN is nonzero, implement the default value of the action:
       `$$ = $1'.  Otherwise, use the top of the stack.

       Otherwise, the following line sets YYVAL to garbage.
       This behavior is undocumented and Bison
       users should not rely upon it.  */
    if (yylen)
      yyval = yysemantic_stack_[yylen - 1];
    else
      yyval = yysemantic_stack_[0];

    {
      slice<location_type, location_stack_type> slice (yylocation_stack_, yylen);
      YYLLOC_DEFAULT (yyloc, slice, yylen);
    }
    YY_REDUCE_PRINT (yyn);
    switch (yyn)
      {
	  case 2:

/* Line 690 of lalr1.cc  */
#line 176 "rpgml.yy"
    { ((yyval.str)) = ((yysemantic_stack_[(1) - (1)].str)); }
    break;

  case 3:

/* Line 690 of lalr1.cc  */
#line 177 "rpgml.yy"
    { ((yyval.str)) = scanner.unify( "in" ); }
    break;

  case 4:

/* Line 690 of lalr1.cc  */
#line 178 "rpgml.yy"
    { ((yyval.str)) = scanner.unify( "to" ); }
    break;

  case 5:

/* Line 690 of lalr1.cc  */
#line 179 "rpgml.yy"
    { ((yyval.str)) = scanner.unify( "step" ); }
    break;

  case 6:

/* Line 690 of lalr1.cc  */
#line 183 "rpgml.yy"
    { ((yyval.expr)) = new ArrayConstantExpression( RPGML_LOC((yyloc)) ); }
    break;

  case 7:

/* Line 690 of lalr1.cc  */
#line 184 "rpgml.yy"
    { ((yyval.expr)) = new ArrayConstantExpression( RPGML_LOC((yyloc)), ((yysemantic_stack_[(3) - (2)].seq)) ); }
    break;

  case 8:

/* Line 690 of lalr1.cc  */
#line 188 "rpgml.yy"
    { ((yyval.expr)) = new MapConstantExpression( RPGML_LOC((yyloc)), ((yysemantic_stack_[(1) - (1)].comp)) ); }
    break;

  case 9:

/* Line 690 of lalr1.cc  */
#line 192 "rpgml.yy"
    { ((yyval.expr)) = new LookupVariableExpression( RPGML_LOC((yyloc)), ((yysemantic_stack_[(1) - (1)].str)) ); }
    break;

  case 10:

/* Line 690 of lalr1.cc  */
#line 193 "rpgml.yy"
    { ((yyval.expr)) = ((yysemantic_stack_[(1) - (1)].expr)); }
    break;

  case 11:

/* Line 690 of lalr1.cc  */
#line 194 "rpgml.yy"
    { ((yyval.expr)) = ((yysemantic_stack_[(3) - (2)].expr)); }
    break;

  case 12:

/* Line 690 of lalr1.cc  */
#line 195 "rpgml.yy"
    { ((yyval.expr)) = new ParenthisSequenceExpression( RPGML_LOC((yyloc)), ((yysemantic_stack_[(3) - (2)].seq)) ); }
    break;

  case 13:

/* Line 690 of lalr1.cc  */
#line 199 "rpgml.yy"
    { ((yyval.expr)) = new ConstantExpression( RPGML_LOC((yyloc)), ((yysemantic_stack_[(1) - (1)].ival)) ); }
    break;

  case 14:

/* Line 690 of lalr1.cc  */
#line 200 "rpgml.yy"
    { ((yyval.expr)) = new ConstantExpression( RPGML_LOC((yyloc)), ((yysemantic_stack_[(1) - (1)].fval)) ); }
    break;

  case 15:

/* Line 690 of lalr1.cc  */
#line 201 "rpgml.yy"
    { ((yyval.expr)) = new ConstantExpression( RPGML_LOC((yyloc)), ((yysemantic_stack_[(1) - (1)].str)) ); }
    break;

  case 16:

/* Line 690 of lalr1.cc  */
#line 202 "rpgml.yy"
    { ((yyval.expr)) = new ConstantExpression( RPGML_LOC((yyloc)), true ); }
    break;

  case 17:

/* Line 690 of lalr1.cc  */
#line 203 "rpgml.yy"
    { ((yyval.expr)) = new ConstantExpression( RPGML_LOC((yyloc)), false ); }
    break;

  case 20:

/* Line 690 of lalr1.cc  */
#line 210 "rpgml.yy"
    {
      ((yyval.expr)) = new FunctionCallExpression( RPGML_LOC((yyloc)), ((yysemantic_stack_[(3) - (1)].expr)), new FunctionCallExpression::Args() );
    }
    break;

  case 21:

/* Line 690 of lalr1.cc  */
#line 214 "rpgml.yy"
    {
      ((yyval.expr)) = new FunctionCallExpression( RPGML_LOC((yyloc)), ((yysemantic_stack_[(4) - (1)].expr)), ((yysemantic_stack_[(4) - (3)].args)) );
    }
    break;

  case 22:

/* Line 690 of lalr1.cc  */
#line 222 "rpgml.yy"
    {
      ((yyval.expr)) = new AccessExpression( RPGML_LOC((yyloc)), ((yysemantic_stack_[(4) - (1)].expr)), ((yysemantic_stack_[(4) - (3)].expr)) );
    }
    break;

  case 23:

/* Line 690 of lalr1.cc  */
#line 226 "rpgml.yy"
    {
      ((yyval.expr)) = new AccessExpression( RPGML_LOC((yyloc)), ((yysemantic_stack_[(3) - (1)].expr)), new ConstantExpression( RPGML_LOC((yyloc)), ((yysemantic_stack_[(3) - (3)].str)) ) );
    }
    break;

  case 24:

/* Line 690 of lalr1.cc  */
#line 229 "rpgml.yy"
    { ((yyval.expr)) = ((yysemantic_stack_[(1) - (1)].expr)); }
    break;

  case 25:

/* Line 690 of lalr1.cc  */
#line 232 "rpgml.yy"
    { ((yyval.expr)) = ((yysemantic_stack_[(1) - (1)].expr)); }
    break;

  case 26:

/* Line 690 of lalr1.cc  */
#line 237 "rpgml.yy"
    {
      ((yyval.arg)) = new FunctionCallExpression::Arg( RPGML_LOC((yyloc)), ((yysemantic_stack_[(1) - (1)].expr)) );
    }
    break;

  case 27:

/* Line 690 of lalr1.cc  */
#line 241 "rpgml.yy"
    {
      ((yyval.arg)) = new FunctionCallExpression::Arg( RPGML_LOC((yyloc)), ((yysemantic_stack_[(3) - (3)].expr)), ((yysemantic_stack_[(3) - (1)].str)) );
    }
    break;

  case 28:

/* Line 690 of lalr1.cc  */
#line 248 "rpgml.yy"
    {
      ((yyval.args)) = new FunctionCallExpression::Args();
      ((yyval.args))->append( ((yysemantic_stack_[(1) - (1)].arg)) );
    }
    break;

  case 29:

/* Line 690 of lalr1.cc  */
#line 253 "rpgml.yy"
    {
      ((yyval.args)) = ((yysemantic_stack_[(3) - (1)].args));
      ((yyval.args))->append( ((yysemantic_stack_[(3) - (3)].arg)) );
    }
    break;

  case 30:

/* Line 690 of lalr1.cc  */
#line 260 "rpgml.yy"
    { ((yyval.expr)) = ((yysemantic_stack_[(1) - (1)].expr)); }
    break;

  case 31:

/* Line 690 of lalr1.cc  */
#line 263 "rpgml.yy"
    { ((yyval.expr)) = new UnaryExpression( RPGML_LOC((yyloc)), ((yysemantic_stack_[(2) - (1)].uop)), ((yysemantic_stack_[(2) - (2)].expr)) ); }
    break;

  case 32:

/* Line 690 of lalr1.cc  */
#line 267 "rpgml.yy"
    { ((yyval.uop)) = UOP_PLUS; }
    break;

  case 33:

/* Line 690 of lalr1.cc  */
#line 268 "rpgml.yy"
    { ((yyval.uop)) = UOP_MINUS; }
    break;

  case 34:

/* Line 690 of lalr1.cc  */
#line 269 "rpgml.yy"
    { ((yyval.uop)) = UOP_BIT_NOT; }
    break;

  case 35:

/* Line 690 of lalr1.cc  */
#line 270 "rpgml.yy"
    { ((yyval.uop)) = UOP_LOG_NOT; }
    break;

  case 36:

/* Line 690 of lalr1.cc  */
#line 274 "rpgml.yy"
    { ((yyval.expr)) = ((yysemantic_stack_[(1) - (1)].expr)); }
    break;

  case 37:

/* Line 690 of lalr1.cc  */
#line 276 "rpgml.yy"
    {
      ((yyval.expr)) = new BinaryExpression( RPGML_LOC((yyloc)), ((yysemantic_stack_[(3) - (1)].expr)), BOP_MUL, ((yysemantic_stack_[(3) - (3)].expr)) );
    }
    break;

  case 38:

/* Line 690 of lalr1.cc  */
#line 280 "rpgml.yy"
    {
      ((yyval.expr)) = new BinaryExpression( RPGML_LOC((yyloc)), ((yysemantic_stack_[(3) - (1)].expr)), BOP_DIV, ((yysemantic_stack_[(3) - (3)].expr)) );
    }
    break;

  case 39:

/* Line 690 of lalr1.cc  */
#line 284 "rpgml.yy"
    {
      ((yyval.expr)) = new BinaryExpression( RPGML_LOC((yyloc)), ((yysemantic_stack_[(3) - (1)].expr)), BOP_MOD, ((yysemantic_stack_[(3) - (3)].expr)) );
    }
    break;

  case 40:

/* Line 690 of lalr1.cc  */
#line 290 "rpgml.yy"
    { ((yyval.expr)) = ((yysemantic_stack_[(1) - (1)].expr)); }
    break;

  case 41:

/* Line 690 of lalr1.cc  */
#line 292 "rpgml.yy"
    {
      ((yyval.expr)) = new BinaryExpression( RPGML_LOC((yyloc)), ((yysemantic_stack_[(3) - (1)].expr)), BOP_ADD, ((yysemantic_stack_[(3) - (3)].expr)) );
    }
    break;

  case 42:

/* Line 690 of lalr1.cc  */
#line 296 "rpgml.yy"
    {
      ((yyval.expr)) = new BinaryExpression( RPGML_LOC((yyloc)), ((yysemantic_stack_[(3) - (1)].expr)), BOP_SUB, ((yysemantic_stack_[(3) - (3)].expr)) );
    }
    break;

  case 43:

/* Line 690 of lalr1.cc  */
#line 302 "rpgml.yy"
    { ((yyval.expr)) = ((yysemantic_stack_[(1) - (1)].expr)); }
    break;

  case 44:

/* Line 690 of lalr1.cc  */
#line 304 "rpgml.yy"
    {
      ((yyval.expr)) = new BinaryExpression( RPGML_LOC((yyloc)), ((yysemantic_stack_[(3) - (1)].expr)), BOP_LEFT, ((yysemantic_stack_[(3) - (3)].expr)) );
    }
    break;

  case 45:

/* Line 690 of lalr1.cc  */
#line 308 "rpgml.yy"
    {
      ((yyval.expr)) = new BinaryExpression( RPGML_LOC((yyloc)), ((yysemantic_stack_[(3) - (1)].expr)), BOP_RIGHT, ((yysemantic_stack_[(3) - (3)].expr)) );
    }
    break;

  case 46:

/* Line 690 of lalr1.cc  */
#line 314 "rpgml.yy"
    { ((yyval.expr)) = ((yysemantic_stack_[(1) - (1)].expr)); }
    break;

  case 47:

/* Line 690 of lalr1.cc  */
#line 316 "rpgml.yy"
    {
      ((yyval.expr)) = new BinaryExpression( RPGML_LOC((yyloc)), ((yysemantic_stack_[(3) - (1)].expr)), BOP_LT, ((yysemantic_stack_[(3) - (3)].expr)) );
    }
    break;

  case 48:

/* Line 690 of lalr1.cc  */
#line 320 "rpgml.yy"
    {
      ((yyval.expr)) = new BinaryExpression( RPGML_LOC((yyloc)), ((yysemantic_stack_[(3) - (1)].expr)), BOP_GT, ((yysemantic_stack_[(3) - (3)].expr)) );
    }
    break;

  case 49:

/* Line 690 of lalr1.cc  */
#line 324 "rpgml.yy"
    {
      ((yyval.expr)) = new BinaryExpression( RPGML_LOC((yyloc)), ((yysemantic_stack_[(3) - (1)].expr)), BOP_LE, ((yysemantic_stack_[(3) - (3)].expr)) );
    }
    break;

  case 50:

/* Line 690 of lalr1.cc  */
#line 328 "rpgml.yy"
    {
      ((yyval.expr)) = new BinaryExpression( RPGML_LOC((yyloc)), ((yysemantic_stack_[(3) - (1)].expr)), BOP_GE, ((yysemantic_stack_[(3) - (3)].expr)) );
    }
    break;

  case 51:

/* Line 690 of lalr1.cc  */
#line 334 "rpgml.yy"
    { ((yyval.expr)) = ((yysemantic_stack_[(1) - (1)].expr)); }
    break;

  case 52:

/* Line 690 of lalr1.cc  */
#line 336 "rpgml.yy"
    {
      ((yyval.expr)) = new BinaryExpression( RPGML_LOC((yyloc)), ((yysemantic_stack_[(3) - (1)].expr)), BOP_EQ, ((yysemantic_stack_[(3) - (3)].expr)) );
    }
    break;

  case 53:

/* Line 690 of lalr1.cc  */
#line 340 "rpgml.yy"
    {
      ((yyval.expr)) = new BinaryExpression( RPGML_LOC((yyloc)), ((yysemantic_stack_[(3) - (1)].expr)), BOP_NE, ((yysemantic_stack_[(3) - (3)].expr)) );
    }
    break;

  case 54:

/* Line 690 of lalr1.cc  */
#line 346 "rpgml.yy"
    { ((yyval.expr)) = ((yysemantic_stack_[(1) - (1)].expr)); }
    break;

  case 55:

/* Line 690 of lalr1.cc  */
#line 348 "rpgml.yy"
    {
      ((yyval.expr)) = new BinaryExpression( RPGML_LOC((yyloc)), ((yysemantic_stack_[(3) - (1)].expr)), BOP_BIT_AND, ((yysemantic_stack_[(3) - (3)].expr)) );
    }
    break;

  case 56:

/* Line 690 of lalr1.cc  */
#line 354 "rpgml.yy"
    { ((yyval.expr)) = ((yysemantic_stack_[(1) - (1)].expr)); }
    break;

  case 57:

/* Line 690 of lalr1.cc  */
#line 356 "rpgml.yy"
    {
      ((yyval.expr)) = new BinaryExpression( RPGML_LOC((yyloc)), ((yysemantic_stack_[(3) - (1)].expr)), BOP_BIT_XOR, ((yysemantic_stack_[(3) - (3)].expr)) );
    }
    break;

  case 58:

/* Line 690 of lalr1.cc  */
#line 362 "rpgml.yy"
    { ((yyval.expr)) = ((yysemantic_stack_[(1) - (1)].expr)); }
    break;

  case 59:

/* Line 690 of lalr1.cc  */
#line 364 "rpgml.yy"
    {
      ((yyval.expr)) = new BinaryExpression( RPGML_LOC((yyloc)), ((yysemantic_stack_[(3) - (1)].expr)), BOP_BIT_OR, ((yysemantic_stack_[(3) - (3)].expr)) );
    }
    break;

  case 60:

/* Line 690 of lalr1.cc  */
#line 370 "rpgml.yy"
    { ((yyval.expr)) = ((yysemantic_stack_[(1) - (1)].expr)); }
    break;

  case 61:

/* Line 690 of lalr1.cc  */
#line 372 "rpgml.yy"
    {
      ((yyval.expr)) = new BinaryExpression( RPGML_LOC((yyloc)), ((yysemantic_stack_[(3) - (1)].expr)), BOP_LOG_AND, ((yysemantic_stack_[(3) - (3)].expr)) );
    }
    break;

  case 62:

/* Line 690 of lalr1.cc  */
#line 378 "rpgml.yy"
    { ((yyval.expr)) = ((yysemantic_stack_[(1) - (1)].expr)); }
    break;

  case 63:

/* Line 690 of lalr1.cc  */
#line 380 "rpgml.yy"
    {
      ((yyval.expr)) = new BinaryExpression( RPGML_LOC((yyloc)), ((yysemantic_stack_[(3) - (1)].expr)), BOP_LOG_XOR, ((yysemantic_stack_[(3) - (3)].expr)) );
    }
    break;

  case 64:

/* Line 690 of lalr1.cc  */
#line 386 "rpgml.yy"
    { ((yyval.expr)) = ((yysemantic_stack_[(1) - (1)].expr)); }
    break;

  case 65:

/* Line 690 of lalr1.cc  */
#line 388 "rpgml.yy"
    {
      ((yyval.expr)) = new BinaryExpression( RPGML_LOC((yyloc)), ((yysemantic_stack_[(3) - (1)].expr)), BOP_LOG_OR, ((yysemantic_stack_[(3) - (3)].expr)) );
    }
    break;

  case 66:

/* Line 690 of lalr1.cc  */
#line 394 "rpgml.yy"
    { ((yyval.expr)) = ((yysemantic_stack_[(1) - (1)].expr)); }
    break;

  case 67:

/* Line 690 of lalr1.cc  */
#line 396 "rpgml.yy"
    {
      ((yyval.expr)) = new IfThenElseExpression( RPGML_LOC((yyloc)), ((yysemantic_stack_[(5) - (1)].expr)), ((yysemantic_stack_[(5) - (3)].expr)), ((yysemantic_stack_[(5) - (5)].expr)) );
    }
    break;

  case 68:

/* Line 690 of lalr1.cc  */
#line 402 "rpgml.yy"
    { ((yyval.expr)) = ((yysemantic_stack_[(1) - (1)].expr)); }
    break;

  case 69:

/* Line 690 of lalr1.cc  */
#line 407 "rpgml.yy"
    {
      ((yyval.stmt)) = new AssignIdentifierStatement( RPGML_LOC((yyloc)), ((yysemantic_stack_[(4) - (1)].str)), ((yysemantic_stack_[(4) - (2)].assign)), ((yysemantic_stack_[(4) - (3)].expr)) );
    }
    break;

  case 70:

/* Line 690 of lalr1.cc  */
#line 411 "rpgml.yy"
    {
      ((yyval.stmt)) = new AssignDotStatement( RPGML_LOC((yyloc)), ((yysemantic_stack_[(6) - (1)].expr)), ((yysemantic_stack_[(6) - (3)].str)), ((yysemantic_stack_[(6) - (4)].assign)), ((yysemantic_stack_[(6) - (5)].expr)) );
    }
    break;

  case 71:

/* Line 690 of lalr1.cc  */
#line 415 "rpgml.yy"
    {
      ((yyval.stmt)) = new AssignBracketStatement( RPGML_LOC((yyloc)), ((yysemantic_stack_[(7) - (1)].expr)), ((yysemantic_stack_[(7) - (3)].expr)), ((yysemantic_stack_[(7) - (5)].assign)), ((yysemantic_stack_[(7) - (6)].expr)) );
    }
    break;

  case 72:

/* Line 690 of lalr1.cc  */
#line 421 "rpgml.yy"
    { ((yyval.assign)) = ID_ASSIGN; }
    break;

  case 92:

/* Line 690 of lalr1.cc  */
#line 448 "rpgml.yy"
    { ((yyval.stmt)) = ((yysemantic_stack_[(1) - (1)].comp)); }
    break;

  case 100:

/* Line 690 of lalr1.cc  */
#line 456 "rpgml.yy"
    { ((yyval.stmt)) = new NOPStatement( RPGML_LOC((yyloc)) ); }
    break;

  case 101:

/* Line 690 of lalr1.cc  */
#line 457 "rpgml.yy"
    { ((yyval.stmt)) = new NOPStatement( RPGML_LOC((yyloc)) ); }
    break;

  case 102:

/* Line 690 of lalr1.cc  */
#line 462 "rpgml.yy"
    {
      ((yyval.comp)) = new CompoundStatement( RPGML_LOC((yyloc)) );
      ((yyval.comp))->append( ((yysemantic_stack_[(1) - (1)].stmt)) );
    }
    break;

  case 103:

/* Line 690 of lalr1.cc  */
#line 467 "rpgml.yy"
    {
      ((yyval.comp)) = ((yysemantic_stack_[(2) - (1)].comp));
      ((yyval.comp))->append( ((yysemantic_stack_[(2) - (2)].stmt)) );
    }
    break;

  case 104:

/* Line 690 of lalr1.cc  */
#line 472 "rpgml.yy"
    {
      ((yyval.comp)) = ((yysemantic_stack_[(3) - (1)].comp));
    }
    break;

  case 105:

/* Line 690 of lalr1.cc  */
#line 476 "rpgml.yy"
    {
      ((yyval.comp)) = new CompoundStatement( RPGML_LOC((yyloc)) );
    }
    break;

  case 106:

/* Line 690 of lalr1.cc  */
#line 483 "rpgml.yy"
    {
      ((yyval.comp)) = new CompoundStatement( RPGML_LOC((yyloc)) );
    }
    break;

  case 107:

/* Line 690 of lalr1.cc  */
#line 487 "rpgml.yy"
    {
      ((yyval.comp)) = ((yysemantic_stack_[(3) - (2)].comp));
    }
    break;

  case 108:

/* Line 690 of lalr1.cc  */
#line 491 "rpgml.yy"
    {
      ((yyval.comp)) = new CompoundStatement( RPGML_LOC((yyloc)) );
    }
    break;

  case 109:

/* Line 690 of lalr1.cc  */
#line 498 "rpgml.yy"
    {
      ((yyval.stmt)) = new IfStatement( RPGML_LOC((yyloc)), ((yysemantic_stack_[(7) - (3)].expr)), ((yysemantic_stack_[(7) - (5)].stmt)), ((yysemantic_stack_[(7) - (7)].stmt)) );
    }
    break;

  case 110:

/* Line 690 of lalr1.cc  */
#line 502 "rpgml.yy"
    {
      ((yyval.stmt)) = new IfStatement( RPGML_LOC((yyloc)), ((yysemantic_stack_[(5) - (3)].expr)), ((yysemantic_stack_[(5) - (5)].stmt)) );
    }
    break;

  case 111:

/* Line 690 of lalr1.cc  */
#line 509 "rpgml.yy"
    {
      ((yyval.expr_seq)) = new ExpressionSequenceExpression( RPGML_LOC((yyloc)) );
      ((yyval.expr_seq))->append( ((yysemantic_stack_[(1) - (1)].expr)) );
    }
    break;

  case 112:

/* Line 690 of lalr1.cc  */
#line 514 "rpgml.yy"
    {
      ((yyval.expr_seq)) = ((yysemantic_stack_[(3) - (1)].expr_seq));
      ((yyval.expr_seq))->append( ((yysemantic_stack_[(3) - (3)].expr)) );
    }
    break;

  case 113:

/* Line 690 of lalr1.cc  */
#line 522 "rpgml.yy"
    {
      ((yyval.seq)) = new FromToStepSequenceExpression( RPGML_LOC((yyloc)), ((yysemantic_stack_[(3) - (1)].expr)), ((yysemantic_stack_[(3) - (3)].expr)) );
    }
    break;

  case 114:

/* Line 690 of lalr1.cc  */
#line 526 "rpgml.yy"
    {
      ((yyval.seq)) = new FromToStepSequenceExpression( RPGML_LOC((yyloc)), ((yysemantic_stack_[(5) - (1)].expr)), ((yysemantic_stack_[(5) - (3)].expr)), ((yysemantic_stack_[(5) - (5)].expr)) );
    }
    break;

  case 115:

/* Line 690 of lalr1.cc  */
#line 530 "rpgml.yy"
    {
      ((yyval.seq)) = ((yysemantic_stack_[(1) - (1)].expr_seq));
    }
    break;

  case 116:

/* Line 690 of lalr1.cc  */
#line 537 "rpgml.yy"
    {
      ((yyval.seq)) = ((yysemantic_stack_[(1) - (1)].seq));
    }
    break;

  case 117:

/* Line 690 of lalr1.cc  */
#line 544 "rpgml.yy"
    {
      ((yyval.stmt)) = new ForContainerStatement( RPGML_LOC((yyloc)), ((yysemantic_stack_[(5) - (2)].str)), ((yysemantic_stack_[(5) - (4)].expr)), ((yysemantic_stack_[(5) - (5)].stmt)) );
    }
    break;

  case 118:

/* Line 690 of lalr1.cc  */
#line 548 "rpgml.yy"
    {
      ((yyval.stmt)) = new ForSequenceStatement( RPGML_LOC((yyloc)), ((yysemantic_stack_[(5) - (2)].str)), ((yysemantic_stack_[(5) - (4)].seq)), ((yysemantic_stack_[(5) - (5)].stmt)) );
    }
    break;

  case 119:

/* Line 690 of lalr1.cc  */
#line 555 "rpgml.yy"
    {
      ((yyval.fad)) = new FunctionDefinitionStatement::ArgDecl( RPGML_LOC((yyloc)), ((yysemantic_stack_[(1) - (1)].str)) );
    }
    break;

  case 120:

/* Line 690 of lalr1.cc  */
#line 559 "rpgml.yy"
    {
      ((yyval.fad)) = new FunctionDefinitionStatement::ArgDecl( RPGML_LOC((yyloc)), ((yysemantic_stack_[(3) - (1)].str)), ((yysemantic_stack_[(3) - (3)].expr)) );
    }
    break;

  case 121:

/* Line 690 of lalr1.cc  */
#line 566 "rpgml.yy"
    {
      ((yyval.fadl)) = new FunctionDefinitionStatement::ArgDeclList();
      ((yyval.fadl))->append( ((yysemantic_stack_[(1) - (1)].fad)) );
    }
    break;

  case 122:

/* Line 690 of lalr1.cc  */
#line 571 "rpgml.yy"
    {
      ((yyval.fadl)) = ((yysemantic_stack_[(3) - (1)].fadl));
      ((yyval.fadl))->append( ((yysemantic_stack_[(3) - (3)].fad)) );
    }
    break;

  case 123:

/* Line 690 of lalr1.cc  */
#line 579 "rpgml.yy"
    {
      ((yyval.stmt)) = new FunctionDefinitionStatement( RPGML_LOC((yyloc)), ((yysemantic_stack_[(5) - (2)].str)), new FunctionDefinitionStatement::ArgDeclList(), ((yysemantic_stack_[(5) - (5)].comp)) );
      (void)((yysemantic_stack_[(5) - (1)].type));
    }
    break;

  case 124:

/* Line 690 of lalr1.cc  */
#line 584 "rpgml.yy"
    {
      ((yyval.stmt)) = new FunctionDefinitionStatement( RPGML_LOC((yyloc)), ((yysemantic_stack_[(6) - (2)].str)), ((yysemantic_stack_[(6) - (4)].fadl)), ((yysemantic_stack_[(6) - (6)].comp)) );
      (void)((yysemantic_stack_[(6) - (1)].type));
    }
    break;

  case 125:

/* Line 690 of lalr1.cc  */
#line 589 "rpgml.yy"
    {
      ((yyval.stmt)) = new VariableCreationStatement( RPGML_LOC((yyloc)), ((yysemantic_stack_[(5) - (1)].type)), ((yysemantic_stack_[(5) - (2)].str)), ((yysemantic_stack_[(5) - (4)].expr)) );
    }
    break;

  case 126:

/* Line 690 of lalr1.cc  */
#line 596 "rpgml.yy"
    {
      ((yyval.stmt)) = new FunctionCallStatement( RPGML_LOC((yyloc)), ((yysemantic_stack_[(2) - (1)].expr)) );
    }
    break;

  case 127:

/* Line 690 of lalr1.cc  */
#line 603 "rpgml.yy"
    {
      ((yyval.stmt)) = new VariableCreationStatement( RPGML_LOC((yyloc)), ((yysemantic_stack_[(5) - (1)].type)), ((yysemantic_stack_[(5) - (2)].str)), ((yysemantic_stack_[(5) - (4)].expr)) );
    }
    break;

  case 128:

/* Line 690 of lalr1.cc  */
#line 612 "rpgml.yy"
    {
      ((yyval.stmt)) = new ReturnStatement( RPGML_LOC((yyloc)), ((yysemantic_stack_[(3) - (2)].expr)) );
    }
    break;

  case 129:

/* Line 690 of lalr1.cc  */
#line 619 "rpgml.yy"
    {
      scanner.append( ((yysemantic_stack_[(1) - (1)].stmt)) );
    }
    break;

  case 130:

/* Line 690 of lalr1.cc  */
#line 623 "rpgml.yy"
    {
      scanner.append( ((yysemantic_stack_[(2) - (2)].stmt)) );
    }
    break;



/* Line 690 of lalr1.cc  */
#line 1845 "rpgml.tab.cc"
	default:
          break;
      }
    /* User semantic actions sometimes alter yychar, and that requires
       that yytoken be updated with the new translation.  We take the
       approach of translating immediately before every use of yytoken.
       One alternative is translating here after every semantic action,
       but that translation would be missed if the semantic action
       invokes YYABORT, YYACCEPT, or YYERROR immediately after altering
       yychar.  In the case of YYABORT or YYACCEPT, an incorrect
       destructor might then be invoked immediately.  In the case of
       YYERROR, subsequent parser actions might lead to an incorrect
       destructor call or verbose syntax error message before the
       lookahead is translated.  */
    YY_SYMBOL_PRINT ("-> $$ =", yyr1_[yyn], &yyval, &yyloc);

    yypop_ (yylen);
    yylen = 0;
    YY_STACK_PRINT ();

    yysemantic_stack_.push (yyval);
    yylocation_stack_.push (yyloc);

    /* Shift the result of the reduction.  */
    yyn = yyr1_[yyn];
    yystate = yypgoto_[yyn - yyntokens_] + yystate_stack_[0];
    if (0 <= yystate && yystate <= yylast_
	&& yycheck_[yystate] == yystate_stack_[0])
      yystate = yytable_[yystate];
    else
      yystate = yydefgoto_[yyn - yyntokens_];
    goto yynewstate;

  /*------------------------------------.
  | yyerrlab -- here on detecting error |
  `------------------------------------*/
  yyerrlab:
    /* Make sure we have latest lookahead translation.  See comments at
       user semantic actions for why this is necessary.  */
    yytoken = yytranslate_ (yychar);

    /* If not already recovering from an error, report this error.  */
    if (!yyerrstatus_)
      {
	++yynerrs_;
	if (yychar == yyempty_)
	  yytoken = yyempty_;
	error (yylloc, yysyntax_error_ (yystate, yytoken));
      }

    yyerror_range[1] = yylloc;
    if (yyerrstatus_ == 3)
      {
	/* If just tried and failed to reuse lookahead token after an
	 error, discard it.  */

	if (yychar <= yyeof_)
	  {
	  /* Return failure if at end of input.  */
	  if (yychar == yyeof_)
	    YYABORT;
	  }
	else
	  {
	    yydestruct_ ("Error: discarding", yytoken, &yylval, &yylloc);
	    yychar = yyempty_;
	  }
      }

    /* Else will try to reuse lookahead token after shifting the error
       token.  */
    goto yyerrlab1;


  /*---------------------------------------------------.
  | yyerrorlab -- error raised explicitly by YYERROR.  |
  `---------------------------------------------------*/
  yyerrorlab:

    /* Pacify compilers like GCC when the user code never invokes
       YYERROR and the label yyerrorlab therefore never appears in user
       code.  */
    if (false)
      goto yyerrorlab;

    yyerror_range[1] = yylocation_stack_[yylen - 1];
    /* Do not reclaim the symbols of the rule which action triggered
       this YYERROR.  */
    yypop_ (yylen);
    yylen = 0;
    yystate = yystate_stack_[0];
    goto yyerrlab1;

  /*-------------------------------------------------------------.
  | yyerrlab1 -- common code for both syntax error and YYERROR.  |
  `-------------------------------------------------------------*/
  yyerrlab1:
    yyerrstatus_ = 3;	/* Each real token shifted decrements this.  */

    for (;;)
      {
	yyn = yypact_[yystate];
	if (!yy_pact_value_is_default_ (yyn))
	{
	  yyn += yyterror_;
	  if (0 <= yyn && yyn <= yylast_ && yycheck_[yyn] == yyterror_)
	    {
	      yyn = yytable_[yyn];
	      if (0 < yyn)
		break;
	    }
	}

	/* Pop the current state because it cannot handle the error token.  */
	if (yystate_stack_.height () == 1)
	YYABORT;

	yyerror_range[1] = yylocation_stack_[0];
	yydestruct_ ("Error: popping",
		     yystos_[yystate],
		     &yysemantic_stack_[0], &yylocation_stack_[0]);
	yypop_ ();
	yystate = yystate_stack_[0];
	YY_STACK_PRINT ();
      }

    yyerror_range[2] = yylloc;
    // Using YYLLOC is tempting, but would change the location of
    // the lookahead.  YYLOC is available though.
    YYLLOC_DEFAULT (yyloc, yyerror_range, 2);
    yysemantic_stack_.push (yylval);
    yylocation_stack_.push (yyloc);

    /* Shift the error token.  */
    YY_SYMBOL_PRINT ("Shifting", yystos_[yyn],
		     &yysemantic_stack_[0], &yylocation_stack_[0]);

    yystate = yyn;
    goto yynewstate;

    /* Accept.  */
  yyacceptlab:
    yyresult = 0;
    goto yyreturn;

    /* Abort.  */
  yyabortlab:
    yyresult = 1;
    goto yyreturn;

  yyreturn:
    if (yychar != yyempty_)
      {
        /* Make sure we have latest lookahead translation.  See comments
           at user semantic actions for why this is necessary.  */
        yytoken = yytranslate_ (yychar);
        yydestruct_ ("Cleanup: discarding lookahead", yytoken, &yylval,
                     &yylloc);
      }

    /* Do not reclaim the symbols of the rule which action triggered
       this YYABORT or YYACCEPT.  */
    yypop_ (yylen);
    while (yystate_stack_.height () != 1)
      {
	yydestruct_ ("Cleanup: popping",
		   yystos_[yystate_stack_[0]],
		   &yysemantic_stack_[0],
		   &yylocation_stack_[0]);
	yypop_ ();
      }

    return yyresult;
  }

  // Generate an error message.
  std::string
  _Parser::yysyntax_error_ (int yystate, int yytoken)
  {
    std::string yyres;
    // Number of reported tokens (one for the "unexpected", one per
    // "expected").
    size_t yycount = 0;
    // Its maximum.
    enum { YYERROR_VERBOSE_ARGS_MAXIMUM = 5 };
    // Arguments of yyformat.
    char const *yyarg[YYERROR_VERBOSE_ARGS_MAXIMUM];

    /* There are many possibilities here to consider:
       - If this state is a consistent state with a default action, then
         the only way this function was invoked is if the default action
         is an error action.  In that case, don't check for expected
         tokens because there are none.
       - The only way there can be no lookahead present (in yytoken) is
         if this state is a consistent state with a default action.
         Thus, detecting the absence of a lookahead is sufficient to
         determine that there is no unexpected or expected token to
         report.  In that case, just report a simple "syntax error".
       - Don't assume there isn't a lookahead just because this state is
         a consistent state with a default action.  There might have
         been a previous inconsistent state, consistent state with a
         non-default action, or user semantic action that manipulated
         yychar.
       - Of course, the expected token list depends on states to have
         correct lookahead information, and it depends on the parser not
         to perform extra reductions after fetching a lookahead from the
         scanner and before detecting a syntax error.  Thus, state
         merging (from LALR or IELR) and default reductions corrupt the
         expected token list.  However, the list is correct for
         canonical LR with one exception: it will still contain any
         token that will not be accepted due to an error action in a
         later state.
    */
    if (yytoken != yyempty_)
      {
        yyarg[yycount++] = yytname_[yytoken];
        int yyn = yypact_[yystate];
        if (!yy_pact_value_is_default_ (yyn))
          {
            /* Start YYX at -YYN if negative to avoid negative indexes in
               YYCHECK.  In other words, skip the first -YYN actions for
               this state because they are default actions.  */
            int yyxbegin = yyn < 0 ? -yyn : 0;
            /* Stay within bounds of both yycheck and yytname.  */
            int yychecklim = yylast_ - yyn + 1;
            int yyxend = yychecklim < yyntokens_ ? yychecklim : yyntokens_;
            for (int yyx = yyxbegin; yyx < yyxend; ++yyx)
              if (yycheck_[yyx + yyn] == yyx && yyx != yyterror_
                  && !yy_table_value_is_error_ (yytable_[yyx + yyn]))
                {
                  if (yycount == YYERROR_VERBOSE_ARGS_MAXIMUM)
                    {
                      yycount = 1;
                      break;
                    }
                  else
                    yyarg[yycount++] = yytname_[yyx];
                }
          }
      }

    char const* yyformat = 0;
    switch (yycount)
      {
#define YYCASE_(N, S)                         \
        case N:                               \
          yyformat = S;                       \
        break
        YYCASE_(0, YY_("syntax error"));
        YYCASE_(1, YY_("syntax error, unexpected %s"));
        YYCASE_(2, YY_("syntax error, unexpected %s, expecting %s"));
        YYCASE_(3, YY_("syntax error, unexpected %s, expecting %s or %s"));
        YYCASE_(4, YY_("syntax error, unexpected %s, expecting %s or %s or %s"));
        YYCASE_(5, YY_("syntax error, unexpected %s, expecting %s or %s or %s or %s"));
#undef YYCASE_
      }

    // Argument number.
    size_t yyi = 0;
    for (char const* yyp = yyformat; *yyp; ++yyp)
      if (yyp[0] == '%' && yyp[1] == 's' && yyi < yycount)
        {
          yyres += yytnamerr_ (yyarg[yyi++]);
          ++yyp;
        }
      else
        yyres += *yyp;
    return yyres;
  }


  /* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
     STATE-NUM.  */
  const short int _Parser::yypact_ninf_ = -148;
  const short int
  _Parser::yypact_[] =
  {
       231,   -62,  -148,  -148,  -148,  -148,  -148,  -148,  -148,  -148,
    -148,  -148,  -148,    29,  -148,  -148,  -148,   -12,    29,  -148,
    -148,  -148,  -148,  -148,   518,   438,   518,  -148,   283,    93,
    -148,  -148,  -148,  -148,   -49,   -31,  -148,    29,  -148,   -13,
    -148,  -148,  -148,  -148,  -148,  -148,   387,    27,  -148,   -45,
     518,   -35,  -148,  -148,  -148,  -148,  -148,  -148,    23,  -148,
     518,    19,    -9,    24,     1,    38,   -36,   -25,   -11,    31,
      45,   -16,  -148,    -7,  -148,  -148,    13,    18,  -148,    25,
     -39,    35,   -28,  -148,  -148,   335,  -148,  -148,  -148,  -148,
    -148,  -148,  -148,  -148,  -148,  -148,  -148,   518,  -148,   518,
     459,    29,    37,  -148,  -148,    26,   518,    49,   518,   518,
     518,    29,  -148,   518,   518,   518,   518,   518,   518,   518,
     518,   518,   518,   518,   518,   518,   518,   518,   518,   518,
     518,   518,   518,  -148,   518,   518,  -148,  -148,  -148,  -148,
    -148,     7,  -148,  -148,    33,    54,  -148,    51,  -148,   -38,
    -148,    93,   518,    36,    52,  -148,   -29,    50,   387,   387,
     387,    59,  -148,  -148,  -148,  -148,    19,    19,    -9,    -9,
      24,    24,    24,    24,     1,     1,    38,   -36,   -25,   -11,
      31,    45,    53,    79,  -148,  -148,  -148,    93,   518,  -148,
     518,   518,    55,  -148,   518,    36,    29,  -148,    85,  -148,
    -148,  -148,   518,   518,   518,  -148,  -148,    56,  -148,  -148,
    -148,  -148,   387,  -148,  -148,    57,  -148,  -148,  -148
  };

  /* YYDEFACT[S] -- default reduction number in state S.  Performed when
     YYTABLE doesn't specify something else to do.  Zero means the
     default is an error.  */
  const unsigned char
  _Parser::yydefact_[] =
  {
         0,     0,   100,     2,    13,    14,    15,    83,    84,    85,
      86,    87,    88,     0,    89,    90,    91,     0,     0,     3,
       4,     5,    16,    17,     0,     0,     0,   101,     0,     9,
      18,    19,    25,    10,    24,     0,    97,     0,   129,    92,
      93,    94,    95,    96,    98,    99,   132,     0,   131,     0,
       0,     0,    32,    33,    34,    35,     9,    24,    30,    36,
       0,    40,    43,    46,    51,    54,    56,    58,    60,    62,
      64,    66,    68,     0,     8,     6,   111,   115,   116,     0,
     111,     0,     0,   106,   102,     0,    73,    74,    75,    76,
      77,    78,    79,    80,    81,    82,    72,     0,   126,     0,
       0,     0,     0,   130,     1,     0,     0,     0,     0,     0,
       0,     0,    31,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   128,     0,     0,     7,    11,    12,   105,
     108,     0,   107,   103,     0,     0,    20,     9,    28,     0,
      26,    23,     0,     0,   119,   121,     0,     0,     0,     0,
       0,     0,    23,    37,    38,    39,    41,    42,    44,    45,
      49,    50,    47,    48,    52,    53,    55,    57,    59,    61,
      63,    65,     0,   113,   112,   104,    69,    22,     0,    21,
       0,     0,     0,   123,     0,     0,     0,   125,   110,   117,
     118,    22,     0,     0,     0,    27,    29,     0,   127,   120,
     124,   122,     0,    67,   114,     0,    70,   109,    71
  };

  /* YYPGOTO[NTERM-NUM].  */
  const short int
  _Parser::yypgoto_[] =
  {
      -148,     0,  -148,  -148,  -148,  -148,     6,    10,   -63,  -148,
     -53,  -148,   -30,   -26,   -24,   -22,     8,    14,     9,    11,
       5,    12,  -148,   -60,    39,  -148,  -147,  -148,   -27,  -148,
       3,  -148,  -148,   118,    42,  -148,   -50,  -148,  -148,  -148,
    -148,  -148,  -148,  -148
  };

  /* YYDEFGOTO[NTERM-NUM].  */
  const short int
  _Parser::yydefgoto_[] =
  {
        -1,    56,    30,    31,    32,    33,    57,    58,   148,   149,
      59,    60,    61,    62,    63,    64,    65,    66,    67,    68,
      69,    70,    71,    72,    76,    36,    97,    37,    38,    85,
      74,    40,    77,    78,    79,    41,   155,   156,    42,    43,
      44,    45,    46,    47
  };

  /* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
     positive, shift that token.  If negative, reduce the rule which
     number is the opposite.  If YYTABLE_NINF_, syntax error.  */
  const short int _Parser::yytable_ninf_ = -134;
  const short int
  _Parser::yytable_[] =
  {
        29,    84,   131,    39,   191,   134,    34,   112,   108,   105,
      35,    48,   106,    49,   120,   121,   137,   189,    51,   103,
     190,    99,   109,   100,    98,   101,   195,   104,    29,   196,
       3,    39,   126,     3,    34,   118,   119,   102,    35,    -8,
     204,    -8,    50,    -8,   127,   139,    29,   140,   129,    39,
     116,   117,    34,   124,   125,   132,    35,   134,   143,   128,
     163,   164,   165,    73,   130,    80,   133,   122,   123,    19,
      20,    21,    19,    20,    21,   110,   135,   100,   136,   111,
     185,   153,   113,   114,   115,    29,   166,   167,    39,   107,
     138,    34,   168,   169,   152,    35,   170,   171,   172,   173,
     147,   151,   174,   175,   158,   154,   186,   187,   188,   194,
      28,   162,   201,    86,    87,    88,    89,    90,    91,    92,
      93,    94,    95,   197,   203,   202,   212,   206,   208,   216,
     218,   198,   199,   200,   176,   180,   144,   178,   145,   150,
     179,   177,   213,   181,    81,   157,   211,   159,     0,   161,
      96,   160,     0,     0,     0,     0,   193,     0,    29,    29,
      29,    39,    39,    39,    34,    34,    34,     0,    35,    35,
      35,   182,     0,   183,   184,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   217,     0,     0,     0,     0,
     147,   192,     0,     0,     0,     0,   154,     0,   210,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,    29,     0,     0,    39,     0,     0,    34,     0,
       0,     0,    35,     0,     0,     0,     0,   205,     0,   150,
     207,  -133,     1,   209,     2,     3,     4,     5,     6,     0,
       0,     0,   214,   215,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     7,     8,     9,    10,    11,    12,    13,    14,    15,
      16,    17,     0,    18,    19,    20,    21,     0,     0,    22,
      23,     0,    24,    25,    82,    26,     2,     3,     4,     5,
       6,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,    27,    28,     0,     0,     0,     0,
       0,     0,     0,     7,     8,     9,    10,    11,    12,    13,
      14,    15,    16,    17,     0,    18,    19,    20,    21,     0,
       0,    22,    23,     0,    24,    25,   141,    26,     2,     3,
       4,     5,     6,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,    27,    28,    83,     0,
       0,     0,     0,     0,     0,     7,     8,     9,    10,    11,
      12,    13,    14,    15,    16,    17,     0,    18,    19,    20,
      21,     0,     0,    22,    23,     0,    24,    25,     0,    26,
       2,     3,     4,     5,     6,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,    27,    28,
     142,     0,     0,     0,     0,     0,     0,     7,     8,     9,
      10,    11,    12,    13,    14,    15,    16,    17,     0,    18,
      19,    20,    21,     0,     0,    22,    23,     0,    24,    25,
       0,    26,     3,     4,     5,     6,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      27,    28,     0,     3,     4,     5,     6,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,    19,    20,    21,     0,     0,    22,    23,     0,     0,
      25,    75,    26,     0,     0,     0,     0,    52,    53,    54,
      55,     0,    19,    20,    21,     0,     0,    22,    23,     0,
       0,    25,    28,    26,   146,     0,     0,     0,    52,    53,
      54,    55,     3,     4,     5,     6,     0,     0,     0,     0,
       0,     0,     0,    28,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,    19,    20,    21,     0,     0,    22,    23,     0,     0,
      25,     0,    26,     0,     0,     0,     0,    52,    53,    54,
      55,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,    28
  };

  /* YYCHECK.  */
  const short int
  _Parser::yycheck_[] =
  {
         0,    28,    18,     0,   151,    44,     0,    60,    43,    54,
       0,    73,    57,    13,    13,    14,    55,    55,    18,    46,
      58,    52,    57,    54,    73,    56,    55,     0,    28,    58,
       4,    28,    68,     4,    28,    11,    12,    37,    28,    52,
     187,    54,    54,    56,    69,    73,    46,    75,    17,    46,
      59,    60,    46,    15,    16,    71,    46,    44,    85,    70,
     113,   114,   115,    24,    19,    26,    73,    66,    67,    43,
      44,    45,    43,    44,    45,    52,    58,    54,    53,    56,
      73,    55,    63,    64,    65,    85,   116,   117,    85,    50,
      55,    85,   118,   119,    57,    85,   120,   121,   122,   123,
     100,   101,   124,   125,    55,   105,    73,    53,    57,    57,
      74,   111,    53,    20,    21,    22,    23,    24,    25,    26,
      27,    28,    29,    73,    45,    72,    41,   190,    73,    73,
      73,   158,   159,   160,   126,   130,    97,   128,    99,   100,
     129,   127,   202,   131,    26,   106,   196,   108,    -1,   110,
      57,   109,    -1,    -1,    -1,    -1,   153,    -1,   158,   159,
     160,   158,   159,   160,   158,   159,   160,    -1,   158,   159,
     160,   132,    -1,   134,   135,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,   212,    -1,    -1,    -1,    -1,
     190,   152,    -1,    -1,    -1,    -1,   196,    -1,   195,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,   212,    -1,    -1,   212,    -1,    -1,   212,    -1,
      -1,    -1,   212,    -1,    -1,    -1,    -1,   188,    -1,   190,
     191,     0,     1,   194,     3,     4,     5,     6,     7,    -1,
      -1,    -1,   203,   204,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    30,    31,    32,    33,    34,    35,    36,    37,    38,
      39,    40,    -1,    42,    43,    44,    45,    -1,    -1,    48,
      49,    -1,    51,    52,     1,    54,     3,     4,     5,     6,
       7,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    73,    74,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    30,    31,    32,    33,    34,    35,    36,
      37,    38,    39,    40,    -1,    42,    43,    44,    45,    -1,
      -1,    48,    49,    -1,    51,    52,     1,    54,     3,     4,
       5,     6,     7,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    73,    74,    75,    -1,
      -1,    -1,    -1,    -1,    -1,    30,    31,    32,    33,    34,
      35,    36,    37,    38,    39,    40,    -1,    42,    43,    44,
      45,    -1,    -1,    48,    49,    -1,    51,    52,    -1,    54,
       3,     4,     5,     6,     7,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    73,    74,
      75,    -1,    -1,    -1,    -1,    -1,    -1,    30,    31,    32,
      33,    34,    35,    36,    37,    38,    39,    40,    -1,    42,
      43,    44,    45,    -1,    -1,    48,    49,    -1,    51,    52,
      -1,    54,     4,     5,     6,     7,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      73,    74,    -1,     4,     5,     6,     7,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    43,    44,    45,    -1,    -1,    48,    49,    -1,    -1,
      52,    53,    54,    -1,    -1,    -1,    -1,    59,    60,    61,
      62,    -1,    43,    44,    45,    -1,    -1,    48,    49,    -1,
      -1,    52,    74,    54,    55,    -1,    -1,    -1,    59,    60,
      61,    62,     4,     5,     6,     7,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    74,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    43,    44,    45,    -1,    -1,    48,    49,    -1,    -1,
      52,    -1,    54,    -1,    -1,    -1,    -1,    59,    60,    61,
      62,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    74
  };

  /* STOS_[STATE-NUM] -- The (internal number of the) accessing
     symbol of state STATE-NUM.  */
  const unsigned char
  _Parser::yystos_[] =
  {
         0,     1,     3,     4,     5,     6,     7,    30,    31,    32,
      33,    34,    35,    36,    37,    38,    39,    40,    42,    43,
      44,    45,    48,    49,    51,    52,    54,    73,    74,    77,
      78,    79,    80,    81,    82,    83,   101,   103,   104,   106,
     107,   111,   114,   115,   116,   117,   118,   119,    73,    77,
      54,    77,    59,    60,    61,    62,    77,    82,    83,    86,
      87,    88,    89,    90,    91,    92,    93,    94,    95,    96,
      97,    98,    99,   100,   106,    53,   100,   108,   109,   110,
     100,   109,     1,    75,   104,   105,    20,    21,    22,    23,
      24,    25,    26,    27,    28,    29,    57,   102,    73,    52,
      54,    56,    77,   104,     0,    54,    57,   100,    43,    57,
      52,    56,    86,    63,    64,    65,    59,    60,    11,    12,
      13,    14,    66,    67,    15,    16,    68,    69,    70,    17,
      19,    18,    71,    73,    44,    58,    53,    55,    55,    73,
      75,     1,    75,   104,   100,   100,    55,    77,    84,    85,
     100,    77,    57,    55,    77,   112,   113,   100,    55,   100,
     110,   100,    77,    86,    86,    86,    88,    88,    89,    89,
      90,    90,    90,    90,    91,    91,    92,    93,    94,    95,
      96,    97,   100,   100,   100,    73,    73,    53,    57,    55,
      58,   102,   100,   106,    57,    55,    58,    73,   104,   104,
     104,    53,    72,    45,   102,   100,    84,   100,    73,   100,
     106,   112,    41,    99,   100,   100,    73,   104,    73
  };

#if YYDEBUG
  /* TOKEN_NUMBER_[YYLEX-NUM] -- Internal symbol number corresponding
     to YYLEX-NUM.  */
  const unsigned short int
  _Parser::yytoken_number_[] =
  {
         0,   256,   257,   258,   259,   260,   261,   262,   263,   264,
     265,   266,   267,   268,   269,   270,   271,   272,   273,   274,
     275,   276,   277,   278,   279,   280,   281,   282,   283,   284,
     285,   286,   287,   288,   289,   290,   291,   292,   293,   294,
     295,   296,   297,   298,   299,   300,   301,   302,   303,   304,
     305,   306,    91,    93,    40,    41,    46,    61,    44,    43,
      45,   126,    33,    42,    47,    37,    60,    62,    38,    94,
     124,    63,    58,    59,   123,   125
  };
#endif

  /* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
  const unsigned char
  _Parser::yyr1_[] =
  {
         0,    76,    77,    77,    77,    77,    78,    78,    79,    80,
      80,    80,    80,    81,    81,    81,    81,    81,    81,    81,
      82,    82,    83,    83,    83,    83,    84,    84,    85,    85,
      86,    86,    87,    87,    87,    87,    88,    88,    88,    88,
      89,    89,    89,    90,    90,    90,    91,    91,    91,    91,
      91,    92,    92,    92,    93,    93,    94,    94,    95,    95,
      96,    96,    97,    97,    98,    98,    99,    99,   100,   101,
     101,   101,   102,   102,   102,   102,   102,   102,   102,   102,
     102,   102,   102,   103,   103,   103,   103,   103,   103,   103,
     103,   103,   104,   104,   104,   104,   104,   104,   104,   104,
     104,   104,   105,   105,   105,   105,   106,   106,   106,   107,
     107,   108,   108,   109,   109,   109,   110,   111,   111,   112,
     112,   113,   113,   114,   114,   114,   115,   116,   117,   118,
     118,   118,   119,   119
  };

  /* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
  const unsigned char
  _Parser::yyr2_[] =
  {
         0,     2,     1,     1,     1,     1,     2,     3,     1,     1,
       1,     3,     3,     1,     1,     1,     1,     1,     1,     1,
       3,     4,     4,     3,     1,     1,     1,     3,     1,     3,
       1,     2,     1,     1,     1,     1,     1,     3,     3,     3,
       1,     3,     3,     1,     3,     3,     1,     3,     3,     3,
       3,     1,     3,     3,     1,     3,     1,     3,     1,     3,
       1,     3,     1,     3,     1,     3,     1,     5,     1,     4,
       6,     7,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     2,     3,     2,     2,     3,     3,     7,
       5,     1,     3,     3,     5,     1,     1,     5,     5,     1,
       3,     1,     3,     5,     6,     5,     2,     5,     3,     1,
       2,     2,     1,     0
  };

#if YYDEBUG || YYERROR_VERBOSE || YYTOKEN_TABLE
  /* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
     First, the terminals, then, starting at \a yyntokens_, nonterminals.  */
  const char*
  const _Parser::yytname_[] =
  {
    "\"end of file\"", "error", "$undefined", "\"flush evaluaton\"",
  "\"identifier\"", "\"integer constant\"", "\"floating point constant\"",
  "\"string constant\"", "\"->\"", "\"++\"", "\"--\"", "\"<<\"", "\">>\"",
  "\"<=\"", "\">=\"", "\"==\"", "\"!=\"", "\"&&\"", "\"||\"", "\"^^\"",
  "\"*=\"", "\"/=\"", "\"%=\"", "\"+=\"", "\"-=\"", "\"<<=\"", "\">>=\"",
  "\"&=\"", "\"^=\"", "\"|=\"", "\"bool\"", "\"int\"", "\"float\"",
  "\"string\"", "\"Array\"", "\"Map\"", "\"Function\"", "\"Output\"",
  "\"Input\"", "\"Param\"", "\"if\"", "\"else\"", "\"for\"", "\"in\"",
  "\"to\"", "\"step\"", "\"...\"", "\"..\"", "\"true\"", "\"false\"",
  "\"this\"", "\"return\"", "'['", "']'", "'('", "')'", "'.'", "'='",
  "','", "'+'", "'-'", "'~'", "'!'", "'*'", "'/'", "'%'", "'<'", "'>'",
  "'&'", "'^'", "'|'", "'?'", "':'", "';'", "'{'", "'}'", "$accept",
  "identifier", "array_constant", "map_constant", "primary_expression",
  "constant", "function_call_expression", "postfix_expression",
  "argument_expression", "argument_expression_list", "unary_expression",
  "unary_operator", "multiplicative_expression", "additive_expression",
  "shift_expression", "relational_expression", "equality_expression",
  "and_expression", "exclusive_or_expression", "inclusive_or_expression",
  "logical_and_expression", "logical_exclusive_or_expression",
  "logical_inclusive_or_expression", "conditional_expression",
  "expression", "assignment_statement", "assignment_operator",
  "type_specifier", "statement", "statements", "compound_statement",
  "selection_statement", "expression_sequence_expression",
  "sequence_expression", "sequence", "iteration_statement",
  "function_argument_decl", "function_argument_decl_list",
  "function_definition_statement", "function_call_statement",
  "variable_creation_statement", "return_statement",
  "translation_unit_statements", "translation_unit", 0
  };
#endif

#if YYDEBUG
  /* YYRHS -- A `-1'-separated list of the rules' RHS.  */
  const _Parser::rhs_number_type
  _Parser::yyrhs_[] =
  {
       119,     0,    -1,     4,    -1,    43,    -1,    44,    -1,    45,
      -1,    52,    53,    -1,    52,   110,    53,    -1,   106,    -1,
      77,    -1,    81,    -1,    54,   100,    55,    -1,    54,   109,
      55,    -1,     5,    -1,     6,    -1,     7,    -1,    48,    -1,
      49,    -1,    78,    -1,    79,    -1,    83,    54,    55,    -1,
      83,    54,    85,    55,    -1,    83,    52,   100,    53,    -1,
      83,    56,    77,    -1,    82,    -1,    80,    -1,   100,    -1,
      77,    57,   100,    -1,    84,    -1,    85,    58,    84,    -1,
      83,    -1,    87,    86,    -1,    59,    -1,    60,    -1,    61,
      -1,    62,    -1,    86,    -1,    88,    63,    86,    -1,    88,
      64,    86,    -1,    88,    65,    86,    -1,    88,    -1,    89,
      59,    88,    -1,    89,    60,    88,    -1,    89,    -1,    90,
      11,    89,    -1,    90,    12,    89,    -1,    90,    -1,    91,
      66,    90,    -1,    91,    67,    90,    -1,    91,    13,    90,
      -1,    91,    14,    90,    -1,    91,    -1,    92,    15,    91,
      -1,    92,    16,    91,    -1,    92,    -1,    93,    68,    92,
      -1,    93,    -1,    94,    69,    93,    -1,    94,    -1,    95,
      70,    94,    -1,    95,    -1,    96,    17,    95,    -1,    96,
      -1,    97,    19,    96,    -1,    97,    -1,    98,    18,    97,
      -1,    98,    -1,    98,    71,   100,    72,    99,    -1,    99,
      -1,    77,   102,   100,    73,    -1,    83,    56,    77,   102,
     100,    73,    -1,    83,    52,   100,    53,   102,   100,    73,
      -1,    57,    -1,    20,    -1,    21,    -1,    22,    -1,    23,
      -1,    24,    -1,    25,    -1,    26,    -1,    27,    -1,    28,
      -1,    29,    -1,    30,    -1,    31,    -1,    32,    -1,    33,
      -1,    34,    -1,    35,    -1,    37,    -1,    38,    -1,    39,
      -1,   106,    -1,   107,    -1,   111,    -1,   114,    -1,   115,
      -1,   101,    -1,   116,    -1,   117,    -1,     3,    -1,    73,
      -1,   104,    -1,   105,   104,    -1,   105,     1,    73,    -1,
       1,    73,    -1,    74,    75,    -1,    74,   105,    75,    -1,
      74,     1,    75,    -1,    40,    54,   100,    55,   104,    41,
     104,    -1,    40,    54,   100,    55,   104,    -1,   100,    -1,
     108,    58,   100,    -1,   100,    44,   100,    -1,   100,    44,
     100,    45,   100,    -1,   108,    -1,   109,    -1,    42,    77,
      43,   100,   104,    -1,    42,    77,    57,   110,   104,    -1,
      77,    -1,    77,    57,   100,    -1,   112,    -1,   113,    58,
     112,    -1,    36,    77,    54,    55,   106,    -1,    36,    77,
      54,   113,    55,   106,    -1,    36,    77,    57,   100,    73,
      -1,    82,    73,    -1,   103,    77,    57,   100,    73,    -1,
      51,   100,    73,    -1,   104,    -1,   118,   104,    -1,     1,
      73,    -1,   118,    -1,    -1
  };

  /* YYPRHS[YYN] -- Index of the first RHS symbol of rule number YYN in
     YYRHS.  */
  const unsigned short int
  _Parser::yyprhs_[] =
  {
         0,     0,     3,     5,     7,     9,    11,    14,    18,    20,
      22,    24,    28,    32,    34,    36,    38,    40,    42,    44,
      46,    50,    55,    60,    64,    66,    68,    70,    74,    76,
      80,    82,    85,    87,    89,    91,    93,    95,    99,   103,
     107,   109,   113,   117,   119,   123,   127,   129,   133,   137,
     141,   145,   147,   151,   155,   157,   161,   163,   167,   169,
     173,   175,   179,   181,   185,   187,   191,   193,   199,   201,
     206,   213,   221,   223,   225,   227,   229,   231,   233,   235,
     237,   239,   241,   243,   245,   247,   249,   251,   253,   255,
     257,   259,   261,   263,   265,   267,   269,   271,   273,   275,
     277,   279,   281,   283,   286,   290,   293,   296,   300,   304,
     312,   318,   320,   324,   328,   334,   336,   338,   344,   350,
     352,   356,   358,   362,   368,   375,   381,   384,   390,   394,
     396,   399,   402,   404
  };

  /* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
  const unsigned short int
  _Parser::yyrline_[] =
  {
         0,   176,   176,   177,   178,   179,   183,   184,   188,   192,
     193,   194,   195,   199,   200,   201,   202,   203,   204,   205,
     209,   213,   221,   225,   229,   232,   236,   240,   247,   252,
     260,   263,   267,   268,   269,   270,   274,   275,   279,   283,
     290,   291,   295,   302,   303,   307,   314,   315,   319,   323,
     327,   334,   335,   339,   346,   347,   354,   355,   362,   363,
     370,   371,   378,   379,   386,   387,   394,   395,   402,   406,
     410,   414,   421,   422,   423,   424,   425,   426,   427,   428,
     429,   430,   431,   435,   436,   437,   438,   439,   440,   442,
     443,   444,   448,   449,   450,   451,   452,   453,   454,   455,
     456,   457,   461,   466,   471,   475,   482,   486,   490,   497,
     501,   508,   513,   521,   525,   529,   536,   543,   547,   554,
     558,   565,   570,   578,   583,   588,   595,   602,   611,   618,
     622,   626,   630,   631
  };

  // Print the state stack on the debug stream.
  void
  _Parser::yystack_print_ ()
  {
    *yycdebug_ << "Stack now";
    for (state_stack_type::const_iterator i = yystate_stack_.begin ();
	 i != yystate_stack_.end (); ++i)
      *yycdebug_ << ' ' << *i;
    *yycdebug_ << std::endl;
  }

  // Report on the debug stream that the rule \a yyrule is going to be reduced.
  void
  _Parser::yy_reduce_print_ (int yyrule)
  {
    unsigned int yylno = yyrline_[yyrule];
    int yynrhs = yyr2_[yyrule];
    /* Print the symbols being reduced, and their result.  */
    *yycdebug_ << "Reducing stack by rule " << yyrule - 1
	       << " (line " << yylno << "):" << std::endl;
    /* The symbols being reduced.  */
    for (int yyi = 0; yyi < yynrhs; yyi++)
      YY_SYMBOL_PRINT ("   $" << yyi + 1 << " =",
		       yyrhs_[yyprhs_[yyrule] + yyi],
		       &(yysemantic_stack_[(yynrhs) - (yyi + 1)]),
		       &(yylocation_stack_[(yynrhs) - (yyi + 1)]));
  }
#endif // YYDEBUG

  /* YYTRANSLATE(YYLEX) -- Bison symbol number corresponding to YYLEX.  */
  _Parser::token_number_type
  _Parser::yytranslate_ (int t)
  {
    static
    const token_number_type
    translate_table[] =
    {
           0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,    62,     2,     2,     2,    65,    68,     2,
      54,    55,    63,    59,    58,    60,    56,    64,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,    72,    73,
      66,    57,    67,    71,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,    52,     2,    53,    69,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,    74,    70,    75,    61,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     1,     2,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    13,    14,
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
      35,    36,    37,    38,    39,    40,    41,    42,    43,    44,
      45,    46,    47,    48,    49,    50,    51
    };
    if ((unsigned int) t <= yyuser_token_number_max_)
      return translate_table[t];
    else
      return yyundef_token_;
  }

  const int _Parser::yyeof_ = 0;
  const int _Parser::yylast_ = 592;
  const int _Parser::yynnts_ = 44;
  const int _Parser::yyempty_ = -2;
  const int _Parser::yyfinal_ = 104;
  const int _Parser::yyterror_ = 1;
  const int _Parser::yyerrcode_ = 256;
  const int _Parser::yyntokens_ = 76;

  const unsigned int _Parser::yyuser_token_number_max_ = 306;
  const _Parser::token_number_type _Parser::yyundef_token_ = 2;


/* Line 1136 of lalr1.cc  */
#line 5 "rpgml.yy"
} // RPGML

/* Line 1136 of lalr1.cc  */
#line 2639 "rpgml.tab.cc"


/* Line 1138 of lalr1.cc  */
#line 634 "rpgml.yy"





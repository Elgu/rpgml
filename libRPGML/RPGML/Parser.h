#ifndef RPGML_Parser_h
#define RPGML_Parser_h

#include "rpgml.tab.hh"
#include "Scanner.h"

namespace RPGML {

class Parser : public Scanner, public _Parser
{
public:
  explicit
  Parser( StringUnifier *unifier, Source *_source=0 );

  virtual ~Parser( void );

  virtual void append( CountPtr< Statement > statement ) = 0;
};

class CompoundingParser : public Parser
{
public:
  explicit
  CompoundingParser( StringUnifier *unifier, Source *_source=0 )
  : Parser( unifier, _source )
  {}

  virtual ~CompoundingParser( void )
  {}

  virtual void append( Statement *statement )
  {
    statements.push_back( statement );
  }

  std::vector< CountPtr< Statement > > statements;
};

} // namespace RPGML

#endif


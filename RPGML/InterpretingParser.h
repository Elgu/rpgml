#ifndef RPGML_InterpretingParser_h
#define RPGML_InterpretingParser_h

#include "Parser.h"
#include "StringUnifier.h"
#include "Scanner.h"
#include "Context.h"
#include "Scope.h"
#include "InterpretingASTVisitor.h"

namespace RPGML {

class InterpretingParser : public Refcounted, public Parser
{
public:
  explicit
  InterpretingParser( Scope *scope, Source *source )
  : Parser( scope->getUnifier(), source )
  , interpreter( new InterpretingASTVisitor( scope ) )
  {}

  virtual ~InterpretingParser( void )
  {}

  virtual void append( CountPtr< Statement > statement )
  {
    try
    {
      statement->invite( interpreter );
    }
    catch( const char *e )
    {
      std::cerr << e << std::endl;
    }
  }

private:
  CountPtr< InterpretingASTVisitor > interpreter;
};

} // namespace RPGML

#endif


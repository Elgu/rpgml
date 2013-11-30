#include <RPGML/Parser.h>
#include <RPGML/AST_PrettyPrinter.h>
#include <RPGML/InterpretingASTVisitor.h>
#include <RPGML/Scope.h>
#include <RPGML/Context.h>
#include <RPGML/GarbageCollector.h>
#include <RPGML/Map.h>
#include <RPGML/FileSource.h>
#include <RPGML/InterpretingParser.h>

#include <cerrno>
#include <cstring>

using namespace RPGML;

class PrettyPrintingParser : public Parser
{
public:
  explicit
  PrettyPrintingParser( StringUnifier *unifier, Source *source, std::ostream *o )
  : Parser( unifier, source )
  , m_printer( o )
  {}

  virtual ~PrettyPrintingParser( void )
  {}

  virtual void append( CountPtr< Statement > statement )
  {
    statement->invite( &m_printer );
    m_printer.getStream() << std::endl;
  }

private:
  AST::PrettyPrinter m_printer;
};

int main( int argc, char **argv )
{
  CountPtr< Source > source;

  if( argc > 1 )
  {
    FILE *file = fopen( argv[ 1 ], "r" );
    if( !file )
    {
      std::cerr << "Could not open file '" << argv[ 1 ] << "': " << strerror( errno ) << std::endl;
      return -1;
    }
    source = new FileSource( file );
  }
  else
  {
    source = new FileSource( stdin );
  }

  CountPtr< StringUnifier > unifier = new StringUnifier();
  CountPtr< Context > context = new Context( unifier );
  CountPtr< Scope > scope = context->createScope();

//  PrettyPrintingParser parser( unifier, source, &std::cerr );
  InterpretingParser parser( scope, source );

  parser.parse();

  return 0;
}


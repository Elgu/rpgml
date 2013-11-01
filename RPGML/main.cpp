#include <RPGML/Parser.h>
#include <RPGML/AST_PrettyPrinter.h>
#include <RPGML/InterpretingASTVisitor.h>
#include <RPGML/Scope.h>
#include <RPGML/Context.h>
#include <RPGML/GarbageCollector.h>
#include <RPGML/Map.h>

#include <cstdio>
#include <cerrno>
#include <cstring>

using namespace RPGML;

class FileSource : public Source
{
public:
  FileSource( FILE *file )
  : m_file( file )
  {}

  virtual ~FileSource( void )
  {
    fclose( m_file );
  }

protected:
  virtual char nextChar( void )
  {
    const int ret = fgetc( m_file );
    if( EOF == ret )
    {
      return '\0';
    }
    return char( ret );
  }

private:
  FILE *m_file;
};

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

class InterpretingParser : public Parser
{
public:
  explicit
  InterpretingParser( StringUnifier *unifier, Source *source )
  : Parser( unifier, source )
  {
    context = new Context( unifier );
    scope = context->createScope();
    interpreter = new InterpretingASTVisitor( scope );
  }

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
  CountPtr< Context > context;
  CountPtr< Scope > scope;
  CountPtr< InterpretingASTVisitor > interpreter;
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
//  PrettyPrintingParser parser( unifier, source, &std::cerr );
  InterpretingParser parser( unifier, source );

  parser.parse();

  return 0;
}


/* This file is part of RPGML.
 *
 * Copyright (c) 2014, Gunnar Payer, All rights reserved.
 *
 * RPGML is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 3.0 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library.
 */
#include <RPGML/Parser.h>
#include <RPGML/AST_PrettyPrinter.h>
#include <RPGML/InterpretingASTVisitor.h>
#include <RPGML/Scope.h>
#include <RPGML/Context.h>
#include <RPGML/JobQueue.h>
#include <RPGML/Thread.h>
#include <RPGML/Graph.h>
#include <RPGML/Frame.h>
#include <RPGML/FileSource.h>
#include <RPGML/InterpretingParser.h>
#include <RPGML/ThreadPool.h>

#include <cerrno>
#include <cstring>

// RPGML_CXXFLAGS=
// RPGML_LDFLAGS=

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

static GarbageCollector gc( 5 );

int main( int argc, char **argv )
{
  const int num_threads = 2;

  const char *searchPath = getenv( "RPGML_PATH" );
  if( !searchPath ) searchPath = ".";

  try
  {
    CountPtr< Source > source;
    String filename;

    if( argc > 1 )
    {
      filename = argv[ 1 ];
      FILE *file = fopen( filename, "r" );
      if( !file )
      {
        std::cerr << "Could not open file '" << argv[ 1 ] << "': " << strerror( errno ) << std::endl;
        return -1;
      }
      source = new FileSource( file );
    }
    else
    {
      filename = "stdin";
      source = new FileSource( stdin );
    }

    CountPtr< StringUnifier > unifier = new StringUnifier();
    CountPtr< Context > context = new Context( &gc, unifier, searchPath );
    CountPtr< Scope > scope = context->createScope();

  //  PrettyPrintingParser parser( unifier, source, &std::cerr );
    CountPtr< InterpretingParser >  parser = new InterpretingParser( &gc, scope, source );
    parser->setFilename( filename );

    parser->parse();

    std::cerr << "Parsing done." << std::endl;

    parser.reset();
    source.reset();
    scope.reset();

    gc.run();

    CountPtr< Graph > graph = context->createGraph();
    if( graph->empty() ) return 0;

    graph->merge();
    gc.run();

    CountPtr< ThreadPool > pool = new ThreadPool( &gc, num_threads );

    std::cerr << "executing ..." << std::endl;
    graph->setEverythingChanged( true );
    for(;;)
    {
      graph->execute( pool->getQueue() );
      if( graph->hasErrors() )
      {
        std::cerr << "execution done" << std::endl;
        graph->printErrors( std::cerr );
        return 1;
      }
      if( graph->hasExitRequest() )
      {
        return 0;
      }
      graph->setEverythingChanged( false );
    }

    return 0;
  }
  catch( const RPGML::Exception &e )
  {
    e.print_backtrace();
    std::cerr << e.what() << std::endl;
    return -1;
  }
  catch( const std::exception &e )
  {
    std::cerr << e.what() << std::endl;
    return -1;
  }
  catch( const char *e )
  {
    std::cerr << e << std::endl;
    return -1;
  }
}


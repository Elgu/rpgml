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
#include <RPGML/Guard.h>
#include <RPGML/make_printable.h>

#include <readline/readline.h>
#include <readline/history.h>
#include <getopt.h>

#include <cerrno>
#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <unistd.h>

// RPGML_CXXFLAGS=
// RPGML_LDFLAGS=-lreadline

using namespace RPGML;

class ReadlineSource : public Source
{
public:
  ReadlineSource( void ) {}
  virtual ~ReadlineSource( void ) {}

protected:
  virtual const char *nextChars( void )
  {
    if( m_line.isNull() )
    {
      m_line.set( ::readline( "" ) );
      return m_line;
    }
    else
    {
      m_line.clear();
      return "\n";
    }
  }

  class CharPtrGuard : public Guard< const char >
  {
    typedef Guard< const char > Base;
  public:
    CharPtrGuard( void )
    : Base( (const char*)0, free_c_str )
    {}

    explicit
    CharPtrGuard( const char *c )
    : Base( c, free_c_str )
    {}

  private:
    static void free_c_str( const char *c )
    {
      ::free( (void*)c );
    }
  };

  CharPtrGuard m_line;
};

static GarbageCollector gc( 5 );

static const char *rpgml_file = 0;
static int         num_threads = -1;
static std::string searchPath;
static CountPtr< StringArray > rpgml_argv;

static
void parse_argv( int argc, char **argv )
{
  static struct option long_options[] =
  {
    { "num_threads", 1, 0, 'j' },
    { "path"       , 1, 0, 'p' },
    { 0            , 0, 0, 0   }
  };
  static const char *options = "j:p:";

  int c = 0;
  int option_index = 0;

  while( -1 != ( c = getopt_long( argc, argv, options, long_options, &option_index ) ) )
  {
    switch( c )
    {
      case 'j':
        num_threads = atoi( optarg );
        if( num_threads < 1 )
        {
          throw Exception()
            << "Option --num_threads must be greater than 0, is " << num_threads
            ;
        }
        break;

      case 'p':
        if( !searchPath.empty() ) searchPath += ":";
        searchPath += optarg;
        break;

      case '?':
      case ':':
      default:
        throw Exception() << "Parsing options failed";
        break;
    }
  }

  if( optind < argc )
  {
    rpgml_file = argv[ optind ];
  }

  rpgml_argv = new StringArray( &gc, 1 );
  for( int i = optind; i < argc; ++i )
  {
    rpgml_argv->push_back( from_printable( String::Static( argv[ i ] ) ) );
  }
}

int main( int argc, char **argv )
{
  try
  {
    parse_argv( argc, argv );

    const char *searchPath_env = getenv( "RPGML_PATH" );
    if( searchPath_env )
    {
      if( !searchPath.empty() ) searchPath += ":";
      searchPath += searchPath_env;
    }

    if( searchPath.empty() )
    {
      searchPath = ".";
    }

    if( num_threads < 1 )
    {
      const char *num_threads_env = getenv( "RPGML_NUM_THREADS" );
      if( num_threads_env ) num_threads = atoi( num_threads_env );
      if( num_threads < 1 ) num_threads = 1;
    }

    CountPtr< Source > source;
    String filename;

    if( rpgml_file )
    {
      filename = rpgml_file;
      FILE *file = fopen( filename, "r" );
      if( !file )
      {
        throw Exception() << "Could not open file '" << filename << "': " << strerror( errno );
      }
      source = new FileSource( file );
    }
    else
    {
      filename = "stdin";
//      source = new FileSource( stdin );
      source = new ReadlineSource;
    }

    CountPtr< StringUnifier > unifier = new StringUnifier();
    CountPtr< Context > context = new Context( &gc, unifier, searchPath );
    CountPtr< Scope > scope = context->createScope();

    scope->create( String::Static( "argc" ), Value( int( rpgml_argv->size() ) ) );
    scope->create( String::Static( "argv" ), Value( rpgml_argv ) );

    CountPtr< InterpretingParser >  parser = new InterpretingParser( &gc, scope, source );
    parser->setFilename( filename );

    parser->parse();

//    std::cerr << "Parsing done." << std::endl;

    parser.reset();
    source.reset();
    scope.reset();

    gc.run();

    CountPtr< Graph > graph = context->createGraph();
    if( graph->empty() ) return 0;

    graph->merge();
    gc.run();

    CountPtr< ThreadPool > pool = new ThreadPool( &gc, num_threads );
    CountPtr< JobQueue > main_thread_queue = new JobQueue( &gc );

    graph->execute( pool->getQueue() );

    if( graph->hasErrors() )
    {
      graph->printErrors( std::cerr );
      return -1;
    }
    else
    {
      return 0;
    }
  }
  catch( const RPGML::Exception &e )
  {
    std::cerr << e.getBacktrace() << "\n" << e.what() << std::endl;
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


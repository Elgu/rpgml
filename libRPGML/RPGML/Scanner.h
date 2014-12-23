#ifndef RPGML_Scanner_h
#define RPGML_Scanner_h

#include "rpgml.tab.hh"
#include "StringUnifier.h"

namespace RPGML {

class Source : public Refcounted
{
public:
  typedef _Parser::location_type location_type;

  Source( void )
  : m_prevChar( '\0' )
  , m_putBackChar( '\0' )
  , m_charWasPutBack( false )
  {}

  virtual ~Source( void ) {}

  char next( location_type *loc );
  void putBackChar( void );

  void tokenBegin( location_type *loc );
  void tokenEnd( location_type *loc );

protected:
  virtual char nextChar( void ) = 0;

private:
  char trackLocation( char c, location_type *loc );

  char m_prevChar;
  char m_putBackChar;
  bool m_charWasPutBack;
};

class Scanner
{
public:
  typedef _Parser::location_type location_type;
  typedef _Parser::semantic_type semantic_type;

  explicit
  Scanner( StringUnifier *unifier, Source *source=0 )
  : m_source( source )
  , m_unifier( unifier )
  {
    setFilename( "" );
  }

  Scanner &getScanner( void ) { return (*this); }

  Scanner &setSource( Source *source );
  Scanner &setFilename( const char *filename );
  Scanner &setFilename( const std::string &filename );
  Scanner &setFilename( const String &filename );

  int yylex( semantic_type *yylval, location_type *yylloc );

  virtual void append( CountPtr< Statement > statement ) = 0;

private:
  CountPtr< Source > m_source;
  CountPtr< StringUnifier > m_unifier;

  std::string m_filename;
public:
  String unified_filename;
  std::string *filename;

  const StringData *unify_move( String &str )       { return m_unifier->unify_move( str ); }
  const StringData *unify_move( std::string &str )  { return m_unifier->unify_move( str ); }
  const StringData *unify( const String &str )      { return m_unifier->unify( str ); }
  const StringData *unify( const char *str )        { return m_unifier->unify( str ); }
  const StringData *unify( const std::string &str ) { return m_unifier->unify( str ); }
};

} // namespace RPGML

#endif


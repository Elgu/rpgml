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
#ifndef RPGML_Scanner_h
#define RPGML_Scanner_h

#include <RPGML/rpgml.tab.hh>
#include "StringUnifier.h"

namespace RPGML {

class GarbageCollector;

class Source : public Refcounted
{
public:
  typedef _Parser::location_type location_type;

  Source( void );
  virtual ~Source( void );

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
  Scanner( GarbageCollector *_gc, StringUnifier *unifier, Source *source=0 );
  ~Scanner( void );

  Scanner &getScanner( void );

  Scanner &setSource( Source *source );
  Scanner &setFilename( const char *filename );
  Scanner &setFilename( const std::string &filename );
  Scanner &setFilename( const String &filename );
  const String &getFilename( void ) const;

  GarbageCollector *getScannerGC( void ) const;

  int yylex( semantic_type *yylval, location_type *yylloc );

  virtual void append( CountPtr< Statement > statement ) = 0;

private:
  CountPtr< Source > m_source;
  CountPtr< StringUnifier > m_unifier;
  GarbageCollector *m_gc;

  std::string m_filename;
public:
  String unified_filename;
  std::string *filename;

  const StringData *unify_move( std::string &str )  { return m_unifier->unify_move( str ); }
  const StringData *unify( const String &str )      { return m_unifier->unify( str ); }
  const StringData *unify( const char *str )        { return m_unifier->unify( str ); }
  const StringData *unify( const std::string &str ) { return m_unifier->unify( str ); }
};

} // namespace RPGML

#endif


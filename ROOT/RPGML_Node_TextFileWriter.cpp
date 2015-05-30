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
#include "RPGML_Node_TextFileWriter.h"

// RPGML_CXXFLAGS=
// RPGML_LDFLAGS=

#include <RPGML/Guard.h>

#include <algorithm>
#include <cstdio>
#include <cerrno>
#include <cstring>

using namespace std;

namespace RPGML {


TextFileWriter::TextFileWriter( GarbageCollector *_gc, const String &identifier, const RPGML::SharedObject *so )
: Node( _gc, identifier, so, NUM_INPUTS, NUM_OUTPUTS, NUM_PARAMS )
{
  DEFINE_INPUT ( INPUT_FILENAME, "filename" );
  DEFINE_INPUT ( INPUT_DOIT, "doit" );
  DEFINE_INPUT ( INPUT_IN  , "in"  );
  DEFINE_OUTPUT( OUTPUT_OUT, "out" );
  DEFINE_PARAM ( PARAM_APPEND , "append", TextFileWriter::set_append );
  DEFINE_PARAM ( PARAM_WHOLE_FILE, "whole_file", TextFileWriter::set_whole_file );
}

TextFileWriter::~TextFileWriter( void )
{}

const char *TextFileWriter::getName( void ) const
{
  return "TextFileWriter";
}

void TextFileWriter::gc_clear( void )
{
  Base::gc_clear();
}

void TextFileWriter::gc_getChildren( Children &children ) const
{
  Base::gc_getChildren( children );
}

void TextFileWriter::set_append( const Value &value, index_t, int, const index_t * )
{
  m_append = value.save_cast< bool >();
}

void TextFileWriter::set_whole_file( const Value &value, index_t, int, const index_t * )
{
  m_whole_file = value.save_cast< bool >();
}

bool TextFileWriter::tick( void )
{
  GET_INPUT_AS_DIMS_IF_CONNECTED( INPUT_DOIT, doit, bool, 0 );

  GET_INPUT_AS_DIMS( INPUT_FILENAME, filename, String, 0 );
  getOutput( OUTPUT_OUT )->setData( const_cast< StringArray* >( filename ) );

  if( doit && !(**doit) ) return true;

  setAllOutputChanged();
  GET_INPUT_AS_DIMS( INPUT_IN, in, String, 0 );

  if( (**filename) != m_last_filename )
  {
    m_last_filename = (**filename);
    m_first = true;
  }

  const char *flags = nullptr;
  if( m_append || ( !m_whole_file && !m_first ) )
  {
    flags = "wa";
  }
  else
  {
    flags = "w";
  }

  Guard< FILE, int > file( ::fopen( (**filename), flags ), ::fclose );
  if( !file )
  {
    throw Exception()
      << "Could not open file '" << (**filename) << "' for '" << flags << "'"
      << ": " << ::strerror( errno )
      ;
  }

  const size_t length = (**in).length();
  if( length > 0 )
  {
    size_t n_written = 0;
    if( 1 != ( n_written = ::fwrite( (**in).c_str(), length, 1, file ) ) )
    {
      throw Exception()
        << "Could not write to already opened file '" << (**filename) << "'"
        << ": " << ::strerror( errno )
        << ", n_written = " << n_written
        ;
    }
  }


  return true;
}

 //
} // namespace RPGML

RPGML_CREATE_NODE( TextFileWriter,  )


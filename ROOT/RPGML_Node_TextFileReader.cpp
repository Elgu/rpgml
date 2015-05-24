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
#include "RPGML_Node_TextFileReader.h"

// RPGML_CXXFLAGS=
// RPGML_LDFLAGS=

#include <RPGML/Guard.h>
#include <algorithm>
#include <cstdio>
#include <cerrno>
#include <cstring>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

using namespace std;

namespace RPGML {


TextFileReader::TextFileReader( GarbageCollector *_gc, const String &identifier, const RPGML::SharedObject *so )
: Node( _gc, identifier, so, NUM_INPUTS, NUM_OUTPUTS, NUM_PARAMS )
{
  DEFINE_INPUT ( INPUT_FILENAME, "filename"  );
  DEFINE_INPUT ( INPUT_REWIND  , "rewind"  );
  DEFINE_INPUT ( INPUT_DEFAULT  , "default"  );
  DEFINE_OUTPUT( OUTPUT_OUT, "out" );
  DEFINE_OUTPUT( OUTPUT_LAST, "last" );
  DEFINE_OUTPUT( OUTPUT_FILENAME_OUT, "filename_out" );
  DEFINE_PARAM ( PARAM_WHOLE_FILE , "whole_file", TextFileReader::set_whole_file );
  DEFINE_PARAM ( PARAM_STRIP_NEWLINE, "strip_newline", TextFileReader::set_strip_newline );
}

TextFileReader::~TextFileReader( void )
{}

const char *TextFileReader::getName( void ) const
{
  return "TextFileReader";
}

void TextFileReader::gc_clear( void )
{
  Base::gc_clear();
}

void TextFileReader::gc_getChildren( Children &children ) const
{
  Base::gc_getChildren( children );
}

void TextFileReader::set_whole_file( const Value &value, index_t, int, const index_t* )
{
  m_whole_file = value.save_cast< bool >();
}

void TextFileReader::set_strip_newline( const Value &value, index_t, int, const index_t* )
{
  m_strip_newline = value.save_cast< bool >();
}

bool TextFileReader::tick( void )
{
  GET_INPUT_AS_DIMS_IF_CONNECTED( INPUT_REWIND  , rewind  , bool  , 0 );
  GET_INPUT_AS_DIMS_IF_CONNECTED( INPUT_DEFAULT , default_output, String, 0 );
  GET_OUTPUT_INIT( OUTPUT_OUT , out , String, 0, nullptr );
  GET_OUTPUT_INIT( OUTPUT_LAST, last, bool  , 0, nullptr );

  CountPtr< const StringArray > filename;
  try
  {
    GET_INPUT_AS_DIMS( INPUT_FILENAME, _filename, String, 0 );
    filename = _filename;
  }
  catch( const NotReady & )
  {
    if( default_output )
    {
      (**out) = (**default_output);
      getOutput( OUTPUT_FILENAME_OUT )->setData( const_cast< StringArray* >( filename.get() ) );
      setAllOutputChanged();
      return true;
    }
    else
    {
      throw;
    }
  }

  if( rewind && (**rewind) )
  {
    m_last_filename.clear();
  }

  if( m_whole_file )
  {
    if( m_last_filename == (**filename) ) return true;
  }
  else
  {
    if( m_last_filename != (**filename) )
    {
      m_last_filename.clear();
    }
  }

  Guard< FILE, int > file( ::fopen( (**filename), "r" ), ::fclose );
  if( !file )
  {
    if( default_output )
    {
      (**out) = (**default_output);
      getOutput( OUTPUT_FILENAME_OUT )->setData( const_cast< StringArray* >( filename.get() ) );
      setAllOutputChanged();
      return true;
    }
    else
    {
      throw Exception()
        << "Could not open file '" << (**filename) << "' for reading"
        << ": " << ::strerror( errno )
        ;
    }
  }

  std::string ret;
  ret.reserve( (**out).length() );

  if( m_whole_file )
  {
    vector< char > buffer( 64*1024 );
    while( ::fgets( &buffer[ 0 ], int( buffer.size() ), file ) )
    {
      ret += &buffer[ 0 ];
    }
    (**last) = true;
  }
  else
  {
    if( !m_last_filename.empty() )
    {
      if( 0 != ::fsetpos( file, &m_pos ) )
      {
        throw Exception()
          << "Failed to seek to the last known file position in '" << (**filename) << "'"
          << ": " << ::strerror( errno )
          ;
      }
    }

    vector< char > buffer( 64*1024 );
    if( !::fgets( &buffer[ 0 ], int( buffer.size() ), file ) )
    {
      throw Exception()
        << "End of file reached at '" << (**filename) << "'"
        ;
    }

    ret = &buffer[ 0 ];

    if( 0 != ::fgetpos( file, &m_pos ) )
    {
      throw Exception()
        << "Failed to get the last known file position in '" << (**filename) << "'"
        << ":" << ::strerror( errno )
        ;
    }

    // Test-read at least 1 char (+ '\0')
    {
      char c2[ 2 ];
      (**last) = ( NULL == ::fgets( c2, 2, file ) );
    }
  }

  if( m_strip_newline )
  {
    if( !ret.empty() )
    {
      char &nl = ret.back();
      if( '\n' == nl ) nl = '\0';
    }
  }

  m_last_filename = (**filename);
  (**out) = String::MoveFrom( ret );
  getOutput( OUTPUT_FILENAME_OUT )->setData( const_cast< StringArray* >( filename.get() ) );
  setAllOutputChanged();
  return true;
}

 //
} // namespace RPGML

RPGML_CREATE_NODE( TextFileReader,  )


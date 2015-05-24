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
#ifndef RPGML_Node_TextFileReader_h
#define RPGML_Node_TextFileReader_h

#include <RPGML/Node.h>
#include <cstdio>

namespace RPGML {


class TextFileReader : public Node
{
  typedef Node Base;
public:
  EXCEPTION_BASE( Exception );

  TextFileReader( GarbageCollector *gc, const String &identifier, const SharedObject *so );
  virtual ~TextFileReader( void );

  virtual const char *getName( void ) const;

  virtual bool tick( void );

  virtual void gc_clear( void );
  virtual void gc_getChildren( Children &children ) const;

  void set_whole_file( const Value &value, index_t index, int n_coords, const index_t *coords );
  void set_strip_newline( const Value &value, index_t index, int n_coords, const index_t *coords );

private:
  typedef NodeParam< TextFileReader > NParam;

  enum Inputs
  {
    INPUT_FILENAME,
    INPUT_REWIND,
    INPUT_DEFAULT,
    NUM_INPUTS
  };

  enum Outputs
  {
    OUTPUT_OUT,
    OUTPUT_LAST,
    OUTPUT_FILENAME_OUT,
    NUM_OUTPUTS
  };

  enum Params
  {
    PARAM_WHOLE_FILE,
    PARAM_STRIP_NEWLINE,
    NUM_PARAMS
  };

  String m_last_filename;
  fpos_t m_pos;
  bool m_whole_file = true;
  bool m_strip_newline = false;
};

 //
} // namespace RPGML

#endif


/* This file is part of RPGML.
 *
 * Copyright (c) 2015, Gunnar Payer, All rights reserved.
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
#ifndef RPGML_Node_util_FileMapper_h
#define RPGML_Node_util_FileMapper_h

#include <RPGML/Node.h>

namespace RPGML {
namespace util {

class FileMapper : public Node
{
  typedef Node Base;
public:
  EXCEPTION_BASE( Exception );

  FileMapper( GarbageCollector *gc, const String &identifier, const SharedObject *so );
  virtual ~FileMapper( void );

  virtual const char *getName( void ) const;

  virtual bool tick( void );

  virtual void gc_clear( void );
  virtual void gc_getChildren( Children &children ) const;

private:
  typedef NodeParam< FileMapper > NParam;

  enum Inputs
  {
    INPUT_FILENAME,
    INPUT_TYPE,
    INPUT_DIMS,
    INPUT_SX,
    INPUT_SY,
    INPUT_SZ,
    INPUT_ST,
    INPUT_OFFSET_BYTES,
    NUM_INPUTS
  };

  enum Outputs
  {
    OUTPUT_OUT,
    NUM_OUTPUTS
  };

  enum Params
  {
    NUM_PARAMS
  };

  template< class Element >
  void tick2( const char *filename, size_t offset_bytes, int dims, const index_t *size );

  String m_filename;
  size_t m_offset_bytes = 0;
  index_t m_size[ 4 ];
  int m_dims = 0;
};

 } // namespace util {
} // namespace RPGML

#endif


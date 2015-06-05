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
#ifndef RPGML_util_FileMapper_h
#define RPGML_util_FileMapper_h

#include <RPGML/GarbageCollector.h>
#include <RPGML/ArrayData.h>

#include <sys/mman.h>
#include <unistd.h>
#include <cerrno>
#include <cstring>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>


namespace RPGML {
namespace util {

template< class Element >
class MappedFile : public ArrayData< Element >
{
  typedef ArrayData< Element > Base;
public:
  explicit
  MappedFile( GarbageCollector *_gc, int dims, const index_t *s )
  : Base( _gc, dims, s )
  {}

  explicit
  MappedFile( GarbageCollector *_gc, const ArrayBase::Size &s )
  : Base( _gc, s )
  {}

  virtual ~MappedFile( void )
  {
    clear();
  }

  void clear( void )
  {
    if( m_addr != nullptr )
    {
      ::munmap( m_addr, m_map_length );
      m_addr = nullptr;
      m_map_length = 0;
    }

    if( -1 != m_fd )
    {
      ::close( m_fd );
      m_fd = -1;
    }
  }

  void map_ro( const char *filename, size_t offset_bytes )
  {
    _map( filename, offset_bytes, PROT_READ, O_RDONLY );
  }

  void map_rw( const char *filename, size_t offset_bytes )
  {
    _map( filename, offset_bytes, PROT_READ | PROT_WRITE, O_RDWR );
  }

private:
  void _map( const char *filename, size_t offset_bytes, int prot, int fd_flags )
  {
    clear();

    const index_t length_elements = Base::calc_size( Base::getSize().getDims(), Base::getSize().getCoords() );

    const size_t page_size = ::sysconf( _SC_PAGE_SIZE );

    if( 0 != ( offset_bytes % page_size ) )
    {
      throw Exception()
        << "'offset_bytes' must be a multiple of the page size"
        << ": is " << offset_bytes
        << ", page size is " << page_size
        ;
    }

    const size_t length_elements_bytes = length_elements*sizeof(Element);
    const size_t map_length = ( length_elements_bytes + (page_size-1) ) % page_size;

    try
    {
      m_fd = ::open( filename, fd_flags );
      if( -1 == m_fd )
      {
        throw Exception()
          << "Could not open file '" << filename << "'"
          << ": " << strerror( errno )
          ;
      }

      m_addr = reinterpret_cast< Element* >(
        ::mmap( nullptr, map_length, prot, MAP_SHARED, m_fd, offset_bytes )
        );
      if( !m_addr )
      {
        throw Exception()
          << "Could not map already opened file '" << filename << "'"
          << ": " << strerror( errno )
          ;
      }

      Base::setElements( m_addr, length_elements );
      m_map_length = map_length;
    }
    catch( ... )
    {
      clear();
      throw;
    }
  }

  Element *m_addr = nullptr;
  size_t m_map_length = 0;
  int m_fd = -1;
};

} // namespace util
} // namespace RPGML

#endif // RPGML_util_FileMapper_h

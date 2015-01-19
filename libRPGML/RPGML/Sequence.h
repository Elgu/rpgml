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
#ifndef RPGML_Sequence_h
#define RPGML_Sequence_h

#include "GarbageCollector.h"
#include "Iterator.h"
#include "Value.h"

#include <ostream>

namespace RPGML {

class Sequence : public Collectable
{
public:
  Sequence( GarbageCollector *gc );
  virtual ~Sequence( void );

  virtual std::ostream &print( std::ostream &o ) const = 0;

  typedef Iterator< Value > Iter;

  virtual CountPtr< Sequence > clone( void ) const = 0;
  virtual CountPtr< Iter > getIter( void ) const = 0;

  virtual index_t length( void ) const = 0;
private:
};

class SequenceValueArray : public Sequence
{
public:
  SequenceValueArray( GarbageCollector *gc, const ArrayBase *array );
  virtual ~SequenceValueArray( void );

  virtual std::ostream &print( std::ostream &o ) const;

  virtual void gc_clear( void );
  virtual void gc_getChildren( Children &children ) const;

  virtual CountPtr< Sequence > clone( void ) const;
  virtual CountPtr< Iter > getIter( void ) const;

  virtual index_t length( void ) const;
private:
  class IteratorValueArray : public Iter
  {
  public:
    IteratorValueArray( const ArrayBase *array, index_t i=0 );
    virtual ~IteratorValueArray( void );
    virtual bool done( void ); // not const
    virtual void next( void );
    virtual Value get( void );
    virtual CountPtr< Iter > clone( void ) const;
  private:
    CountPtr< const ArrayBase > m_array;
    index_t m_i;
  };

  CountPtr< const ArrayBase > m_array;
};

CountPtr< Sequence > genSequenceFromToStep( GarbageCollector *gc, const Value &from, const Value &to, const Value &step );

} // namespace RPGML

#endif


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
#ifndef RPGML_Delay_h
#define RPGML_Delay_h

#include "Frame.h"
#include "Node.h"

namespace RPGML {

class Delay : public Frame
{
  typedef Frame Base;
public:
  explicit
  Delay( GarbageCollector *_gc , const String &identifier );
  virtual ~Delay( void );

private:
  class DelayIn;
  class DelayOut;

  class DelayIn : public Node
  {
    typedef Node Base;
  public:
    explicit
    DelayIn( GarbageCollector *_gc, const String &identifier );
    virtual ~DelayIn( void );

    virtual void gc_clear( void );
    virtual void gc_getChildren( Children &children ) const;

    virtual const char *getName( void ) const;
    virtual bool tick( void );

    void setOut( const CountPtr< DelayOut > &out );
    DelayOut *getOut( void ) const;

  private:
    CountPtr< DelayOut > m_out;
    index_t m_i = 0;
    index_t m_delay = 1;
  };

  class DelayOut : public Node
  {
    typedef Node Base;
  public:
    explicit
    DelayOut( GarbageCollector *_gc, const String &identifier );
    virtual ~DelayOut( void );

    virtual void gc_clear( void );
    virtual void gc_getChildren( Children &children ) const;

    virtual const char *getName( void ) const;
    virtual bool tick( void );

    void setIn( const CountPtr< DelayIn > &in );
    DelayIn *getIn( void ) const;

    void setData( index_t i, const CountPtr< ArrayBase > &data );

  private:
    CountPtr< DelayIn > m_in;
    CountPtr< ArrayArray > m_data;
    index_t m_i = 0;
  };
};

} // namespace RPGML

#endif // RPGML_Delay_h

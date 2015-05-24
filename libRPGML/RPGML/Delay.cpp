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
#include "Delay.h"

namespace RPGML {

Delay::Delay( GarbageCollector *_gc, const String &identifier )
: Base( _gc )
{
  CountPtr< DelayIn  > in  = new DelayIn ( _gc, identifier + ".in"  );
  CountPtr< DelayOut > out = new DelayOut( _gc, identifier + ".out" );
  in->setOut( out );
  out->setIn( in );
  push_back( String::Static( "in"  ), Value( in->getInput( 0 ) ) );
  push_back( String::Static( "out" ), Value( out->getOutput( 0 ) ) );
}

Delay::~Delay( void )
{}

Delay::DelayIn::DelayIn( GarbageCollector *_gc, const String &identifier )
: Base( _gc, identifier, nullptr, 1, 0, 0 )
{}

Delay::DelayIn::~DelayIn( void )
{}

void Delay::DelayIn::gc_clear( void )
{
  Base::gc_clear();
  m_out.reset();
}

void Delay::DelayIn::gc_getChildren( Children &children ) const
{
  Base::gc_getChildren( children );
  children
    << m_out
    ;
}

const char *Delay::DelayIn::getName( void ) const
{
  return "Delay.DelayIn";
}

bool Delay::DelayIn::tick( void )
{
  m_out->setData( m_i, const_cast< ArrayBase* >( getInput( 0 )->getData() ) );
  m_i = (m_i+1) % (m_delay+1);
  return true;
}

void Delay::DelayIn::setOut( const CountPtr< DelayOut > &out )
{
  m_out = out;
}

Delay::DelayOut *Delay::DelayIn::getOut( void ) const
{
  return m_out;
}

Delay::DelayOut::DelayOut( GarbageCollector *_gc, const String &identifier )
: Base( _gc, identifier, nullptr, 0, 1, 0 )
, m_data( new ArrayArray( _gc, 1, 2 ) )
{}

Delay::DelayOut::~DelayOut( void )
{}

void Delay::DelayOut::gc_clear( void )
{
  Base::gc_clear();
  m_in.reset();
  m_data.reset();
}

void Delay::DelayOut::gc_getChildren( Children &children ) const
{
  Base::gc_getChildren( children );
  children
    << m_in
    << m_data
    ;
}

const char *Delay::DelayOut::getName( void ) const
{
  return "Delay.DelayOut";
}

bool Delay::DelayOut::tick( void )
{
  const index_t n = m_data->getSizeX();
  const index_t delay = n-1;
  if( m_i >= delay )
  {
    getOutput( 0 )->setData( m_data->at( (m_i-delay) % n ) );
  }
  m_i = (m_i+1) % n;
  return true;
}

void Delay::DelayOut::setIn( const CountPtr< DelayIn > &in )
{
  m_in = in;
}

Delay::DelayIn *Delay::DelayOut::getIn( void ) const
{
  return m_in;
}

void Delay::DelayOut::setData( index_t i, const CountPtr< ArrayBase > &data )
{
  const index_t n = m_data->getSizeX();
  m_data->at( i % n ) = data;
}


} // namespace RPGML

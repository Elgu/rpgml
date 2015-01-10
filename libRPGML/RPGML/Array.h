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
#ifndef RPGML_Array_h
#define RPGML_Array_h

#include "Refcounted.h"
#include "GarbageCollector.h"
#include "Value.h"
#include "Iterator.h"
#include "types.h"
#include "Type.h"
#include "Exception.h"

#include <typeinfo>
#include <ostream>

namespace RPGML {

class Value;

template< class _Element, int _Dims >
class Array;

class ArrayBase : public Collectable
{
  typedef Collectable Base;
public:
  //! @brief Array Exception base class
  EXCEPTION_BASE( Exception );

  //! @brief Exception for when the specified dimensions do not match the Array's
  class DimensionsMismatch : public Exception
  {
  public:
    typedef Exception Base;
    DimensionsMismatch( int _specified, int _expected )
    : specified( _specified )
    , expected( _expected )
    {
      (*this)
        << "Specified dims (" << specified << ")"
        << " does not match Array Dims (" << expected << ")"
        ;
    }
    EXCEPTION_BODY( DimensionsMismatch );
    int specified;
    int expected;
  };

  class Coordinates
  {
  public:
    explicit
    Coordinates( int dims, const index_t *coords )
    : m_coords( dims > 0 ? coords : 0 )
    , m_dims( dims )
    {}

    int            getDims  ( void ) const { return m_dims; }
    const index_t *getCoords( void ) const { return m_coords; }

    operator const index_t *( void ) const { return getCoords(); }
    const index_t &operator[]( int d ) const { return getCoords()[ d ]; }

    bool operator==( const Coordinates &other ) const
    {
      return
           getDims() == other.getDims()
        && std::equal( getCoords(), getCoords() + getDims(), other.getCoords() )
        ;
    }

    bool operator!=( const Coordinates &other ) const
    {
      return !( (*this) == other );
    }

  private:
    const index_t *const m_coords;
    int m_dims;
  };

  typedef Coordinates Size;

  class OutOfRange : public Exception
  {
  public:
    typedef Exception Base;
    explicit
    OutOfRange( const Size &size, const Coordinates &coords )
    {
      (*this)
        << "Out of range: Size is " << size
        << ", coordinates are " << coords
        ;
    }
    EXCEPTION_BODY( OutOfRange );
  };

  explicit
  ArrayBase( GarbageCollector *_gc )
  : Base( _gc )
  {}

  ArrayBase( const ArrayBase &other )
  : Base( other )
  {}

  virtual ~ArrayBase( void ) {}

  template< class DataType >
  DataType *getAs( DataType* &as )
  {
    as = dynamic_cast< DataType* >( this );
    return as;
  }

  template< class DataType >
  const DataType *getAs( const DataType* &as ) const
  {
    as = dynamic_cast< const DataType* >( this );
    return as;
  }

  virtual const std::type_info &getTypeInfo( void ) const = 0;

  virtual ArrayBase &clear( void ) = 0;

  virtual index_t        getDims( void ) const = 0;
  virtual Type           getType( void ) const = 0;
  virtual Size           getSize( void ) const = 0;
  virtual index_t        size   ( void ) const = 0;

  virtual ArrayBase &resize_v( index_t dims, const index_t *new_size ) = 0;
  virtual ArrayBase &resize( index_t x, index_t y=1, index_t z=1, index_t t=1 ) = 0;
  virtual ArrayBase &resize( const Size &size ) = 0;

  virtual CountPtr< ArrayBase > clone( void ) const = 0;

  typedef Iterator< Value > ValueIterator;
  virtual CountPtr< ValueIterator > getValueIterator( void ) const = 0;

  typedef Iterator< Coordinates > CoordinatesIterator;
  virtual CountPtr< CoordinatesIterator > getCoordinatesIterator( void ) const = 0;

  virtual Value getValue( index_t x=0, index_t y=0, index_t z=0, index_t t=0 ) const = 0;
  virtual Value getValue_v( index_t dims, const index_t *x ) const = 0;
  virtual Value getValue( const Coordinates &x ) const = 0;
  virtual void setValue( const Value &value, index_t x=0, index_t y=0, index_t z=0, index_t t=0 ) = 0;
  virtual void setValue_v( const Value &value, index_t dims, const index_t *x ) = 0;
  virtual void setValue( const Value &value, const Coordinates &x ) = 0;
  virtual void fillValue( const Value &value ) = 0;
  virtual bool isValue( void ) const = 0;

  const char *getTypeName( void ) const
  {
    return getType().getTypeName();
  }
};

template< class _Element >
class ArrayElements : public ArrayBase
{
  typedef ArrayBase Base;
private:
  typedef std::vector< _Element > elements_t;
public:
  typedef _Element Element;
  typedef typename elements_t::iterator               iterator;
  typedef typename elements_t::const_iterator         const_iterator;
  typedef typename elements_t::reverse_iterator       reverse_iterator;
  typedef typename elements_t::const_reverse_iterator const_reverse_iterator;
  typedef typename elements_t::reference              reference;
  typedef typename elements_t::const_reference        const_reference;
  typedef Base::OutOfRange         OutOfRange;
  typedef Base::DimensionsMismatch DimensionsMismatch;

  virtual const std::type_info &getTypeInfo( void ) const { return typeid( *this ); }

  explicit
  ArrayElements( GarbageCollector *_gc )
  : Base( _gc )
  {}

  ArrayElements( const ArrayElements &other )
  : Base( other )
  , m_elements( other.m_elements )
  {}

  virtual ~ArrayElements( void )
  {}

  virtual ArrayBase &clear( void )
  {
    m_elements.clear();
    return (*this);
  }

  bool empty( void ) const
  {
    return m_elements.empty();
  }

  virtual index_t size( void ) const
  {
    return index_t( m_elements.size() );
  }

  const Element *elements( void ) const
  {
    return &m_elements[ 0 ];
  }

  Element *elements( void )
  {
    return &m_elements[ 0 ];
  }

  void swap( ArrayElements &other )
  {
    m_elements.swap( other.m_elements );
  }

  void fill( const Element &x )
  {
    std::fill( m_elements.begin(), m_elements.end(), x );
  }

  virtual reference       at_v( index_t dims, const index_t *x ) = 0;
  virtual const_reference at_v( index_t dims, const index_t *x ) const = 0;

  virtual bool isValue( void ) const { return _isValue( (const Element*)0 ); }

  iterator       begin( void )       { return m_elements.begin(); }
  const_iterator begin( void ) const { return m_elements.begin(); }
  iterator       end  ( void )       { return m_elements.end  (); }
  const_iterator end  ( void ) const { return m_elements.end  (); }
  reverse_iterator       rbegin( void )       { return m_elements.rbegin(); }
  const_reverse_iterator rbegin( void ) const { return m_elements.rbegin(); }
  reverse_iterator       rend  ( void )       { return m_elements.rend  (); }
  const_reverse_iterator rend  ( void ) const { return m_elements.rend  (); }

  Element       &back( void )       { return m_elements.back(); }
  const Element &back( void ) const { return m_elements.back(); }
  Element       &front( void )       { return m_elements.front(); }
  const Element &front( void ) const { return m_elements.front(); }

  virtual void gc_clear( void )
  {
    clear();
  }

  virtual void gc_getChildren( Children &children ) const
  {
    if( isCollectable( (Element*)0 ) )
    {
      for( size_t i=0; i<m_elements.size(); ++i )
      {
        addElement( children, m_elements[ i ] );
      }
    }
  }

protected:
  reference elements_at( index_t i )
  {
    return m_elements[ i ];
  }

  const_reference elements_at( index_t i ) const
  {
    return m_elements[ i ];
  }

  void _push_back( const Element &x )
  {
    m_elements.push_back( x );
  }

  void _pop_back( void )
  {
    m_elements.pop_back();
  }

  ArrayElements &_resize( size_t new_size )
  {
    m_elements.resize( new_size );
    return (*this);
  }

private:
  static inline
  void addElement( Children &children, const void *element )
  {
//    std::cerr << " addElement (void) T " << std::endl;
    (void)children;
    (void)element;
  }

  static inline
  void addElement( Children &children, const Collectable *const *element )
  {
//    std::cerr << " addElement Collectable* " << std::endl;
    children.add( *element );
  }

  static inline
  void addElement( Children &children, const Collectable *element )
  {
//    std::cerr << " addElement Collectable* " << std::endl;
    children.add( element );
  }

  template< class T >
  static inline
  void addElement( Children &children, const CountPtr< T > *element )
  {
//    std::cerr << " addElement CountPtr< T > " << std::endl;
    if( !element->isNull() ) addElement( children, element->get() );
  }

  static inline
  void addElement( Children &children, const Element &element )
  {
    addElement( children, &element );
  }

  static inline
  void addElement( Children &children, std::vector< bool >::reference element )
  {
//    std::cerr << " addElement (bool) T " << std::endl;
    (void)children;
    (void)element;
  }

  static inline bool _isValue( const Value* ) { return true; }
  template< class T >
  static inline bool _isValue( const T* ) { return false; }

  elements_t m_elements;
};

template< class _Element, int _Dims >
class Array : public ArrayElements< _Element >
{
  typedef ArrayElements< _Element > Base;
public:
  static const int Dims = _Dims;
  typedef _Element Element;
  typedef typename Base::iterator               iterator;
  typedef typename Base::const_iterator         const_iterator;
  typedef typename Base::reverse_iterator       reverse_iterator;
  typedef typename Base::const_reverse_iterator const_reverse_iterator;
  typedef typename Base::reference              reference;
  typedef typename Base::const_reference        const_reference;
  typedef typename Base::ValueIterator          ValueIterator;
  typedef typename Base::CoordinatesIterator    CoordinatesIterator;
  typedef typename Base::Children               Children;
  typedef typename Base::Coordinates            Coordinates;
  typedef typename Base::Size                   Size;
  typedef typename Base::OutOfRange             OutOfRange;
  typedef typename Base::DimensionsMismatch     DimensionsMismatch;

  virtual const std::type_info &getTypeInfo( void ) const { return typeid( *this ); }

  explicit
  Array( GarbageCollector *_gc )
  : Base( _gc )
  {
    if( Dims > 0 ) std::fill( m_size, m_size+Dims, index_t( 0 ) );
    if( Dims > 1 ) std::fill( m_size2, m_size2+(Dims-1), index_t( 0 ) );
    if( Dims == 0 ) Base::_resize( 1 );
  }

  explicit
  Array( GarbageCollector *_gc, index_t x, index_t y=1, index_t z=1, index_t t=1 )
  : Base( _gc )
  {
    if( Dims == 0 ) Base::_resize( 1 );
    else resize( x, y, z, t );
  }

  explicit
  Array( GarbageCollector *_gc, index_t _dims, const index_t *_size )
  : Base( _gc )
  {
    if( Dims == 0 ) Base::_resize( 1 );
    else if( _size ) resize_v( _dims, _size );
  }

  Array( const Array &other )
  : Base( other )
  {
    if( Dims > 0 ) std::copy( other.m_size , other.m_size  +  Dims   , m_size );
    if( Dims > 1 ) std::copy( other.m_size2, other.m_size2 + (Dims-1), m_size2 );
  }

  virtual ~Array( void )
  {}

  virtual Array &clear( void )
  {
    Base::clear();
    if( Dims > 0 ) std::fill( m_size , m_size + Dims   , 0 );
    if( Dims > 1 ) std::fill( m_size2, m_size2+(Dims-1), 0 );
    return (*this);
  }

  virtual index_t        getDims( void ) const
  {
    return Dims;
  }

  virtual Type           getType( void ) const
  {
    return TypeOf< Element >::E;
  }

  virtual Size           getSize( void ) const
  {
    return Size( Dims, m_size );
  }

  virtual Array &resize_v( index_t dims, const index_t *new_size )
  {
    if( dims != Dims )
    {
      throw ArrayBase::DimensionsMismatch( dims, Dims )
        << ": Array resize failed"
        ;
    }
    if( Dims > 0 )
    {
      std::copy( new_size, new_size+Dims, m_size );
      return calc_size2_resize();
    }
    else
    {
      return (*this);
    }
  }

  virtual Array &resize( const Size &s )
  {
    return resize_v( s.getDims(), s.getCoords() );
  }

  virtual Array &resize( index_t x, index_t y=1, index_t z=1, index_t t=1 )
  {
    switch( Dims )
    {
      case 0: if( x != 1 ) throw Exception() << "If Dims is 0, x must be 1, is " << x; // fallthrough
      case 1: if( y != 1 ) throw Exception() << "If Dims less than 2, y must be 1, is " << y; // fallthrough
      case 2: if( z != 1 ) throw Exception() << "If Dims less than 3, z must be 1, is " << z; // fallthrough
      case 3: if( t != 1 ) throw Exception() << "If Dims less than 4, t must be 1, is " << t; // fallthrough
      default: if( Dims >= 4 ) throw Exception() << "If Dims is greater than 4, resize( x, y, z, t ) must not be used.";
    }
    switch( Dims )
    {
      case 4: m_size[ 3 ] = t; // fallthrough
      case 3: m_size[ 2 ] = z; // fallthrough
      case 2: m_size[ 1 ] = y; // fallthrough
      case 1: m_size[ 0 ] = x; // fallthrough
      default: {}
    }
    return calc_size2_resize();
  }

  virtual CountPtr< ArrayBase > clone( void ) const
  {
    return new Array( *this );
  }

  bool inRange( index_t x, index_t y=0, index_t z=0, index_t t=0 ) const
  {
    if( Dims >= 4 ) throw Exception() << "If Dims is greater than 4, inRange( x, y, z, t ) must not be used.";
    switch( Dims )
    {
      case 4: if( t >= m_size[ 3 ] ) return false; // fallthrough
      case 3: if( z >= m_size[ 2 ] ) return false; // fallthrough
      case 2: if( y >= m_size[ 1 ] ) return false; // fallthrough
      case 1: if( x >= m_size[ 0 ] ) return false; // fallthrough
      default: return true;
    }
    return true;
  }

  bool inRange_v( index_t dims, const index_t *x ) const
  {
    if( dims != Dims )
    {
      throw ArrayBase::DimensionsMismatch( dims, Dims )
        << "Array range check failed"
        ;
    }
    for( index_t i=0; i<Dims; ++i )
    {
      if( x[ i ] >= m_size[ i ] ) return false;
    }
    return true;
  }

  index_t getPos( index_t x, index_t y=0, index_t z=0, index_t t=0 ) const
  {
  #ifndef NDEBUG
    switch( Dims )
    {
      case 0: if( x != 0 ) throw Exception() << "If Dims is 0, x must be 0."; // fallthrough
      case 1: if( y != 0 ) throw Exception() << "If Dims less than 2, y must be 0."; // fallthrough
      case 2: if( z != 0 ) throw Exception() << "If Dims less than 3, z must be 0."; // fallthrough
      case 3: if( t != 0 ) throw Exception() << "If Dims less than 4, t must be 0."; // fallthrough
      default: if( Dims >= 4 ) throw Exception() << "If Dims is greater than 4, at( x, y, z, t ) must not be used.";
    }
    switch( Dims )
    {
      case 4: if( t > m_size[ 3 ] ) throw Exception() << "Coordinate t out of range: max " << m_size[ 3 ] << ", is " << t;
      case 3: if( z > m_size[ 2 ] ) throw Exception() << "Coordinate z out of range: max " << m_size[ 2 ] << ", is " << z;
      case 2: if( y > m_size[ 1 ] ) throw Exception() << "Coordinate y out of range: max " << m_size[ 1 ] << ", is " << y;
      case 1: if( x > m_size[ 0 ] ) throw Exception() << "Coordinate x out of range: max " << m_size[ 0 ] << ", is " << x;
      default: {}
    }
  #endif
    index_t pos = 0;
    switch( Dims )
    {
      case 4: pos += m_size2[ 2 ] * t ; // fallthrough
      case 3: pos += m_size2[ 1 ] * z; // fallthrough
      case 2: pos += m_size2[ 0 ] * y; // fallthrough
      case 1: pos += x; // fallthrough
      default: {}
    }

    return pos;
  }

  index_t getPos_v( index_t dims, const index_t *x ) const
  {
  #ifndef NDEBUG
    if( Dims != dims )
    {
      throw ArrayBase::DimensionsMismatch( dims, Dims )
        << ": Getting Array element position failed"
        ;
    }
    for( index_t i=0; i<Dims; ++i )
    {
      if( x[ i ] >= m_size[ i ] )
      {
        throw OutOfRange( getSize(), Coordinates( dims, x ) )
          << ": Getting Array element position failed"
          ;
      }
    }
  #endif
    index_t pos = x[ 0 ];
    for( index_t i=1; i<Dims; ++i )
    {
      pos += m_size2[ i-1 ] * x[ i ];
    }
    return pos;
  }

  reference at( index_t x, index_t y=0, index_t z=0, index_t t=0 )
  {
    return Base::elements_at( getPos( x, y, z, t ) );
  }

  virtual reference at_v( index_t dims, const index_t *x )
  {
    return Base::elements_at( getPos_v( dims, x ) );
  }

  const_reference at( index_t x, index_t y=0, index_t z=0, index_t t=0 ) const
  {
    return Base::elements_at( getPos( x, y, z, t ) );
  }

  virtual const_reference at_v( index_t dims, const index_t *x ) const
  {
    return Base::elements_at( getPos_v( dims, x ) );
  }

  reference operator[]( index_t x )
  {
    assert( Dims == 1 );
    return at( x );
  }

  const_reference operator[]( index_t x ) const
  {
    assert( Dims == 1 );
    return at( x );
  }

  reference operator*( void )
  {
    assert( Dims == 0 );
    return Base::elements_at( 0 );
  }

  const_reference operator*( void ) const
  {
    assert( Dims == 0 );
    return Base::elements_at( 0 );
  }

  void swap( Array &other )
  {
    Base::swap( other );
    for( index_t i=0; i<Dims; ++i )
    {
      std::swap( m_size[ i ], other.m_size[ i ] );
    }
    for( index_t i=0; i<Dims-1; ++i )
    {
      std::swap( m_size2[ i ], other.m_size2[ i ] );
    }
  }

  void push_back( const Element &x )
  {
    if( Dims != 1 ) throw Exception() << "push_back() only makes sense on 1D-Arrays, is " << Dims;
    ++m_size[ 0 ];
    Base::_push_back( x );
  }

  void pop_back( void )
  {
    if( Dims != 1 ) throw Exception() << "pop_back() only makes sense on 1D-Arrays, is " << Dims;
    if( m_size[ 0 ] > 0 )
    {
      --m_size[ 0 ];
      Base::_pop_back();
    }
  }

  virtual CountPtr< ValueIterator > getValueIterator( void ) const
  {
    return new _ValueIterator( this );
  }

  virtual CountPtr< CoordinatesIterator > getCoordinatesIterator( void ) const
  {
    return new _CoordinatesIterator( this );
  }

  virtual Value getValue( index_t x=0, index_t y=0, index_t z=0, index_t t=0 ) const
  {
    if( inRange( x, y, z, t ) )
    {
      return CreateValue< Element >::doit( at( x, y, z, t ) );
    }
    else
    {
      const index_t xyzt[ 4 ] = { x, y, z, t };
      throw OutOfRange( getSize(), Coordinates( Dims, xyzt ) );
    }
  }

  virtual Value getValue_v( index_t dims, const index_t *x ) const
  {
    if( inRange_v( dims, x ) )
    {
      return CreateValue< Element >::doit( at_v( dims, x ) );
    }
    else
    {
      throw OutOfRange( getSize(), Coordinates( dims, x ) );
    }
  }

  virtual Value getValue( const Coordinates &x ) const
  {
    return getValue_v( x.getDims(), x );
  }

  virtual void setValue( const Value &value, index_t x=0, index_t y=0, index_t z=0, index_t t=0 )
  {
    if( inRange( x, y, z, t ) )
    {
      at( x, y, z, t ) = value.get< Element >();
    }
    else
    {
      const index_t xyzt[ 4 ] = { x, y, z, t };
      throw OutOfRange( getSize(), Coordinates( Dims, xyzt ) );
    }
  }

  virtual void setValue_v( const Value &value, index_t dims, const index_t *x )
  {
    if( inRange_v( dims, x ) )
    {
      at_v( dims, x ) = value.get< Element >();
    }
    else
    {
      throw OutOfRange( getSize(), Coordinates( dims, x ) );
    }
  }

  virtual void setValue( const Value &value, const Coordinates &x )
  {
    setValue_v( value, x.getDims(), x );
  }

  virtual void fillValue( const Value &value )
  {
    Base::fill( value.get< Element >() );
  }

  typedef Iterator< Element& > Elements;
  typedef Iterator< const Element& > ConstElements;
  CountPtr< Elements      > getElements( void ) { return _Elements( this ); }
  CountPtr< ConstElements > getElements( void ) const { return _ConstElements( this ); }
  CountPtr< ConstElements > getElementsConst( void ) const { return getElements(); }

private:
  reference _elements_at( index_t i )
  {
    return Base::elements_at( i );
  }

  const_reference _elements_at( index_t i ) const
  {
    return Base::elements_at( i );
  }

  class _Elements;
  class _ConstElements;
  friend class _Children;
  friend class _Elements;
  friend class _ConstElements;

  class _ConstElements : public ConstElements
  {
  public:
    _ConstElements( const Array *array, index_t i=0 ) : m_array( array ), m_i( i ) {}
    virtual ~_ConstElements( void ) {}

    virtual bool done( void ) { return ( m_i >= m_array->size() ); }
    virtual void next( void ) { ++m_i; }
    virtual const Element &get( void ) { return m_array->_elements_at( m_i ); }
    virtual CountPtr< ConstElements > clone( void ) const { return new _ConstElements( *this ); }

  private:
    CountPtr< const Array > m_array;
    index_t m_i;
  };

  class _Elements : public Elements
  {
  public:
    _Elements( Array *array, index_t i=0 ) : m_array( array ), m_i( i ) {}
    virtual ~_Elements( void ) {}

    virtual bool done( void ) { return ( m_i >= m_array->size() ); }
    virtual void next( void ) { ++m_i; }
    virtual Element &get( void ) { return m_array->_elements_at( m_i ); }
    virtual CountPtr< Elements > clone( void ) const { return new _Elements( *this ); }

  private:
    CountPtr< Array > m_array;
    index_t m_i;
  };

  class _ValueIterator : public ValueIterator
  {
  public:
    explicit
    _ValueIterator( const Array *array, index_t i=0 ) : m_array( array ), m_i( i ) {}
    virtual ~_ValueIterator( void ) {}

    virtual bool done( void ) { return ( m_i >= m_array->size() ); }
    virtual void next( void ) { ++m_i; }
    virtual Value get( void ) { return CreateValue< Element >::doit( m_array->_elements_at( m_i ) ); }
    virtual CountPtr< ValueIterator > clone( void ) const { return new _ValueIterator( *this ); }

  private:
    CountPtr< const Array > m_array;
    index_t m_i;
  };

  class _CoordinatesIterator : public CoordinatesIterator
  {
  public:
    explicit
    _CoordinatesIterator( const Array *array )
    : m_array( array )
    , m_size( array->getSize() )
    , m_coord( array->getDims()+1 )
    {}
    virtual ~_CoordinatesIterator( void ) {}

    virtual bool done( void )
    {
      const int dims = m_size.getDims();
      return ( m_coord[ dims ] > 0 );
    }

    virtual void next( void )
    {
      const int dims = m_size.getDims();
      for( int d = 0; d < dims; ++d )
      {
        ++m_coord[ d ];
        if( m_coord[ d ] < m_size[ d ] )
        {
          return;
        }
        else
        {
          m_coord[ d ] = 0;
        }
      }
      ++m_coord[ dims ];
    }

    virtual Coordinates get( void ) { return Coordinates( m_size.getDims(), &m_coord[ 0 ] ); }
    virtual CountPtr< CoordinatesIterator > clone( void ) const { return new _CoordinatesIterator( *this ); }

  private:
    CountPtr< const Array > m_array;
    const Size m_size;
    std::vector< index_t > m_coord;
  };

  Array &calc_size2_resize( void )
  {
    if( Dims > 1 )
    {
      m_size2[ 0 ] = m_size[ 0 ];
      for( index_t dim = 1; dim < Dims-1; ++dim )
      {
        m_size2[ dim ] = m_size2[ dim-1 ] * m_size[ dim ];
      }
      Base::_resize( m_size2[ Dims-2 ] * m_size[ Dims-1 ] );
    }
    else
    {
      Base::_resize( m_size[ Dims-1 ] );
    }
    return (*this);
  }

  index_t m_size[ Dims ];
  index_t m_size2[ ( Dims > 0 ? Dims-1 : 0 ) ];
};

template< class Element, int Dims >
CountPtr< Array< Element, Dims > > new_Array( GarbageCollector *gc, const Element *fill_value=0 )
{
  CountPtr< Array< Element, Dims > > ret = new Array< Element, Dims >( gc );

  if( fill_value ) ret->fill( *fill_value );

  return ret;
}

template< class Element >
CountPtr< ArrayElements< Element > > new_Array( GarbageCollector *gc, int dims, const Element *fill_value=0 )
{
  switch( dims )
  {
    case 0: return new_Array< Element, 0 >( gc, fill_value );
    case 1: return new_Array< Element, 1 >( gc, fill_value );
    case 2: return new_Array< Element, 2 >( gc, fill_value );
    case 3: return new_Array< Element, 3 >( gc, fill_value );
    case 4: return new_Array< Element, 4 >( gc, fill_value );
    default:
      throw ArrayBase::Exception()
        << "Only dimensions for 0 to 4 supported for now, tried " << dims
        ;
  }
}

template< class Element >
CountPtr< ArrayElements< Element > > new_Array( GarbageCollector *gc, int dims, const Value &fill_value )
{
  const Type element_type( TypeOf< Element >::E );
  if( element_type.isOther() )
  {
    throw ArrayBase::Exception() << "Cannot create an Array of 'other' custom type";
  }

  if( !fill_value.isNil() )
  {
    const Value fill_value_cast = fill_value.save_cast( element_type );
    const Element fill_element = fill_value_cast.get< Element >();
    return new_Array< Element >( gc, dims, &fill_element );
  }
  return new_Array< Element >( gc, dims );
}

typedef ArrayElements< bool                       >     BoolArrayElements;
typedef ArrayElements< uint8_t                    >    UInt8ArrayElements;
typedef ArrayElements< int8_t                     >     Int8ArrayElements;
typedef ArrayElements< uint16_t                   >   UInt16ArrayElements;
typedef ArrayElements< int16_t                    >    Int16ArrayElements;
typedef ArrayElements< uint32_t                   >   UInt32ArrayElements;
typedef ArrayElements< int32_t                    >    Int32ArrayElements;
typedef ArrayElements< uint64_t                   >   UInt64ArrayElements;
typedef ArrayElements< int64_t                    >    Int64ArrayElements;
typedef ArrayElements< float                      >    FloatArrayElements;
typedef ArrayElements< double                     >   DoubleArrayElements;
typedef ArrayElements< String                     >   StringArrayElements;
typedef ArrayElements< CountPtr< Frame          > >    FrameArrayElements;
typedef ArrayElements< CountPtr< Function       > > FunctionArrayElements;
typedef ArrayElements< CountPtr< Node           > >     NodeArrayElements;
typedef ArrayElements< CountPtr< Output         > >   OutputArrayElements;
typedef ArrayElements< CountPtr< Input          > >    InputArrayElements;
typedef ArrayElements< CountPtr< Param          > >    ParamArrayElements;
typedef ArrayElements< CountPtr< const Sequence > > SequenceArrayElements;
typedef ArrayElements< CountPtr< ArrayBase      > >    ArrayArrayElements;
typedef ArrayElements< int                        >      IntArrayElements;

typedef Array< bool                      , 0 >     BoolArray0D;
typedef Array< uint8_t                   , 0 >    UInt8Array0D;
typedef Array< int8_t                    , 0 >     Int8Array0D;
typedef Array< uint16_t                  , 0 >   UInt16Array0D;
typedef Array< int16_t                   , 0 >    Int16Array0D;
typedef Array< uint32_t                  , 0 >   UInt32Array0D;
typedef Array< int32_t                   , 0 >    Int32Array0D;
typedef Array< uint64_t                  , 0 >   UInt64Array0D;
typedef Array< int64_t                   , 0 >    Int64Array0D;
typedef Array< float                     , 0 >    FloatArray0D;
typedef Array< double                    , 0 >   DoubleArray0D;
typedef Array< String                    , 0 >   StringArray0D;
typedef Array< CountPtr< Frame          >, 0 >    FrameArray0D;
typedef Array< CountPtr< Function       >, 0 > FunctionArray0D;
typedef Array< CountPtr< Node           >, 0 >     NodeArray0D;
typedef Array< CountPtr< Output         >, 0 >   OutputArray0D;
typedef Array< CountPtr< Input          >, 0 >    InputArray0D;
typedef Array< CountPtr< Param          >, 0 >    ParamArray0D;
typedef Array< CountPtr< const Sequence >, 0 > SequenceArray0D;
typedef Array< CountPtr< ArrayBase      >, 0 >    ArrayArray0D;
typedef Array< int                       , 0 >      IntArray0D;

typedef Array< bool                      , 1 >     BoolArray1D;
typedef Array< uint8_t                   , 1 >    UInt8Array1D;
typedef Array< int8_t                    , 1 >     Int8Array1D;
typedef Array< uint16_t                  , 1 >   UInt16Array1D;
typedef Array< int16_t                   , 1 >    Int16Array1D;
typedef Array< uint32_t                  , 1 >   UInt32Array1D;
typedef Array< int32_t                   , 1 >    Int32Array1D;
typedef Array< uint64_t                  , 1 >   UInt64Array1D;
typedef Array< int64_t                   , 1 >    Int64Array1D;
typedef Array< float                     , 1 >    FloatArray1D;
typedef Array< double                    , 1 >   DoubleArray1D;
typedef Array< String                    , 1 >   StringArray1D;
typedef Array< CountPtr< Frame          >, 1 >    FrameArray1D;
typedef Array< CountPtr< Function       >, 1 > FunctionArray1D;
typedef Array< CountPtr< Node           >, 1 >     NodeArray1D;
typedef Array< CountPtr< Output         >, 1 >   OutputArray1D;
typedef Array< CountPtr< Input          >, 1 >    InputArray1D;
typedef Array< CountPtr< Param          >, 1 >    ParamArray1D;
typedef Array< CountPtr< const Sequence >, 1 > SequenceArray1D;
typedef Array< CountPtr< ArrayBase      >, 1 >    ArrayArray1D;
typedef Array< int                       , 1 >      IntArray1D;

typedef Array< bool                      , 2 >     BoolArray2D;
typedef Array< uint8_t                   , 2 >    UInt8Array2D;
typedef Array< int8_t                    , 2 >     Int8Array2D;
typedef Array< uint16_t                  , 2 >   UInt16Array2D;
typedef Array< int16_t                   , 2 >    Int16Array2D;
typedef Array< uint32_t                  , 2 >   UInt32Array2D;
typedef Array< int32_t                   , 2 >    Int32Array2D;
typedef Array< uint64_t                  , 2 >   UInt64Array2D;
typedef Array< int64_t                   , 2 >    Int64Array2D;
typedef Array< float                     , 2 >    FloatArray2D;
typedef Array< double                    , 2 >   DoubleArray2D;
typedef Array< String                    , 2 >   StringArray2D;
typedef Array< CountPtr< Frame          >, 2 >    FrameArray2D;
typedef Array< CountPtr< Function       >, 2 > FunctionArray2D;
typedef Array< CountPtr< Node           >, 2 >     NodeArray2D;
typedef Array< CountPtr< Output         >, 2 >   OutputArray2D;
typedef Array< CountPtr< Input          >, 2 >    InputArray2D;
typedef Array< CountPtr< Param          >, 2 >    ParamArray2D;
typedef Array< CountPtr< const Sequence >, 2 > SequenceArray2D;
typedef Array< CountPtr< ArrayBase      >, 2 >    ArrayArray2D;
typedef Array< int                       , 2 >      IntArray2D;

typedef Array< bool                      , 3 >     BoolArray3D;
typedef Array< uint8_t                   , 3 >    UInt8Array3D;
typedef Array< int8_t                    , 3 >     Int8Array3D;
typedef Array< uint16_t                  , 3 >   UInt16Array3D;
typedef Array< int16_t                   , 3 >    Int16Array3D;
typedef Array< uint32_t                  , 3 >   UInt32Array3D;
typedef Array< int32_t                   , 3 >    Int32Array3D;
typedef Array< uint64_t                  , 3 >   UInt64Array3D;
typedef Array< int64_t                   , 3 >    Int64Array3D;
typedef Array< float                     , 3 >    FloatArray3D;
typedef Array< double                    , 3 >   DoubleArray3D;
typedef Array< String                    , 3 >   StringArray3D;
typedef Array< CountPtr< Frame          >, 3 >    FrameArray3D;
typedef Array< CountPtr< Function       >, 3 > FunctionArray3D;
typedef Array< CountPtr< Node           >, 3 >     NodeArray3D;
typedef Array< CountPtr< Output         >, 3 >   OutputArray3D;
typedef Array< CountPtr< Input          >, 3 >    InputArray3D;
typedef Array< CountPtr< Param          >, 3 >    ParamArray3D;
typedef Array< CountPtr< const Sequence >, 3 > SequenceArray3D;
typedef Array< CountPtr< ArrayBase      >, 3 >    ArrayArray3D;
typedef Array< int                       , 3 >      IntArray3D;

typedef Array< bool                      , 4 >     BoolArray4D;
typedef Array< uint8_t                   , 4 >    UInt8Array4D;
typedef Array< int8_t                    , 4 >     Int8Array4D;
typedef Array< uint16_t                  , 4 >   UInt16Array4D;
typedef Array< int16_t                   , 4 >    Int16Array4D;
typedef Array< uint32_t                  , 4 >   UInt32Array4D;
typedef Array< int32_t                   , 4 >    Int32Array4D;
typedef Array< uint64_t                  , 4 >   UInt64Array4D;
typedef Array< int64_t                   , 4 >    Int64Array4D;
typedef Array< float                     , 4 >    FloatArray4D;
typedef Array< double                    , 4 >   DoubleArray4D;
typedef Array< String                    , 4 >   StringArray4D;
typedef Array< CountPtr< Frame          >, 4 >    FrameArray4D;
typedef Array< CountPtr< Function       >, 4 > FunctionArray4D;
typedef Array< CountPtr< Node           >, 4 >     NodeArray4D;
typedef Array< CountPtr< Output         >, 4 >   OutputArray4D;
typedef Array< CountPtr< Input          >, 4 >    InputArray4D;
typedef Array< CountPtr< Param          >, 4 >    ParamArray4D;
typedef Array< CountPtr< const Sequence >, 4 > SequenceArray4D;
typedef Array< CountPtr< ArrayBase      >, 4 >    ArrayArray4D;
typedef Array< int                       , 4 >      IntArray4D;

} // namespace RPGML

namespace std {

template< class Element, RPGML::index_t Dims >
inline
void swap( RPGML::Array< Element, Dims > &x1, RPGML::Array< Element, Dims > &x2 )
{
  x1.swap( x2 );
}

inline
std::ostream &operator<<( std::ostream &o, const RPGML::ArrayBase::Coordinates &x )
{
  const int dims = x.getDims();
  if( dims < 1 ) return o;
  o << "[ ";
  for( int i=0; i<dims; ++i )
  {
    if( i > 0 ) o << ", ";
    o << x[ i ];
  }
  o << " ]";
  return o;
}

} // namespace std


#endif


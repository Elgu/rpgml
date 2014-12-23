#ifndef RPGML_Atomic_h
#define RPGML_Atomic_h

namespace RPGML {

template< class T >
class Atomic
{
public:
  Atomic( void )
  : m_value( T() )
  {}

  Atomic( const Atomic &other )
  : m_value( other.m_value )
  {}

  Atomic( T value )
  : m_value( value )
  {}

  ~Atomic( void )
  {}

  Atomic &operator=( T value )
  {
    while( !compare_and_swap( m_value, value ) ) {}
    return (*this);
  }

  T get( void ) const { return m_value; }
  operator T( void ) const { return get(); }

  //! prefix
  T operator++( void )
  {
    const T ret = __sync_fetch_and_add( &m_value, 1 );
    return ret + 1;
  }

  //! suffix
  T operator++( int )
  {
    const T ret = __sync_fetch_and_add( &m_value, 1 );
    return ret;
  }

  //! prefix
  T operator--( void )
  {
    const T ret = __sync_fetch_and_sub( &m_value, 1 );
    return ret - 1;
  }

  //! suffix
  T operator--( int )
  {
    const T ret = __sync_fetch_and_sub( &m_value, 1 );
    return ret;
  }

  T operator+=( T x )
  {
    const T ret = __sync_fetch_and_add( &m_value, x );
    return ret + x;
  }

  T operator-=( T x )
  {
    const T ret = __sync_fetch_and_sub( &m_value, x );
    return ret - x;
  }

  T operator|=( T x )
  {
    const T ret = __sync_fetch_and_or( &m_value, x );
    return ret | x;
  }

  T operator&=( T x )
  {
    const T ret = __sync_fetch_and_and( &m_value, x );
    return ret & x;
  }

  T operator^=( T x )
  {
    const T ret = __sync_fetch_and_xor( &m_value, x );
    return ret ^ x;
  }

  bool compare_and_swap( T oldval, T newval )
  {
    return __sync_bool_compare_and_swap( &m_value, oldval, newval );
  }

private:
  T m_value;
};


} // namespace RPGML

#endif


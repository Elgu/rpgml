#ifndef RPGML_Guard_h
#define RPGML_Guard_h

namespace RPGML {

template< typename _Guarded, typename _DestructorRet=void >
class Guard
{
public:
  typedef _Guarded Guarded;
  typedef _DestructorRet DestructorRet;
  typedef DestructorRet Destructor( Guarded * );

  Guard( void )
  : m_guarded( 0 )
  , m_destructor( 0 )
  {}

  explicit
  Guard( Guarded *guarded, Destructor *destructor )
  : m_guarded( guarded )
  , m_destructor( destructor )
  {}

  ~Guard( void )
  {
    clear();
  }

  void set( Guarded *guarded, Destructor *destructor = 0 )
  {
    clear();
    m_guarded = guarded;
    if( destructor ) m_destructor = destructor;
  }

  Guarded *get( void ) const
  {
    return m_guarded;
  }

  operator Guarded*( void ) const
  {
    return get();
  }

  Guarded *operator->( void ) const
  {
    return get();
  }

  Guarded *&set_direct( Destructor *destructor = 0 )
  {
    clear();
    m_guarded = 0;
    if( destructor ) m_destructor = destructor;
    return m_guarded;
  }

  void clear( void )
  {
    if( m_guarded && m_destructor )
    {
      m_destructor( m_guarded );
    }
    m_guarded = 0;
    // destructor is kept
  }

  bool isNull( void ) const
  {
    return !m_guarded;
  }

protected:
  void setDestructor( Destructor *destructor )
  {
    m_destructor = destructor;
  }

public:
  Guarded *m_guarded;
  Destructor *m_destructor;

};

} // namespace RPGML

#endif


#ifndef RPGML_SharedObject_h
#define RPGML_SharedObject_h

#include "String.h"
#include <string>

namespace RPGML {

  class SharedObject : public Refcounted
  {
  public:
    explicit SharedObject( const String      &so, String &err );
    explicit SharedObject( const char        *so, String &err );
    explicit SharedObject( const std::string &so, String &err );

    virtual ~SharedObject( void );

    bool isValid( void ) const;

    template< class SymbolType >
    bool getSymbol( const char *sym_id, SymbolType *&sym, String &err ) const
    {
      sym = (SymbolType*)getVoidPtrSymbol( sym_id, err );
      return ( sym != NULL );
    }

    const String &getSO( void ) const { return m_so; }

  private:
    const String m_so;
    void *getVoidPtrSymbol( const char *sym, String &err ) const;
    void *const m_handle;
  };

} // namespace RPGML

#endif


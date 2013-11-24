#ifndef RPGML_SharedObject_h
#define RPGML_SharedObject_h

#include "String.h"
#include <string>

namespace RPGML {

  class SharedObject : public Recounted
  {
  public:
    explicit SharedObject( const String      &so, String &err );
    explicit SharedObject( const char        *so, String &err );
    explicit SharedObject( const std::string &so, String &err );

    virtual ~SharedObject( void );

    bool isValid( void ) const;

    template< class SymbolType >
    class Symbol : public RefCounted
    {
      friend class SharedObject;
    private:
      Symbol( const SharedObject *so, SymbolType *sym )
      : m_so( so ), m_sym( sym )
      {}
    public:
      virtual ~Symbol( void ) {}
      SymbolType *get( void ) const { return m_sym; }
    private:
      CountPtr< const SharedObject > m_so;
      SymbolType *m_sym;
    };

    template< class SymbolType >
    CountPtr< Symbol< SymbolType > > getSymbol( const char *sym, String &err ) const
    {
      return new Symbol< SymbolType >( this, (SymbolType*)getVoidPtrSymbol( sym, err );
    }

  private:
    void *getVoidPtrSymbol( const char *sym, String &err ) const
    void *m_handle;
  };

} // namespace RPGML

#endif


#ifndef RPGML_Data_h
#define RPGML_Data_h

#include "GarbageCollector.h"

#include <typeinfo>

namespace RPGML {

class Data : public Collectable
{
  typedef Collectable Base;
public:
  explicit
  Data( GarbageCollector *_gc )
  : Base( _gc )
  {}

  Data( const Data &other )
  : Base( other )
  {}

  virtual ~Data( void ) {}

  // not implemented: virtual void gc_clear( void )
  // not implemented: virtual void gc_getChildren( Children &children ) const

  template< class DataType >
  inline
  DataType *getAs( DataType* &as )
  {
    as = dynamic_cast< DataType* >( this );
    return as;
  }

  template< class DataType >
  inline
  const DataType *getAs( const DataType* &as ) const
  {
    as = dynamic_cast< const DataType* >( this );
    return as;
  }

  virtual const std::type_info &getTypeInfo( void ) const = 0;
};

} // namespace RPGML

#endif


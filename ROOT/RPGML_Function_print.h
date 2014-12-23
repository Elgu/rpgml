#ifndef RPGML_Function_print_h
#define RPGML_Function_print_h

#include <RPGML/Function.h>

namespace RPGML {

class Function_print : public Function
{
public:
  Function_print( GarbageCollector *gc, Frame *parent, const SharedObject *so=0 );
  virtual ~Function_print( void );

  virtual bool call_impl( const Location *loc, index_t recursion_depth, Scope *scope, Value &ret, index_t n_args, const Value *args );
  virtual const char *getName( void ) const { return "print"; }

private:
  static
  CountPtr< Args > genDeclArgs( void );
};

} // namespace RPGML

#endif


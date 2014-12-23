#ifndef RPGML_SaveBool_h
#define RPGML_SaveBool_h

namespace RPGML {

class SaveBool
{
public:
  typedef void (SaveBool::*bool_type)() const;

  static
  bool_type toSaveBool( bool b )
  {
    return ( b ? &SaveBool::bool_type_method : 0 );
  }

private:
  void bool_type_method( void ) const {}
};

} // namespace RPGML

#endif


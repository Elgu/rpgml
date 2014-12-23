#ifndef RPGML_FileSource_h
#define RPGML_FileSource_h

#include "Scanner.h"

#include <cstdio>

namespace RPGML {

class FileSource : public Source
{
public:
  FileSource( FILE *file )
  : m_file( file )
  {}

  virtual ~FileSource( void )
  {
    fclose( m_file );
  }

protected:
  virtual char nextChar( void )
  {
    const int ret = fgetc( m_file );
    if( EOF == ret )
    {
      return '\0';
    }
    return char( ret );
  }

private:
  FILE *m_file;
};

} // namespace RPGML

#endif


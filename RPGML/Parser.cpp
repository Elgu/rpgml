#include "Parser.h"

namespace RPGML {

Parser::Parser( StringUnifier *_unifier, Source *_source )
: Scanner( _unifier, _source )
, _Parser( getScanner() )
{}

Parser::~Parser( void )
{}

void _Parser::error( location const &loc, std::string const &msg )
{
  std::cerr << loc << ":" << msg << std::endl;
}

} // namespace RPGML


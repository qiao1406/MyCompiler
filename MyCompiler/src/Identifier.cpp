#include "Identifier.h"

Identifier::Identifier( string name, IdType type,  int level, int offset ) {
    this->name = name;
    this->type = type;
    this->level = level;
    this->offset = offset;
}

Identifier::~Identifier()
{
    //dtor
}

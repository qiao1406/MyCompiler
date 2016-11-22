#ifndef IDENTIFIER_H
#define IDENTIFIER_H

#include <string>

using namespace std;

enum IdType {
    INT_VAR,
    CHAR_VAR,
    FLOAT_VAR,
    INT_ARRAY,
    CHAR_ARRAY,
    FLOAT_ARRAY,
    INT_CONST,
    CHAR_CONST,
    FLOAT_CONST,
    INT_FUNCTION,
    CHAR_FUNCTION,
    FLOAT_FUNCTION,
    VOID_FUNCTION
};

class Identifier {
    public:
        Identifier(string name, IdType type,  int level, int offset );
        virtual ~Identifier();
        string name; //标识符的名字
        IdType type; //该标识符的类型
        int level; //该标识符所在的层次
        int offset; //该标识符在所在层次内的偏移量

    protected:
    private:
};

#endif // IDENTIFIER_H

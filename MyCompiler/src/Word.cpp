#include "Word.h"

Word::Word( string value, string type ) {
    this->value = value;
    this->type = type;
}

Word::Word() {

}

Word::~Word() {
}

bool Word::is_vartype ( ) {
    /*
        判断一个词是否是类型标识符
    */
    return ( this->value == "int" ) || ( this->value == "char")
                || ( this->value == "float");
}

bool Word::is_addition_op() {
    /*
        判断一个词是否为加法运算符
    */
    return ( this->value == "+" ) || ( this->value == "-" );
}

bool Word::is_multiplication_op() {
    /*
        判断一个词是否为乘法运算符
    */
    return ( this->value == "*" ) || ( this->value == "/" );
}

bool Word::is_relation_op() {
    /*
        判断一个词是否为关系运算符
    */
    return ( this->value == "<=" ) || ( this->value == "<" )
            || ( this->value == ">=" ) || ( this->value == ">" )
            || ( this->value == "==" ) || ( this->value == "!=" );
}

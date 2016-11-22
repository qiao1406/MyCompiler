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
        �ж�һ�����Ƿ������ͱ�ʶ��
    */
    return ( this->value == "int" ) || ( this->value == "char")
                || ( this->value == "float");
}

bool Word::is_addition_op() {
    /*
        �ж�һ�����Ƿ�Ϊ�ӷ������
    */
    return ( this->value == "+" ) || ( this->value == "-" );
}

bool Word::is_multiplication_op() {
    /*
        �ж�һ�����Ƿ�Ϊ�˷������
    */
    return ( this->value == "*" ) || ( this->value == "/" );
}

bool Word::is_relation_op() {
    /*
        �ж�һ�����Ƿ�Ϊ��ϵ�����
    */
    return ( this->value == "<=" ) || ( this->value == "<" )
            || ( this->value == ">=" ) || ( this->value == ">" )
            || ( this->value == "==" ) || ( this->value == "!=" );
}

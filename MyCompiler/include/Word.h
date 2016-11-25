#ifndef WORD_H
#define WORD_H

#include <string>

using namespace std;

class Word {
    public:
        Word ( string value, string type );
        Word ( string value, string type, int loc );
        Word();
        string value; //词的值
        string type; //词的类型
        int loc; // 词所在行值

        bool is_vartype();
        bool is_relation_op();
        bool is_addition_op();
        bool is_multiplication_op();

    protected:
    private:
};

#endif // WORD_H

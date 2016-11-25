#ifndef WORD_H
#define WORD_H

#include <string>

using namespace std;

class Word {
    public:
        Word ( string value, string type );
        Word ( string value, string type, int loc );
        Word();
        string value; //�ʵ�ֵ
        string type; //�ʵ�����
        int loc; // ��������ֵ

        bool is_vartype();
        bool is_relation_op();
        bool is_addition_op();
        bool is_multiplication_op();

    protected:
    private:
};

#endif // WORD_H

#ifndef RUNTIME_H
#define RUNTIME_H

#include <stack>

using namespace std;

enum data_type {
    RS_INT,
    RS_FLOAT,
    RS_CHAR,
    RS_STR
};

struct run_stack{
    data_type type;
    int value;
};

class Runtime {
    public:
        static int get_top_value();
        static data_type get_top_type();
        static void pop_rs();

    private:
        static stack<run_stack> runtime_stack;//运行栈
        static int ret_adr;

};

#endif // RUNTIME_H

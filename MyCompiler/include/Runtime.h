#ifndef RUNTIME_H
#define RUNTIME_H

#include <stack>

enum data_type {
    RS_INT,
    RS_FLOAT,
    RS_CHAR
};

struct run_stack{
    data_type type;
    int value;
};

class Runtime {
    public:
        int get_top_value();
        data_type get_top_type();
    private:
        static stack<int> runtime_stack;//运行栈

};

#endif // RUNTIME_H

#ifndef RUNTIME_H
#define RUNTIME_H

#include <stack>
#include <vector>

#include "Table.h"

using namespace std;

const int DATA_AREA_SIZE = 10000;

enum data_type {
    RS_INT,
    RS_FLOAT,
    RS_CHAR,
    RS_STR,
    RS_INTFUN,
    RS_CHARFUN,
    RS_FLOATFUN,
    RS_VOIDFUN
};

struct run_stack{
    data_type type;
    int value;
};

class Runtime {
    public:
        static run_stack get_val ( int index );
        static int get_top_value();
        static data_type get_top_type();
        static void pop_rs();
        static void push_rs ( run_stack r );
        static void push_rs ( int n, run_stack r );
        static void set_rs ( int index, run_stack r );
        static void set_main_pointer ( int i );
        static void interpret ( vector<pcode> p );
        static void print_stack();

    private:
        static vector<run_stack> runtime_stack;//运行栈
        static stack<run_stack> data_stack;//数据栈
        static stack<int> fun_stack;//函数栈
        static stack<int> ret_adr;
        static int main_pointer;
        static int data_area[DATA_AREA_SIZE];//数据区



};

#endif // RUNTIME_H

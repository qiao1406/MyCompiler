#include "Runtime.h"

//��ʾ���ص�ַ�ļĴ���
int Runtime::ret_adr = 0;

int Runtime::get_top_value() {
    return runtime_stack.top().value;
}

data_type Runtime::get_top_type() {
    return runtime_stack.top().type;
}

void Runtime::pop_rs() {
    runtime_stack.pop();
}

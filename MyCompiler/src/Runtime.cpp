#include "Runtime.h"

int Runtime::get_top_value() {
    return runtime_stack.top().value;
}

data_type Runtime::get_top_type() {
    return runtime_stack.top().type;
}

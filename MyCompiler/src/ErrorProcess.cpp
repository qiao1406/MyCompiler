#include "ErrorProcess.h"

queue<Error> ErrorProcess::errs;

bool ErrorProcess::is_err_exist() {
    return !errs.empty();
}

void ErrorProcess::add_err ( int line, int index ) {
    // 往错误队列中添加一个错误
    errs.push( Error(line,index) );
}

void ErrorProcess::prt_errs() {
    //输出错误队列中的所有错误
    while ( !errs.empty() ) {
        errs.front().prt_err();
        errs.pop();
    }
}

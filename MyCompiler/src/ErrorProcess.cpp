#include "ErrorProcess.h"

queue<Error> ErrorProcess::errs;

void ErrorProcess::add_err ( int line, int index ) {
    // 往错误队列中添加一个错误
    errs.push( Error(line,index) );
}

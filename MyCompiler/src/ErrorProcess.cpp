#include "ErrorProcess.h"

queue<Error> ErrorProcess::errs;

void ErrorProcess::add_err ( int line, int index ) {
    // ���������������һ������
    errs.push( Error(line,index) );
}
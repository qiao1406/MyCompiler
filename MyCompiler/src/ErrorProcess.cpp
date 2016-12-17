#include "ErrorProcess.h"

queue<Error> ErrorProcess::errs;

bool ErrorProcess::is_err_exist() {
    return !errs.empty();
}

void ErrorProcess::add_err ( int line, int index ) {
    // ��������������һ������
    errs.push( Error(line,index) );
}

void ErrorProcess::prt_errs() {
    //�����������е����д���
    while ( !errs.empty() ) {
        errs.front().prt_err();
        errs.pop();
    }
}

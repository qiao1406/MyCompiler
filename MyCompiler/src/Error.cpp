#include "Error.h"

Error::errtyp_list[] = { {0,"Undified Identifier"},
                         {1,"Repeat Identifier"},
                         {2,"Should Be Identifier"},
                         {3,"Except Equal Symbol"},
                         {4,"Except Left Parenthese"},
                         {5,"Except Right Parenthese"},
                         {6,"Eccept Left Bracket"},
                         {7,"Except Right Bracket"},
                         {8,"Except Left Brace"},
                         {9,"Except Right Brace"},
                         {10,"Except Colon"},
                         {11,"Except Semicolon"},
                         {12,"Except Comma"},
                         {13,"Except Single Quotation Mark"},
                         {14,"Except Double Quotation Mark"},
                         {15,"Illegal Character"},
                         {16,"Illegal Character Constant"},
                         {17,"Illegal Constant Overflow"},
                         {18,"Should Be Constant"},
                         {19,"Should Be Variable"},
                         {20,"Memory Overflow"},
                         {21,"Unequal Arguments Amount"},
                         {22,"Unequal Arguments Type"},
                         {23,"Unequal Object Type"},
                         {24,"Except Case"},
                         {25,"Scanf Error"},
                         {26,"Wrong Return Type"},
                         {27,"Wrong Main Function Return Type"},
                         {28,"Should Be Type Symbol"},
                         {29,"Except Return Value"} };

Error::Error(int line, int index ) {
    // line是错误所在源代码的行值
    // index表示报的错误的编号
    this->location = line;
    if ( index >= 0 && index < ERROR_NUM ) {
        this->errinfo = errtyp_list[index];
    }
    else { // 未知错误
        this->errinfo = {-1,"Unkown Error"};
    }

}

void Error::prt_err() {
    //输出错误信息
    cout << "[Error " << errinfo.index << "] ";
    cout << " Line: " << location <<" -> ";
    cout << errinfo.prompt_info << endl;
}

















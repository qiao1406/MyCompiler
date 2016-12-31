#include <iostream>
#include "Word.h"
#include "WordAnalysis.h"
#include "GrammarAnalysis.h"
#include "Runtime.h"
#include "ErrorProcess.h"

using namespace std;

int main() {

    string filename;
    cout << "�������ļ���";
    cin >> filename;

    //�ʷ�����
    WordAnalysis::establish_cache(filename);
    //�﷨����
    GrammarAnalysis::ga_programme();
    Table::test_id_table();
    Table::test_arr_table();
    Table::test_rconst_table();
    Table::test_pcode_table();
    Table::test_func_table();
    Table::test_str_table();

    //����ִ��
    if ( ErrorProcess::is_err_exist() ) {
        cout << "������������ڴ���"<<endl;
        ErrorProcess::prt_errs();
    }
    else {
        cout << "�������������ʼִ��"<<endl;
        Runtime::interpret(Table::pcode_table);
        cout << "\n�������������������н���������������"<<endl;
    }

    return 0;
}

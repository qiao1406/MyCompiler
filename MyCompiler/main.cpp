#include <iostream>
#include "Word.h"
#include "WordAnalysis.h"
#include "GrammarAnalysis.h"
#include "Runtime.h"
#include "ErrorProcess.h"

using namespace std;

int main() {

    //string filename;
    //cout << "请输入文件名";
    //cin >> filename;
    //WordAnalysis::prt_analysis_res("test.txt");
    ofstream out;
    out.open("result.txt");
    //WordAnalysis::establish_cache(filename);
    WordAnalysis::establish_cache("test.txt");
    for ( int i = 0; i < WordAnalysis::linewords.size(); i++ ) {
        for ( int j = 0; j < WordAnalysis::linewords[i].size(); j++ ) {
            out << WordAnalysis::linewords[i][j].type << " ";
            out << WordAnalysis::linewords[i][j].value << " ";
            out << WordAnalysis::linewords[i][j].loc;
            out << endl;
        }
    }
    out.close();
    GrammarAnalysis::ga_programme();
    Table::test_id_table();
    Table::test_arr_table();
    Table::test_rconst_table();
    Table::test_pcode_table();
    Table::test_func_table();
    Table::test_str_table();

    if ( ErrorProcess::is_err_exist() ) {
        cout << "编译结束，存在错误"<<endl;
        ErrorProcess::prt_errs();
    }
    else {
        cout << "编译结束，程序开始执行"<<endl;
        Runtime::interpret(Table::pcode_table);
        cout << "\n——————程序运行结束——————"<<endl;
    }

    return 0;
}

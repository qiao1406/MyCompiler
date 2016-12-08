#include <iostream>
#include "Word.h"
#include "WordAnalysis.h"
#include "GrammarAnalysis.h"

using namespace std;

int main() {
    //WordAnalysis::prt_analysis_res("test.txt");
    ofstream out;
    out.open("result.txt");
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
    return 0;
}

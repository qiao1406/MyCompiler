#include <iostream>
#include "Word.h"
#include "WordAnalysis.h"
#include "Identifier.h"
#include "GrammarAnalysis.h"

using namespace std;

int main()
{
    //WordAnalysis::prt_analysis_res("test.txt");
    WordAnalysis::establish_cache("test.txt");
//    for ( int i = 0; i < WordAnalysis::linewords.size(); i++ ) {
//        for ( int j = 0; j < WordAnalysis::linewords[i].size(); j++ ) {
//            cout << WordAnalysis::linewords[i][j].type;
//            cout << WordAnalysis::linewords[i][j].value;
//            cout << endl;
//        }
//    }
    GrammarAnalysis::ga_programme();
    return 0;
}

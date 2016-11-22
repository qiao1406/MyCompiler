#ifndef WORDANALYSIS_H
#define WORDANALYSIS_H

#include <iostream>
#include <fstream>
#include <string>
#include <queue>
#include <vector>

#include "Word.h"

using namespace std;

class WordAnalysis {

    public:
        static void read_programme_code ( string file_name );
        static string getsym();
        static void prt_analysis_res (string filename);
        static void establish_cache ( string filename );

        static vector<vector<Word> > linewords;

    private:
        static bool is_letter ( char c );
        static bool is_noletter_char ( char c );
        static bool is_string ( char c );
        static string judge_type ( string name );

        static queue<string> code_strs;
};

#endif // WORDANALYSIS_H

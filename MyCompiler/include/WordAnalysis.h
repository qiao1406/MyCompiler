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
        static void establish_cache ( string filename );
        static vector<vector<Word> > linewords;

    private:
        static bool is_letter ( char c );
        static bool is_noletter_char ( char c );
        static bool is_string ( char c );
        static bool is_emptyline ( string s );
        static Word gen_word ( string name );

        static queue<string> code_strs;
        static int now_line;//记录词法分析进行到的行数
};

#endif // WORDANALYSIS_H

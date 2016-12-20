#include "WordAnalysis.h"

// 非字母字符
char noletter_char[] = {'+','-','*','/', ',',';','=','>','<','!',
                        '(',')','{','}','[',']',':'};

// 存放一些常见的符号及其名称
Word syblist[] = { Word(",","COMMA"), Word(";","SEMICOLON"), Word(":","COLON"),
            Word("+","PLUSSYM"), Word("-","SUBSYM"), Word("*","MULSYM"),
            Word("/","DIVSYM"), Word("=","BECOME"), Word("==","EQUAL"),
            Word("!=","NEQUAL"), Word("<","LESS"), Word("<=","LESSEQUAL"),
            Word(">","GREATER"), Word(">=","GREATEREQUAL"), Word("(","LPARENT"),
            Word(")","RPARENT"), Word("[","LBRACK"), Word("]","LBRACK"),
            Word("{","LBRACE"), Word("}","RBRACE"), Word("const","CONSTSYM"),
            Word("void","VOIDSYM"), Word("int","INTSYM"), Word("char","CHARSYM"),
            Word("float","FLOATSYM"), Word("return","RETURNSYM"), Word("if","IFSYM"),
            Word("else","ELSESYM"), Word("while","WHILESYM"), Word("case","CASESYM"),
            Word("switch","SWITCHSYM"), Word("scanf","SCANFSYM"),
            Word("printf","PRINTFSYM"),Word("main","MAINSYM") };

// 存放从源代码中读到的所有符号
queue<string> WordAnalysis::code_strs;
// 词法分析的缓存：存放每一行识别到的词
vector<vector<Word> > WordAnalysis::linewords;
//记录词法分析进度的标志——行值，初始值为1
int WordAnalysis::now_line = 1;

bool WordAnalysis::is_noletter_char ( char c ) {
    /*
        判断是否是非字母字符
        若是非字母字符则返回true
        不是则返回false
    */
    int size = sizeof(noletter_char)/sizeof(char);
    int i = 0;

    for ( ; i < size; ++i ) {
        if ( noletter_char[i] == c ) {
            break;
        }
    }

    return ( i<size );
}

bool WordAnalysis::is_letter ( char c ) {
    /*
        判断一个字符是否是字母
    */
    return isalpha(c) || c == '_';
}

bool WordAnalysis::is_string ( char c ) {
    /*
        判断一个字符是否是字符串的组成成分
    */
    return c == 32 || c == 33 || ( c >= 35 && c <= 126);
}

bool WordAnalysis::is_emptyline ( string s ) {
    /*
        判断一行代码是否为空行
    */

    bool res = true;

    for ( int i = 0; i < s.size(); i++ ) {
        if ( s[i] == '\t' || s[i] == '\n' || s[i] == ' ' ) {
            continue;
        }
        else {
            res = false;
            break;
        }
    }

    return res;
}

void WordAnalysis::read_programme_code ( string file_name ) {
    /*
        打开程序源代码的文件
        依次读取所有的读到的字符串存到code_strs里面
        *modifies:code_strs
    */
    string tempstr = "";
    string buff_str;
    ifstream fin;
    fin.open(file_name.c_str());

    while ( getline(fin,buff_str) ) {
        //buff_str不会读入回车的

        if ( is_emptyline(buff_str) ){
            code_strs.push("%%");
            continue;
        }
        // 逐个字符分析
        for ( int i = 0; i < buff_str.length(); i++ ) {

            if ( buff_str[i] == ' ' || buff_str[i] == '\t') {
                //跳过空格和制表符,并且除去他们
                buff_str.erase(i,1);
                i--;
                continue;
            }

            else if ( buff_str[i] == '+' || buff_str[i] == '-' ) {
                tempstr = "";

                if ( buff_str[i-1] == '>' || buff_str[i-1] == '<'
                     || buff_str[i-1] == '=' || buff_str[i-1] == ','
                     || buff_str[i-1] == '(' ) {
                    tempstr += buff_str[i];
                    i++;
                    if ( buff_str[i] == '+' || buff_str[i] == '-' ) {
                        tempstr += buff_str[i];
                        i++;
                    }
                    while ( ( buff_str[i] >= '0' && buff_str[i] <= '9' )
                                ||  buff_str[i] == '.' ) {
                        tempstr += buff_str[i];
                        i++;
                    }
                    code_strs.push(tempstr);
                    i--;

                }
                else{
                    tempstr += buff_str[i];
                    code_strs.push(tempstr);
                }

            }

            else if ( is_noletter_char(buff_str[i]) ) {
                tempstr = "";
                //非字母字符
                //cout << buff_str[i];

                if ( ((buff_str[i] == '=' && buff_str[i+1] == '='  )// ==
                    || ( buff_str[i] == '!' && buff_str[i+1] == '=')// !=
                    || ( buff_str[i] == '<' && buff_str[i+1] == '=')// <=
                    || ( buff_str[i] == '>' && buff_str[i+1] == '=') ) // >=
                    &&  i+1 < buff_str.length()
                    ) {
                    tempstr += buff_str[i];
                    tempstr += buff_str[i+1];
                    i++;
                    code_strs.push(tempstr);
                }
                else {
                    code_strs.push(tempstr+buff_str[i]);
                }

            }

            else if ( buff_str[i] == '\'') { // 有可能是字符
                //cout << "asd555666";
                tempstr = buff_str[i];
                if ( i+1 < buff_str.length() ) {
                    tempstr += buff_str[i+1];
                    if ( i+2 < buff_str.length() && buff_str[i+2] == '\'') {
                    tempstr += buff_str[i+2];
                    i+=2;
                    code_strs.push(tempstr);
                    }
                }

            }

            else if ( buff_str[i] == '"' ) { //有可能是字符串
                tempstr = "\"";
                i++;
                while ( is_string(buff_str[i])) {
                    tempstr += buff_str[i];
                    i++;
                }
                if ( buff_str[i] == '"' ) {
                    tempstr += buff_str[i];
                }
                code_strs.push(tempstr);
            }

            else {
                tempstr = "";
                do {
                    tempstr += buff_str[i];
                    i++;
                } while ( i < buff_str.length() && buff_str[i] != ' '
                       && !is_noletter_char(buff_str[i])
                       &&  buff_str[i] != '\'' );
                code_strs.push(tempstr);
                i--;
            }

        }

        // 读完一行之后，加上一个回车表示一行结束
        code_strs.push("\n");

    } // 每次循环读入源代码的一行

    fin.close();
}

string WordAnalysis::getsym () {
    /*
        给出下一个符号
        *modifies:code_strs
    */
    string s = code_strs.front();
    code_strs.pop();
    return s;
}

Word WordAnalysis::gen_word ( string name ) {
    /*
        根据名字判断类型,并且返回一个Word对象
        先在syblist里面找，若找不到再去判断其他的可能性
    */

    string res = "";//res表示的是这个词的类型
    for ( int i = 0; i < 34; i++ ) {
        if ( syblist[i].value == name ) {
            res = syblist[i].type;
            break;
        }
    }

    if ( res != "" ) {
        return Word(name,res);
    }
    else { //判断是字符串或数字，或字符，还是标识符

        if ( name.length() >= 2 && name[0] == '"' && name[name.length()-1] == '"' ) { //字符串
            res = "STRING";
        }

        else if ( name == "0" ) { //整数0
            res = "INTEGER";
        }

        else if ( name.length() == 3 && name[0] == '\'' && name[2] == '\'' ) { //字符
            res = "CHARACTER";
        }

        else if ( (name[0] >= '0' && name[0] <= '9')
                 || name[0] == '-' || name[0] == '+' ) {

            int point_num = 0;
            int i;
            bool is_neg = false;//标志该数字是否为负数的变量

            if ( name.length() >= 3 &&
                ( name[0] == '+' || name[0] == '-') &&
                ( name[1] == '+' || name[1] == '-') ) {//有两个符号的实数

                if ( name[0] == '+' && name[1] == '+' ||
                    name[0] == '-' && name[1] == '-' ) { //归类为正数

                    name.erase(0,2);
                    i = 0;
                    for ( ; i < name.length(); i++ ) {
                        if ( name[i] == '.' ) {
                            point_num++;
                        }
                        if (!( name[i] == '.' || isdigit(name[i]) ) ){
                            break; // 不是小数点或1-9 就退出
                        }
                    }

                    //不能以小数点开头
                    res = ( i == name.length() && point_num <= 1
                           && name[0] != '.')? "FLOAT":"WRONGSYMBOL";
                }
                else { //归类成负数
                    name.erase(0,1);
                    name[0] = '-';
                    i = 1;
                    for ( ; i < name.length(); i++ ) {
                        if ( name[i] == '.' ) {
                            point_num++;
                        }
                        if (!( name[i] == '.' || isdigit(name[i]) ) ){
                            break; // 不是小数点或1-9 就退出
                        }
                    }
                    //不能以小数点开头
                    res = ( i == name.length() && point_num <= 1
                           && name[1] != '.')? "FLOAT":"WRONGSYMBOL";
                }

            } else if ( name.length() >= 2 && ( name[0] == '+' || name[0] == '-')) {
                //有一个符号的整数或实数

                i = 1;
                for ( ; i < name.length(); i++ ) {
                    if ( name[i] == '.' ) {
                        point_num++;
                    }
                    if (!( name[i] == '.' || isdigit(name[i]) ) ){
                        break; // 不是小数点或1-9 就退出
                    }
                }
                //不能以小数点开头
                res = ( i == name.length() && point_num <= 1 && name[1] != '.')?
                    (( point_num == 0 )?"INTEGER":"FLOAT"):"WRONGSYMBOL";
            } else { //正常情况的整数和实数
                i = 0;
                for ( ; i < name.length(); i++ ) {
                    if ( name[i] == '.' ) {
                        point_num++;
                    }
                    if (!( name[i] == '.' || isdigit(name[i]) ) ){
                        break; // 不是小数点或1-9 就退出
                    }
                }
                res = ( i == name.length() && point_num <= 1 )?
                                (( point_num==0)?"INTEGER":"FLOAT"):"WRONGSYMBOL";
            }

        }

        else if ( is_letter(name[0]) ) { //有可能是标识符
            int i = 0;
            for ( ; i < name.length(); i++ ) {
                if ( !( isdigit(name[i]) || is_letter(name[i]) ) ) {
                    break;
                }
            }
            res = ( i == name.length() )?"IDENT":"WRONGSYMBOL";
        }

        else {
            res = "WRONGSYMBOL";
        }

        return Word(name,res);
    }

}

//void WordAnalysis::prt_analysis_res( string filename ) {
//    int order = 1;
//    ofstream fout;
//    read_programme_code(filename);
//    fout.open("result.txt");
//
//    while (!code_strs.empty()){
//        string sym = getsym();
//        string type = judge_type(sym);
//        if ( sym != "\n") {
//            cout << order << "\t" << type << "\t" << sym << endl;
//            fout << order << "\t" << type << "\t" << sym << endl;
//            order++;
//        }
//    }
//
//    fout.close();
//
//}

void WordAnalysis::establish_cache ( string filename ) {
    /*
        建立词法分析的缓存
        填充二维向量linewords
    */
    vector<Word> linetemp;
    read_programme_code(filename);

    while ( !code_strs.empty()) {
        string sym = getsym();

        if ( sym == "%%") {//空行
            now_line++;
            continue;
        }

        if ( sym == "\n" ) {
            now_line++;
            linewords.push_back(linetemp);
            linetemp.clear();
        }
        else {
            //string typ = judge_type(sym);
            Word w = gen_word(sym);
            w.loc = now_line;
            linetemp.push_back(w);
        }
    }
}

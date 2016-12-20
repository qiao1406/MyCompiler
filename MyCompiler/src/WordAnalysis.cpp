#include "WordAnalysis.h"

// ����ĸ�ַ�
char noletter_char[] = {'+','-','*','/', ',',';','=','>','<','!',
                        '(',')','{','}','[',']',':'};

// ���һЩ�����ķ��ż�������
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

// ��Ŵ�Դ�����ж��������з���
queue<string> WordAnalysis::code_strs;
// �ʷ������Ļ��棺���ÿһ��ʶ�𵽵Ĵ�
vector<vector<Word> > WordAnalysis::linewords;
//��¼�ʷ��������ȵı�־������ֵ����ʼֵΪ1
int WordAnalysis::now_line = 1;

bool WordAnalysis::is_noletter_char ( char c ) {
    /*
        �ж��Ƿ��Ƿ���ĸ�ַ�
        ���Ƿ���ĸ�ַ��򷵻�true
        �����򷵻�false
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
        �ж�һ���ַ��Ƿ�����ĸ
    */
    return isalpha(c) || c == '_';
}

bool WordAnalysis::is_string ( char c ) {
    /*
        �ж�һ���ַ��Ƿ����ַ�������ɳɷ�
    */
    return c == 32 || c == 33 || ( c >= 35 && c <= 126);
}

bool WordAnalysis::is_emptyline ( string s ) {
    /*
        �ж�һ�д����Ƿ�Ϊ����
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
        �򿪳���Դ������ļ�
        ���ζ�ȡ���еĶ������ַ����浽code_strs����
        *modifies:code_strs
    */
    string tempstr = "";
    string buff_str;
    ifstream fin;
    fin.open(file_name.c_str());

    while ( getline(fin,buff_str) ) {
        //buff_str�������س���

        if ( is_emptyline(buff_str) ){
            code_strs.push("%%");
            continue;
        }
        // ����ַ�����
        for ( int i = 0; i < buff_str.length(); i++ ) {

            if ( buff_str[i] == ' ' || buff_str[i] == '\t') {
                //�����ո���Ʊ��,���ҳ�ȥ����
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
                //����ĸ�ַ�
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

            else if ( buff_str[i] == '\'') { // �п������ַ�
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

            else if ( buff_str[i] == '"' ) { //�п������ַ���
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

        // ����һ��֮�󣬼���һ���س���ʾһ�н���
        code_strs.push("\n");

    } // ÿ��ѭ������Դ�����һ��

    fin.close();
}

string WordAnalysis::getsym () {
    /*
        ������һ������
        *modifies:code_strs
    */
    string s = code_strs.front();
    code_strs.pop();
    return s;
}

Word WordAnalysis::gen_word ( string name ) {
    /*
        ���������ж�����,���ҷ���һ��Word����
        ����syblist�����ң����Ҳ�����ȥ�ж������Ŀ�����
    */

    string res = "";//res��ʾ��������ʵ�����
    for ( int i = 0; i < 34; i++ ) {
        if ( syblist[i].value == name ) {
            res = syblist[i].type;
            break;
        }
    }

    if ( res != "" ) {
        return Word(name,res);
    }
    else { //�ж����ַ��������֣����ַ������Ǳ�ʶ��

        if ( name.length() >= 2 && name[0] == '"' && name[name.length()-1] == '"' ) { //�ַ���
            res = "STRING";
        }

        else if ( name == "0" ) { //����0
            res = "INTEGER";
        }

        else if ( name.length() == 3 && name[0] == '\'' && name[2] == '\'' ) { //�ַ�
            res = "CHARACTER";
        }

        else if ( (name[0] >= '0' && name[0] <= '9')
                 || name[0] == '-' || name[0] == '+' ) {

            int point_num = 0;
            int i;
            bool is_neg = false;//��־�������Ƿ�Ϊ�����ı���

            if ( name.length() >= 3 &&
                ( name[0] == '+' || name[0] == '-') &&
                ( name[1] == '+' || name[1] == '-') ) {//���������ŵ�ʵ��

                if ( name[0] == '+' && name[1] == '+' ||
                    name[0] == '-' && name[1] == '-' ) { //����Ϊ����

                    name.erase(0,2);
                    i = 0;
                    for ( ; i < name.length(); i++ ) {
                        if ( name[i] == '.' ) {
                            point_num++;
                        }
                        if (!( name[i] == '.' || isdigit(name[i]) ) ){
                            break; // ����С�����1-9 ���˳�
                        }
                    }

                    //������С���㿪ͷ
                    res = ( i == name.length() && point_num <= 1
                           && name[0] != '.')? "FLOAT":"WRONGSYMBOL";
                }
                else { //����ɸ���
                    name.erase(0,1);
                    name[0] = '-';
                    i = 1;
                    for ( ; i < name.length(); i++ ) {
                        if ( name[i] == '.' ) {
                            point_num++;
                        }
                        if (!( name[i] == '.' || isdigit(name[i]) ) ){
                            break; // ����С�����1-9 ���˳�
                        }
                    }
                    //������С���㿪ͷ
                    res = ( i == name.length() && point_num <= 1
                           && name[1] != '.')? "FLOAT":"WRONGSYMBOL";
                }

            } else if ( name.length() >= 2 && ( name[0] == '+' || name[0] == '-')) {
                //��һ�����ŵ�������ʵ��

                i = 1;
                for ( ; i < name.length(); i++ ) {
                    if ( name[i] == '.' ) {
                        point_num++;
                    }
                    if (!( name[i] == '.' || isdigit(name[i]) ) ){
                        break; // ����С�����1-9 ���˳�
                    }
                }
                //������С���㿪ͷ
                res = ( i == name.length() && point_num <= 1 && name[1] != '.')?
                    (( point_num == 0 )?"INTEGER":"FLOAT"):"WRONGSYMBOL";
            } else { //���������������ʵ��
                i = 0;
                for ( ; i < name.length(); i++ ) {
                    if ( name[i] == '.' ) {
                        point_num++;
                    }
                    if (!( name[i] == '.' || isdigit(name[i]) ) ){
                        break; // ����С�����1-9 ���˳�
                    }
                }
                res = ( i == name.length() && point_num <= 1 )?
                                (( point_num==0)?"INTEGER":"FLOAT"):"WRONGSYMBOL";
            }

        }

        else if ( is_letter(name[0]) ) { //�п����Ǳ�ʶ��
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
        �����ʷ������Ļ���
        ����ά����linewords
    */
    vector<Word> linetemp;
    read_programme_code(filename);

    while ( !code_strs.empty()) {
        string sym = getsym();

        if ( sym == "%%") {//����
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

#include "Table.h"
#include "Runtime.h"
#include "ErrorProcess.h"
#include "GrammarAnalysis.h"

#include <iostream>
#include <iomanip>
#include <fstream>
#include <algorithm>

//数据区地址初始化为0
int Table::data_adr = 0;
int Table::fun_adr = 0;

vector<id_rcd> Table::id_table;//符号表
vector<array_rcd> Table::array_table;//数组信息表
vector<func_rcd> Table::func_table;//函数信息表
vector<float> Table::rconst_table;//实常量表
vector<string> Table::str_table;//字符串常量表
vector<pcode> Table::pcode_table;//PCode码表

void Table::report_err ( int index ) {
    ErrorProcess::add_err(GrammarAnalysis::get_line_loc(),index);
}

bool Table::is_id_repeat ( string name, int lev ) {
    /*
        判断标识符是否重复
        重复返回true
    */
    if ( lev == 0 ) { //全局
        int i = 0;
        for ( ; i < id_table.size(); i++ ) {
            if ( id_table[i].name == name && id_table[i].lev == lev ) {
                break;
            }
        }

        return i < id_table.size();
    }
    else { // 局部
        int i = id_table.size()-1;
        while ( i >= 0 && id_table[i].lev == 1 ) {
            if ( id_table[i].name == name ) {
                break;
            }
            i--;
        }

        return id_table[i].lev == 1;
    }
}

bool Table::is_funcrcd ( id_rcd r ) {
    /*
        判断符号表的某条记录是否为函数名记录
    */
    return r.type == INT_FUNCTION || r.type == CHAR_FUNCTION
            || r.type == FLOAT_FUNCTION || r.type == VOID_FUNCTION;
}

bool Table::is_varrcd ( id_rcd r ) {
    /*
        判断符号表的某条记录是否为变量名记录
    */
    return r.type == INT_VAR || r.type == CHAR_VAR
                || r.type == FLOAT_VAR;
}

bool Table::is_constrcd ( id_rcd r ) {
    /*
        判断符号表的某条记录是否为常量名记录
    */
    return r.type == INT_CONST || r.type == CHAR_CONST
                || r.type == FLOAT_CONST;
}

bool Table::is_arrayrcd ( id_rcd r ) {
    /*
        判断符号表的某条记录是否为数组名记录
    */
    return r.type == INT_ARRAY || r.type == CHAR_ARRAY || r.type == FLOAT_ARRAY;
}

id_type Table::get_func_type ( string func_name ) {
    /*
        在符号表中寻找名为func_name的函数，返回其返回值类型
        若它不是函数或者没有找到则返回ERROR
    */
    //查找前先把标识符改成小写的
    transform(func_name.begin(),func_name.end(),func_name.begin(),::tolower);

    id_type r = ERROR;
    int i = 0;
    for( ; i < id_table.size(); i++ ) {
        if ( id_table[i].lev == 0 && id_table[i].name == func_name ) {
            r = id_table[i].type;
        }
    }

    if ( r == INT_FUNCTION || r == CHAR_FUNCTION
        || r == FLOAT_FUNCTION || r == VOID_FUNCTION ) {
        return r;
    }
    else{
        return ERROR;
    }
}

void Table::set_lastpar ( string func_name ) {
    /*
        设定func_table中名为func_name的函数的最后一个参数
        在符号表中的位置,设置成当前符号表最后一个元素的下标值
    */
    //查找前先把标识符改成小写的
    transform(func_name.begin(),func_name.end(),func_name.begin(),::tolower);

    //找出ref值
    int i = 0;
    int ref;
    for ( ; i<id_table.size(); i++ ) {
        if ( id_table[i].name == func_name && id_table[i].lev == 0 ) {
            ref = id_table[i].ref;
            break;
        }
    }

    //设定last_par
    func_table[ref].lastpar = id_table.size()-1;
}

void Table::set_ploc ( string func_name ) {
    /*
        设定func_table中名为func_name的函数的入口位置
        设为当前PCode表的大小
    */

    //查找前先把标识符改成小写的
    transform(func_name.begin(),func_name.end(),func_name.begin(),::tolower);

    //找出ref值
    int i = 0;
    int ref;
    for ( ; i<id_table.size(); i++ ) {
        if ( id_table[i].name == func_name && id_table[i].lev == 0 ) {
            ref = id_table[i].ref;
            break;
        }
    }

    //设定ploc
    func_table[ref].ploc = pcode_table.size();
}

int Table::get_lastpar ( id_rcd r ) {
    /*
        求得某个函数的最后一个参数在符号表中的下标值
        如果这个函数没参数则该值是该函数在符号表中的下标值
    */
    if ( is_funcrcd(r) ) {
        return func_table[r.ref].lastpar;
    }
    else {
        report_err(25);
    }

}

int Table::get_ploc ( id_rcd r ) {
    /*
        求得某个函数的入口地址
    */
    if ( is_funcrcd(r) ) {
        return func_table[r.ref].ploc;
    }
    else {
        report_err(25);
    }

}

void Table::set_lastid () {
    /*
        更新当前函数表中的last项
    */
    func_table[func_table.size()-1].last = id_table.size();
}

void Table::add_floatrcd ( float fl ) {
    /*
        增加一项实常量记录
    */
    rconst_table.push_back(fl);
}

void Table::add_strrcd ( string s ) {
    /*
        增加一个字符串记录
    */
    str_table.push_back(s);
}

int Table::get_idtable_size() {
    return id_table.size();
}

int Table::get_rctable_size() {
    return rconst_table.size();
}

int Table::get_pctable_size() {
    return pcode_table.size();
}

int Table::get_strtable_size() {
    return str_table.size();
}

int Table::find_ident ( int p, string name ) {
    /*
        根据标识符的名字和开始查找的位置来查找
        若找到则返回其下标值，否则返回-1
    */

    if ( p >= id_table.size() || !is_funcrcd(id_table[p]) ) {
        //排除掉越界的情况和p没有指向函数的情况
        return -1;
    }

    // 把标识符改成小写形式
    transform(name.begin(),name.end(),name.begin(),::tolower);
    //先在函数内部找
    int i = p+1;
    for ( ; i <= func_table[id_table[p].ref].last; i++ ) {
        if ( id_table[i].name == name ){
            return i;
        }
    }

    //若没有找到则在全局找
    if ( i > func_table[id_table[p].ref].last ) {
        for ( i = 0; i < id_table.size(); i++ ) {
            if ( id_table[i].lev == 0 && id_table[i].name == name ) {
                return i;
            }
        }
    }

    //还是没有找到，则返回-1
    if ( i >= id_table.size() ) {
        return -1;
    }
}

int Table::find_ident_fun ( string name ) {
     /*
        根据标识符的名字和开始查找的位置来查找一个函数记录
        若找到则返回其下标值，否则返回-1
    */

    int i;

    // 把标识符改成小写形式
    transform(name.begin(),name.end(),name.begin(),::tolower);
    for ( i = 0; i < id_table.size(); i++ ) {
        if ( id_table[i].lev == 0 && id_table[i].name == name
             && is_funcrcd(id_table[i]) ) {
            return i;
        }
    }

    //还是没有找到，则返回-1
    if ( i >= id_table.size() ) {
        return -1;
    }

}

id_rcd Table::get_idrcd ( int index ) {
    /*
        返回对应合法下标的符号表记录
    */

    if ( index < id_table.size() && index >= 0 ) {
        return id_table[index];
    }
    else {
        return {};
    }
}

array_rcd Table::get_arrayrcd ( int index ) {
    /*
        返回对应合法下标的数组信息表记录
    */

    if ( index < array_table.size() && index >= 0 ) {
        return array_table[index];
    }
    else {
        return {};
    }
}

func_rcd Table::get_funrcd ( int index ) {
    /*
        返回对应合法下标的函数信息表记录
    */

    if ( index < func_table.size() && index >= 0 ) {
        return func_table[index];
    }
    else {
        return {};
    }
}

id_rcd Table::get_funrcd() {
    /*
        返回符号表中最近的一个函数记录
    */

    int i = id_table.size()-1;
    while ( i >= 0 ) {
        if ( is_funcrcd(id_table[i]) ) {
            return id_table[i];
        }
        i--;
    }
}

float Table::get_floatval ( int index ) {
    /*
        返回对应合法下标的实数表记录
    */

    if ( index < rconst_table.size() && index >= 0 ) {
        return rconst_table[index];
    }
    else {
        return 0;
    }
}

string Table::get_str ( int index ) {
    /*
        返回对应合法下标的字符串表记录
    */

    if ( index < str_table.size() && index >= 0 ) {
        return str_table[index];
    }
    else {
        return "";
    }

}

int Table::get_array_size ( id_rcd r ) {
    /*
        返回数组的大小，即元素个数
    */
    if ( !is_arrayrcd(r) ) { //不是数组的话返回0
        return 0;
    }
    else {
        return array_table[r.ref].size;
    }
}

void Table::add_arrayrcd ( element_type type, int elsize, int size ) {
    /*
        往数组信息表中添加一项记录
    */
    array_table.push_back({type,elsize,size});
}

void Table::add_idrcd ( string name, id_type type ) {
    /*
        往符号表中添加一项记录
        普通变量、函数
    */

    // 把标识符统一存成小写形式
    transform(name.begin(),name.end(),name.begin(),::tolower);

    int adr;
    if ( type == INT_VAR || type == CHAR_VAR || type == FLOAT_VAR ) {
        int lev;
        //判断lev和adr的值
        if ( id_table.empty() ) {
            lev = 0;
            adr = data_adr++;
            if ( type == INT_VAR ) { //整数默认先存为0
                Runtime::push_rs({RS_INT,0});
            }
            else if ( type == CHAR_VAR ) {//字符默认先存为‘a’
                Runtime::push_rs({RS_CHAR,97});
            }
            else {
                Runtime::push_rs({RS_FLOAT,0});
            }
        }
        else {
            id_rcd r = id_table.back();
            if ( r.lev == 1 || ( r.lev == 0 && is_funcrcd(r)) ) {
                lev = 1;
                adr = fun_adr++;
                set_lastid();
            }
            else {//全局变量

                lev = 0;
                adr = data_adr++;
                if ( type == INT_VAR ) { //整数默认先存为0
                Runtime::push_rs({RS_INT,0});
                }
                else if ( type == CHAR_VAR ) {//字符默认先存为‘a’
                    Runtime::push_rs({RS_CHAR,97});
                }
                else {
                    Runtime::push_rs({RS_FLOAT,0});
                }

            }
        }

        if ( !id_table.empty() && is_id_repeat(name,lev) ) {
            //名称重复，报错
            report_err(1);
        }

        id_table.push_back({name,type,-1,lev,adr});
    }
    else if ( type == INT_FUNCTION || type == CHAR_FUNCTION
             || type == FLOAT_FUNCTION || type == VOID_FUNCTION ) {

        if ( !id_table.empty() && is_id_repeat(name,0) ) {
            //名称重复，报错
            report_err(1);
        }

        //设定adr的值，以及将fun_adr置零
        //函数记录的lev都是0，所以不用额外设定
        adr = data_adr++;
        fun_adr = 0;

        if ( type == CHAR_FUNCTION ) {
            Runtime::push_rs({RS_CHARFUN,97});
        }
        else if ( type == FLOAT_FUNCTION ) {
            Runtime::push_rs({RS_FLOATFUN,0});
        }
        else if ( type == INT_FUNCTION ){
            Runtime::push_rs({RS_INTFUN,0});
        }
        else {
            Runtime::push_rs({RS_VOIDFUN,0});
        }

        //填充func_table表和id_table表
        //func_table表的两个项的值先初始化为该函数名的在符号表中下标
        int ref = func_table.size();
        func_table.push_back({id_table.size(),id_table.size(),pcode_table.size()});
        id_table.push_back({name,type,ref,0,adr});

    }
}

void Table::add_idrcd ( string name, int adr, id_type type) {
    /*
        往符号表中添加一项记录
        整数常量、字符常量
    */
    // 把标识符统一存成小写形式
    transform(name.begin(),name.end(),name.begin(),::tolower);
    //cout << name;
    int lev;
    //判断lev
    if ( id_table.empty() ) {
        lev = 0;
    }
    else {
        id_rcd r = id_table.back();
        if ( r.lev == 0 && is_funcrcd(r) || r.lev == 1 ) {
            lev = 1;
            set_lastid();
        }
        else {
            lev = 0;
        }
    }

    if ( !id_table.empty() && is_id_repeat(name,lev) ) {
        //名称重复，报错
        report_err(1);
    }

    id_table.push_back({name,type,-1,lev,adr});

}

void Table::add_idrcd ( string name, float float_value ) {
    /*
        往符号表中添加一项实常量记录
    */

    // 把标识符统一存成小写形式
    transform(name.begin(),name.end(),name.begin(),::tolower);

    //设定lev的值
    int lev;
    if ( id_table.empty() ) {
        lev = 0;
    }
    else {
        id_rcd r = id_table.back();
        if ( r.lev == 0 && is_funcrcd(r) || r.lev == 1 ) {
            lev = 1;
            set_lastid();
        }
        else {
            lev = 0;
        }
    }

    if ( !id_table.empty() && is_id_repeat(name,lev) ) {
        //名称重复，报错
        report_err(1);
    }

    //填实常量表和标识符表
    int adr = rconst_table.size();
    add_floatrcd(float_value);
    id_table.push_back({name,FLOAT_CONST,-1,lev,adr});

}

void Table::add_idrcd ( string name, id_type type, int adr, int size) {
    /*
        往符号表中添加一项变量（数组）记录
    */
    // 把标识符统一存成小写形式
    transform(name.begin(),name.end(),name.begin(),::tolower);

    int elsize;
    element_type et;
    if ( type == INT_ARRAY ) {
        elsize = sizeof(int);
        et = INT;
    }
    else if ( type == CHAR_ARRAY ) {
        elsize = sizeof(char);
        et = CHAR;
    }
    else {
        elsize = sizeof(float);
        et = FLOAT;
    }

    //设定lev和adr的值
    int lev;
    if ( id_table.empty() ) {
        lev = 0;
        adr = data_adr;
        data_adr += size;

        if ( type == CHAR_ARRAY ) {
            for ( int i = 0; i < size; i++) {
                Runtime::push_rs({RS_CHAR,97});
            }

        }
        else if ( type == FLOAT_ARRAY ) {
            for ( int i = 0; i < size; i++) {
                Runtime::push_rs({RS_FLOAT,0});
            }

        }
        else {
            for ( int i = 0; i < size; i++) {
                Runtime::push_rs({RS_INT,0});
            }

        }

    }
    else {
        id_rcd r = id_table.back();
        if ( r.lev == 1 || r.lev == 0 && is_funcrcd(r) ) {
            lev = 1;
            adr = fun_adr;
            fun_adr += size;
            set_lastid();
        }
        else {
            lev = 0;
            adr = data_adr;
            data_adr += size;

            if ( type == CHAR_ARRAY ) {
                for ( int i = 0; i < size; i++) {
                    Runtime::push_rs({RS_CHAR,97});
                }

            }
            else if ( type == FLOAT_ARRAY ) {
                for ( int i = 0; i < size; i++) {
                    Runtime::push_rs({RS_FLOAT,0});
                }

            }
            else {
                for ( int i = 0; i < size; i++) {
                    Runtime::push_rs({RS_INT,0});
                }

            }

        }
    }

    if ( !id_table.empty() && is_id_repeat(name,lev) ) {
        //名称重复，报错
        report_err(1);
    }

    int ref = array_table.size();
    add_arrayrcd(et,elsize,size);
    id_table.push_back({name,type,ref,lev,adr});

}

void Table::emit ( op_code f ) {
    /*
        往PCode表中增加一条指令
    */
    pcode_table.push_back({f,0,0});
}

void Table::emit ( op_code f, int l ) {
    /*
        往PCode表中增加一条指令
    */
    pcode_table.push_back({f,l,0});
}

void Table::emit ( op_code f, int l, int a ) {
    /*
        往PCode表中增加一条指令
    */
    pcode_table.push_back({f,l,a});
}

void Table::update_emit ( op_code f, int l, int a, int loc ){
    /*
        修改位于PCode表的loc处的指令
    */
    pcode_table[loc].f = f;
    pcode_table[loc].l = l;
    pcode_table[loc].a = a;
}

//—————————————— 测试函数 ——————————————

void Table::test_id_table(){
    ofstream fout;
    fout.open("id_table.txt");
    fout<<setw(5) <<"i";
    fout<<setw(20) <<"Name";
    fout<<setw(5)<<"Type";
    fout<<setw(5)<<"ref";
    fout<<setw(5)<<"lev";
    fout<<setw(5)<<"adr"<<endl;
    for ( int i = 0; i < id_table.size(); i++ ) {
        fout<<setw(5)  << i;
        fout<<setw(20)  << id_table[i].name;
        fout<<setw(5)  << id_table[i].type;
        fout<<setw(5)  << id_table[i].ref;
        fout<<setw(5)  << id_table[i].lev;
        fout<<setw(5)  << id_table[i].adr;
        fout<< endl;
    }
}

void Table::test_arr_table(){
    ofstream fout;
    fout.open("arr_table.txt");
    fout<<"Eltype\tElsize\tsizer"<<endl;
    for ( int i = 0; i < array_table.size(); i++ ) {
        fout<<setw(10) << array_table[i].eltype;
        fout<<setw(5) << array_table[i].elsize;
        fout<<setw(5) << array_table[i].size;
        fout<<setw(5) << endl;
    }
}

void Table::test_rconst_table(){
    ofstream fout;
    fout.open("rconst_table.txt");
    //fout<<"Eltype\tElsize\tsizer"
    for ( int i = 0; i < rconst_table.size(); i++ ) {
       fout << rconst_table[i] << endl;
    }
}

void Table::test_str_table(){
    ofstream fout;
    fout.open("str_table.txt");
    for ( int i = 0; i < str_table.size(); i++ ) {
       fout << str_table[i] << endl;
    }
}

void Table::test_pcode_table(){
    ofstream fout;
    fout.open("pcode_table.txt");
    for ( int i = 0; i < pcode_table.size(); i++ ) {

        string s;
        switch ( pcode_table[i].f) {
            case ADD:
                s = "ADD";
                break;
            case BNE:
                s = "BNE";
                break;
            case BRF:
                s = "BRF";
                break;
            case DIV:
                s = "DIV";
                break;
            case EQL:
                s = "EQL";
                break;
            case GEQ:
                s = "GEQ";
                break;
            case GRT:
                s = "GRT";
                break;
            case JMP:
                s = "JMP";
                break;
            case JR:
                s = "JR";
                break;
            case JSR:
                s = "JSR";
                break;
            case LDS:
                s = "LDS";
                break;
            case LDT:
                s = "LDT";
                break;
            case LEQ:
                s = "LEQ";
                break;
            case LES:
                s = "LES";
                break;
            case LIT:
                s = "LIT";
                break;
            case LOD:
                s = "LOD";
                break;
            case LOI:
                s = "LOI";
                break;
            case MUL:
                s = "MUL";
                break;
            case NEQ:
                s = "NEQ";
                break;
            case PRT:
                s = "PRT";
                break;
            case PUF:
                s = "PUF";
                break;
            case RDA:
                s = "RDA";
                break;
            case SDS:
                s = "SDS";
                break;
            case STA:
                s = "STA";
                break;
            case STO:
                s = "STO";
                break;
            case SUB:
                s = "SUB";
                break;
        }
        fout << "["<<i<<"]" <<s << "\t";
        fout << pcode_table[i].l << "\t";
        fout << pcode_table[i].a << "\t";
        fout << endl;
    }
}

void Table::test_func_table(){
    ofstream fout;
    fout.open("func_table.txt");
    fout<<"Lastpar\tlast\t" <<endl;
    for ( int i = 0; i < func_table.size(); i++ ) {
       fout << func_table[i].lastpar << "\t";
       fout << func_table[i].last << "\t";
       fout << func_table[i].ploc;
       fout << endl;
    }
}


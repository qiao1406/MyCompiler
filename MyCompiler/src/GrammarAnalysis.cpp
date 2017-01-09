#include "GrammarAnalysis.h"
#include "ErrorProcess.h"

#include<sstream>
#include<queue>

//分别表示在词法分析缓存里的行列坐标值，用于定位
int GrammarAnalysis::row_index = 0;
int GrammarAnalysis::col_index = 0;

//表示当前分析的时候所在的函数块位置，值表示该函数名在
//符号表中的位置，-1表示不在任何函数块内，而是在全局中
//所以该指针初始化为-1
int GrammarAnalysis::pointer = -1;

//当期指针指向的词法分析缓存的位置的词语
Word GrammarAnalysis::nowword;

//模板函数：将string类型变量转换为常用的数值类型（此方法具有普遍适用性）
template <class Type>
Type str2num(const string& str) {
    istringstream iss(str);
    Type num;
    iss >> num;
    return num;
}

Word GrammarAnalysis::nextword () {
    /*
        返回词法分析缓存中的当前词
        并且把指针往下移动一位
    */

    if ( row_index >= WordAnalysis::linewords.size() || row_index < 0 ) {
        return Word("wrong","WRONG"); //检查row_index是否在合法范围内
    }
    else {

        if ( col_index >= WordAnalysis::linewords[row_index].size() ) {
            //到了一行的末尾了
            row_index++; //指针移到下一行
            if ( row_index >= WordAnalysis::linewords.size() ) {
                return Word("wrong","WRONG");
            } //再检查是否超出行数
            col_index = 0;
        }

        return WordAnalysis::linewords[row_index][col_index++];
    }
}

void GrammarAnalysis::lastword () {
    /*
        把指针往上移动一位
    */
    if ( row_index >= WordAnalysis::linewords.size() || row_index < 0 ) {
        //return Word("wrong","WRONG"); //检查row_index是否在合法范围内
        return;
    }
    else {

        if ( col_index == 0 ) { // 到一行的开头了
            //指针移到上一行
            row_index--;
            if ( row_index < 0 ) {
                //return Word("wrong","WRONG");  //再次检查row_index是否在合法范围内
                return;
            }
            col_index = WordAnalysis::linewords[row_index].size();
        }

        col_index--;
    }
}

int GrammarAnalysis::get_line_loc() {
    return nowword.loc;
}

void GrammarAnalysis::renew_pointer() {
    /*
        更新指向当前函数位置的指针
    */
    pointer = Table::get_idtable_size();
}

void GrammarAnalysis::prt_grammar_info ( string name ) {
    /*
        输出语法分析的结果
    */
    cout << "This is a " << name << endl;
}

void GrammarAnalysis::err_report ( int index ) {
    //报错
    ErrorProcess::add_err(nowword.loc,index);
}

void GrammarAnalysis::jump_read ( vector<string> v ) {
    /*
        跳读源代码,直到读到v中的字符串为止
        入口无预读
        出口无预读
    */

    if ( v.empty() ) {
        return;
    }


    while ( true ) {

        if (row_index >= WordAnalysis::linewords.size() ){
            return;
        }

        for ( auto iter = v.begin(); iter < v.end(); iter++ ) {
            if ( nowword.value == (*iter) ) {
                return;
            }
        }

        nowword = nextword();
    }
}

void GrammarAnalysis::ga_programme () {
    /*
        <程序>
    */
    nowword = nextword();
    if ( nowword.value == "const" ) {//全局常量读取
        ga_constant();
    }

    if ( nowword.is_vartype() ) {
        nowword = nextword();
        nowword = nextword();
        if ( nowword.value == "," || nowword.value == ";"
              || nowword.value == "[") { //变量说明
            //cout << nowword.value << "yyyoioi";
            lastword();
            lastword();
            lastword();
            nowword = nextword();
            ga_variable();

        }
        else if ( nowword.value == "(" ) { //函数说明
            lastword();
            lastword();
            lastword();
            nowword = nextword();
            ga_returnfun();
            nowword = nextword();
        }
        else {//不是函数命名，跳读到下一个var_type
            err_report(33);
            jump_read({"int","char","float","void"});
        }
    }

    while ( true ) {

        if ( nowword.is_vartype() ) { //有返回值函数说明
            ga_returnfun();
            nowword = nextword();
        }
        else if ( nowword.value == "void" ) {
            nowword = nextword();
            if ( nowword.value == "main") { //此时所有函数已经都定义完了
                lastword();
                lastword();
                nowword = nextword();
                break;
            }
            else {
                lastword();
                lastword();
                nowword = nextword();
                ga_voidfun();
                nowword = nextword();
            }
        }
        else {//报错，结束编译
            err_report(34);
            return;
        }
    }
    //读取主函数
    ga_mainfun();
    return;
}

void GrammarAnalysis::ga_constant () {
    /*
        <常量说明>子程序
        入口有预读
        出口有预读
    */
    while ( nowword.value == "const" ) {
        ga_constdef();

        if ( nowword.value != ";" ) {
            //遗漏分号，报错并且跳读
            err_report(11);
            jump_read({"const","int","char","void","float"});
        }
        else {
            nowword = nextword();
        }

    }
    prt_grammar_info("constant declaration");
    return;
}

void GrammarAnalysis::ga_constdef () {
    /*
        <常量定义>子程序
        入口不要求预读
        出口有预读
    */
    nowword = nextword();

    if ( nowword.is_vartype() ) {

        string data_typ = (nowword.value == "int")?"INTEGER"
                    :( nowword.value == "char")?"CHARACTER":"FLOAT";

        while ( true ) {

            nowword = nextword();
            if ( nowword.type == "IDENT") {
                string name = nowword.value; //记录标识符的名字
                nowword = nextword();
                if ( nowword.type != "BECOME" ) {
                    err_report(3);
                    jump_read({";"});
                    return;
                }
                nowword = nextword();
                if ( nowword.type != data_typ ) { //检查常量类型是否一致
                    //若不一致则报错，继续循环
                    err_report(35);
                    continue;
                }

                //填符号表
                if ( data_typ == "INTEGER" ) { //把string转成int型
                    Table::add_idrcd(name,str2num<int>(nowword.value),INT_CONST);
                }
                else if ( data_typ == "CHARACTER" ) {
                    int ascii_code = nowword.value[1]; //例如'A，应该取第[1]位的ascii值
                    Table::add_idrcd(name,ascii_code,CHAR_CONST);
                }
                else {//把string转成float型
                    Table::add_idrcd(name,str2num<float>(nowword.value));
                }

            }
            else {
                //报错,应该是标识符
                err_report(2);
                jump_read({";"});
                return;
            }

            //看是否有多个常量一起定义
            nowword = nextword();

            if ( nowword.value != "," ) {
                break; //若是逗号则继续循环
            }
        }

       return;
    }
    else { //不是变量类型符
        err_report(28);
        jump_read({";"});
        return;
    }

}

void GrammarAnalysis::ga_variable () {
    /*
        <变量说明>子程序
        入口有预读
        出口有预读
    */
    while (true) {
        ga_vardef();
        if ( nowword.value != ";" ) {
            //遗漏分号，报错并且跳读
            err_report(11);
            jump_read({"int","char","float","void"});
        }
        else {
            nowword = nextword();
        }

        if ( nowword.value == "void" ) { //有可能是主函数或无返回值函数
            break; // 此时变量说明已结束，跳出循环
        }
        else {

            if ( nowword.is_vartype() ) {
                nowword = nextword();
                if ( nowword.type != "IDENT" ) {
                    //不是标识符，
                    err_report(2);
                    jump_read({";"});
                    continue;
                }

                nowword = nextword();
                if ( nowword.value == "(" ) { //有可能是有返回值的函数
                    //此时要把指针往回退两位，然后跳出循环
                    lastword();
                    lastword();
                    lastword();
                    nowword = nextword();
                    break;
                }
                else if ( nowword.value == ";" || nowword.value == ","
                         || nowword.value == "[") {
                    // 此时还在变量定义阶段
                    //此时要把指针往回退两位，然后继续循环
                    lastword();
                    lastword();
                    lastword();
                    nowword = nextword();
                    continue;
                }
            }

            else {
               break;
            }
        }
    }

    prt_grammar_info("variable declaration");
}

void GrammarAnalysis::ga_vardef () {
    /*
        <变量定义>子程序
        入口有预读
        出口有预读
    */

    if ( nowword.is_vartype() ) {
        string data_typ = nowword.value;//记录数据类型(int,char,float)
        while ( true ) {

            nowword = nextword();
            string name = nowword.value;//记录标识符名字
            if ( nowword.type != "IDENT" ) {
                err_report(2);
                jump_read({";"});
                return;
            }

            nowword = nextword();
            if ( nowword.value == "[" ) { // 有可能是数组

                nowword = nextword();
                if ( nowword.type != "INTEGER" ) { //数组的大小必须是整型
                    err_report(31);
                    jump_read({";"});
                    return;
                }
                int arr_size = str2num<int>(nowword.value);//记录数组的大小
                if ( arr_size <= 0 ) {
                    //数组大小不是正数，报错并继续变量定义
                    err_report(42);
                }
                nowword = nextword();
                if ( nowword.value != "]" ) {
                    //缺少右中括号，跳读
                    err_report(7);
                    jump_read({";"});
                    return;
                }

                //填符号表，增加一项数组记录
                id_type t = (data_typ == "int")?INT_ARRAY:
                                (data_typ =="char")?CHAR_ARRAY:FLOAT_ARRAY;
                Table::add_idrcd(name,t,0,arr_size);

                nowword = nextword();
            }
            else {//普通变量
                //填符号表,增加一项普通变量记录
                id_type t = (data_typ == "int")?INT_VAR:
                                (data_typ=="char")?CHAR_VAR:FLOAT_VAR;
                Table::add_idrcd(name,t);
            }

            if ( nowword.value != ",") { // 多个变量的定义以逗号分隔
                break;
            }
        }

        //prt_grammar_info("variable define");
        return;
    }
    else {//不是类型类型符
        err_report(28);
        jump_read({";"});
        return;
    }
}

void GrammarAnalysis::ga_returnfun () {
    /*
        <有返回值函数定义>子程序
        入口无预读
        出口无预读

    */

    //当前读到的是返回值类型
    string rtn_type = nowword.value;//记录返回类型
    nowword = nextword();
    string name = nowword.value;//记录函数名
    if ( nowword.type != "IDENT" ) {
        err_report(2);
        jump_read({"int","char","void","float"});
        return;
    }
    else {//填符号表
        renew_pointer();//更新所在函数块指针
        id_type t = (rtn_type == "int")?INT_FUNCTION:
                        (rtn_type=="char")?CHAR_FUNCTION:FLOAT_FUNCTION;
        Table::add_idrcd(name,t);
    }

    nowword = nextword();
    if ( nowword.value != "(" ) {
        err_report(4);
    }
    else {
        nowword = nextword();
    }

    if ( nowword.value == ")" ) {
        //无参数,设lastpar值
        Table::set_lastpar(name);
    }
    else if ( nowword.is_vartype() ) {
        ga_argulist(name);
    }
    else {
        err_report(36);
        jump_read({"int","char","void","float"});
        return;
    }

    if ( nowword.value != ")" ) {
        err_report(5);
    }
    else {
        nowword = nextword();
    }

    if ( nowword.value != "{" ) {
        //缺少大括号，报错并且退一个词
        err_report(8);
        lastword();
        lastword();
        nowword = nextword();
    }

    //记录函数的入口
    Table::set_ploc(name);

    //生成函数主体部分的PCode
    ga_complex_stmt();
    if ( nowword.value != "}" ) {
        //缺少反大括号，报错
        err_report(9);
    }

    prt_grammar_info("function that has return value\n");
}

void GrammarAnalysis::ga_voidfun () {
    /*
        <无返回值函数定义>子程序
        入口无预读
        出口无预读

    */
    nowword = nextword();
    string name = nowword.value;//记录函数名
    if ( nowword.type != "IDENT" ) { //函数名若不是标识符则报错
        err_report(2);
        jump_read({"int","char","void","float"});
        return;
    }
    else { //填符号表
        renew_pointer();//更新所在函数块指针
        Table::add_idrcd(name,VOID_FUNCTION);
    }

    nowword = nextword();
    if ( nowword.value != "(" ) {
        //缺少左括号，报错
        err_report(4);
    }
    else {
        nowword = nextword();
    }

    if ( nowword.value == ")" ) {
        //无参数,设lastpar值
        Table::set_lastpar(name);
    }
    else if ( nowword.is_vartype() ) {
        ga_argulist(name);
    }
    else {
        err_report(36);
        jump_read({"int","char","void","float"});
        return;
    }

    if ( nowword.value != ")" ) { //缺少反括号，报错
        err_report(5);
    }
    else {
        nowword = nextword();
    }

    if ( nowword.value != "{" ) {
        //缺少大括号，报错并且退一个词
        err_report(8);
        lastword();
        lastword();
        nowword = nextword();
    }

    //记录函数的入口
    Table::set_ploc(name);

    //生成主体部分的PCode
    ga_complex_stmt();
    if ( nowword.value != "}" ) {
        //缺少反大括号，报错
        err_report(9);
    }

    Table::emit(JR);//无返回值的函数末尾都要加一个JR指令
    prt_grammar_info("function that has no return value\n");
}

void GrammarAnalysis::ga_mainfun () {
    /*
        <主函数>子程序
        入口无预读
        出口无预读

    */

    nowword = nextword();
    if ( nowword.value != "main" ) {
        err_report(37);
        return;
    }
    else { //填符号表
        renew_pointer();//更新所在函数块指针
        Table::add_idrcd("main",VOID_FUNCTION);
    }

    nowword = nextword();
    if ( nowword.value != "(" ) {
        //缺少左括号,报错
        err_report(4);
    }
    else {
        nowword = nextword();
    }

    if ( nowword.value != ")" ) {
        //缺少右括号报错
        err_report(5);
    }
    else {
        nowword = nextword();
    }

    if ( nowword.value != "{" ) {
        //缺少左大括号。报错
        err_report(8);
    }
    else {
        nowword = nextword();
    }

    //在main函数的读取分析时不调用ga_complex_stmt()
    //是为了添加PUF指令

    if ( nowword.value == "const" ) {//常量说明
        ga_constant();
    }

    if ( nowword.is_vartype() ) { //变量说明
        ga_variable();
    }

    //记录函数的入口
    Table::set_ploc("main");

    //设定main函数开始的位置
    Runtime::set_main_pointer(Table::get_pctable_size());

    //生成PUF指令
    Table::emit(PUF,3,pointer);

    while ( nowword.value != "}") {
        ga_statement();
    }

    if ( nowword.value != "}" ) {
        //缺少右大括号，报错
        err_report(9);
    }

    prt_grammar_info("main function");
}

void GrammarAnalysis::ga_argulist ( string func_name ) {
    /*
        <参数表>子程序
        入口无预读
        出口有预读
    */

    if ( nowword.is_vartype() ) {
        string data_type = nowword.value;//记录参数类型
        nowword = nextword();
        if ( nowword.type != "IDENT" ) {
            err_report(2);
        }
        else{ //登录符号表,同时改变函数表的lastpar值
            id_type t = (data_type == "int")?INT_VAR:
                            (data_type=="char")?CHAR_VAR:FLOAT_VAR;
            Table::add_idrcd(nowword.value,t);
            Table::set_lastpar(func_name);
        }
        nowword = nextword();

        while ( nowword.value == "," ) {
            nowword = nextword();
            data_type = nowword.value;
            if ( !nowword.is_vartype() ) {
                err_report(28);
                jump_read({"{"});
                return;
            }
            nowword = nextword();

            if ( nowword.type != "IDENT") {
                //不是标识符，跳读一个词，并继续循环
                err_report(2);
                nowword = nextword();
                continue;
            }
            else { //登录符号表，同时改变函数表的lastpar值
                id_type t = (data_type == "int")?INT_VAR:
                            (data_type=="char")?CHAR_VAR:FLOAT_VAR;
                Table::add_idrcd(nowword.value,t);
                Table::set_lastpar(func_name);
            }

            nowword = nextword();
        }

        return;
    }
    else {
        //不是变量类型符号，报错
        err_report(28);
        return;
    }
}

void GrammarAnalysis::ga_complex_stmt(){
    /*
        <复合语句>子程序
        入口无预读
        出口有预读
    */
    nowword = nextword();

    if ( nowword.value == "}" ) { //什么都没有的复合语句
        prt_grammar_info("complex statement");
        return;
    }

    if ( nowword.value == "const" ) {//常量说明
        ga_constant();
    }

    if ( nowword.is_vartype() ) { //变量说明
        ga_variable();
    }

    while ( nowword.value != "}") {
        ga_statement();
    }
    prt_grammar_info("complex statement");
}

void GrammarAnalysis::ga_statement(){
    /*
        <语句>子程序
        入口有预读
        出口有预读
    */

    if ( nowword.value == ";" ) { // 空语句
        nowword = nextword();
        prt_grammar_info("empty statement");
        return;
    }
    else if ( nowword.value == "if" ) { //条件语句
        nowword = nextword();
        ga_condition_stmt();
        return;
    }
    else if ( nowword.value == "while" ) { //循环语句

        nowword = nextword();
        ga_cycle_stmt();
        return;
    }
    else if ( nowword.value == "switch" ) { //情况语句
        nowword = nextword();
        ga_case_stmt();
        nowword = nextword();
        return;
    }
    else if ( nowword.value == "scanf" ) { //读语句
        nowword = nextword();
        ga_read_stmt();
        nowword = nextword();
        if ( nowword.value != ";" ) {
            err_report(11);
        }
        else {
            nowword = nextword();
        }
        return;
    }
    else if ( nowword.value == "printf" ) { //写语句
        nowword = nextword();
        ga_write_stmt();
        nowword = nextword();
        if ( nowword.value != ";" ) {
            err_report(11);
        }
        else {
            nowword = nextword();
        }
        return;
    }
    else if ( nowword.value == "return" ) { //返回语句
        nowword = nextword();
        ga_return_stmt();
        nowword = nextword();
        if ( nowword.value != ";" ) {
            err_report(11);
        }
        else{
            nowword = nextword();
        }
        return;
    }
    else if ( nowword.value == "{" ) { //语句列
        nowword = nextword();
        if ( nowword.value == "}" ) {
            prt_grammar_info("statement list");
            nowword = nextword();
            return;
        }
        else {
            while ( nowword.value != "}") {
                ga_statement();
            }
            if ( nowword.value != "}") {
                //缺少右大括号，后退一个词
                err_report(9);
                lastword();
                lastword();
                nowword = nextword();
            }
            else {
                prt_grammar_info("statement list");
                nowword = nextword();
                return;
            }

        }

    }
    else if ( nowword.type == "IDENT" ) {

        string name = nowword.value;// 记录标识符的名称
        nowword = nextword();
        if ( nowword.value == "=" || nowword.value == "[" ) { //赋值语句
            ga_assign_stmt(name);
            if ( nowword.value != ";" ) {
                //遗漏分号
                err_report(11);
            }
            else{
                nowword = nextword();
            }
            return;
        }
        else if ( nowword.value == "(" ) { //函数调用语句

            id_type rt = Table::get_func_type(name);//获得该函数的返回值类型
            if ( rt == ERROR ) {
                //函数的返回值错误
                err_report(38);
                return;
            }
            else if ( rt == VOID_FUNCTION ) {//无返回值函数调用
                //nowword = nextword();
                ga_voidfuncall_stmt(name);
            }
            else { //有返回值函数调用
                //nowword = nextword();
                ga_retfuncall_stmt(name);
            }

            if ( nowword.value != ";" ) {
                //baocuo
                err_report(11);
            }
            else{
                nowword = nextword();
            }
            //prt_grammar_info("funcall statement");
            return;
        }
        else {
            err_report(39);
            return;
        }

    }
    else {
        err_report(40);
        return;
    }

}

void GrammarAnalysis::ga_condition_stmt(){
    /*
        <条件语句>子程序
        要求入口预读
        出口有预读
    */

    if ( nowword.value != "(" ) {
        //不是左括号，报错并后退一个词
        err_report(4);
        lastword();
        lastword();
        nowword = nextword();
    }
    ga_condition();
    //此时条件表达式的真值已经写到栈顶
    if ( nowword.value != ")" ) {
        //baocuo
        err_report(5);
    }
    int brf_loc = Table::get_pctable_size();//记录BRF指令的位置
    Table::emit(BRF,0,0);
    nowword = nextword();
    ga_statement();

    if( nowword.value == "else" ) {
        int jmp_loc = Table::get_pctable_size();//记录JMP指令的位置
        Table::emit(JMP,0,0);
        Table::update_emit(BRF,0,Table::get_pctable_size(),brf_loc);
        nowword = nextword();
        ga_statement();
        Table::update_emit(JMP,0,Table::get_pctable_size(),jmp_loc);
    }
    else { //没有else块的情况
        Table::update_emit(BRF,0,Table::get_pctable_size(),brf_loc);
    }

    prt_grammar_info("condition statement#");
}

void GrammarAnalysis::ga_cycle_stmt(){
    /*
        <循环语句>子程序
        要求入口预读
        出口有预读
    */
    if ( nowword.value != "(" ) {
        //不是左括号，报错并后退一个词
        err_report(4);
        lastword();
        lastword();
        nowword = nextword();
    }
    int jp_loc = Table::get_pctable_size();//一次循环后跳转的地址

    ga_condition();
    //此时已经把条件结果加载到栈顶了
    if ( nowword.value != ")" ) {
        err_report(5);
    }
    else {
        nowword = nextword();
    }

    int brf_loc = Table::get_pctable_size();
    Table::emit(BRF,0,0);
    ga_statement();
    Table::emit(JMP,0,jp_loc);
    Table::update_emit(BRF,0,Table::get_pctable_size(),brf_loc);
    prt_grammar_info("cycle statement#");
}

void GrammarAnalysis::ga_retfuncall_stmt ( string func_name ){
    /*
        <有返回值的函数调用语句>子程序
        要求入口处无预读
        出口处有预读

        此处采取的策略是先正向加载表示值参数的表达式
        然后生成PUF指令，把函数入栈，然后再倒序载入形参
    */

    //i是函数名在符号表中的下标值
    int i = Table::find_ident_fun(func_name);
    int i1 = i;//记录函数名的位置
    id_rcd r, temp;//r是函数名的记录
    if ( i == -1 ) {
        //没找到该函数
        err_report(38);
        jump_read({";"});
        return;
    }
    else {
        r = Table::get_idrcd(i);
    }

    nowword = nextword();
    if ( nowword.value == ")" ) { //值参数表为空
        if (  i != Table::get_lastpar(r) ) { //参数个数不对
            err_report(21);
            jump_read({";"});
            return;
        }
    }
    else { //装入表示值参数的表达式
        ga_expression();//把值参数的值写到栈顶
        i++;
        Table::emit(SDS);//把值参数的值写到数据栈栈顶

        while ( nowword.value == "," ) {
            nowword = nextword();
            ga_expression();//把值参数的值写到栈顶
            i++;
            Table::emit(SDS);//把值参数的写到数据栈栈顶

        }

        if (  i != Table::get_lastpar(r) ) { //参数个数不对
            err_report(21);
            jump_read({";"});
            return;
        }

    }

    if ( nowword.value == ")" ) {
        //生成PUF指令，将函数入栈
        int t = ( r.type == INT_FUNCTION )?0:(( r.type == CHAR_FUNCTION )?1:2);
        Table::emit(PUF,t,i1);

        //逆向登录形参的值
        for ( i = Table::get_lastpar(r); i > i1; i-- ){
            temp = Table::get_idrcd(i);
            Table::emit(LDS);//从数据栈栈顶得到实参数的值
            Table::emit(STO,temp.lev,temp.adr);//把实参的值填入形参的位置
        }

        //设定返回地址以及跳转到函数入口
        Table::emit(JSR,Table::get_ploc(r),Table::get_pctable_size()+1);
        nowword = nextword();
        prt_grammar_info("return-funcall statement");
        return;
    }
    else {
        //缺少右括号，报错
        err_report(5);
//        lastword();
//        lastword();
//        nowword = nextword();
    }

}

void GrammarAnalysis::ga_voidfuncall_stmt ( string func_name ){
    /*
        <无返回值的函数调用语句>子程序
        要求入口处有预读
        出口处有预读

        此处采取的策略是先正向加载表示值参数的表达式
        然后生成PUF指令，把函数入栈，然后再倒序载入形参
    */

    //i是函数名在符号表中的下标值
    int i = Table::find_ident_fun(func_name);
    int i1 = i;//记录函数名的位置
    id_rcd r, tmp;//r是函数名的记录
    if ( i == -1 ) { //找不到该函数
        err_report(38);
        jump_read({";"});
        return;
    }
    else {
        r = Table::get_idrcd(i);
    }

    nowword = nextword();
    if ( nowword.value == ")" ) { //值参数表为空
        if (  i != Table::get_lastpar(r) ) { //参数个数不对
            err_report(21);
            jump_read({";"});
            return;
        }
    }
    else {
        ga_expression();
        i++;
        Table::emit(SDS);//把值参数的值写到数据栈栈顶

        while ( nowword.value == "," ) {
            nowword = nextword();
            ga_expression();
            i++;
            Table::emit(SDS);//把值参数的值写到数据栈栈顶;
        }

        if (  i != Table::get_lastpar(r) ) { //参数个数不对
            err_report(21);
            jump_read({";"});
            return;
        }

    }

    if ( nowword.value == ")" ) {

        //生成PUF指令，将函数入栈
        Table::emit(PUF,3,i1);

        //逆向登录形参的值
        for ( i = Table::get_lastpar(r); i > i1; i-- ){
            tmp = Table::get_idrcd(i);
            Table::emit(LDS);//从数据栈栈顶得到实参数的值
            Table::emit(STO,tmp.lev,tmp.adr);
        }

        //设定返回地址以及跳转到函数入口
        Table::emit(JSR,Table::get_ploc(r),Table::get_pctable_size()+1);
        nowword = nextword();
        prt_grammar_info("noreturn-funcall statement");
        return;

    }
    else {
        //缺少右括号，报错
        err_report(5);
//        lastword();
//        lastword();
//        nowword = nextword();
    }
}

void GrammarAnalysis::ga_assign_stmt ( string id_name ) {
    /*
       <赋值语句>子程序
        要求入口处有预读
        出口处有预读
    */

    //查表，试图找到该标识符的记录
    int loc = Table::find_ident(pointer,id_name);
    id_rcd r;
    if ( loc == -1 ) { //没找到该标识符，报错并返回
        err_report(2);
        jump_read({";"});
        return;
    }
    else {
        //从符号表中提取出该标识符的记录
        r = Table::get_idrcd(loc);
    }

    if( !( Table::is_varrcd(r) || Table::is_arrayrcd(r) ) ) {
        //该标识符不是变量或数组，报错
        err_report(41);
        jump_read({";"});
        return;
    }

    if ( nowword.value == "[" ) { //数组的赋值

        //先判断该标识符是不是数组
        if ( !Table::is_arrayrcd(r) ) {
            err_report(32);
            jump_read({";"});
            return;
        }
        int arr_size = Table::get_array_size(r);
        int index;
        array_rcd ar = Table::get_arrayrcd(r.ref);
        if ( arr_size <= 0 ) { //元素个数应该是正数
            err_report(42);
        }

        nowword = nextword();
        ga_expression();//计算下标表达式的值
        //此时数组下标的值已经写到栈顶,此时计算存放的地址

        Table::emit(LOI,0,r.adr);
        Table::emit(ADD);

        if ( nowword.value != "]" ) {
            //缺少右中括号，报错
            err_report(7);
        }
        else {
           nowword = nextword();
        }

        if ( nowword.value != "=" ) {
            //缺少等号，报错
            err_report(3);
            jump_read({";"});
            return;
        }

        nowword = nextword();
        ga_expression();//计算赋值表达式的值
        //此时要写入的地址在次栈顶，表达式的值在栈顶
        Table::emit(STA,r.lev);

        prt_grammar_info("assignation statement");
        return;
    }
    else if (  nowword.value == "=" ) {
        nowword = nextword();
        ga_expression();
        Table::emit(STO,r.lev,r.adr);
        prt_grammar_info("assignation statement");
        return;
    }
    else {
        err_report(3);
        jump_read({";"});
    }
}

void GrammarAnalysis::ga_read_stmt(){
    /*
       <读语句>子程序
        要求入口处有预读
        出口处无预读
    */
    if ( nowword.value != "(" ) {
        err_report(4);
    }
    else {
        nowword = nextword();
    }

    id_rcd r;
    if ( nowword.type != "IDENT" ) { //不是标识符，报错
        err_report(2);
        jump_read({","});
    }
    else {//判断标识符的类型，只能是普通变量
        int loc = Table::find_ident(pointer,nowword.value);
        if ( loc == -1 ) {//未找到标识符，报错
            err_report(43);
            jump_read({","});
        }
        else {

            r = Table::get_idrcd(loc);
            if ( ! Table::is_varrcd(r) ) {
                //读的不是变量，报错
                err_report(19);
            }
            else {//生成读指令
                //根据不同的数据类型生成不同的指令
                ( r.type == INT_VAR )? Table::emit(RDA,0):
                    ( r.type == CHAR_VAR )? Table::emit(RDA,1):Table::emit(RDA,2);
                Table::emit(STO,r.lev,r.adr);
            }
            nowword = nextword();
        }

    }

    while ( nowword.value == "," ) {

        nowword = nextword();
        if ( nowword.type != "IDENT" ) {
            //不是标识符，报错
            err_report(2);
            jump_read({","});
        }
        else {//判断标识符的类型，只能是普通变量
            int loc = Table::find_ident(pointer,nowword.value);
            if ( loc == -1 ) {
                //未找到标识符，报错
                err_report(43);
                jump_read({","});
            }
            else {
                r = Table::get_idrcd(loc);
                if ( ! Table::is_varrcd(r) ) {
                    //该标识符不是变量，报错
                    err_report(19);
                }
                else {//生成读指令
                    ( r.type == INT_VAR )? Table::emit(RDA,0):
                        ( r.type == CHAR_VAR )? Table::emit(RDA,1):Table::emit(RDA,2);
                    Table::emit(STO,r.lev,r.adr);
                }

            }
        }
        nowword = nextword();
    }

    if ( nowword.value != ")" ) {
        //缺少右括号，后退一个词
        err_report(5);
        lastword();
        lastword();
        nowword = nextword();
    }
    prt_grammar_info("read statement");
}

void GrammarAnalysis::ga_write_stmt(){
    /*
        <写语句>子程序
        要求入口处有预读
        出口处无预读
    */
    if ( nowword.value != "(" ) {
        err_report(4);
    }
    else {
        nowword = nextword();
    }

    if ( nowword.type == "STRING" ) {
        //先把字符串登记到字符串表中
        Table::add_strrcd(nowword.value);
        Table::emit(LIT,3,Table::get_strtable_size()-1);
        Table::emit(PRT);
        nowword = nextword();
        if ( nowword.value == "," ) {
            nowword = nextword();
            ga_expression();
            Table::emit(PRT);
        }

    }
    else {
        ga_expression();
        Table::emit(PRT);
    }

    if ( nowword.value != ")") {
        //缺少右括号，后退一个词
        err_report(5);
        lastword();
        lastword();
        nowword = nextword();
    }
    prt_grammar_info("write statement");
}

void GrammarAnalysis::ga_return_stmt(  ){
    /*
        <返回语句>子程序
        要求入口处有预读
        出口处无预读
    */

    id_rcd r = Table::get_funrcd();//记录当前的返回语句所在的函数

    if ( nowword.value != "(" ) {//只有“return”也是合法的
        //但是此时要后退一个词
        lastword();
        lastword();
        nowword = nextword();
        Table::emit(JR);
        prt_grammar_info("return statement");
        return;
    }
    nowword = nextword();

    if ( nowword.value == ")" ) {
        Table::emit(JR);
        prt_grammar_info("return statement");
        return;
    }
    else {
        ga_expression();

        if ( nowword.value != ")" ) {
            //缺少右括号，后退一个词
            err_report(5);
            lastword();
            lastword();
            nowword = nextword();
        }
        else {
            //判断一下是否是在void函数里，否则要报错
            if ( r.type == VOID_FUNCTION ) {
                err_report(26);
            }
            Table::emit(JR);
        }
        prt_grammar_info("return statement");
    }

}

void GrammarAnalysis::ga_case_stmt(){
    /*
        <情况语句>子程序
        要求入口预读
        出口无预读
    */

    queue<int> bq;//记录BNE指令的地址
    queue<int> jq;//记录JMP指令的地址

    if ( nowword.value == "(" ) {

        nowword = nextword();
        ga_expression();
        //此时表达式的值已写入栈顶用于比较
        if ( nowword.value != ")" ) {
            //报错
            err_report(5);
        }
        else {
            nowword = nextword();
            if ( nowword.value != "{" ) {
                //baocuo
                err_report(8);
            }
            else {
                nowword = nextword();
                if ( nowword.value != "case" ) {
                    //baocuo
                    err_report(24);
                }
                else {

                    nowword = nextword();
                    if ( nowword.type != "INTEGER" && nowword.type != "CHARACTER" ) {
                        //baocuo
                        err_report(44);
                    }
                    else {
                        ( nowword.type == "CHARACTER" )? Table::emit(LOI,1,(int)nowword.value[1])
                                    :Table::emit(LOI,0,str2num<int>(nowword.value));
                        //生成BNE指令
                        bq.push(Table::get_pctable_size());
                        Table::emit(BNE,0,0);
                        ga_subcase_stmt();
                        jq.push(Table::get_pctable_size());
                        Table::emit(JMP,0,0);

                    }

                    while ( nowword.value == "case" ) { //存在多个情况子语句的时候
                        nowword = nextword();
                        if ( nowword.type != "INTEGER"
                             && nowword.type != "CHARACTER" ) {
                            //baocuo
                            err_report(44);
                        }
                        else {
                            ( nowword.type == "CHARACTER" )? Table::emit(LOI,1,(int)nowword.value[1])
                                    :Table::emit(LOI,0,str2num<int>(nowword.value));
                            //生成BNE指令
                            bq.push(Table::get_pctable_size());
                            Table::emit(BNE,0,0);
                            ga_subcase_stmt();
                            jq.push(Table::get_pctable_size());
                            Table::emit(JMP,0,0);
                        }
                    }
                }

                if ( nowword.value != "}") { //语句结尾是反大括号
                    //baocuo
                    err_report(9);
                }

                int end_adr = Table::get_pctable_size();
                while ( !jq.empty() ) {
                    Table::update_emit(JMP,0,end_adr,jq.front());
                    jq.pop();
                }
                while ( !bq.empty() ) {
                    int i = bq.front();
                    bq.pop();
                    if ( bq.empty() ) {
                        Table::update_emit(BNE,0,end_adr-1,i);
                    }
                    else {
                        Table::update_emit(BNE,0,bq.front()-1,i);
                    }
                }
                Table::test_pcode_table();
                prt_grammar_info("case statement");
            }
        }

    }
    else {
        err_report(4);
    }
}

void GrammarAnalysis::ga_subcase_stmt() {
    /*
        <情况子语句>子程序
        入口要求预读
        出口有预读
    */

    nowword = nextword();
    if ( nowword.value != ":" ) {
        //缺少冒号，往后退一个词
        err_report(10);
        lastword();
        lastword();
        nowword = nextword();
    }
    nowword = nextword();
    ga_statement();
}

void GrammarAnalysis::ga_condition() {
    /*
        <条件>子程序
        入口不需要预读
        出口有预读
    */

    nowword = nextword();
    ga_expression();
    if ( nowword.is_relation_op() ) {

        string op = nowword.value;
        nowword = nextword();
        ga_expression();

        //此时表达式的右值在栈顶，左值在次栈顶
        //按照关系运算符的值的不同来进行分类
        if ( op == "==" ) {
            Table::emit(EQL);
        }
        else if ( op == "!=" ) {
            Table::emit(NEQ);
        }
        else if ( op == "<" ) {
            Table::emit(LES);
        }
        else if  ( op == "<=" ) {
            Table::emit(LEQ);
        }
        else if ( op == ">" ) {
            Table::emit(GRT);
        }
        else { // op是>=的情况
            Table::emit(GEQ);
        }

    }
    //只有一个表达式而没有关系运算符的时候，可以不用生成比较的指令
}

void GrammarAnalysis::ga_expression() {
    /*
        <表达式>子程序
        要求入口有预读
        出口处有预读
    */
    bool flag = false;

    if ( nowword.is_addition_op() ) {

        if ( nowword.value == "-" ) {
            flag = true;
        }
        nowword = nextword();
    }
    ga_item();
    if ( flag ) {//如果有前置的负号则要给第一个项取反
        Table::emit(LOI,0,-1);
        Table::emit(MUL);
    }

    while ( nowword.is_addition_op() ) {
        string op = nowword.value;
        nowword = nextword();
        ga_item();
        //生成操作符的指令
        ( op == "+" )? Table::emit(ADD): Table::emit(SUB);
    }

}

void GrammarAnalysis::ga_item() {
    /*
        <项>子程序
        入口有预读
        出口有预读
    */
    ga_factor();
    while ( nowword.is_multiplication_op() ) {
        string op = nowword.value;
        nowword = nextword();
        ga_factor();
        //生成操作符指令
        ( op == "*" )? Table::emit(MUL): Table::emit(DIV);
    }

}

void GrammarAnalysis::ga_factor() {
    /*
        <因子>子程序
        入口有预读
        出口有预读
    */
    if ( nowword.type == "INTEGER" || nowword.type == "CHARACTER"
        || nowword.type == "FLOAT" ) { //未定义的常量

        int a;
        int t;
        if ( nowword.type == "INTEGER") {
            a = str2num<int>(nowword.value);
            t = 0;
        }
        else if ( nowword.type == "FLOAT" ) {
            a = Table::get_rctable_size();
            //把该实数的值加入实常量表
            Table::add_floatrcd(str2num<float>(nowword.value));
            t = 2;
        }
        else {
            a = nowword.value[1];
            t = 1;
        }
        Table::emit(LOI,t,a);
        nowword = nextword();
        return;
    }
    else if ( nowword.type == "IDENT" ) { //变量、常量或函数调用

        string id = nowword.value;//记录标识符的名字
        int loc;
        id_rcd r;

        nowword = nextword();

        if ( nowword.value == "(" ) { // 可能是有返回值的函数调用语句

            //从符号表中取出对应的函数记录
            loc = Table::find_ident_fun(id);

            if ( loc == -1 ) { //没找到该标识符
                err_report(43);
                jump_read({";","+","-","*","/",")"});
                return;
            }
            else {
                //从符号表中提取出该标识符的记录
                r = Table::get_idrcd(loc);
            }

            if ( r.type != VOID_FUNCTION ) {
                ga_retfuncall_stmt(id);
                return;
            }
            else { //不是有返回值的
                err_report(29);
                jump_read({")"});
                nowword = nextword();
                return;
            }

        }
        else {

            loc = Table::find_ident(pointer,id);

            if ( loc == -1 ) { //没找到该标识符
                err_report(43);
                jump_read({";","+","-","*","/",")"});
                return;
            }
            else {
                //从符号表中提取出该标识符的记录
                r = Table::get_idrcd(loc);
            }

           if ( nowword.value == "[" ) { // 有可能是数组

                //先判断该标识符是不是数组
                if ( !Table::is_arrayrcd(r) ) {
                    err_report(32);
                }
                int arr_size = Table::get_array_size(r);
                int index;
                if ( arr_size <= 0 ) { //元素个数应该是正数
                    err_report(42);
                }

                nowword = nextword();
                ga_expression(); // 识别表示数组下标的表达式
                //此时数组下标值已存到栈顶
                //得到运行栈栈顶的类型,进行判断

                if ( nowword.value != "]" ) {
                    err_report(7);
                    jump_read({";","+","-","*","/"});
                    return;
                }
                else { //生成指令
                    Table::emit(LOI,0,r.adr);
                    Table::emit(ADD);//计算得到要取的数的地址
                    Table::emit(LDT,r.lev);
                }
                nowword = nextword();
                return;

            }
            else if ( Table::is_constrcd(r) ) { //常量

                int l = ( r.type == INT_CONST )? 0:( r.type == CHAR_CONST )? 1:2;
                Table::emit(LIT,l,r.adr);
                return;
            }
            else if ( Table::is_varrcd(r) ) { //普通变量
                Table::emit(LOD,r.lev,r.adr);
                return;
            }
            else {//未知标识符,报错
                err_report(43);
            }




        }
    }

    else if ( nowword.value == "(" ) { //可能是表达式
        nowword = nextword();
        ga_expression();
        if( nowword.value != ")") {
            //缺少右括号，报错并后退一个词
            err_report(5);
            lastword();
            lastword();
            nowword = nextword();
        }
        nowword = nextword();
        return;
    }
    else {//不符合因子的条件，报错
        err_report(45);
    }
}

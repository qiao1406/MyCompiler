#include "GrammarAnalysis.h"
#include "ErrorProcess.h"
#include "Runtime.h"

#include<sstream>

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

void GrammarAnalysis::ga_programme () {
    /*
        <程序>
    */
    nowword = nextword();
    if ( nowword.value == "const" ) {
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
        else {
            //baocuo
        }
    }

   //cout << endl;

    while ( true ) {

        if ( nowword.is_vartype() ) { //有返回值函数说明
            //cout << nowword.value << "#yyy";
            ga_returnfun();
            //cout << nowword.value << "###yhdg" << endl;
            nowword = nextword();
        }
        else if ( nowword.value == "void" ) {
            nowword = nextword();
            //cout << nowword.value << "#yyy";
            if ( nowword.value == "main") { //此时所有函数已经都定义完了
                lastword();
                lastword();
                nowword = nextword();
                break;
            }
            else {
                lastword();
                lastword();
                //nowword = nextword();

                nowword = nextword();
                //cout << nowword.value << "#yyy";
                ga_voidfun();
                nowword = nextword();
                //cout << nowword.value << "#yyy";
            }
        }
        else {
            //baocuo
        }
    }

    //cout << nowword.value << "xxx%";
    ga_mainfun();
    return;
}

void GrammarAnalysis::ga_constant () {
    /*
        <常量声明>子程序
        入口有预读
        出口有预读
    */
    while ( nowword.value == "const" ) {
        ga_constdef();

        if ( nowword.value != ";" ) {

            //baocuo
        }
        nowword = nextword();
    }
    prt_grammar_info("constant declaration\n");

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
                if ( nowword.type != "EQUAL" ) {
                    //报错
                }
                nowword = nextword();
                if ( nowword.type != data_typ ) { //检查常量类型是否一致
                    //baocuo
                }

                //填符号表
                if ( data_typ == "int" ) {
                    //把string转成数字
                    Table::add_idrcd(name,str2num<int>(nowword.value),INT_CONST);
                }
                else if ( data_type == "char" ) {
                    int ascii_code = nowword.value[0];
                    Table::add_idrcd(name,ascii_code,CHAR_CONST);
                }
                else {
                    Table::add_idrcd(name,str2num<float>(nowword.value),FLOAT_CONST);
                }

            }
            else {
                //报错
            }

            //看是否有多个常量一起定义
            nowword = nextword();

            if ( nowword.value != "," ) {
                break; //若是逗号则继续循环
            }
        }
       return;
    }
    else {
        //baocuo
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
            //baocuo
        }

        nowword = nextword();
        if ( nowword.value == "void" ) { //有可能是主函数或无返回值函数
            break; // 此时变量说明已结束，跳出循环
        }
        else {

            if (  nowword.is_vartype() ) {
                nowword = nextword();
                if ( nowword.type != "IDENT" ) {
                //baocuo
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

    prt_grammar_info("variable declaration\n");
}

void GrammarAnalysis::ga_vardef () {
    /*
        <变量定义>子程序
        入口有预读
        出口有预读
    */
    if ( nowword.is_vartype() ) {
        string data_type = nowword.value;//记录数据类型
        while ( true ) {
            nowword = nextword();
            string name = nowword.value;//记录标识符名字
            if ( nowword.type != "IDENT" ) {
                //baocuo
            }
            nowword = nextword();
            if ( nowword.value == "[" ) { // 有可能是数组
                nowword = nextword();
                if ( nowword.type != "INTEGER" ) {
                    // 报错
                }
                int arr_size = str2num<int>(nowword);//记录数组的大小
                nowword = nextword();
                if ( nowword.value != "]" ) {
                    //baocuo
                }

                //填符号表，增加一项数组记录
                id_type t = (data_type == "int")?INT_ARRAY:
                                (data_type=="char")?CHAR_ARRAY:FLOAT_ARRAY;
                Table::add_idrcd(name,t,0,arr_size);

                nowword = nextword();
            }
            else {//普通变量
                //填符号表,增加一项普通变量记录
                id_type t = (data_type == "int")?INT_VAR:
                                (data_type=="char")?CHAR_VAR:FLOAT_VAR;
                Table::add_idrcd(name,t);
            }

            if ( nowword.value != ",") { // 多个变量的定义以逗号分隔
                break;
            }
        }

        //prt_grammar_info("variable define");
        return;
    }
    else {
        //baocuo
    }
}

void GrammarAnalysis::ga_returnfun () {
    /*
        <有返回值函数定义>子程序
        入口无预读
        出口无预读

    */
    //当前读到的是返回值类型
    string rtn_type = nowword;//记录返回类型
    nowword = nextword();
    string name = nowword;//记录函数名
    if ( nowword.type != "IDENT" ) {
        //baocuo
    }
    else {//填符号表
        id_type t = (rtn_type == "int")?INT_FUNCTION:
                        (rtn_type=="char")?CHAR_FUNCTION:FLOAT_FUNCTION;
        Table::add_idrcd(name,t);
        renew_pointer();
    }

    nowword = nextword();
    if ( nowword.value != "(" ) {
        //baocuo
    }

    nowword = nextword();
    if ( nowword.type == ")" ) {
        //无参数,设lastpar值
        Table::set_lastpar(name);
    }
    else if ( nowword.is_vartype() ) {
        ga_argulist(name);
    }
    else {
        //baocuo
    }


    if ( nowword.value != ")" ) {
        //baocuo
    }

    nowword = nextword();
    if ( nowword.value != "{" ) {
        //baocuo
    }

    ga_complex_stmt();
    if ( nowword.value != "}" ) {
        //baocuo
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
    string name = nowword;//记录函数名
    if ( nowword.type != "IDENT" ) { //函数名若不是标识符则报错
        //baocuo
    }
    else { //填符号表
        Table::add_idrcd(name,VOID_FUNCTION);
        renew_pointer();
    }

    nowword = nextword();
    if ( nowword.value != "(" ) {
        //baocuo
    }

    nowword = nextword();
    if ( nowword.type == ")" ) {
        //无参数,设lastpar值
        Table::set_lastpar(name);
    }
    else if ( nowword.is_vartype() ) {
        ga_argulist(name);
    }
    else {
        //baocuo
    }

    if ( nowword.value != ")" ) {
        //baocuo
    }
   // cout << nowword.value << "#666yyy";

    nowword = nextword();
    if ( nowword.value != "{" ) {
        //baocuo
    }

    ga_complex_stmt();
    if ( nowword.value != "}" ) {
        //baocuo
    }

    prt_grammar_info("function that has no return value\n");
}

void GrammarAnalysis::ga_mainfun () {
    /*
        <主函数>子程序
        入口无预读
        出口无预读

    */
    nowword = nextword();
    if ( nowword.type != "main" ) {
        //baocuo
    }
    else { //填符号表
        Table::add_idrcd("main",VOID_FUNCTION);
        renew_pointer();
    }

    nowword = nextword();
    if ( nowword.value != "(" ) {
        //baocuo
    }

    nowword = nextword();
    if ( nowword.value != ")" ) {
        //baocuo
    }

    nowword = nextword();
    if ( nowword.value != "{" ) {
        //baocuo
    }

    //cout << nowword.value << "jojojxxx%";
    ga_complex_stmt();
    if ( nowword.value != "}" ) {
        //baocuo
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
        string data_type = nowword;//记录参数类型
        nowword = nextword();
        if ( nowword.type != "IDENT" ) {
            //baocuo
        }
        else{ //登录符号表,同时改变函数表的lastpar值
            id_type t = (data_type == "int")?INT_VAR:
                            (data_type=="char")?CHAR_VAR:FLOAT_VAR;
            Table::add_idrcd(nowword,t);
            Table::set_lastpar(func_name);
        }
        nowword = nextword();

        while ( nowword.value == "," ) {
            nowword = nextword();
            data_type = nowword;
            if ( !nowword.is_vartype() ) {
                //baocuo
            }
            nowword = nextword();

            if ( nowword.type != "IDENT") {
                //baocuo
            }
            else { //登录符号表，同时改变函数表的lastpar值
                id_type t = (data_type == "int")?INT_VAR:
                            (data_type=="char")?CHAR_VAR:FLOAT_VAR;
                Table::add_idrcd(nowword,t);
                Table::set_lastpar(func_name);
            }

            nowword = nextword();
        }

        return;
    }
    else {
        //baocuo
    }
}

void GrammarAnalysis::ga_complex_stmt(){
    /*
        <复合语句>子程序
        入口无预读
        出口有预读
    */
    nowword = nextword();
    //cout << nowword.value << "rrrtttrrt";

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

    //cout << nowword.value << "#yui8888yyy";
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
        //cout << nowword.value << "jojojxxx%";
        nowword = nextword();
        ga_condition_stmt();
        //cout << nowword.value << "jojojxxx%";
        return;
    }
    else if ( nowword.value == "while" ) { //循环语句

        nowword = nextword();
        ga_cycle_stmt();
        //cout << nowword.value << "jojojxxx%";
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
            //baocuo
        }
        nowword = nextword();
        return;
    }
    else if ( nowword.value == "printf" ) { //写语句
        nowword = nextword();
        ga_write_stmt();
        nowword = nextword();
        if ( nowword.value != ";" ) {
            //baocuo
        }
        nowword = nextword();
        return;
    }
    else if ( nowword.value == "return" ) { //返回语句
        nowword = nextword();
        ga_return_stmt();
        nowword = nextword();
       // cout << nowword.value << "xxx%";
        if ( nowword.value != ";" ) {
            //baocuo
            cout << "漏了分号";
        }
        nowword = nextword();
        return;
    }
    else if ( nowword.value == "{" ) { //语句列
       //cout << nowword.value << "jnnnojojxxx%";
        nowword = nextword();
        if ( nowword.value == "}" ) {
            prt_grammar_info("statement list");
            nowword = nextword();
            return;
        }
        else {
            //cout << nowword.value << "j999ojojxxx%";
            while ( nowword.value != "}") {
                ga_statement();
            }
            if ( nowword.value != "}") {
                //baocuo
            }
            else {
                prt_grammar_info("statement list");
                nowword = nextword();
                return;
            }

        }

    }
    else if ( nowword.type == "IDENT" ) {
        string name = nowword;// 记录标识符的名称

        nowword = nextword();
        if ( nowword.value == "=" || nowword.value == "[" ) { //赋值语句
            //nowword = nextword();
            ga_assign_stmt();
            if ( nowword.value != ";" ) {
            //baocuo
            cout << "漏了分号";
            }
            nowword = nextword();
            return;
        }
        else if ( nowword.value == "(" ) { //函数调用语句

            id_type rt = Table::get_func_type(name);//获得该函数的返回值
            if ( rt == ERROR ) {
                //baocuo
            }
            else if ( rt == VOID_FUNCTION ) {//无返回值函数调用
                nowword = nextword();
                ga_voidfuncall_stmt();
            }
            else { //有返回值函数调用
                nowword = nextword();
                ga_retfuncall_stmt();
            }

            if ( nowword.value != ";" ) {
            //baocuo
            }
            prt_grammar_info("funcall statement");
            nowword = nextword();
            return;
        }
        else {
            //baocuo
        }

    }
    else {
        //baocuo
    }

}

void GrammarAnalysis::ga_condition_stmt(){
    /*
        <条件语句>子程序
        要求入口预读
        出口有预读
    */
    //cout << nowword.value << "jojojxxx%";

    if ( nowword.value != "(" ) {
        //baocuo
    }
    ga_condition();
    if ( nowword.value != ")" ) {
        //baocuo
    }
    //cout << nowword.value << "jojojxxx%";
    nowword = nextword();
    ga_statement();

    //cout << nowword.value << "jojojxxx%";
    //cout << nowword.value << "jojojxxx%";
    if( nowword.value == "else" ) {
        nowword = nextword();
        ga_statement();
    }
    //cout << nowword.value << "tytyjojojxxx%";

    prt_grammar_info("condition statement#");
}

void GrammarAnalysis::ga_cycle_stmt(){
    /*
        <循环语句>子程序
        要求入口预读
        出口有预读
    */
    if ( nowword.value != "(" ) {
        //baocuo
    }
    ga_condition();
    if ( nowword.value != ")" ) {
        //baocuo
    }

    nowword = nextword();
    ga_statement();
   // cout << nowword.value << "jojojxxijijijx%";
    prt_grammar_info("cycle statement");
}

void GrammarAnalysis::ga_retfuncall_stmt(){
    /*
        <有返回值的函数调用语句>子程序
        要求入口处无预读
        出口处无预读
    */
    nowword = nextword();
    if ( nowword.value == ")" ) { //值参数表为空
       // prt_grammar_info("return-funcall statement");
        return;
    }
    else {
        ga_expression();
        while ( nowword.value == "," ) {
            nowword = nextword();
            ga_expression();
        }
        if ( nowword.value == ")" ) {
            nowword = nextword();
            //prt_grammar_info("return-funcall statement");
            return;
        }
        else {
            //baocuo
        }
    }

}

void GrammarAnalysis::ga_voidfuncall_stmt(){
    /*
        <有返回值的函数调用语句>子程序
        要求入口处有预读
        出口处无预读
    */
    nowword = nextword();
    if ( nowword.value == ")" ) { //值参数表为空
        prt_grammar_info("noreturn-funcall statement");
        return;
    }
    else {
        ga_expression();
        while ( nowword.value == "," ) {
            nowword = nextword();
            ga_expression();
        }
        if ( nowword.value == ")" ) {
            prt_grammar_info("noreturn-funcall statement");
            return;
        }
        else {
            //baocuo
        }
    }
}

void GrammarAnalysis::ga_assign_stmt(){
    /*
       <赋值语句>子程序
        要求入口处有预读
        出口处有预读
    */

    if ( nowword.value == "[" ) { //数组的赋值

        nowword = nextword();
        ga_expression();
        //cout << nowword.value << "#yyy";
        if ( nowword.value != "]" ) {
            //baocuo
        }

        nowword = nextword();
        if ( nowword.value != "=" ) {
            //baocuo
        }

        nowword = nextword();
        ga_expression();
        prt_grammar_info("assignation statement");
        return;
    }
    else if (  nowword.value == "=" ) {
        nowword = nextword();
        ga_expression();
        //cout << nowword.value << "#yyy";
        prt_grammar_info("assignation statement");
        return;
    }
}

void GrammarAnalysis::ga_read_stmt(){
    /*
       <读语句>子程序
        要求入口处有预读
        出口处无预读
    */
    if ( nowword.value != "(" ) {
        //baocuo
    }
    nowword = nextword();

    if ( nowword.type != "IDENT" ) {
        //baocuo
    }
    nowword = nextword();
    while ( nowword.value == "," ) {

        nowword = nextword();
        if ( nowword.type != "IDENT" ) {
        //baocuo
        }
        nowword = nextword();
    }

    if ( nowword.value != ")" ) {
        //baocuo
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
        //baocuo
    }

    nowword = nextword();
    if ( nowword.type == "STRING" ) {
        nowword = nextword();
        if ( nowword.value == "," ) {
            nowword = nextword();
            ga_expression();
        }

    }
    else {
        ga_expression();
    }

    if ( nowword.value != ")") {
            //baocuo
    }
    prt_grammar_info("write statement");
}

void GrammarAnalysis::ga_return_stmt(){
    /*
        <返回语句>子程序
        要求入口处有预读
        出口处无预读
    */
    if ( nowword.value != "(" ) {
        //baocuo
        cout << "漏了左括号";
    }
    nowword = nextword();
    ga_expression();
    //cout << nowword.value << "mmmmdiaozhi%";

    if ( nowword.value != ")" ) {
        //baocuo
        cout << nowword.value << "diaozhi%";
    }
    prt_grammar_info("return statement");
}

void GrammarAnalysis::ga_case_stmt(){
    /*
        <情况语句>子程序
        要求入口预读
        出口无预读
    */
    if ( nowword.value == "(" ) {

        nowword = nextword();
        ga_expression();
        if ( nowword.value != ")" ) {
            //报错
        }
        else {
            nowword = nextword();
            if ( nowword.value != "{" ) {
                //baocuo
            }
            else {
                nowword = nextword();
                if ( nowword.value != "case" ) {
                    //baocuo
                }
                else {
                    ga_statement();
                    while ( nowword.value == "case" ) { //存在多个情况子语句的时候
                        ga_subcase_stmt();
                    }
                }

                if ( nowword.value != "}") { //语句结尾是反大括号
                    //baocuo
                }
                //cout << nowword.value << "jb一样555xxx%";
                prt_grammar_info("case statement");
            }
        }

    }
    else {
        //baocuo
    }
}

void GrammarAnalysis::ga_subcase_stmt() {
    /*
        <情况子语句>子程序
        入口不要求预读
        出口有预读
    */
    nowword = nextword();
    //cout << nowword.value << "rrtytyxxx%";
    if ( nowword.type != "INTEGER" && nowword.type != "CHARACTER" ) {
        //baocuo
    }

    nowword = nextword();
    if ( nowword.value != ":" ) {
        //baocuo
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
   // cout << nowword.value << "ppppojojxxx%";
    if ( nowword.is_relation_op() ) {
        nowword = nextword();
        ga_expression();
    }
    //cout << nowword.value << "jojojxxx%";
}

void GrammarAnalysis::ga_expression() {
    /*
        <表达式>子程序
        要求入口有预读
        出口处有预读
    */
    if ( nowword.is_addition_op() ) {
        nowword = nextword();
    }

    ga_item();
    while ( nowword.is_addition_op() ) {
        string op = nowword;
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
        string op = nowword;
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
            a = nowword.value[0];
            t = 1;
        }
        Table::emit(LOI,t,a);
        nowword = nextword();
        return;
    }
    else if ( nowword.type == "IDENT" ) { //变量、常量或函数调用

        string id = nowword.value;//记录标识符的名字
        int loc = Table::find_ident(pointer,id);
        id_rcd r;
        if ( loc == -1 ) { //没找到该标识符
            //baocuo
        }
        else {
            //从符号表中提取出该标识符的记录
            r = Table::get_idrcd(loc);
        }

        nowword = nextword();
        if ( nowword.value == "[" ) { // 有可能是数组

            //先判断该标识符是不是数组
            if ( !Table::is_arrayrcd(r) ) {
                //baocuo
            }
            int arr_size = Table::get_array_size(r);
            int index;
            if ( arr_size <= 0 ) { //元素个数应该是正数
                //baocuo
            }

            nowword = nextword();
            ga_expression(); // 识别表示数组下标的表达式
            //得到运行栈栈顶的类型,进行判断
            if ( Runtime.get_top_type() != RS_INT ) {
            //baocuo
            }
            else {
                index = Runtime::get_top_value();//数组下标
                if ( index < 0 || index >= arr_size ) { //数组下标越界
                    //报错
                }
            }

            if ( nowword.value != "]" ) {
                //baocuo
            }
            else { //生成指令
                int l = ( r.type == INT_ARRAY )? 0:( r.type == CHAR_ARRAY )? 1:2;
                Table::emit(LOD,l,r.adr+index);
            }
            nowword = nextword();
            return;

        }

        else if ( nowword.value == "(" ) { // 可能是有返回值的函数调用语句
            ga_retfuncall_stmt();
            return;
        }

        else if ( Table::is_constrcd(r) ) { //常量
            int l = ( r.type == INT_CONST )? 0:( r.type == CHAR_CONST )? 1:2;
            Table::emit(LIT,l,r.adr);
            return;
        }

        else if ( Table::is_varrcd(r) ) { //普通变量
            int l = ( r.type == INT_VAR )? 0:( r.type == CHAR_VAR )? 1:2;
            Table::emit(LOD,l,r.adr);
            return;
        }
        else {
            //未知标识符报错
        }



    }
    else if ( nowword.type == "(" ) { //可能是表达式
        nowword = nextword();
        ga_expression();



        if( nowword.type != ")") {
            //baocuo
        }
        return;
    }
    else {
        //baocuo
    }
}

#include "GrammarAnalysis.h"

//分别表示在词法分析缓存里的行列坐标值，用于定位
int GrammarAnalysis::row_index = 0;
int GrammarAnalysis::col_index = 0;

//当期指针指向的词法分析缓存的位置的词语
Word GrammarAnalysis::nowword;

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

void GrammarAnalysis::lastword ( ) {
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

void GrammarAnalysis::prt_grammar_info ( string name ) {
    /*
        输出语法分析的结果
    */
    cout << "This is a " << name << endl;
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
                nowword = nextword();
                if ( nowword.type != "EQUAL" ) {
                    //报错
                }
                nowword = nextword();
                if ( nowword.type != data_typ ) {
                    //baocuo
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
       //prt_grammar_info("constant define");
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

        while ( true ) {
            nowword = nextword();
            if ( nowword.type != "IDENT" ) {
                //baocuo
            }
            nowword = nextword();
            if ( nowword.value == "[" ) { // 有可能是数组
                nowword = nextword();
                if ( nowword.type != "INTEGER" ) {
                    // 报错
                }
                nowword = nextword();
                if ( nowword.value != "]" ) {
                    //baocuo
                }
                nowword = nextword();
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
    nowword = nextword();
    if ( nowword.type != "IDENT" ) {
        //baocuo
    }

    nowword = nextword();
    if ( nowword.value != "(" ) {
        //baocuo
    }

    nowword = nextword();
    if ( nowword.type == ")" ) {
        //无参数
    }
    else if ( nowword.is_vartype() ) {
        ga_argulist();
    }
    else {
        //baocuo
    }


    if ( nowword.value != ")" ) {
        //baocuo
        //cout << nowword.value << "7bhhhhh";
    }

    nowword = nextword();
    if ( nowword.value != "{" ) {
        //baocuo
        //cout << nowword.value;
        //cout << "hijb";
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
    if ( nowword.type != "IDENT" ) {
        //baocuo
    }

    nowword = nextword();
    if ( nowword.value != "(" ) {
        //baocuo
    }

    nowword = nextword();
    if ( nowword.type == ")" ) {
        //无参数
    }
    else if ( nowword.is_vartype() ) {
        ga_argulist();
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
       // cout << nowword.value << "@eryyy";
        nowword = nextword();
        if ( nowword.value == "=" || nowword.value == "[" ) { //赋值语句
            //cout << nowword.value << "#yyy";
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
            // 由于还没有建好符号表，所以不能判断是有返回值还是无返回值
            // 默认先判断为有返回值
            nowword = nextword();
            ga_retfuncall_stmt();
            if ( nowword.value != ";" ) {
            //baocuo
            cout << nowword.value << "#yyy";
            cout << "漏了分号";
            }
            prt_grammar_info("return-funcall statement");
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

void GrammarAnalysis::ga_argulist () {
    /*
        <参数表>子程序
        入口无预读
        出口有预读
    */

    if ( nowword.is_vartype() ) {

        nowword = nextword();
       // cout << nowword.value << "gsdgsd";
        if ( nowword.type != "IDENT" ) {
            //baocuo
        }
        nowword = nextword();

        while ( nowword.value == "," ) {
            nowword = nextword();
            //cout << nowword.value << "gsdgsd";
            if ( !nowword.is_vartype() ) {
                //baocuo
            }
            nowword = nextword();

            if ( nowword.type != "IDENT") {
                //baocuo
                //cout << "ubuntu";
            }

            nowword = nextword();
            //cout << nowword.value << "bhhhhh";
        }

        //prt_grammar_info("arguments list");
        return;
    }
    else {
        //baocuo
    }
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
        nowword = nextword();
        ga_item();
    }

}

void GrammarAnalysis::ga_item() {
    /*
        <项>子程序
        入口有预读
        出口有预读
    */
    ga_factor();
    //nowword = nextword();
    //cout << nowword.value << "一样555xxx%";
    while ( nowword.is_multiplication_op() ) {
        nowword = nextword();
        ga_factor();
        //nowword = nextword();
    }
    //cout << nowword.value << "iioodiaozhi%";
}

void GrammarAnalysis::ga_factor() {
    /*
        <因子>子程序
        入口有预读
        出口有预读
    */
    if ( nowword.type == "INTEGER" || nowword.type == "CHARACTER"
        || nowword.type == "FLOAT" ) {
            nowword = nextword();
            return;
    }
    else if ( nowword.type == "IDENT" ) {
        nowword = nextword();

        if ( nowword.value == "[" ) { // 有可能是数组
            nowword = nextword();
            ga_expression();
            if ( nowword.value != "]" ) {
                //baocuo
            }
            nowword = nextword();
            return;//数组
        }
        else if ( nowword.value == "(" ) { // 可能是有返回值的函数调用语句
            ga_retfuncall_stmt();
            return;
        }
        else {
            //cout << "mimi";
            return;//标识符
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

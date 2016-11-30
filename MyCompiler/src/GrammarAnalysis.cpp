#include "GrammarAnalysis.h"
#include "ErrorProcess.h"
#include "Runtime.h"

#include<sstream>

//�ֱ��ʾ�ڴʷ��������������������ֵ�����ڶ�λ
int GrammarAnalysis::row_index = 0;
int GrammarAnalysis::col_index = 0;

//��ʾ��ǰ������ʱ�����ڵĺ�����λ�ã�ֵ��ʾ�ú�������
//���ű��е�λ�ã�-1��ʾ�����κκ������ڣ�������ȫ����
//���Ը�ָ���ʼ��Ϊ-1
int GrammarAnalysis::pointer = -1;

//����ָ��ָ��Ĵʷ����������λ�õĴ���
Word GrammarAnalysis::nowword;

//ģ�庯������string���ͱ���ת��Ϊ���õ���ֵ���ͣ��˷��������ձ������ԣ�
template <class Type>
Type str2num(const string& str) {
    istringstream iss(str);
    Type num;
    iss >> num;
    return num;
}

Word GrammarAnalysis::nextword () {
    /*
        ���شʷ����������еĵ�ǰ��
        ���Ұ�ָ�������ƶ�һλ
    */

    if ( row_index >= WordAnalysis::linewords.size() || row_index < 0 ) {
        return Word("wrong","WRONG"); //���row_index�Ƿ��ںϷ���Χ��
    }
    else {

        if ( col_index >= WordAnalysis::linewords[row_index].size() ) {
            //����һ�е�ĩβ��
            row_index++; //ָ���Ƶ���һ��
            if ( row_index >= WordAnalysis::linewords.size() ) {
                return Word("wrong","WRONG");
            } //�ټ���Ƿ񳬳�����
            col_index = 0;
        }

        return WordAnalysis::linewords[row_index][col_index++];
    }
}

void GrammarAnalysis::lastword () {
    /*
        ��ָ�������ƶ�һλ
    */
    if ( row_index >= WordAnalysis::linewords.size() || row_index < 0 ) {
        //return Word("wrong","WRONG"); //���row_index�Ƿ��ںϷ���Χ��
        return;
    }
    else {

        if ( col_index == 0 ) { // ��һ�еĿ�ͷ��
            //ָ���Ƶ���һ��
            row_index--;
            if ( row_index < 0 ) {
                //return Word("wrong","WRONG");  //�ٴμ��row_index�Ƿ��ںϷ���Χ��
                return;
            }
            col_index = WordAnalysis::linewords[row_index].size();
        }

        col_index--;
    }
}

void GrammarAnalysis::renew_pointer() {
    /*
        ����ָ��ǰ����λ�õ�ָ��
    */
    pointer = Table::get_idtable_size();
}

void GrammarAnalysis::prt_grammar_info ( string name ) {
    /*
        ����﷨�����Ľ��
    */
    cout << "This is a " << name << endl;
}

void GrammarAnalysis::err_report ( int index ) {
    //����
    ErrorProcess::add_err(nowword.loc,index);
}

void GrammarAnalysis::ga_programme () {
    /*
        <����>
    */
    nowword = nextword();
    if ( nowword.value == "const" ) {
        ga_constant();
    }

    if ( nowword.is_vartype() ) {
        nowword = nextword();
        nowword = nextword();
        if ( nowword.value == "," || nowword.value == ";"
              || nowword.value == "[") { //����˵��
            //cout << nowword.value << "yyyoioi";
            lastword();
            lastword();
            lastword();
            nowword = nextword();
            ga_variable();

        }
        else if ( nowword.value == "(" ) { //����˵��
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

        if ( nowword.is_vartype() ) { //�з���ֵ����˵��
            //cout << nowword.value << "#yyy";
            ga_returnfun();
            //cout << nowword.value << "###yhdg" << endl;
            nowword = nextword();
        }
        else if ( nowword.value == "void" ) {
            nowword = nextword();
            //cout << nowword.value << "#yyy";
            if ( nowword.value == "main") { //��ʱ���к����Ѿ�����������
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
        <��������>�ӳ���
        �����Ԥ��
        ������Ԥ��
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
        <��������>�ӳ���
        ��ڲ�Ҫ��Ԥ��
        ������Ԥ��
    */
    nowword = nextword();

    if ( nowword.is_vartype() ) {

        string data_typ = (nowword.value == "int")?"INTEGER"
                            :( nowword.value == "char")?"CHARACTER":"FLOAT";
        while ( true ) {
            nowword = nextword();
            if ( nowword.type == "IDENT") {
                string name = nowword.value; //��¼��ʶ��������
                nowword = nextword();
                if ( nowword.type != "EQUAL" ) {
                    //����
                }
                nowword = nextword();
                if ( nowword.type != data_typ ) { //��鳣�������Ƿ�һ��
                    //baocuo
                }

                //����ű�
                if ( data_typ == "int" ) {
                    //��stringת������
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
                //����
            }

            //���Ƿ��ж������һ����
            nowword = nextword();

            if ( nowword.value != "," ) {
                break; //���Ƕ��������ѭ��
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
        <����˵��>�ӳ���
        �����Ԥ��
        ������Ԥ��
    */
    while (true) {
        ga_vardef();
        if ( nowword.value != ";" ) {
            //baocuo
        }

        nowword = nextword();
        if ( nowword.value == "void" ) { //�п��������������޷���ֵ����
            break; // ��ʱ����˵���ѽ���������ѭ��
        }
        else {

            if (  nowword.is_vartype() ) {
                nowword = nextword();
                if ( nowword.type != "IDENT" ) {
                //baocuo
                }

                nowword = nextword();
                if ( nowword.value == "(" ) { //�п������з���ֵ�ĺ���
                    //��ʱҪ��ָ����������λ��Ȼ������ѭ��
                    lastword();
                    lastword();
                    lastword();
                    nowword = nextword();
                    break;
                }
                else if ( nowword.value == ";" || nowword.value == ","
                         || nowword.value == "[") {
                    // ��ʱ���ڱ�������׶�
                    //��ʱҪ��ָ����������λ��Ȼ�����ѭ��
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
        <��������>�ӳ���
        �����Ԥ��
        ������Ԥ��
    */
    if ( nowword.is_vartype() ) {
        string data_type = nowword.value;//��¼��������
        while ( true ) {
            nowword = nextword();
            string name = nowword.value;//��¼��ʶ������
            if ( nowword.type != "IDENT" ) {
                //baocuo
            }
            nowword = nextword();
            if ( nowword.value == "[" ) { // �п���������
                nowword = nextword();
                if ( nowword.type != "INTEGER" ) {
                    // ����
                }
                int arr_size = str2num<int>(nowword);//��¼����Ĵ�С
                nowword = nextword();
                if ( nowword.value != "]" ) {
                    //baocuo
                }

                //����ű�����һ�������¼
                id_type t = (data_type == "int")?INT_ARRAY:
                                (data_type=="char")?CHAR_ARRAY:FLOAT_ARRAY;
                Table::add_idrcd(name,t,0,arr_size);

                nowword = nextword();
            }
            else {//��ͨ����
                //����ű�,����һ����ͨ������¼
                id_type t = (data_type == "int")?INT_VAR:
                                (data_type=="char")?CHAR_VAR:FLOAT_VAR;
                Table::add_idrcd(name,t);
            }

            if ( nowword.value != ",") { // ��������Ķ����Զ��ŷָ�
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
        <�з���ֵ��������>�ӳ���
        �����Ԥ��
        ������Ԥ��

    */
    //��ǰ�������Ƿ���ֵ����
    string rtn_type = nowword;//��¼��������
    nowword = nextword();
    string name = nowword;//��¼������
    if ( nowword.type != "IDENT" ) {
        //baocuo
    }
    else {//����ű�
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
        //�޲���,��lastparֵ
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
        <�޷���ֵ��������>�ӳ���
        �����Ԥ��
        ������Ԥ��

    */
    nowword = nextword();
    string name = nowword;//��¼������
    if ( nowword.type != "IDENT" ) { //�����������Ǳ�ʶ���򱨴�
        //baocuo
    }
    else { //����ű�
        Table::add_idrcd(name,VOID_FUNCTION);
        renew_pointer();
    }

    nowword = nextword();
    if ( nowword.value != "(" ) {
        //baocuo
    }

    nowword = nextword();
    if ( nowword.type == ")" ) {
        //�޲���,��lastparֵ
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
        <������>�ӳ���
        �����Ԥ��
        ������Ԥ��

    */
    nowword = nextword();
    if ( nowword.type != "main" ) {
        //baocuo
    }
    else { //����ű�
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
        <������>�ӳ���
        �����Ԥ��
        ������Ԥ��
    */

    if ( nowword.is_vartype() ) {
        string data_type = nowword;//��¼��������
        nowword = nextword();
        if ( nowword.type != "IDENT" ) {
            //baocuo
        }
        else{ //��¼���ű�,ͬʱ�ı亯�����lastparֵ
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
            else { //��¼���ű�ͬʱ�ı亯�����lastparֵ
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
        <�������>�ӳ���
        �����Ԥ��
        ������Ԥ��
    */
    nowword = nextword();
    //cout << nowword.value << "rrrtttrrt";

    if ( nowword.value == "}" ) { //ʲô��û�еĸ������
        prt_grammar_info("complex statement");
        return;
    }

    if ( nowword.value == "const" ) {//����˵��
        ga_constant();
    }

    if ( nowword.is_vartype() ) { //����˵��
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
        <���>�ӳ���
        �����Ԥ��
        ������Ԥ��
    */

    if ( nowword.value == ";" ) { // �����
        nowword = nextword();
        prt_grammar_info("empty statement");
        return;
    }
    else if ( nowword.value == "if" ) { //�������
        //cout << nowword.value << "jojojxxx%";
        nowword = nextword();
        ga_condition_stmt();
        //cout << nowword.value << "jojojxxx%";
        return;
    }
    else if ( nowword.value == "while" ) { //ѭ�����

        nowword = nextword();
        ga_cycle_stmt();
        //cout << nowword.value << "jojojxxx%";
        return;
    }
    else if ( nowword.value == "switch" ) { //������
        nowword = nextword();
        ga_case_stmt();
        nowword = nextword();
        return;
    }
    else if ( nowword.value == "scanf" ) { //�����
        nowword = nextword();
        ga_read_stmt();
        nowword = nextword();
        if ( nowword.value != ";" ) {
            //baocuo
        }
        nowword = nextword();
        return;
    }
    else if ( nowword.value == "printf" ) { //д���
        nowword = nextword();
        ga_write_stmt();
        nowword = nextword();
        if ( nowword.value != ";" ) {
            //baocuo
        }
        nowword = nextword();
        return;
    }
    else if ( nowword.value == "return" ) { //�������
        nowword = nextword();
        ga_return_stmt();
        nowword = nextword();
       // cout << nowword.value << "xxx%";
        if ( nowword.value != ";" ) {
            //baocuo
            cout << "©�˷ֺ�";
        }
        nowword = nextword();
        return;
    }
    else if ( nowword.value == "{" ) { //�����
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
        string name = nowword;// ��¼��ʶ��������

        nowword = nextword();
        if ( nowword.value == "=" || nowword.value == "[" ) { //��ֵ���
            //nowword = nextword();
            ga_assign_stmt();
            if ( nowword.value != ";" ) {
            //baocuo
            cout << "©�˷ֺ�";
            }
            nowword = nextword();
            return;
        }
        else if ( nowword.value == "(" ) { //�����������

            id_type rt = Table::get_func_type(name);//��øú����ķ���ֵ
            if ( rt == ERROR ) {
                //baocuo
            }
            else if ( rt == VOID_FUNCTION ) {//�޷���ֵ��������
                nowword = nextword();
                ga_voidfuncall_stmt();
            }
            else { //�з���ֵ��������
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
        <�������>�ӳ���
        Ҫ�����Ԥ��
        ������Ԥ��
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
        <ѭ�����>�ӳ���
        Ҫ�����Ԥ��
        ������Ԥ��
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
        <�з���ֵ�ĺ����������>�ӳ���
        Ҫ����ڴ���Ԥ��
        ���ڴ���Ԥ��
    */
    nowword = nextword();
    if ( nowword.value == ")" ) { //ֵ������Ϊ��
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
        <�з���ֵ�ĺ����������>�ӳ���
        Ҫ����ڴ���Ԥ��
        ���ڴ���Ԥ��
    */
    nowword = nextword();
    if ( nowword.value == ")" ) { //ֵ������Ϊ��
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
       <��ֵ���>�ӳ���
        Ҫ����ڴ���Ԥ��
        ���ڴ���Ԥ��
    */

    if ( nowword.value == "[" ) { //����ĸ�ֵ

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
       <�����>�ӳ���
        Ҫ����ڴ���Ԥ��
        ���ڴ���Ԥ��
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
        <д���>�ӳ���
        Ҫ����ڴ���Ԥ��
        ���ڴ���Ԥ��
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
        <�������>�ӳ���
        Ҫ����ڴ���Ԥ��
        ���ڴ���Ԥ��
    */
    if ( nowword.value != "(" ) {
        //baocuo
        cout << "©��������";
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
        <������>�ӳ���
        Ҫ�����Ԥ��
        ������Ԥ��
    */
    if ( nowword.value == "(" ) {

        nowword = nextword();
        ga_expression();
        if ( nowword.value != ")" ) {
            //����
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
                    while ( nowword.value == "case" ) { //���ڶ�����������ʱ��
                        ga_subcase_stmt();
                    }
                }

                if ( nowword.value != "}") { //����β�Ƿ�������
                    //baocuo
                }
                //cout << nowword.value << "jbһ��555xxx%";
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
        <��������>�ӳ���
        ��ڲ�Ҫ��Ԥ��
        ������Ԥ��
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
        <����>�ӳ���
        ��ڲ���ҪԤ��
        ������Ԥ��
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
        <���ʽ>�ӳ���
        Ҫ�������Ԥ��
        ���ڴ���Ԥ��
    */
    if ( nowword.is_addition_op() ) {
        nowword = nextword();
    }

    ga_item();
    while ( nowword.is_addition_op() ) {
        string op = nowword;
        nowword = nextword();
        ga_item();
        //���ɲ�������ָ��
        ( op == "+" )? Table::emit(ADD): Table::emit(SUB);
    }

}

void GrammarAnalysis::ga_item() {
    /*
        <��>�ӳ���
        �����Ԥ��
        ������Ԥ��
    */
    ga_factor();
    while ( nowword.is_multiplication_op() ) {
        string op = nowword;
        nowword = nextword();
        ga_factor();
        //���ɲ�����ָ��
        ( op == "*" )? Table::emit(MUL): Table::emit(DIV);
    }

}

void GrammarAnalysis::ga_factor() {
    /*
        <����>�ӳ���
        �����Ԥ��
        ������Ԥ��
    */
    if ( nowword.type == "INTEGER" || nowword.type == "CHARACTER"
        || nowword.type == "FLOAT" ) { //δ����ĳ���

        int a;
        int t;
        if ( nowword.type == "INTEGER") {
            a = str2num<int>(nowword.value);
            t = 0;
        }
        else if ( nowword.type == "FLOAT" ) {
            a = Table::get_rctable_size();
            //�Ѹ�ʵ����ֵ����ʵ������
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
    else if ( nowword.type == "IDENT" ) { //������������������

        string id = nowword.value;//��¼��ʶ��������
        int loc = Table::find_ident(pointer,id);
        id_rcd r;
        if ( loc == -1 ) { //û�ҵ��ñ�ʶ��
            //baocuo
        }
        else {
            //�ӷ��ű�����ȡ���ñ�ʶ���ļ�¼
            r = Table::get_idrcd(loc);
        }

        nowword = nextword();
        if ( nowword.value == "[" ) { // �п���������

            //���жϸñ�ʶ���ǲ�������
            if ( !Table::is_arrayrcd(r) ) {
                //baocuo
            }
            int arr_size = Table::get_array_size(r);
            int index;
            if ( arr_size <= 0 ) { //Ԫ�ظ���Ӧ��������
                //baocuo
            }

            nowword = nextword();
            ga_expression(); // ʶ���ʾ�����±�ı��ʽ
            //�õ�����ջջ��������,�����ж�
            if ( Runtime.get_top_type() != RS_INT ) {
            //baocuo
            }
            else {
                index = Runtime::get_top_value();//�����±�
                if ( index < 0 || index >= arr_size ) { //�����±�Խ��
                    //����
                }
            }

            if ( nowword.value != "]" ) {
                //baocuo
            }
            else { //����ָ��
                int l = ( r.type == INT_ARRAY )? 0:( r.type == CHAR_ARRAY )? 1:2;
                Table::emit(LOD,l,r.adr+index);
            }
            nowword = nextword();
            return;

        }

        else if ( nowword.value == "(" ) { // �������з���ֵ�ĺ����������
            ga_retfuncall_stmt();
            return;
        }

        else if ( Table::is_constrcd(r) ) { //����
            int l = ( r.type == INT_CONST )? 0:( r.type == CHAR_CONST )? 1:2;
            Table::emit(LIT,l,r.adr);
            return;
        }

        else if ( Table::is_varrcd(r) ) { //��ͨ����
            int l = ( r.type == INT_VAR )? 0:( r.type == CHAR_VAR )? 1:2;
            Table::emit(LOD,l,r.adr);
            return;
        }
        else {
            //δ֪��ʶ������
        }



    }
    else if ( nowword.type == "(" ) { //�����Ǳ��ʽ
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

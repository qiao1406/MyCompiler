#include "GrammarAnalysis.h"
#include "ErrorProcess.h"

#include<sstream>
#include<queue>

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

int GrammarAnalysis::get_line_loc() {
    return nowword.loc;
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

void GrammarAnalysis::jump_read ( vector<string> v ) {
    /*
        ����Դ����,ֱ������v�е��ַ���Ϊֹ
        �����Ԥ��
        ������Ԥ��
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
        <����>
    */
    nowword = nextword();
    if ( nowword.value == "const" ) {//ȫ�ֳ�����ȡ
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
        else {//���Ǻ�����������������һ��var_type
            err_report(33);
            jump_read({"int","char","float","void"});
        }
    }

    while ( true ) {

        if ( nowword.is_vartype() ) { //�з���ֵ����˵��
            ga_returnfun();
            nowword = nextword();
        }
        else if ( nowword.value == "void" ) {
            nowword = nextword();
            if ( nowword.value == "main") { //��ʱ���к����Ѿ�����������
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
        else {//������������
            err_report(34);
            return;
        }
    }
    //��ȡ������
    ga_mainfun();
    return;
}

void GrammarAnalysis::ga_constant () {
    /*
        <����˵��>�ӳ���
        �����Ԥ��
        ������Ԥ��
    */
    while ( nowword.value == "const" ) {
        ga_constdef();

        if ( nowword.value != ";" ) {
            //��©�ֺţ�����������
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
                if ( nowword.type != "BECOME" ) {
                    err_report(3);
                    jump_read({";"});
                    return;
                }
                nowword = nextword();
                if ( nowword.type != data_typ ) { //��鳣�������Ƿ�һ��
                    //����һ���򱨴�����ѭ��
                    err_report(35);
                    continue;
                }

                //����ű�
                if ( data_typ == "INTEGER" ) { //��stringת��int��
                    Table::add_idrcd(name,str2num<int>(nowword.value),INT_CONST);
                }
                else if ( data_typ == "CHARACTER" ) {
                    int ascii_code = nowword.value[1]; //����'A��Ӧ��ȡ��[1]λ��asciiֵ
                    Table::add_idrcd(name,ascii_code,CHAR_CONST);
                }
                else {//��stringת��float��
                    Table::add_idrcd(name,str2num<float>(nowword.value));
                }

            }
            else {
                //����,Ӧ���Ǳ�ʶ��
                err_report(2);
                jump_read({";"});
                return;
            }

            //���Ƿ��ж������һ����
            nowword = nextword();

            if ( nowword.value != "," ) {
                break; //���Ƕ��������ѭ��
            }
        }

       return;
    }
    else { //���Ǳ������ͷ�
        err_report(28);
        jump_read({";"});
        return;
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
            //��©�ֺţ�����������
            err_report(11);
            jump_read({"int","char","float","void"});
        }
        else {
            nowword = nextword();
        }

        if ( nowword.value == "void" ) { //�п��������������޷���ֵ����
            break; // ��ʱ����˵���ѽ���������ѭ��
        }
        else {

            if ( nowword.is_vartype() ) {
                nowword = nextword();
                if ( nowword.type != "IDENT" ) {
                    //���Ǳ�ʶ����
                    err_report(2);
                    jump_read({";"});
                    continue;
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

    prt_grammar_info("variable declaration");
}

void GrammarAnalysis::ga_vardef () {
    /*
        <��������>�ӳ���
        �����Ԥ��
        ������Ԥ��
    */

    if ( nowword.is_vartype() ) {
        string data_typ = nowword.value;//��¼��������(int,char,float)
        while ( true ) {

            nowword = nextword();
            string name = nowword.value;//��¼��ʶ������
            if ( nowword.type != "IDENT" ) {
                err_report(2);
                jump_read({";"});
                return;
            }

            nowword = nextword();
            if ( nowword.value == "[" ) { // �п���������

                nowword = nextword();
                if ( nowword.type != "INTEGER" ) { //����Ĵ�С����������
                    err_report(31);
                    jump_read({";"});
                    return;
                }
                int arr_size = str2num<int>(nowword.value);//��¼����Ĵ�С
                if ( arr_size <= 0 ) {
                    //�����С��������������������������
                    err_report(42);
                }
                nowword = nextword();
                if ( nowword.value != "]" ) {
                    //ȱ���������ţ�����
                    err_report(7);
                    jump_read({";"});
                    return;
                }

                //����ű�����һ�������¼
                id_type t = (data_typ == "int")?INT_ARRAY:
                                (data_typ =="char")?CHAR_ARRAY:FLOAT_ARRAY;
                Table::add_idrcd(name,t,0,arr_size);

                nowword = nextword();
            }
            else {//��ͨ����
                //����ű�,����һ����ͨ������¼
                id_type t = (data_typ == "int")?INT_VAR:
                                (data_typ=="char")?CHAR_VAR:FLOAT_VAR;
                Table::add_idrcd(name,t);
            }

            if ( nowword.value != ",") { // ��������Ķ����Զ��ŷָ�
                break;
            }
        }

        //prt_grammar_info("variable define");
        return;
    }
    else {//�����������ͷ�
        err_report(28);
        jump_read({";"});
        return;
    }
}

void GrammarAnalysis::ga_returnfun () {
    /*
        <�з���ֵ��������>�ӳ���
        �����Ԥ��
        ������Ԥ��

    */

    //��ǰ�������Ƿ���ֵ����
    string rtn_type = nowword.value;//��¼��������
    nowword = nextword();
    string name = nowword.value;//��¼������
    if ( nowword.type != "IDENT" ) {
        err_report(2);
        jump_read({"int","char","void","float"});
        return;
    }
    else {//����ű�
        renew_pointer();//�������ں�����ָ��
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
        //�޲���,��lastparֵ
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
        //ȱ�ٴ����ţ���������һ����
        err_report(8);
        lastword();
        lastword();
        nowword = nextword();
    }

    //��¼���������
    Table::set_ploc(name);

    //���ɺ������岿�ֵ�PCode
    ga_complex_stmt();
    if ( nowword.value != "}" ) {
        //ȱ�ٷ������ţ�����
        err_report(9);
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
    string name = nowword.value;//��¼������
    if ( nowword.type != "IDENT" ) { //�����������Ǳ�ʶ���򱨴�
        err_report(2);
        jump_read({"int","char","void","float"});
        return;
    }
    else { //����ű�
        renew_pointer();//�������ں�����ָ��
        Table::add_idrcd(name,VOID_FUNCTION);
    }

    nowword = nextword();
    if ( nowword.value != "(" ) {
        //ȱ�������ţ�����
        err_report(4);
    }
    else {
        nowword = nextword();
    }

    if ( nowword.value == ")" ) {
        //�޲���,��lastparֵ
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

    if ( nowword.value != ")" ) { //ȱ�ٷ����ţ�����
        err_report(5);
    }
    else {
        nowword = nextword();
    }

    if ( nowword.value != "{" ) {
        //ȱ�ٴ����ţ���������һ����
        err_report(8);
        lastword();
        lastword();
        nowword = nextword();
    }

    //��¼���������
    Table::set_ploc(name);

    //�������岿�ֵ�PCode
    ga_complex_stmt();
    if ( nowword.value != "}" ) {
        //ȱ�ٷ������ţ�����
        err_report(9);
    }

    Table::emit(JR);//�޷���ֵ�ĺ���ĩβ��Ҫ��һ��JRָ��
    prt_grammar_info("function that has no return value\n");
}

void GrammarAnalysis::ga_mainfun () {
    /*
        <������>�ӳ���
        �����Ԥ��
        ������Ԥ��

    */

    nowword = nextword();
    if ( nowword.value != "main" ) {
        err_report(37);
        return;
    }
    else { //����ű�
        renew_pointer();//�������ں�����ָ��
        Table::add_idrcd("main",VOID_FUNCTION);
    }

    nowword = nextword();
    if ( nowword.value != "(" ) {
        //ȱ��������,����
        err_report(4);
    }
    else {
        nowword = nextword();
    }

    if ( nowword.value != ")" ) {
        //ȱ�������ű���
        err_report(5);
    }
    else {
        nowword = nextword();
    }

    if ( nowword.value != "{" ) {
        //ȱ��������š�����
        err_report(8);
    }
    else {
        nowword = nextword();
    }

    //��main�����Ķ�ȡ����ʱ������ga_complex_stmt()
    //��Ϊ�����PUFָ��

    if ( nowword.value == "const" ) {//����˵��
        ga_constant();
    }

    if ( nowword.is_vartype() ) { //����˵��
        ga_variable();
    }

    //��¼���������
    Table::set_ploc("main");

    //�趨main������ʼ��λ��
    Runtime::set_main_pointer(Table::get_pctable_size());

    //����PUFָ��
    Table::emit(PUF,3,pointer);

    while ( nowword.value != "}") {
        ga_statement();
    }

    if ( nowword.value != "}" ) {
        //ȱ���Ҵ����ţ�����
        err_report(9);
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
        string data_type = nowword.value;//��¼��������
        nowword = nextword();
        if ( nowword.type != "IDENT" ) {
            err_report(2);
        }
        else{ //��¼���ű�,ͬʱ�ı亯�����lastparֵ
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
                //���Ǳ�ʶ��������һ���ʣ�������ѭ��
                err_report(2);
                nowword = nextword();
                continue;
            }
            else { //��¼���ű�ͬʱ�ı亯�����lastparֵ
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
        //���Ǳ������ͷ��ţ�����
        err_report(28);
        return;
    }
}

void GrammarAnalysis::ga_complex_stmt(){
    /*
        <�������>�ӳ���
        �����Ԥ��
        ������Ԥ��
    */
    nowword = nextword();

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
        nowword = nextword();
        ga_condition_stmt();
        return;
    }
    else if ( nowword.value == "while" ) { //ѭ�����

        nowword = nextword();
        ga_cycle_stmt();
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
            err_report(11);
        }
        else {
            nowword = nextword();
        }
        return;
    }
    else if ( nowword.value == "printf" ) { //д���
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
    else if ( nowword.value == "return" ) { //�������
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
    else if ( nowword.value == "{" ) { //�����
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
                //ȱ���Ҵ����ţ�����һ����
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

        string name = nowword.value;// ��¼��ʶ��������
        nowword = nextword();
        if ( nowword.value == "=" || nowword.value == "[" ) { //��ֵ���
            ga_assign_stmt(name);
            if ( nowword.value != ";" ) {
                //��©�ֺ�
                err_report(11);
            }
            else{
                nowword = nextword();
            }
            return;
        }
        else if ( nowword.value == "(" ) { //�����������

            id_type rt = Table::get_func_type(name);//��øú����ķ���ֵ����
            if ( rt == ERROR ) {
                //�����ķ���ֵ����
                err_report(38);
                return;
            }
            else if ( rt == VOID_FUNCTION ) {//�޷���ֵ��������
                //nowword = nextword();
                ga_voidfuncall_stmt(name);
            }
            else { //�з���ֵ��������
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
        <�������>�ӳ���
        Ҫ�����Ԥ��
        ������Ԥ��
    */

    if ( nowword.value != "(" ) {
        //���������ţ���������һ����
        err_report(4);
        lastword();
        lastword();
        nowword = nextword();
    }
    ga_condition();
    //��ʱ�������ʽ����ֵ�Ѿ�д��ջ��
    if ( nowword.value != ")" ) {
        //baocuo
        err_report(5);
    }
    int brf_loc = Table::get_pctable_size();//��¼BRFָ���λ��
    Table::emit(BRF,0,0);
    nowword = nextword();
    ga_statement();

    if( nowword.value == "else" ) {
        int jmp_loc = Table::get_pctable_size();//��¼JMPָ���λ��
        Table::emit(JMP,0,0);
        Table::update_emit(BRF,0,Table::get_pctable_size(),brf_loc);
        nowword = nextword();
        ga_statement();
        Table::update_emit(JMP,0,Table::get_pctable_size(),jmp_loc);
    }
    else { //û��else������
        Table::update_emit(BRF,0,Table::get_pctable_size(),brf_loc);
    }

    prt_grammar_info("condition statement#");
}

void GrammarAnalysis::ga_cycle_stmt(){
    /*
        <ѭ�����>�ӳ���
        Ҫ�����Ԥ��
        ������Ԥ��
    */
    if ( nowword.value != "(" ) {
        //���������ţ���������һ����
        err_report(4);
        lastword();
        lastword();
        nowword = nextword();
    }
    int jp_loc = Table::get_pctable_size();//һ��ѭ������ת�ĵ�ַ

    ga_condition();
    //��ʱ�Ѿ�������������ص�ջ����
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
        <�з���ֵ�ĺ����������>�ӳ���
        Ҫ����ڴ���Ԥ��
        ���ڴ���Ԥ��

        �˴���ȡ�Ĳ�������������ر�ʾֵ�����ı��ʽ
        Ȼ������PUFָ��Ѻ�����ջ��Ȼ���ٵ��������β�
    */

    //i�Ǻ������ڷ��ű��е��±�ֵ
    int i = Table::find_ident_fun(func_name);
    int i1 = i;//��¼��������λ��
    id_rcd r, temp;//r�Ǻ������ļ�¼
    if ( i == -1 ) {
        //û�ҵ��ú���
        err_report(38);
        jump_read({";"});
        return;
    }
    else {
        r = Table::get_idrcd(i);
    }

    nowword = nextword();
    if ( nowword.value == ")" ) { //ֵ������Ϊ��
        if (  i != Table::get_lastpar(r) ) { //������������
            err_report(21);
            jump_read({";"});
            return;
        }
    }
    else { //װ���ʾֵ�����ı��ʽ
        ga_expression();//��ֵ������ֵд��ջ��
        i++;
        Table::emit(SDS);//��ֵ������ֵд������ջջ��

        while ( nowword.value == "," ) {
            nowword = nextword();
            ga_expression();//��ֵ������ֵд��ջ��
            i++;
            Table::emit(SDS);//��ֵ������д������ջջ��

        }

        if (  i != Table::get_lastpar(r) ) { //������������
            err_report(21);
            jump_read({";"});
            return;
        }

    }

    if ( nowword.value == ")" ) {
        //����PUFָ���������ջ
        int t = ( r.type == INT_FUNCTION )?0:(( r.type == CHAR_FUNCTION )?1:2);
        Table::emit(PUF,t,i1);

        //�����¼�βε�ֵ
        for ( i = Table::get_lastpar(r); i > i1; i-- ){
            temp = Table::get_idrcd(i);
            Table::emit(LDS);//������ջջ���õ�ʵ������ֵ
            Table::emit(STO,temp.lev,temp.adr);//��ʵ�ε�ֵ�����βε�λ��
        }

        //�趨���ص�ַ�Լ���ת���������
        Table::emit(JSR,Table::get_ploc(r),Table::get_pctable_size()+1);
        nowword = nextword();
        prt_grammar_info("return-funcall statement");
        return;
    }
    else {
        //ȱ�������ţ�����
        err_report(5);
//        lastword();
//        lastword();
//        nowword = nextword();
    }

}

void GrammarAnalysis::ga_voidfuncall_stmt ( string func_name ){
    /*
        <�޷���ֵ�ĺ����������>�ӳ���
        Ҫ����ڴ���Ԥ��
        ���ڴ���Ԥ��

        �˴���ȡ�Ĳ�������������ر�ʾֵ�����ı��ʽ
        Ȼ������PUFָ��Ѻ�����ջ��Ȼ���ٵ��������β�
    */

    //i�Ǻ������ڷ��ű��е��±�ֵ
    int i = Table::find_ident_fun(func_name);
    int i1 = i;//��¼��������λ��
    id_rcd r, tmp;//r�Ǻ������ļ�¼
    if ( i == -1 ) { //�Ҳ����ú���
        err_report(38);
        jump_read({";"});
        return;
    }
    else {
        r = Table::get_idrcd(i);
    }

    nowword = nextword();
    if ( nowword.value == ")" ) { //ֵ������Ϊ��
        if (  i != Table::get_lastpar(r) ) { //������������
            err_report(21);
            jump_read({";"});
            return;
        }
    }
    else {
        ga_expression();
        i++;
        Table::emit(SDS);//��ֵ������ֵд������ջջ��

        while ( nowword.value == "," ) {
            nowword = nextword();
            ga_expression();
            i++;
            Table::emit(SDS);//��ֵ������ֵд������ջջ��;
        }

        if (  i != Table::get_lastpar(r) ) { //������������
            err_report(21);
            jump_read({";"});
            return;
        }

    }

    if ( nowword.value == ")" ) {

        //����PUFָ���������ջ
        Table::emit(PUF,3,i1);

        //�����¼�βε�ֵ
        for ( i = Table::get_lastpar(r); i > i1; i-- ){
            tmp = Table::get_idrcd(i);
            Table::emit(LDS);//������ջջ���õ�ʵ������ֵ
            Table::emit(STO,tmp.lev,tmp.adr);
        }

        //�趨���ص�ַ�Լ���ת���������
        Table::emit(JSR,Table::get_ploc(r),Table::get_pctable_size()+1);
        nowword = nextword();
        prt_grammar_info("noreturn-funcall statement");
        return;

    }
    else {
        //ȱ�������ţ�����
        err_report(5);
//        lastword();
//        lastword();
//        nowword = nextword();
    }
}

void GrammarAnalysis::ga_assign_stmt ( string id_name ) {
    /*
       <��ֵ���>�ӳ���
        Ҫ����ڴ���Ԥ��
        ���ڴ���Ԥ��
    */

    //�����ͼ�ҵ��ñ�ʶ���ļ�¼
    int loc = Table::find_ident(pointer,id_name);
    id_rcd r;
    if ( loc == -1 ) { //û�ҵ��ñ�ʶ������������
        err_report(2);
        jump_read({";"});
        return;
    }
    else {
        //�ӷ��ű�����ȡ���ñ�ʶ���ļ�¼
        r = Table::get_idrcd(loc);
    }

    if( !( Table::is_varrcd(r) || Table::is_arrayrcd(r) ) ) {
        //�ñ�ʶ�����Ǳ��������飬����
        err_report(41);
        jump_read({";"});
        return;
    }

    if ( nowword.value == "[" ) { //����ĸ�ֵ

        //���жϸñ�ʶ���ǲ�������
        if ( !Table::is_arrayrcd(r) ) {
            err_report(32);
            jump_read({";"});
            return;
        }
        int arr_size = Table::get_array_size(r);
        int index;
        array_rcd ar = Table::get_arrayrcd(r.ref);
        if ( arr_size <= 0 ) { //Ԫ�ظ���Ӧ��������
            err_report(42);
        }

        nowword = nextword();
        ga_expression();//�����±���ʽ��ֵ
        //��ʱ�����±��ֵ�Ѿ�д��ջ��,��ʱ�����ŵĵ�ַ

        Table::emit(LOI,0,r.adr);
        Table::emit(ADD);

        if ( nowword.value != "]" ) {
            //ȱ���������ţ�����
            err_report(7);
        }
        else {
           nowword = nextword();
        }

        if ( nowword.value != "=" ) {
            //ȱ�ٵȺţ�����
            err_report(3);
            jump_read({";"});
            return;
        }

        nowword = nextword();
        ga_expression();//���㸳ֵ���ʽ��ֵ
        //��ʱҪд��ĵ�ַ�ڴ�ջ�������ʽ��ֵ��ջ��
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
       <�����>�ӳ���
        Ҫ����ڴ���Ԥ��
        ���ڴ���Ԥ��
    */
    if ( nowword.value != "(" ) {
        err_report(4);
    }
    else {
        nowword = nextword();
    }

    id_rcd r;
    if ( nowword.type != "IDENT" ) { //���Ǳ�ʶ��������
        err_report(2);
        jump_read({","});
    }
    else {//�жϱ�ʶ�������ͣ�ֻ������ͨ����
        int loc = Table::find_ident(pointer,nowword.value);
        if ( loc == -1 ) {//δ�ҵ���ʶ��������
            err_report(43);
            jump_read({","});
        }
        else {

            r = Table::get_idrcd(loc);
            if ( ! Table::is_varrcd(r) ) {
                //���Ĳ��Ǳ���������
                err_report(19);
            }
            else {//���ɶ�ָ��
                //���ݲ�ͬ�������������ɲ�ͬ��ָ��
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
            //���Ǳ�ʶ��������
            err_report(2);
            jump_read({","});
        }
        else {//�жϱ�ʶ�������ͣ�ֻ������ͨ����
            int loc = Table::find_ident(pointer,nowword.value);
            if ( loc == -1 ) {
                //δ�ҵ���ʶ��������
                err_report(43);
                jump_read({","});
            }
            else {
                r = Table::get_idrcd(loc);
                if ( ! Table::is_varrcd(r) ) {
                    //�ñ�ʶ�����Ǳ���������
                    err_report(19);
                }
                else {//���ɶ�ָ��
                    ( r.type == INT_VAR )? Table::emit(RDA,0):
                        ( r.type == CHAR_VAR )? Table::emit(RDA,1):Table::emit(RDA,2);
                    Table::emit(STO,r.lev,r.adr);
                }

            }
        }
        nowword = nextword();
    }

    if ( nowword.value != ")" ) {
        //ȱ�������ţ�����һ����
        err_report(5);
        lastword();
        lastword();
        nowword = nextword();
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
        err_report(4);
    }
    else {
        nowword = nextword();
    }

    if ( nowword.type == "STRING" ) {
        //�Ȱ��ַ����Ǽǵ��ַ�������
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
        //ȱ�������ţ�����һ����
        err_report(5);
        lastword();
        lastword();
        nowword = nextword();
    }
    prt_grammar_info("write statement");
}

void GrammarAnalysis::ga_return_stmt(  ){
    /*
        <�������>�ӳ���
        Ҫ����ڴ���Ԥ��
        ���ڴ���Ԥ��
    */

    id_rcd r = Table::get_funrcd();//��¼��ǰ�ķ���������ڵĺ���

    if ( nowword.value != "(" ) {//ֻ�С�return��Ҳ�ǺϷ���
        //���Ǵ�ʱҪ����һ����
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
            //ȱ�������ţ�����һ����
            err_report(5);
            lastword();
            lastword();
            nowword = nextword();
        }
        else {
            //�ж�һ���Ƿ�����void���������Ҫ����
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
        <������>�ӳ���
        Ҫ�����Ԥ��
        ������Ԥ��
    */

    queue<int> bq;//��¼BNEָ��ĵ�ַ
    queue<int> jq;//��¼JMPָ��ĵ�ַ

    if ( nowword.value == "(" ) {

        nowword = nextword();
        ga_expression();
        //��ʱ���ʽ��ֵ��д��ջ�����ڱȽ�
        if ( nowword.value != ")" ) {
            //����
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
                        //����BNEָ��
                        bq.push(Table::get_pctable_size());
                        Table::emit(BNE,0,0);
                        ga_subcase_stmt();
                        jq.push(Table::get_pctable_size());
                        Table::emit(JMP,0,0);

                    }

                    while ( nowword.value == "case" ) { //���ڶ�����������ʱ��
                        nowword = nextword();
                        if ( nowword.type != "INTEGER"
                             && nowword.type != "CHARACTER" ) {
                            //baocuo
                            err_report(44);
                        }
                        else {
                            ( nowword.type == "CHARACTER" )? Table::emit(LOI,1,(int)nowword.value[1])
                                    :Table::emit(LOI,0,str2num<int>(nowword.value));
                            //����BNEָ��
                            bq.push(Table::get_pctable_size());
                            Table::emit(BNE,0,0);
                            ga_subcase_stmt();
                            jq.push(Table::get_pctable_size());
                            Table::emit(JMP,0,0);
                        }
                    }
                }

                if ( nowword.value != "}") { //����β�Ƿ�������
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
        <��������>�ӳ���
        ���Ҫ��Ԥ��
        ������Ԥ��
    */

    nowword = nextword();
    if ( nowword.value != ":" ) {
        //ȱ��ð�ţ�������һ����
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
        <����>�ӳ���
        ��ڲ���ҪԤ��
        ������Ԥ��
    */

    nowword = nextword();
    ga_expression();
    if ( nowword.is_relation_op() ) {

        string op = nowword.value;
        nowword = nextword();
        ga_expression();

        //��ʱ���ʽ����ֵ��ջ������ֵ�ڴ�ջ��
        //���չ�ϵ�������ֵ�Ĳ�ͬ�����з���
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
        else { // op��>=�����
            Table::emit(GEQ);
        }

    }
    //ֻ��һ�����ʽ��û�й�ϵ�������ʱ�򣬿��Բ������ɱȽϵ�ָ��
}

void GrammarAnalysis::ga_expression() {
    /*
        <���ʽ>�ӳ���
        Ҫ�������Ԥ��
        ���ڴ���Ԥ��
    */
    bool flag = false;

    if ( nowword.is_addition_op() ) {

        if ( nowword.value == "-" ) {
            flag = true;
        }
        nowword = nextword();
    }
    ga_item();
    if ( flag ) {//�����ǰ�õĸ�����Ҫ����һ����ȡ��
        Table::emit(LOI,0,-1);
        Table::emit(MUL);
    }

    while ( nowword.is_addition_op() ) {
        string op = nowword.value;
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
        string op = nowword.value;
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
            a = nowword.value[1];
            t = 1;
        }
        Table::emit(LOI,t,a);
        nowword = nextword();
        return;
    }
    else if ( nowword.type == "IDENT" ) { //������������������

        string id = nowword.value;//��¼��ʶ��������
        int loc;
        id_rcd r;

        nowword = nextword();

        if ( nowword.value == "(" ) { // �������з���ֵ�ĺ����������

            //�ӷ��ű���ȡ����Ӧ�ĺ�����¼
            loc = Table::find_ident_fun(id);

            if ( loc == -1 ) { //û�ҵ��ñ�ʶ��
                err_report(43);
                jump_read({";","+","-","*","/",")"});
                return;
            }
            else {
                //�ӷ��ű�����ȡ���ñ�ʶ���ļ�¼
                r = Table::get_idrcd(loc);
            }

            if ( r.type != VOID_FUNCTION ) {
                ga_retfuncall_stmt(id);
                return;
            }
            else { //�����з���ֵ��
                err_report(29);
                jump_read({")"});
                nowword = nextword();
                return;
            }

        }
        else {

            loc = Table::find_ident(pointer,id);

            if ( loc == -1 ) { //û�ҵ��ñ�ʶ��
                err_report(43);
                jump_read({";","+","-","*","/",")"});
                return;
            }
            else {
                //�ӷ��ű�����ȡ���ñ�ʶ���ļ�¼
                r = Table::get_idrcd(loc);
            }

           if ( nowword.value == "[" ) { // �п���������

                //���жϸñ�ʶ���ǲ�������
                if ( !Table::is_arrayrcd(r) ) {
                    err_report(32);
                }
                int arr_size = Table::get_array_size(r);
                int index;
                if ( arr_size <= 0 ) { //Ԫ�ظ���Ӧ��������
                    err_report(42);
                }

                nowword = nextword();
                ga_expression(); // ʶ���ʾ�����±�ı��ʽ
                //��ʱ�����±�ֵ�Ѵ浽ջ��
                //�õ�����ջջ��������,�����ж�

                if ( nowword.value != "]" ) {
                    err_report(7);
                    jump_read({";","+","-","*","/"});
                    return;
                }
                else { //����ָ��
                    Table::emit(LOI,0,r.adr);
                    Table::emit(ADD);//����õ�Ҫȡ�����ĵ�ַ
                    Table::emit(LDT,r.lev);
                }
                nowword = nextword();
                return;

            }
            else if ( Table::is_constrcd(r) ) { //����

                int l = ( r.type == INT_CONST )? 0:( r.type == CHAR_CONST )? 1:2;
                Table::emit(LIT,l,r.adr);
                return;
            }
            else if ( Table::is_varrcd(r) ) { //��ͨ����
                Table::emit(LOD,r.lev,r.adr);
                return;
            }
            else {//δ֪��ʶ��,����
                err_report(43);
            }




        }
    }

    else if ( nowword.value == "(" ) { //�����Ǳ��ʽ
        nowword = nextword();
        ga_expression();
        if( nowword.value != ")") {
            //ȱ�������ţ���������һ����
            err_report(5);
            lastword();
            lastword();
            nowword = nextword();
        }
        nowword = nextword();
        return;
    }
    else {//���������ӵ�����������
        err_report(45);
    }
}

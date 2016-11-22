#include "GrammarAnalysis.h"

//�ֱ��ʾ�ڴʷ��������������������ֵ�����ڶ�λ
int GrammarAnalysis::row_index = 0;
int GrammarAnalysis::col_index = 0;

//����ָ��ָ��Ĵʷ����������λ�õĴ���
Word GrammarAnalysis::nowword;

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

void GrammarAnalysis::lastword ( ) {
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

void GrammarAnalysis::prt_grammar_info ( string name ) {
    /*
        ����﷨�����Ľ��
    */
    cout << "This is a " << name << endl;
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
                nowword = nextword();
                if ( nowword.type != "EQUAL" ) {
                    //����
                }
                nowword = nextword();
                if ( nowword.type != data_typ ) {
                    //baocuo
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
       //prt_grammar_info("constant define");
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

        while ( true ) {
            nowword = nextword();
            if ( nowword.type != "IDENT" ) {
                //baocuo
            }
            nowword = nextword();
            if ( nowword.value == "[" ) { // �п���������
                nowword = nextword();
                if ( nowword.type != "INTEGER" ) {
                    // ����
                }
                nowword = nextword();
                if ( nowword.value != "]" ) {
                    //baocuo
                }
                nowword = nextword();
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
        //�޲���
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
        <�޷���ֵ��������>�ӳ���
        �����Ԥ��
        ������Ԥ��

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
        //�޲���
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
        <������>�ӳ���
        �����Ԥ��
        ������Ԥ��

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
       // cout << nowword.value << "@eryyy";
        nowword = nextword();
        if ( nowword.value == "=" || nowword.value == "[" ) { //��ֵ���
            //cout << nowword.value << "#yyy";
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
            // ���ڻ�û�н��÷��ű����Բ����ж����з���ֵ�����޷���ֵ
            // Ĭ�����ж�Ϊ�з���ֵ
            nowword = nextword();
            ga_retfuncall_stmt();
            if ( nowword.value != ";" ) {
            //baocuo
            cout << nowword.value << "#yyy";
            cout << "©�˷ֺ�";
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

void GrammarAnalysis::ga_argulist () {
    /*
        <������>�ӳ���
        �����Ԥ��
        ������Ԥ��
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
        nowword = nextword();
        ga_item();
    }

}

void GrammarAnalysis::ga_item() {
    /*
        <��>�ӳ���
        �����Ԥ��
        ������Ԥ��
    */
    ga_factor();
    //nowword = nextword();
    //cout << nowword.value << "һ��555xxx%";
    while ( nowword.is_multiplication_op() ) {
        nowword = nextword();
        ga_factor();
        //nowword = nextword();
    }
    //cout << nowword.value << "iioodiaozhi%";
}

void GrammarAnalysis::ga_factor() {
    /*
        <����>�ӳ���
        �����Ԥ��
        ������Ԥ��
    */
    if ( nowword.type == "INTEGER" || nowword.type == "CHARACTER"
        || nowword.type == "FLOAT" ) {
            nowword = nextword();
            return;
    }
    else if ( nowword.type == "IDENT" ) {
        nowword = nextword();

        if ( nowword.value == "[" ) { // �п���������
            nowword = nextword();
            ga_expression();
            if ( nowword.value != "]" ) {
                //baocuo
            }
            nowword = nextword();
            return;//����
        }
        else if ( nowword.value == "(" ) { // �������з���ֵ�ĺ����������
            ga_retfuncall_stmt();
            return;
        }
        else {
            //cout << "mimi";
            return;//��ʶ��
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

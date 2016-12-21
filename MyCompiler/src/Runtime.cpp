#include <cstdlib>
#include <iostream>
#include <sstream>
#include <fstream>

#include "Runtime.h"
#include "Table.h"

//表示返回地址的栈，因为存在递归调用，所以需要用栈
//来保存多个返回地址
stack<int> Runtime::ret_adr;
//主函数指针，指向主函数部分在PCode表中开始的位置
int Runtime::main_pointer = 0;

//数据区，存放全局变量函数
int Runtime::data_area[DATA_AREA_SIZE];

//运行栈,用vcector来实现运行栈是因为有时候要用
//地址来访问栈的内容
vector<run_stack> Runtime::runtime_stack;

//函数栈，栈顶记录当前正在被调用的函数的
//入口位置在运行栈中的下标值
stack<int> Runtime::fun_stack;

//数据栈，用于临时存放函数调用时形参表达式的值
stack<run_stack> Runtime::data_stack;

//模板函数：将string类型变量转换为常用的数值类型（此方法具有普遍适用性）
template <class Type>
Type str2num(const string& str) {
    istringstream iss(str);
    Type num;
    iss >> num;
    return num;
}

int Runtime::get_top_value() {
    return runtime_stack.back().value;
}

data_type Runtime::get_top_type() {
    return runtime_stack.back().type;
}

void Runtime::set_main_pointer ( int i ) {
    main_pointer = i;
}

void Runtime::pop_rs() {
    runtime_stack.pop_back();
}

void Runtime::push_rs ( run_stack r ) {
    //把记录r压入栈中
    runtime_stack.push_back(r);
}

void Runtime::push_rs ( int n, run_stack r ) {
    //一次性压入n个
    runtime_stack.insert(runtime_stack.end(),n,r);
}

void Runtime::print_stack() {
    ofstream fout;
    fout.open("runtime_stack.txt");

    for ( int i = 0; i < runtime_stack.size(); i++ ) {
        string s;

        switch(runtime_stack[i].type){
        case RS_INT:
            s = "int";
            break;
        case RS_FLOAT:
            s = "float";
            break;
        case RS_CHAR:
            s = "char";
            break;
        case RS_STR:
            s = "str";
            break;
        }

        fout << "["<<i<<"]"<< s << "\t" << runtime_stack[i].value <<endl;
    }
}

run_stack Runtime::get_val ( int index ) {
    /*
        返回运行栈中下标为index的记录
    */
    if ( index >= 0 && index < runtime_stack.size() ) {
        return runtime_stack[index];
    }
}

void Runtime::set_rs ( int index, run_stack r ) {
    /*
        把运行栈内下标为index的记录改成r
    */
    if ( index >= 0 && index < runtime_stack.size() ) {

        //根据目标地址的数据类型的不同，做不同的处理
        if ( runtime_stack[index].type == RS_FLOAT ) {
            if ( r.type == RS_FLOAT ) {
                runtime_stack[index] = r;
            }
            else {
                Table::add_floatrcd(r.value);
                runtime_stack[index] = {RS_FLOAT,Table::get_rctable_size()-1};
            }
        }
        else if ( runtime_stack[index].type == RS_CHAR ) {
            if ( r.type == RS_FLOAT ) {
                r.value = Table::get_floatval(r.value);
            }
            else {
                runtime_stack[index] = {RS_CHAR,r.value%128};
            }
        }
        else if ( runtime_stack[index].type == RS_INT ) {
            //cout << "wwww";
            if ( r.type == RS_FLOAT ) {
                r.value = Table::get_floatval(r.value);
            }
            else {
                runtime_stack[index] = {RS_INT,r.value};
            }
        }


    }
}

void Runtime::interpret ( vector<pcode> p ) {
    /*
        解释执行PCode
    */
    int index = main_pointer;
    bool res;
    run_stack temp;
    string temp_str;
    id_rcd rc, rc1;
    int i;

    //cout << main_pointer << "qwe";
    //cout << p.size() << "runtimestack.size" << runtime_stack.size();


    while ( true ) {

        //cout << "f"<< index << "--->";

        if ( index >= p.size() ) {
            //程序运行结束
            break;
        }
        else {

            pcode c = p[index];

            switch ( c.f ) {

            case ADD:
            //将次栈顶单元与栈顶单元相加，和置于栈顶

                if ( get_top_type() == RS_FLOAT ) {
                    //栈顶为实数则，结果必为实数
                    float f1 = Table::get_floatval(get_top_value());
                    pop_rs();//弹出栈顶的值
                    if ( get_top_type() == RS_FLOAT ) {
                        f1 += Table::get_floatval(get_top_value());
                    }
                    else {
                        f1 += get_top_value();
                    }
                    pop_rs();//弹出次栈顶的值
                    Table::add_floatrcd(f1);
                    push_rs({RS_FLOAT,Table::get_rctable_size()-1});
                }
                else {
                    int i1 = get_top_value();
                    pop_rs();
                    if ( get_top_type() == RS_FLOAT ) {
                        float f1 = Table::get_floatval(get_top_value());
                        pop_rs();//弹出次栈顶的值
                        f1 += i1;
                        Table::add_floatrcd(f1);
                        push_rs({RS_FLOAT,Table::get_rctable_size()-1});
                    }
                    else {
                        i1 += get_top_value();
                        pop_rs();//弹出次栈顶的值
                        push_rs({RS_INT,i1});
                    }
                }

                //cout << get_top_value() << "topval";
                index++;//到下一条指令
                break;

            case BNE:
            //当栈顶和次栈顶的值不相等的时候跳转到地址a，
            //且同时将栈顶的值弹出；否则将弹出栈顶和次栈顶的值

                if ( get_top_type() == RS_FLOAT ) {
                    //栈顶为实数则，结果必为实数
                    float f1 = Table::get_floatval(get_top_value());
                    pop_rs();//弹出栈顶的值
                    if ( get_top_type() == RS_FLOAT ) {
                        res = ( f1 != Table::get_floatval(get_top_value()) );
                    }
                    else {
                        res = ( f1 != get_top_value() );
                    }
                }
                else {
                    int i1 = get_top_value();
                    pop_rs();
                    if ( get_top_type() == RS_FLOAT ) {
                        res = ( i1 != Table::get_floatval(get_top_value()) );
                    }
                    else {
                        res = ( i1 != get_top_value() );
                    }
                }

                if ( res ) {
                    //pop_rs();//把栈顶弹出
                    index = c.a;//跳到a处
                }
                else {
                    //pop_rs();//将栈顶值弹出
                    pop_rs();//把次栈顶弹出
                    index++;//到下一条指令
                }
                break;

            case BRF:
            //条件跳转，弹出栈顶内容，若栈顶的值为0则跳到地址a

                if ( get_top_type() == RS_FLOAT ) {
                    res = ( 0 == Table::get_floatval(get_top_value()) );
                }
                else {
                    res = ( get_top_value() == 0 );
                }

                if (res) { //若栈顶值为0则res为true
                    index = c.a;
                }
                else {
                    index++;
                }
                pop_rs();
                break;

            case DIV:
            //将次栈顶单元除去栈顶单元，商置于栈顶

                if ( get_top_type() == RS_FLOAT ) {
                    //栈顶为实数则，结果必为实数
                    float f1 = Table::get_floatval(get_top_value());

                    if ( f1 == 0 ) {
                        cout << "程序崩溃:除数不能为0" <<endl;
                        exit(0);
                    }

                    pop_rs();//弹出栈顶的值
                    if ( get_top_type() == RS_FLOAT ) {
                        f1 = Table::get_floatval(get_top_value())/f1;
                    }
                    else {
                        f1 = get_top_value()/f1;
                    }
                    pop_rs();//弹出次栈顶的值
                    Table::add_floatrcd(f1);
                    push_rs({RS_FLOAT,Table::get_rctable_size()-1});
                }
                else {
                    int i1 = get_top_value();

                    if ( i1 == 0 ) {
                        cout << "程序崩溃:除数不能为0" <<endl;
                        exit(0);
                    }

                    pop_rs();
                    if ( get_top_type() == RS_FLOAT ) {
                        float f1 = Table::get_floatval(get_top_value());
                        pop_rs();//弹出次栈顶的值
                        f1 /= i1;
                        Table::add_floatrcd(f1);
                        push_rs({RS_FLOAT,Table::get_rctable_size()-1});
                    }
                    else {
                        i1 = get_top_value()/i1;
                        pop_rs();//弹出次栈顶的值
                        push_rs({RS_INT,i1});
                    }
                }

                index++;//到下一条指令
                break;

            case EQL:
            //弹出并比较栈顶和次栈顶的值，
            //若相等则把栈顶置1，不相等则置0

                if ( get_top_type() == RS_FLOAT ) {
                    //栈顶为实数则，结果必为实数
                    float f1 = Table::get_floatval(get_top_value());
                    pop_rs();//弹出栈顶的值
                    if ( get_top_type() == RS_FLOAT ) {
                        res = ( f1 == Table::get_floatval(get_top_value()) );
                    }
                    else {
                        res = ( f1 == get_top_value() );
                    }
                    pop_rs();//弹出次栈顶的值
                }
                else {
                    int i1 = get_top_value();
                    pop_rs();//弹出栈顶的值
                    if ( get_top_type() == RS_FLOAT ) {
                        res = ( i1 == Table::get_floatval(get_top_value()) );
                    }
                    else {
                        res = ( i1 == get_top_value() );
                    }
                    pop_rs();//弹出次栈顶的值
                }

                res? push_rs({RS_INT,1}): push_rs({RS_INT,0});

                index++;//到下一条指令
                break;

            case GEQ:
            //弹出并比较次栈顶和栈顶的值，
            //若大于等于则把栈顶置1，否则置0

                if ( get_top_type() == RS_FLOAT ) {
                    float f1 = Table::get_floatval(get_top_value());
                    pop_rs();//弹出栈顶的值
                    if ( get_top_type() == RS_FLOAT ) {
                        res = ( f1 <= Table::get_floatval(get_top_value()) );
                    }
                    else {
                        res = ( f1 <= get_top_value() );
                    }
                    pop_rs();//弹出次栈顶的值
                }
                else {
                    int i1 = get_top_value();
                    pop_rs();//弹出栈顶的值
                    if ( get_top_type() == RS_FLOAT ) {
                        res = ( i1 <= Table::get_floatval(get_top_value()) );
                    }
                    else {
                        res = ( i1 <= get_top_value() );
                    }
                    pop_rs();//弹出次栈顶的值
                }

                res? push_rs({RS_INT,1}): push_rs({RS_INT,0});

                index++;//到下一条指令
                break;

            case GRT:
            //弹出并比较次栈顶和栈顶的值，
            //若大于则把栈顶置1，否则置0

                if ( get_top_type() == RS_FLOAT ) {
                    float f1 = Table::get_floatval(get_top_value());
                    pop_rs();//弹出栈顶的值
                    if ( get_top_type() == RS_FLOAT ) {
                        res = ( f1 < Table::get_floatval(get_top_value()) );
                    }
                    else {
                        res = ( f1 < get_top_value() );
                    }
                    pop_rs();//弹出次栈顶的值
                }
                else {
                    int i1 = get_top_value();
                    pop_rs();//弹出栈顶的值
                    if ( get_top_type() == RS_FLOAT ) {
                        res = ( i1 < Table::get_floatval(get_top_value()) );
                    }
                    else {
                        res = ( i1 < get_top_value() );
                    }
                    pop_rs();//弹出次栈顶的值
                }

                res? push_rs({RS_INT,1}): push_rs({RS_INT,0});

                index++;//到下一条指令
                break;

            case JMP:
            //无条件跳转到地址a
                index = c.a;
                break;

            case JR:
            //跳转到返回地址（将返回地址从ret_adr中弹出）
            //,并将函数出栈,此时栈顶存的是返回值（保留）
                temp = runtime_stack.back();

                //函数出栈
                while ( runtime_stack.size() > fun_stack.top() ) {
                    pop_rs();
                }
                fun_stack.pop();//把函数记录弹出
                push_rs(temp);//把返回值压回到运行栈中

                index = ret_adr.top();//跳到返回地址
                ret_adr.pop();
                break;

            case JSR:
            //跳转到函数入口fa，同时设定返回地址ra

                index = c.l;
                ret_adr.push(c.a);
                //cout << "jsr" << index;
                break;

            case LDS:
            //把数据栈栈顶的内容弹出，压入运行栈
                push_rs(data_stack.top());
                data_stack.pop();
                index++;//到下一条指令
                //cout << runtime_stack.back().value << "dasdsayyyy";
                break;

            case LDT:
            //把一个变量放到栈顶，t=0表示全局变量，
            //t=1表示局部变量，地址值从栈顶弹出并取值

                if ( get_top_type() != RS_INT ) {
                    //地址值必须是整型的
                    cout << "程序崩溃，地址必须是整型的"<<endl;
                    exit(0);
                }
                else {
                    temp = runtime_stack.back();
                    pop_rs();//把栈顶的值弹出
                    if ( c.l ==  0 ) { //全局变量
                        push_rs(get_val(temp.value));
                    }
                    else if ( c.l == 1 ) { //局部变量
                        push_rs( get_val( fun_stack.top() + temp.value ));
                    }

                }

                index++;//到下一条指令
                break;

            case LEQ:
            //弹出并比较次栈顶和栈顶的值，
            //若小于等于则把栈顶置1，否则置0

                if ( get_top_type() == RS_FLOAT ) {
                    float f1 = Table::get_floatval(get_top_value());
                    pop_rs();//弹出栈顶的值
                    if ( get_top_type() == RS_FLOAT ) {
                        res = ( f1 >= Table::get_floatval(get_top_value()) );
                    }
                    else {
                        res = ( f1 >= get_top_value() );
                    }
                    pop_rs();//弹出次栈顶的值
                }
                else {
                    int i1 = get_top_value();
                    pop_rs();//弹出栈顶的值
                    if ( get_top_type() == RS_FLOAT ) {
                        res = ( i1 >= Table::get_floatval(get_top_value()) );
                    }
                    else {
                        res = ( i1 >= get_top_value() );
                    }
                    pop_rs();//弹出次栈顶的值
                }

                res? push_rs({RS_INT,1}): push_rs({RS_INT,0});

                index++;//到下一条指令
                break;

            case LES:
            //弹出并比较次栈顶和栈顶的值，
            //若小于则把栈顶置1，否则置0

                if ( get_top_type() == RS_FLOAT ) {
                    float f1 = Table::get_floatval(get_top_value());
                    pop_rs();//弹出栈顶的值
                    if ( get_top_type() == RS_FLOAT ) {
                        res = ( f1 > Table::get_floatval(get_top_value()) );
                    }
                    else {
                        res = ( f1 > get_top_value() );
                    }
                    pop_rs();//弹出次栈顶的值
                }
                else {
                    int i1 = get_top_value();
                    pop_rs();//弹出栈顶的值
                    if ( get_top_type() == RS_FLOAT ) {
                        res = ( i1 > Table::get_floatval(get_top_value()) );
                    }
                    else {
                        res = ( i1 > get_top_value() );
                    }
                    pop_rs();//弹出次栈顶的值
                }

                res? push_rs({RS_INT,1}): push_rs({RS_INT,0});

                index++;//到下一条指令
                break;

            case LIT:
            //取常量到栈顶指令；t表示常量的类型（0表示整型，
            //1表示字符型，2表示实型，3表示字符串常量），t = 0时
            //a为整型常量的值；t=1时，a表示ascii码值，
            //t=2时a表示该实数在实数表中的位置，t=3时，
            //a表示该字符串在字符串表中的位置

                if ( c.l == 0 ) { //取整型常量
                    push_rs({RS_INT,c.a});
                }
                else if (  c.l == 1 ) {//取字符常量
                    push_rs({RS_CHAR,c.a});
                }
                else if ( c.l == 2 ) {//取实数常量
                    push_rs({RS_FLOAT,c.a});
                }
                else if ( c.l == 3 ) {//
                    push_rs({RS_STR,c.a});
                }

                index++;//到下一条指令
                break;

            case LOD:
            //把一个变量放到运行栈栈顶，a表示变量的相对地址,
            //l表示层次，l=1为局部变量，l=0是全局变量

                if ( c.l ==  0 ) {
                    push_rs(get_val(c.a));
                }
                else if ( c.l == 1 ) {
                    push_rs( get_val( fun_stack.top()+c.a ) );
                }
                //cout << runtime_stack.back().value <<"tytiosda";
                index++;//到下一条指令
                break;

            case LOI:
            //取立即数放到栈顶指令；t表示立即数的类型（0表示整型，
            //1表示字符型，2表示实型）t = 0时a为整型常量的值；
            //t=1时，a表示ascii码值，t=2时a表示该实数在实数表中的位置

                if ( c.l == 0 ) { //取整型立即数
                    push_rs({RS_INT,c.a});
                }
                else if (  c.l == 1 ) {//取字符立即数
                    push_rs({RS_CHAR,c.a});
                }
                else if ( c.l == 2 ) {//取实数立即数
                    push_rs({RS_FLOAT,c.a});
                }

                index++;//到下一条指令
                break;

            case MUL:
            //将次栈顶单元与栈顶单元相乘，积置于栈顶

                if ( get_top_type() == RS_FLOAT ) {
                    //栈顶为实数则，结果必为实数
                    float f1 = Table::get_floatval(get_top_value());
                    pop_rs();//弹出栈顶的值
                    if ( get_top_type() == RS_FLOAT ) {
                        f1 *= Table::get_floatval(get_top_value());
                    }
                    else {
                        f1 *= get_top_value();
                    }
                    pop_rs();//弹出次栈顶的值
                    Table::add_floatrcd(f1);
                    push_rs({RS_FLOAT,Table::get_rctable_size()-1});
                }
                else {
                    int i1 = get_top_value();
                    pop_rs();
                    if ( get_top_type() == RS_FLOAT ) {
                        float f1 = Table::get_floatval(get_top_value());
                        pop_rs();//弹出次栈顶的值
                        f1 *= i1;
                        Table::add_floatrcd(f1);
                        push_rs({RS_FLOAT,Table::get_rctable_size()-1});
                    }
                    else {
                        i1 *= get_top_value();
                        pop_rs();//弹出次栈顶的值
                        push_rs({RS_INT,i1});
                    }
                }

                index++;//到下一条指令
                break;

            case NEQ:
            //弹出并比较栈顶和次栈顶的值，
            //若相等则把栈顶置0，不相等则置1

                if ( get_top_type() == RS_FLOAT ) {
                    float f1 = Table::get_floatval(get_top_value());
                    pop_rs();//弹出栈顶的值
                    if ( get_top_type() == RS_FLOAT ) {
                        res = ( f1 != Table::get_floatval(get_top_value()) );
                    }
                    else {
                        res = ( f1 != get_top_value() );
                    }
                    pop_rs();//弹出次栈顶的值
                }
                else {
                    int i1 = get_top_value();
                    pop_rs();//弹出栈顶的值
                    if ( get_top_type() == RS_FLOAT ) {
                        res = ( i1 != Table::get_floatval(get_top_value()) );
                    }
                    else {
                        res = ( i1 != get_top_value() );
                    }
                    pop_rs();//弹出次栈顶的值
                }

                res? push_rs({RS_INT,1}): push_rs({RS_INT,0});

                index++;//到下一条指令
                break;

            case PRT:
            //将栈顶的内容弹出并且输出到控制台,
                Table::test_rconst_table();

                switch ( get_top_type() ) {
                case RS_INT:
                    //cout << "asd";
                    cout << get_top_value();
                    break;
                case RS_CHAR:
                    cout << (char) get_top_value();
                    break;
                case RS_FLOAT:
                    //cout << "jibala" <<get_top_value() <<"koi";
                    cout << Table::get_floatval(get_top_value());
                    break;
                case RS_STR:
                    temp_str = Table::get_str(get_top_value());
                    temp_str.erase(0,1);//把字符串开头和结尾的引号去掉
                    temp_str.erase(temp_str.size()-1,1);
                    cout << temp_str;
                    break;
                }

                pop_rs();//弹出栈顶内容
                index++;//到下一条指令
                break;

            case PUF:
            //先将当前栈地址压入到函数栈中，再将该函数的所有
            //变量压入运行栈中a表示该函数在符号表中的位置
                fun_stack.push(runtime_stack.size());

                rc = Table::get_idrcd(c.a);
                for ( i = c.a+1; i <= Table::get_funrcd(rc.ref).last; i++ ) {

                    rc1 = Table::get_idrcd(i);
                    if ( rc1.type == INT_VAR ) { //整数默认先存为0
                        //cout << i <<"intvar" << runtime_stack.size();
                        push_rs({RS_INT,0});
                    }
                    else if ( rc1.type == CHAR_VAR ) {//字符默认先存为‘a’
                        push_rs({RS_CHAR,97});
                    }
                    else if ( rc1.type == FLOAT_VAR ) {//实数默认为实数表的第一项
                        push_rs({RS_FLOAT,0});
                    }
                    else if ( rc1.type == INT_ARRAY ) {
                        push_rs(Table::get_arrayrcd(rc1.ref).size,{RS_INT,0});
                    }
                    else if ( rc1.type == CHAR_ARRAY ) {
                        push_rs(Table::get_arrayrcd(rc1.ref).size,{RS_CHAR,97});
                    }
                    else if ( rc1.type == FLOAT_ARRAY ) {
                        push_rs(Table::get_arrayrcd(rc1.ref).size,{RS_FLOAT,0});
                    }
                }

                //cout<< fun_stack.top() << "funstacktop";
                //cout<< runtime_stack.size() << "size";
                //print_stack();
                index++;//到下一条指令
                break;

            case RDA:
            //从控制台读一个数，写到栈顶,t是读入的类型
            //（0表示整型，1表示字符型，2表示实型）

                cout << "请输入";
                cin >> temp_str;

                switch ( c.l ) {
                case 0://整数
                    push_rs({RS_INT,str2num<int>(temp_str)});
                    //cout << runtime_stack.back().value << "top stack";
                    break;
                case 1://字符
                    push_rs({RS_CHAR,(int)temp_str[0]});
                    break;
                case 2: //实数
                    Table::add_floatrcd(str2num<float>(temp_str));
                    push_rs({RS_FLOAT,Table::get_rctable_size()-1});
                    break;
                }

                index++;//到下一条指令
                break;

            case SDS:
            //把运行栈栈顶的值弹出，存入数据栈
                data_stack.push(runtime_stack.back());
                pop_rs();
                //cout << "datastack.top" << data_stack.top().value;
                index++;//到下一条指令
                break;

            case STA:
            //把栈顶的值存到次栈顶值所在的地址，同时将它们弹出
            //l=1表示局部地址，l=0表示全局地址
                //print_stack();
                //Table::test_rconst_table();
                temp = runtime_stack.back();
                //cout << get_top_type() <<"typeee";
                //cout << get_top_value();

                pop_rs();//弹出栈顶的值
                int target;

                if ( get_top_type() != RS_INT ) {
                    //次栈顶地址值必须是整型的
                    //cout << get_top_type() <<"typeee";
                    //cout << get_top_value();
                    cout << "程序崩溃，地址必须是整型的"<<endl;
                    exit(0);
                }
                else {
                    if ( c.l ==  0 ) { //全局变量
                        set_rs(get_top_value(),temp);
                    }
                    else if ( c.l == 1 ) { //局部变量
                        set_rs(fun_stack.top()+get_top_value(),temp);
                    }

                }
                pop_rs();//弹出次栈顶的值

                //print_stack();
                index++;//到下一条指令
                break;

            case STO:
            //把栈顶的值弹出并赋给变量a；l=1时，该变量是局部变量
            //a是其相对地址，l=0表示该变量是全局变量，a是绝对地址

                if ( c.l ==  0 ) { //全局变量
                    set_rs(c.a,runtime_stack.back());
                }
                else if ( c.l == 1 ) { //局部变量
                    //cout << fun_stack.top()+c.a << "typey";
                    set_rs(fun_stack.top()+c.a,runtime_stack.back());
                    //cout << "mimimi";
                    //cout << runtime_stack[fun_stack.top()+c.a].value << "asdiop";

                }

                pop_rs();//弹出栈顶的值

                index++;//到下一条指令
                break;

            case SUB:
            //将次栈顶单元减去栈顶单元，差置于栈顶

                if ( get_top_type() == RS_FLOAT ) {
                    //栈顶为实数则，结果必为实数
                    float f1 = Table::get_floatval(get_top_value());
                    pop_rs();//弹出栈顶的值
                    if ( get_top_type() == RS_FLOAT ) {
                        f1 = Table::get_floatval(get_top_value())-f1;
                    }
                    else {
                        f1 = get_top_value()-f1;
                    }
                    pop_rs();//弹出次栈顶的值
                    Table::add_floatrcd(f1);
                    push_rs({RS_FLOAT,Table::get_rctable_size()-1});
                }
                else {
                    int i1 = get_top_value();
                    pop_rs();
                    if ( get_top_type() == RS_FLOAT ) {
                        float f1 = Table::get_floatval(get_top_value());
                        pop_rs();//弹出次栈顶的值
                        f1 -= i1;
                        Table::add_floatrcd(f1);
                        push_rs({RS_FLOAT,Table::get_rctable_size()-1});
                    }
                    else {
                        i1 = get_top_value() - i1;
                        pop_rs();//弹出次栈顶的值
                        push_rs({RS_INT,i1});
                    }
                }

                index++;//到下一条指令
                break;
            }
        }

    }

}

#include <cstdlib>
#include <iostream>
#include <sstream>

#include "Runtime.h"
#include "Table.h"

//��ʾ���ص�ַ�ļĴ���
int Runtime::ret_adr = 0;
//������ָ�룬ָ��������������PCode���п�ʼ��λ��
int Runtime::main_pointer = 0;

//�����������ȫ�ֱ�������
int Runtime::data_area[DATA_AREA_SIZE];

//����ջ,��vcector��ʵ������ջ����Ϊ��ʱ��Ҫ��
//��ַ������ջ������
vector<run_stack> Runtime::runtime_stack;

//����ջ��ջ����¼��ǰ���ڱ����õĺ�����
//���λ��������ջ�е��±�ֵ
stack<int> Runtime::fun_stack;

//ģ�庯������string���ͱ���ת��Ϊ���õ���ֵ���ͣ��˷��������ձ������ԣ�
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
    runtime_stack.push_back(r);
}

run_stack Runtime::get_val ( int index ) {
    /*
        ��������ջ���±�Ϊindex�ļ�¼
    */
    if ( index >= 0 && index < runtime_stack.size() ) {
        return runtime_stack[index];
    }
}

void Runtime::set_rs ( int index, run_stack r ) {
    /*
        ������ջ���±�Ϊindex�ļ�¼�ĳ�r
    */
    if ( index >= 0 && index < runtime_stack.size() ) {

        //����Ŀ���ַ���������͵Ĳ�ͬ������ͬ�Ĵ���
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
            runtime_stack[index] = {RS_CHAR,r.value%128};
        }
        else if ( runtime_stack[index].type == RS_INT ) {
            if ( r.type == RS_FLOAT ) {
                r.value = Table::get_floatval(r.value);
            }
            runtime_stack[index] = {RS_CHAR,r.value};
        }


    }
}

void Runtime::interpret ( vector<pcode> p ) {
    /*
        ����ִ��PCode
    */
    int index = main_pointer;
    bool res;
    run_stack temp;
    string temp_str;
    id_rcd rc, rc1;
    int i;

    while ( true ) {

        if ( index >= p.size() ) {
            //�������н���
            break;
        }
        else {

            pcode c = p[index];

            switch ( c.f ) {

            case ADD:
            //����ջ����Ԫ��ջ����Ԫ��ӣ�������ջ��

                if ( get_top_type() == RS_FLOAT ) {
                    //ջ��Ϊʵ���򣬽����Ϊʵ��
                    float f1 = Table::get_floatval(get_top_value());
                    pop_rs();//����ջ����ֵ
                    if ( get_top_type() == RS_FLOAT ) {
                        f1 += Table::get_floatval(get_top_value());
                    }
                    else {
                        f1 += get_top_value();
                    }
                    pop_rs();//������ջ����ֵ
                    Table::add_floatrcd(f1);
                    push_rs({RS_FLOAT,Table::get_rctable_size()-1});
                }
                else {
                    int i1 = get_top_value();
                    pop_rs();
                    if ( get_top_type() == RS_FLOAT ) {
                        float f1 = Table::get_floatval(get_top_value());
                        pop_rs();//������ջ����ֵ
                        f1 += i1;
                        Table::add_floatrcd(f1);
                        push_rs({RS_FLOAT,Table::get_rctable_size()-1});
                    }
                    else {
                        i1 += get_top_value();
                        pop_rs();//������ջ����ֵ
                        push_rs({RS_INT,i1});
                    }
                }

                index++;//����һ��ָ��
                break;

            case BNE:
            //��ջ���ʹ�ջ����ֵ����ȵ�ʱ����ת����ַa��
            //��ͬʱ��ջ����ֵ���������򽫵���ջ���ʹ�ջ����ֵ

                if ( get_top_type() == RS_FLOAT ) {
                    //ջ��Ϊʵ���򣬽����Ϊʵ��
                    float f1 = Table::get_floatval(get_top_value());
                    pop_rs();//����ջ����ֵ
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
                    index = c.a;//����a��
                }
                else {
                    pop_rs();//��ջ��ֵ����
                    index++;//����һ��ָ��
                }
                break;

            case BRF:
            //������ת������ջ�����ݣ���ջ����ֵΪ0��������ַa

                if ( get_top_type() == RS_FLOAT ) {
                    res = ( 0 == Table::get_floatval(get_top_value()) );
                }
                else {
                    res = ( get_top_value() == 0 );
                }

                if (res) { //��ջ��ֵΪ0��resΪtrue
                    index = c.a;
                }
                else {
                    index++;
                }
                pop_rs();
                break;

            case DIV:
            //����ջ����Ԫ��ȥջ����Ԫ��������ջ��

                if ( get_top_type() == RS_FLOAT ) {
                    //ջ��Ϊʵ���򣬽����Ϊʵ��
                    float f1 = Table::get_floatval(get_top_value());

                    if ( f1 == 0 ) {
                        cout << "�������:��������Ϊ0" <<endl;
                        exit(0);
                    }

                    pop_rs();//����ջ����ֵ
                    if ( get_top_type() == RS_FLOAT ) {
                        f1 = Table::get_floatval(get_top_value())/f1;
                    }
                    else {
                        f1 = get_top_value()/f1;
                    }
                    pop_rs();//������ջ����ֵ
                    Table::add_floatrcd(f1);
                    push_rs({RS_FLOAT,Table::get_rctable_size()-1});
                }
                else {
                    int i1 = get_top_value();

                    if ( i1 == 0 ) {
                        cout << "�������:��������Ϊ0" <<endl;
                        exit(0);
                    }

                    pop_rs();
                    if ( get_top_type() == RS_FLOAT ) {
                        float f1 = Table::get_floatval(get_top_value());
                        pop_rs();//������ջ����ֵ
                        f1 /= i1;
                        Table::add_floatrcd(f1);
                        push_rs({RS_FLOAT,Table::get_rctable_size()-1});
                    }
                    else {
                        i1 = get_top_value()/i1;
                        pop_rs();//������ջ����ֵ
                        push_rs({RS_INT,i1});
                    }
                }

                index++;//����һ��ָ��
                break;

            case EQL:
            //�������Ƚ�ջ���ʹ�ջ����ֵ��
            //��������ջ����1�����������0

                if ( get_top_type() == RS_FLOAT ) {
                    //ջ��Ϊʵ���򣬽����Ϊʵ��
                    float f1 = Table::get_floatval(get_top_value());
                    pop_rs();//����ջ����ֵ
                    if ( get_top_type() == RS_FLOAT ) {
                        res = ( f1 == Table::get_floatval(get_top_value()) );
                    }
                    else {
                        res = ( f1 == get_top_value() );
                    }
                    pop_rs();//������ջ����ֵ
                }
                else {
                    int i1 = get_top_value();
                    pop_rs();//����ջ����ֵ
                    if ( get_top_type() == RS_FLOAT ) {
                        res = ( i1 == Table::get_floatval(get_top_value()) );
                    }
                    else {
                        res = ( i1 == get_top_value() );
                    }
                    pop_rs();//������ջ����ֵ
                }

                res? push_rs({RS_INT,1}): push_rs({RS_INT,0});

                index++;//����һ��ָ��
                break;

            case GEQ:
            //�������Ƚϴ�ջ����ջ����ֵ��
            //�����ڵ������ջ����1��������0

                if ( get_top_type() == RS_FLOAT ) {
                    float f1 = Table::get_floatval(get_top_value());
                    pop_rs();//����ջ����ֵ
                    if ( get_top_type() == RS_FLOAT ) {
                        res = ( f1 <= Table::get_floatval(get_top_value()) );
                    }
                    else {
                        res = ( f1 <= get_top_value() );
                    }
                    pop_rs();//������ջ����ֵ
                }
                else {
                    int i1 = get_top_value();
                    pop_rs();//����ջ����ֵ
                    if ( get_top_type() == RS_FLOAT ) {
                        res = ( i1 <= Table::get_floatval(get_top_value()) );
                    }
                    else {
                        res = ( i1 <= get_top_value() );
                    }
                    pop_rs();//������ջ����ֵ
                }

                res? push_rs({RS_INT,1}): push_rs({RS_INT,0});

                index++;//����һ��ָ��
                break;

            case GRT:
            //�������Ƚϴ�ջ����ջ����ֵ��
            //���������ջ����1��������0

                if ( get_top_type() == RS_FLOAT ) {
                    float f1 = Table::get_floatval(get_top_value());
                    pop_rs();//����ջ����ֵ
                    if ( get_top_type() == RS_FLOAT ) {
                        res = ( f1 < Table::get_floatval(get_top_value()) );
                    }
                    else {
                        res = ( f1 < get_top_value() );
                    }
                    pop_rs();//������ջ����ֵ
                }
                else {
                    int i1 = get_top_value();
                    pop_rs();//����ջ����ֵ
                    if ( get_top_type() == RS_FLOAT ) {
                        res = ( i1 < Table::get_floatval(get_top_value()) );
                    }
                    else {
                        res = ( i1 < get_top_value() );
                    }
                    pop_rs();//������ջ����ֵ
                }

                res? push_rs({RS_INT,1}): push_rs({RS_INT,0});

                index++;//����һ��ָ��
                break;

            case JMP:
            //��������ת����ַa
                index = c.a;
                break;

            case JR:
            //��ת�����ص�ַ,����������ջ��
            //��ʱջ������Ƿ���ֵ��������
                temp = runtime_stack.back();

                //������ջ
                while ( runtime_stack.size() > fun_stack.top() ) {
                    pop_rs();
                }
                fun_stack.pop();//�Ѻ�����¼����
                push_rs(temp);//�ѷ���ֵѹ�ص�����ջ��

                index = ret_adr;//�������ص�ַ
                break;

            case JSR:
            //��ת���������fa��ͬʱ�趨���ص�ַra

                index = c.l;
                ret_adr = c.a;
                break;

            case LDT:
            //��һ�������ŵ�ջ����t=0��ʾȫ�ֱ�����
            //t=1��ʾ�ֲ���������ֵַ��ջ��������ȡֵ

                if ( get_top_type() != RS_INT ) {
                    //��ֵַ���������͵�
                    cout << "�����������ַ���������͵�"<<endl;
                    exit(0);
                }
                else {
                    if ( c.l ==  0 ) { //ȫ�ֱ���
                    push_rs(get_val(get_top_value()));
                    }
                    else if ( c.l == 1 ) { //�ֲ�����
                        push_rs( get_val( fun_stack.top()+get_top_value() ));
                    }

                }

                index++;//����һ��ָ��
                break;

            case LEQ:
            //�������Ƚϴ�ջ����ջ����ֵ��
            //��С�ڵ������ջ����1��������0

                if ( get_top_type() == RS_FLOAT ) {
                    float f1 = Table::get_floatval(get_top_value());
                    pop_rs();//����ջ����ֵ
                    if ( get_top_type() == RS_FLOAT ) {
                        res = ( f1 <= Table::get_floatval(get_top_value()) );
                    }
                    else {
                        res = ( f1 <= get_top_value() );
                    }
                    pop_rs();//������ջ����ֵ
                }
                else {
                    int i1 = get_top_value();
                    pop_rs();//����ջ����ֵ
                    if ( get_top_type() == RS_FLOAT ) {
                        res = ( i1 <= Table::get_floatval(get_top_value()) );
                    }
                    else {
                        res = ( i1 <= get_top_value() );
                    }
                    pop_rs();//������ջ����ֵ
                }

                res? push_rs({RS_INT,1}): push_rs({RS_INT,0});

                index++;//����һ��ָ��
                break;

            case LES:
            //�������Ƚϴ�ջ����ջ����ֵ��
            //��С�����ջ����1��������0

                if ( get_top_type() == RS_FLOAT ) {
                    float f1 = Table::get_floatval(get_top_value());
                    pop_rs();//����ջ����ֵ
                    if ( get_top_type() == RS_FLOAT ) {
                        res = ( f1 > Table::get_floatval(get_top_value()) );
                    }
                    else {
                        res = ( f1 > get_top_value() );
                    }
                    pop_rs();//������ջ����ֵ
                }
                else {
                    int i1 = get_top_value();
                    pop_rs();//����ջ����ֵ
                    if ( get_top_type() == RS_FLOAT ) {
                        res = ( i1 > Table::get_floatval(get_top_value()) );
                    }
                    else {
                        res = ( i1 > get_top_value() );
                    }
                    pop_rs();//������ջ����ֵ
                }

                res? push_rs({RS_INT,1}): push_rs({RS_INT,0});

                index++;//����һ��ָ��
                break;

            case LIT:
            //ȡ������ջ��ָ�t��ʾ���������ͣ�0��ʾ���ͣ�
            //1��ʾ�ַ��ͣ�2��ʾʵ�ͣ�3��ʾ�ַ�����������t = 0ʱ
            //aΪ���ͳ�����ֵ��t=1ʱ��a��ʾascii��ֵ��
            //t=2ʱa��ʾ��ʵ����ʵ�����е�λ�ã�t=3ʱ��
            //a��ʾ���ַ������ַ������е�λ��

                if ( c.l == 0 ) { //ȡ���ͳ���
                    push_rs({RS_INT,c.a});
                }
                else if (  c.l == 1 ) {//ȡ�ַ�����
                    push_rs({RS_CHAR,c.a});
                }
                else if ( c.l == 2 ) {//ȡʵ������
                    push_rs({RS_FLOAT,c.a});
                }
                else if ( c.l == 3 ) {//
                    push_rs({RS_STR,c.a});
                }

                index++;//����һ��ָ��
                break;

            case LOD:
            //��һ�������ŵ�����ջջ����a��ʾ��������Ե�ַ,
            //l��ʾ��Σ�l=1Ϊ�ֲ�������l=0��ȫ�ֱ���
                if ( c.l ==  0 ) {
                    push_rs(get_val(c.a));
                }
                else if ( c.l == 1 ) {
                    push_rs( get_val( fun_stack.top()+c.a ) );
                }

                index++;//����һ��ָ��
                break;

            case LOI:
            //ȡ�������ŵ�ջ��ָ�t��ʾ�����������ͣ�0��ʾ���ͣ�
            //1��ʾ�ַ��ͣ�2��ʾʵ�ͣ�t = 0ʱaΪ���ͳ�����ֵ��
            //t=1ʱ��a��ʾascii��ֵ��t=2ʱa��ʾ��ʵ����ʵ�����е�λ��

                if ( c.l == 0 ) { //ȡ����������
                    push_rs({RS_INT,c.a});
                }
                else if (  c.l == 1 ) {//ȡ�ַ�������
                    push_rs({RS_CHAR,c.a});
                }
                else if ( c.l == 2 ) {//ȡʵ��������
                    push_rs({RS_FLOAT,c.a});
                }

                index++;//����һ��ָ��
                break;

            case MUL:
            //����ջ����Ԫ��ջ����Ԫ��ˣ�������ջ��

                if ( get_top_type() == RS_FLOAT ) {
                    //ջ��Ϊʵ���򣬽����Ϊʵ��
                    float f1 = Table::get_floatval(get_top_value());
                    pop_rs();//����ջ����ֵ
                    if ( get_top_type() == RS_FLOAT ) {
                        f1 *= Table::get_floatval(get_top_value());
                    }
                    else {
                        f1 *= get_top_value();
                    }
                    pop_rs();//������ջ����ֵ
                    Table::add_floatrcd(f1);
                    push_rs({RS_FLOAT,Table::get_rctable_size()-1});
                }
                else {
                    int i1 = get_top_value();
                    pop_rs;
                    if ( get_top_type() == RS_FLOAT ) {
                        float f1 = Table::get_floatval(get_top_value());
                        pop_rs();//������ջ����ֵ
                        f1 *= i1;
                        Table::add_floatrcd(f1);
                        push_rs({RS_FLOAT,Table::get_rctable_size()-1});
                    }
                    else {
                        i1 *= get_top_value();
                        pop_rs();//������ջ����ֵ
                        push_rs({RS_INT,i1});
                    }
                }

                index++;//����һ��ָ��
                break;

            case NEQ:
            //�������Ƚ�ջ���ʹ�ջ����ֵ��
            //��������ջ����0�����������1

                if ( get_top_type() == RS_FLOAT ) {
                    float f1 = Table::get_floatval(get_top_value());
                    pop_rs();//����ջ����ֵ
                    if ( get_top_type() == RS_FLOAT ) {
                        res = ( f1 != Table::get_floatval(get_top_value()) );
                    }
                    else {
                        res = ( f1 != get_top_value() );
                    }
                    pop_rs();//������ջ����ֵ
                }
                else {
                    int i1 = get_top_value();
                    pop_rs();//����ջ����ֵ
                    if ( get_top_type() == RS_FLOAT ) {
                        res = ( i1 != Table::get_floatval(get_top_value()) );
                    }
                    else {
                        res = ( i1 != get_top_value() );
                    }
                    pop_rs();//������ջ����ֵ
                }

                res? push_rs({RS_INT,1}): push_rs({RS_INT,0});

                index++;//����һ��ָ��
                break;

            case PRT:
            //��ջ�������ݵ����������������̨,

                switch ( get_top_type() ) {
                case RS_INT:
                    cout << get_top_value();
                    break;
                case RS_CHAR:
                    cout << (char) get_top_value();
                    break;
                case RS_FLOAT:
                    cout << Table::get_floatval(get_top_value());
                    break;
                case RS_STR:
                    cout << Table::get_str(get_top_value());
                    break;
                }

                pop_rs();//����ջ������
                index++;//����һ��ָ��
                break;

            case PUF:
            //�Ƚ���ǰջ��ַѹ�뵽����ջ�У��ٽ��ú���������
            //����ѹ������ջ��a��ʾ�ú����ڷ��ű��е�λ��
                fun_stack.push(runtime_stack.size());

                rc = Table::get_idrcd(c.a);
                for ( i = c.a+1; i < Table::get_funrcd(rc.ref).last; i++ ) {

                    rc1 = Table::get_idrcd(i);
                    if ( rc1.type == INT_VAR ) { //����Ĭ���ȴ�Ϊ0
                        Runtime::push_rs({RS_INT,0});
                    }
                    else if ( rc1.type == CHAR_VAR ) {//�ַ�Ĭ���ȴ�Ϊ��a��
                        Runtime::push_rs({RS_CHAR,97});
                    }
                    else {
                        Runtime::push_rs({RS_FLOAT,0});
                    }

                }

                break;

            case RDA:
            //�ӿ���̨��һ������д��ջ��t�Ƕ��������
            //��0��ʾ���ͣ�1��ʾ�ַ��ͣ�2��ʾʵ�ͣ�

                cout << "������";
                cin >> temp_str;

                switch ( c.l ) {
                case 0://����
                    push_rs({RS_INT,str2num<int>(temp_str)});
                    break;
                case 1://�ַ�
                    push_rs({RS_CHAR,(int)temp_str[0]});
                    break;
                case 2: //ʵ��
                    Table::add_floatrcd(str2num<float>(temp_str));
                    push_rs({RS_FLOAT,Table::get_rctable_size()-1});
                    break;
                }

                index++;//����һ��ָ��
                break;

            case STA:
            //��ջ����ֵ�浽��ջ��ֵ���ڵĵ�ַ��ͬʱ�����ǵ���
            //l=1��ʾ�ֲ���ַ��l=0��ʾȫ�ֵ�ַ

                temp = runtime_stack.back();
                pop_rs();//����ջ����ֵ
                int target;

                if ( get_top_type() != RS_INT ) {
                    //��ջ����ֵַ���������͵�
                    cout << "�����������ַ���������͵�"<<endl;
                    exit(0);
                }
                else {
                    if ( c.l ==  0 ) { //ȫ�ֱ���
                        set_rs(get_top_value(),temp);
                    }
                    else if ( c.l == 1 ) { //�ֲ�����
                        set_rs(fun_stack.top()+get_top_value(),temp);
                    }

                }
                pop_rs();//������ջ����ֵ

                index++;//����һ��ָ��
                break;

            case STO:
            //��ջ����ֵ��������������a��l=1ʱ���ñ����Ǿֲ�����
            //a������Ե�ַ��l=0��ʾ�ñ�����ȫ�ֱ�����a�Ǿ��Ե�ַ

                if ( c.l ==  0 ) { //ȫ�ֱ���
                    set_rs(c.a,runtime_stack.back());
                }
                else if ( c.l == 1 ) { //�ֲ�����
                    set_rs(fun_stack.top()+c.a,runtime_stack.back());
                }
                pop_rs();//����ջ����ֵ

                index++;//����һ��ָ��
                break;

            case SUB:
            //����ջ����Ԫ��ȥջ����Ԫ��������ջ��

                if ( get_top_type() == RS_FLOAT ) {
                    //ջ��Ϊʵ���򣬽����Ϊʵ��
                    float f1 = Table::get_floatval(get_top_value());
                    pop_rs();//����ջ����ֵ
                    if ( get_top_type() == RS_FLOAT ) {
                        f1 = Table::get_floatval(get_top_value())-f1;
                    }
                    else {
                        f1 = get_top_value()-f1;
                    }
                    pop_rs();//������ջ����ֵ
                    Table::add_floatrcd(f1);
                    push_rs({RS_FLOAT,Table::get_rctable_size()-1});
                }
                else {
                    int i1 = get_top_value();
                    pop_rs();
                    if ( get_top_type() == RS_FLOAT ) {
                        float f1 = Table::get_floatval(get_top_value());
                        pop_rs();//������ջ����ֵ
                        f1 -= i1;
                        Table::add_floatrcd(f1);
                        push_rs({RS_FLOAT,Table::get_rctable_size()-1});
                    }
                    else {
                        i1 = get_top_value() - i1;
                        pop_rs();//������ջ����ֵ
                        push_rs({RS_INT,i1});
                    }
                }

                index++;//����һ��ָ��
                break;
            }
        }

    }

}

#include "IdentTable.h"

#include <alogrithm>

//全局地址和局部地址都初始化为0
int Table::glb_adr = 0;
int Table::fun_adr = 0;

bool Table::is_id_repeat ( string name, int lev ) {
    /*
        判断标识符是否重复
        重复返回true
    */
    if ( lev == 0 ) { //全局
        for ( int i = 0; i < id_table.size(); i++ ) {
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

        return id_table[i] == 1;
    }
}

bool Table::is_funcrcd ( id_rcd r ) {
    /*
        判断符号表的某条记录是否为函数名记录
    */
    return r.type == INT_FUNCTION || r.type == CHAR_FUNCTION
            || r.type == FLOAT_FUNCTION || r.type == VOID_FUNCTION;
}

id_type Table::get_func_type ( string func_name ) {
    /*
        在符号表中寻找名为func_name的函数，返回其返回值
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

void Table::set_lastid () {
    /*
        更新当前函数表中的last项
    */
    func_table[func_table.size()-1].last = id_table.size();
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

    if ( type == INT_VAR || type == CHAR_VAR || type == FLOAT_VAR ) {

        if ( !id_table.empty() && is_id_repeat(name,lev) ) {
            //名称重复，报错
        }

        //判断lev和adr的值
        int lev;
        if ( id_table.empty() ) {
            lev = 0;
            adr = glb_adr++;
        }
        else {
            id_rcd r = id_table.back();
            if ( r.lev == 1 || r.lev == 0 && is_funcrcd(r) ) {
                lev = 1;
                adr = ++fun_adr;
                set_lastid();
            }
            else {
                lev = 0;
                adr = glb_adr++;
            }
        }

        id_table.push_back({name,type,-1,lev,adr});
    }
    else if ( type == INT_FUNCTION || type == CHAR_FUNCTION
             || type == FLOAT_FUNCTION || type == VOID_FUNCTION ) {

        if ( !id_table.empty() && is_id_repeat(name,lev) ) {
            //名称重复，报错
        }

        //设定lev和adr的值，以及将fun_adr置零
        int lev = 1;
        adr = glb_adr++;
        fun_adr = 0;


        //填充func_table表和id_table表
        //func_table表的两个项的值先初始化为该函数名的在符号表中下标
        int ref = func_table.size();
        func_table.push_back({id_table.size(),id_table.size()});
        id_table.push_back({name,type,ref,lev,adr});
    }
}

void Table::add_idrcd ( string name, int adr, id_type type) {
    /*
        往符号表中添加一项记录
        整数常量、字符常量
    */

    // 把标识符统一存成小写形式
    transform(name.begin(),name.end(),name.begin(),::tolower);
    if ( !id_table.empty() && is_id_repeat(name,lev) ) {
        //名称重复，报错
    }

    int lev
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

    id_table.push_back({name,type,-1,lev,adr});

}

void Table::add_idrcd ( string name, id_type type, float float_value ) {
    /*
        往符号表中添加一项实常量记录
    */

    // 把标识符统一存成小写形式
    transform(name.begin(),name.end(),name.begin(),::tolower);
    if ( !id_table.empty() && is_id_repeat(name,lev) ) {
        //名称重复，报错
    }

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

    //填实常量表和标识符表
    int adr = rconst_table.size();
    rconst_table.push_back(float_value);
    id_table.push_back({name,type,-1,lev,adr});

}

void Table::add_idrcd ( string name, id_type type, int adr, int size) {
    /*
        往符号表中添加一项变量（数组）记录
    */
    // 把标识符统一存成小写形式
    transform(name.begin(),name.end(),name.begin(),::tolower);
    if ( !id_table.empty() && is_id_repeat(name,lev) ) {
        //名称重复，报错
    }

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
        et = FLOAT:
    }

    //设定lev和adr的值
    int lev;
    if ( id_table.empty() ) {
            lev = 0;
            adr = glb_adr;
            glb += size;
    }
    else {
        id_rcd r = id_table.back();
        if ( r.lev == 1 || r.lev == 0 && is_funcrcd(r) ) {
            lev = 1;
            adr = fun_adr+1;
            fun_adr += size;
            set_lastid();
        }
        else {
            lev = 0;
            adr = glb_adr;
            glb_adr += size;
        }
    }

    int ref = array_table.size();
    add_arrayrcd(et,elsize,size);
    id_table.push_back({name,type,ref,lev,adr});

}

void Table::emit ( op_code f ) {
    /*
        往PCode表中增加一条指令
    */
    pcode_table.push({f,0,0});
}

void Table::emit ( op_code f, int l ) {
    /*
        往PCode表中增加一条指令
    */
    pcode_table.push({f,l,0});
}

void Table::emit ( op_code f, int l, int a ) {
    /*
        往PCode表中增加一条指令
    */
    pcode_table.push({f,l,a});
}

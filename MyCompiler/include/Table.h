#ifndef IDENTTABLE_H
#define IDENTTABLE_H

#include <vector>
#include <string>

using namespace std;

enum id_type {
    INT_VAR, // 0
    CHAR_VAR, // 1
    FLOAT_VAR,// 2
    INT_ARRAY,// 3
    CHAR_ARRAY,// 4
    FLOAT_ARRAY,// 5
    INT_CONST,// 6
    CHAR_CONST,// 7
    FLOAT_CONST,// 8
    INT_FUNCTION,// 9
    CHAR_FUNCTION,// 10
    FLOAT_FUNCTION,// 11
    VOID_FUNCTION,// 12
    ERROR // 13
};

enum element_type { INT, CHAR, FLOAT };

enum op_code {
    ADD,
    BNE,
    BRF,
    DIV,
    EQL,
    GEQ,
    GRT,
    JMP,
    JR,
    JSR,
    LDS,
    LDT,
    LEQ,
    LES,
    LIT,
    LOD,
    LOI,
    MUL,
    NEQ,
    PRT,
    PUF,
    RDA,
    SDS,
    STA,
    STO,
    SUB
};

struct id_rcd {
    /*
        符号表的一项记录
        name是标识符的名字，不区分大小写
        link是同一函数中上一个标识符在id_table表中的位置，每个
            函数在id_table表中的第一个标识符（函数名）的link值为0
        type是标识符类型
        ref 标识符是数组变量名时，指向其在array_table中登录的位置
            标识符是函数名时，指向其在func_table中的位置
            其他情况下为-1
        lev 表示该标识符所在位置的层次，全局为0，函数内为1
        adr 对于变量名，应该填入该变量在数据区中分配存储单元的
            实际地址；对于函数名，填入它的实际地址；
            对于int和char常量名，填入他们对应的整数值或者是ASCII值；
            对于float常量名，填入它在rconst_table中登录的位置
     */

    string name;
    id_type type;
    int ref;
    int lev;
    int adr;
};

struct array_rcd {
    element_type eltype; // 数组元素的类型
    int elsize; // 数组元素的大小
    int size; // 数组的大小
};

struct func_rcd {
    /*
       last指向该函数说明中当前（最后）一个标识符在id_table表中的位置
       lastpar指向函数的最后一个参数在id_table表中的位置如果其指向的位置
        是该函数名称所在的位置，则说明这个函数没有参数
        ploc是指该函数的入口地址（该函数的第一条指令在PCode表中的位置）
    */
    int last;
    int lastpar;
    int ploc;
    //int vsize;
};

struct pcode {
    op_code f;
    int l;
    int a;
};

class Table {
    public:
        static void add_idrcd ( string name, id_type type);
        static void add_idrcd ( string name, int adr, id_type type);
        static void add_idrcd ( string name, float float_value );
        static void add_idrcd ( string name, id_type type, int adr, int size);
        static void add_floatrcd ( float fl );
        static void add_strrcd ( string s );
        static int find_ident ( int p, string name );
        static int find_ident_fun ( string name );
        static bool is_funcrcd ( id_rcd r );
        static bool is_arrayrcd ( id_rcd r );
        static bool is_varrcd ( id_rcd r );
        static bool is_constrcd ( id_rcd r );
        static id_rcd get_idrcd ( int index );
        static array_rcd get_arrayrcd ( int index );
        static func_rcd get_funrcd ( int index );
        static id_rcd get_funrcd ();
        static float get_floatval ( int index );
        static string get_str ( int index );
        static int get_idtable_size();
        static int get_rctable_size();
        static int get_pctable_size();
        static int get_strtable_size();
        static int get_array_size( id_rcd r );
        static void set_lastpar ( string func_name );
        static void set_ploc ( string func_name );
        static int get_lastpar ( id_rcd r );
        static int get_ploc ( id_rcd r );
        static void set_lastid();
        static id_type get_func_type ( string func_name );
        static void emit ( op_code f );
        static void emit ( op_code f, int l );
        static void emit ( op_code f, int l, int a );
        static void update_emit ( op_code f, int l, int a, int loc );

        //测试函数
        static void test_id_table();
        static void test_func_table();
        static void test_rconst_table();
        static void test_arr_table();
        static void test_pcode_table();
        static void test_str_table();

        static vector<pcode> pcode_table;//PCode码表


    private:
        static int data_adr;//数据区地址
        static int fun_adr;//局部地址
        static vector<id_rcd> id_table;//符号表
        static vector<array_rcd> array_table;//数组信息表
        static vector<func_rcd> func_table;//函数信息表
        static vector<float> rconst_table;//实常量表
        static vector<string> str_table;//字符串常量表

        static bool is_id_repeat ( string name, int lev );
        static void add_arrayrcd ( element_type eltype, int elsize, int size);
        static void report_err ( int index );
};

#endif // IDENTTABLE_H

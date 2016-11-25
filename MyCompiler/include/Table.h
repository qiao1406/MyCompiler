#ifndef IDENTTABLE_H
#define IDENTTABLE_H

#include <queue>
#include <vector>
#include <string>

using namespace std;

enum id_type {
    INT_VAR,
    CHAR_VAR,
    FLOAT_VAR,
    INT_ARRAY,
    CHAR_ARRAY,
    FLOAT_ARRAY,
    INT_CONST,
    CHAR_CONST,
    FLOAT_CONST,
    INT_FUNCTION,
    CHAR_FUNCTION,
    FLOAT_FUNCTION,
    VOID_FUNCTION
};

enum element_type { INT, CHAR, FLOAT };

enum op_code {
    LIT, OPR, LOD, STO, CAL,
    INT, JMP, JPC, RED, WRT
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
        adr 对于变量名，应该填入该变量在运行栈S中分配存储单元的
            相对地址；对于函数名，填入它相应目标代码的入口地址；
            对于int和char常量名，填入他们对应的整数值或者是ASCII值；
            对于float常量名，填入它在rconst_table中登录的位置
     */

    string name;
    int link;
    id_type type;
    int ref;
    int lev;
};

struct array_rcd {
    element_type eltype; // 数组元素的类型
    int elsize; // 数组元素的大小
    int size; // 数组的大小
};

struct pcode {
    op_code f;
    int x;
    int y;
};

class Table {
    public:
        static void add_idrcd ( string name, int link, id_type typ, int ref, int adr);

    private:
        static vector<id_rcd> id_table;//符号表
        static vector<array_rcd> array_table;//数组信息表
        static vector<float> rconst_table;//实常量表
        static vector<string> str_table;//字符串常量表
        static queue<pcode> pcode_table;//PCode码表
};

#endif // IDENTTABLE_H

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
        ���ű���һ���¼
        name�Ǳ�ʶ�������֣������ִ�Сд
        link��ͬһ��������һ����ʶ����id_table���е�λ�ã�ÿ��
            ������id_table���еĵ�һ����ʶ��������������linkֵΪ0
        type�Ǳ�ʶ������
        ref ��ʶ�������������ʱ��ָ������array_table�е�¼��λ��
            ��ʶ���Ǻ�����ʱ��ָ������func_table�е�λ��
            ���������Ϊ-1
        lev ��ʾ�ñ�ʶ������λ�õĲ�Σ�ȫ��Ϊ0��������Ϊ1
        adr ���ڱ�������Ӧ������ñ���������ջS�з���洢��Ԫ��
            ��Ե�ַ�����ں���������������ӦĿ��������ڵ�ַ��
            ����int��char���������������Ƕ�Ӧ������ֵ������ASCIIֵ��
            ����float����������������rconst_table�е�¼��λ��
     */

    string name;
    int link;
    id_type type;
    int ref;
    int lev;
};

struct array_rcd {
    element_type eltype; // ����Ԫ�ص�����
    int elsize; // ����Ԫ�صĴ�С
    int size; // ����Ĵ�С
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
        static vector<id_rcd> id_table;//���ű�
        static vector<array_rcd> array_table;//������Ϣ��
        static vector<float> rconst_table;//ʵ������
        static vector<string> str_table;//�ַ���������
        static queue<pcode> pcode_table;//PCode���
};

#endif // IDENTTABLE_H
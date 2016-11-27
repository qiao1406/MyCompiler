#ifndef ERROR_H
#define ERROR_H

#include <iostream>
#include <string>

using namespace std;

struct error_type {
    int index;
    string prompt_info;
};

class Error {
    public:
        Error ( int line, int index);
        const static int ERROR_NUM = 40;

    private:
        int location;
        error_type errinfo;

        static error_type errtyp_list[ERROR_NUM];

        void prt_err();
};

#endif // ERROR_H

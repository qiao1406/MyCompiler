#ifndef ERRORPROCESS_H
#define ERRORPROCESS_H

#include <queue>

#include "Error.h"

using namespace std;

class ErrorProcess
{
    public:
        static void prt_errs ();
        static void add_err ( int line, int index);
        static bool is_err_exist();

    private:
        static queue<Error> errs;
};

#endif // ERRORPROCESS_H

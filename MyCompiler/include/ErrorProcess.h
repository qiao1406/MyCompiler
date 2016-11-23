#ifndef ERRORPROCESS_H
#define ERRORPROCESS_H

#include <queue>

#include "Error.h"

using namespace std;

class ErrorProcess
{
    public:
        static queue<Error> errs;
        static void add_err ( int line, int index);

    private:

};

#endif // ERRORPROCESS_H

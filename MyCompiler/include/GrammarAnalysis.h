#ifndef GRAMMARANALYSIS_H
#define GRAMMARANALYSIS_H

#include "Word.h"
#include "WordAnalysis.h"
#include "Table.h"
#include "Runtime.h"

class GrammarAnalysis
{
    public:
        static void ga_programme();

    private:
        static Word nextword();
        static void lastword();
        static void renew_pointer();
        static void prt_grammar_info ( string name);
        static void err_report ( int index );
        static bool is_sametype ( id_type t1, data_type t2 );
        static void jump_read ( vector<string> v );

        static void ga_constant();
        static void ga_constdef();
        static void ga_variable();
        static void ga_vardef();
        static void ga_returnfun();
        static void ga_voidfun();
        static void ga_mainfun();
        static void ga_complex_stmt();
        static void ga_statement();
        static void ga_condition_stmt();
        static void ga_cycle_stmt();
        static void ga_retfuncall_stmt ( string func_name );
        static void ga_voidfuncall_stmt ( string func_name );
        static void ga_assign_stmt ( string id_name );
        static void ga_read_stmt();
        static void ga_write_stmt();
        static void ga_return_stmt();
        static void ga_case_stmt();
        static void ga_subcase_stmt();
        static void ga_argulist ( string func_name );
        static void ga_condition();
        static void ga_expression();
        static void ga_item();
        static void ga_factor();

        static int row_index;
        static int col_index;
        static int pointer;
        static Word nowword;

};

#endif // GRAMMARANALYSIS_H

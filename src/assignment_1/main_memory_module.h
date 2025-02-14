#ifndef MAIN_MEMORY_MODULE_H
#define MAIN_MEMORY_MODULE_H

#include <systemc.h>
#include <iostream>

#include "constants.h"

SC_MODULE(MainMemory) {
    public:
    enum FunctionMem { FUNC_READ_MEM, FUNC_WRITE_MEM };
    enum RetCodeMem { RET_READ_DONE_MEM, RET_WRITE_DONE_MEM };


        sc_in<bool> Port_CLK;
        sc_in<FunctionMem> Port_MemFunc;
        sc_in<uint64_t> Port_MemAddr;
        sc_out<RetCodeMem> Port_MemDone;
        sc_inout_rv<64> Port_MemData;

        SC_CTOR(MainMemory) {
            SC_THREAD(execute);
            sensitive << Port_CLK.pos();
            dont_initialize();
        }


    private:
        void execute ();
};

#endif
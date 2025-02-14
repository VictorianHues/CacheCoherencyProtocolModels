#ifndef CACHE_MEMORY_MODULE_H
#define CACHE_MEMORY_MODULE_H

#include <systemc.h>
#include <iostream>

#include "psa.h"
#include "constants.h"
#include "cache_struct.h"
#include "main_memory_module.h"

SC_MODULE(CacheMemory) {
    public:
        enum Function { FUNC_READ, FUNC_WRITE };
        enum RetCode { RET_READ_DONE, RET_WRITE_DONE };

        enum FunctionMem { FUNC_READ_MEM, FUNC_WRITE_MEM };
        enum RetCodeMem { RET_READ_DONE_MEM, RET_WRITE_DONE_MEM };

        sc_in<bool> Port_CLK;
        sc_in<Function> Port_Func;
        sc_in<uint64_t> Port_Addr;
        sc_out<RetCode> Port_Done;
        sc_inout_rv<64> Port_Data;

        sc_in<MainMemory::RetCodeMem> Port_MemDone;
        sc_out<MainMemory::FunctionMem> Port_MemFunc;
        sc_out<uint64_t> Port_MemAddr;
        sc_inout_rv<64> Port_MemData;

        SC_CTOR(CacheMemory) {
            SC_THREAD(execute);
            sensitive << Port_CLK.pos();
            dont_initialize();
        }

        void dump();

    private:
        CacheSet cache[NUM_SETS];
        
        void update_lru(CacheSet &set, size_t index);
        size_t find_lru(CacheSet &set);
        void read_from_main_memory(uint64_t addr, uint64_t &data);
        void write_to_main_memory(uint64_t addr, uint64_t &data);
        void execute ();
};

#endif
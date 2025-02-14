#ifndef CACHE_MEMORY_MODULE_H
#define CACHE_MEMORY_MODULE_H

#include <systemc.h>
#include <iostream>

#include "constants.h"
#include "cache_struct.h"

SC_MODULE(CacheMemory) {
    public:
        enum Function { FUNC_READ, FUNC_WRITE };
        enum RetCode { RET_READ_DONE, RET_WRITE_DONE };

        sc_in<bool> Port_CLK;
        sc_in<Function> Port_Func;
        sc_in<uint64_t> Port_Addr;
        sc_out<RetCode> Port_Done;
        sc_inout_rv<64> Port_Data;

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
        void execute ();
};

#endif
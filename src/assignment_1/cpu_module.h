#ifndef CPU_MODULE_H
#define CPU_MODULE_H

#include <iostream>
#include <iomanip>
#include <systemc>
#define SC_ALLOW_DEPRECATED_IEEE_API

#include "psa.h"
#include "cache_memory_module.h"
#include "constants.h"
#include "cache_struct.h"

SC_MODULE(CPU) {
public :
    sc_in<bool> Port_CLK;
    sc_in<CacheMemory::RetCode> Port_CpuDone;
    sc_out<CacheMemory::Function> Port_CpuFunc;
    sc_out<uint64_t> Port_CpuAddr;
    sc_inout_rv<64> Port_CpuData;

    SC_CTOR(CPU) {
        SC_THREAD(execute);
        sensitive << Port_CLK.pos();
        dont_initialize();
    }

private :
    CacheSet cache[NUM_SETS];
    
    void execute() ;
};

#endif 
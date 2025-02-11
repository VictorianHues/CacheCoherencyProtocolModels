#ifndef CPU_MODULE_H
#define CPU_MODULE_H

#include <iostream>
#include <iomanip>
#include <systemc>
#define SC_ALLOW_DEPRECATED_IEEE_API

#include "psa.h"
#include "memory_module.h"

static const size_t MEM_SIZE = 2500;

SC_MODULE(CPU) {
public :
    sc_in<bool> Port_CLK;
    sc_in<Memory::RetCode> Port_MemDone;
    sc_out<Memory::Function> Port_MemFunc;
    sc_out<uint64_t> Port_MemAddr;
    sc_inout_rv<64> Port_MemData;

    SC_CTOR(CPU) {
        SC_THREAD(execute);
        sensitive << Port_CLK.pos();
        dont_initialize();
    }

private :
    void execute() ;
};

#endif 
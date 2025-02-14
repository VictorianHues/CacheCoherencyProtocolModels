/*
 * File: assignment1.cpp
 *
 * Framework to implement Task 1 of the Multi-Core Processor Systems lab
 * session. This uses the framework library to interface with tracefiles which
 * will drive the read/write requests
 *
 * Author(s): Michiel W. van Tol, Mike Lankamp, Jony Zhang,
 *            Konstantinos Bousias, Simon Polstra
 *
 */

#include <iostream>
#include <iomanip>
#include <systemc>
#include <exception>
#define SC_ALLOW_DEPRECATED_IEEE_API

#include "cache_memory_module.h"
#include "cpu_module.h"
# include "main_memory_module.h"
#include "psa.h"

using namespace std;
using namespace sc_core; // This pollutes namespace, better: only import what you need.


int sc_main(int argc, char *argv[]) {
    try {
        // Get the tracefile argument and create Tracefile object
        // This function sets tracefile_ptr and num_cpus
        init_tracefile(&argc, &argv);

        // Initialize statistics counters
        stats_init();

        // Instantiate Modules
        MainMemory mem("main_memory");
        CacheMemory cache("cache_memory");
        CPU cpu("cpu");

        // CPU-Cache Signals
        sc_buffer<MainMemory::FunctionMem> sigFuncMem;
        sc_buffer<MainMemory::RetCodeMem> sigDoneMem;

        sc_buffer<CacheMemory::Function> sigFuncCache;
        sc_buffer<CacheMemory::RetCode> sigDoneCache;

        sc_signal<uint64_t> sigAddr, sigAddrMem;
        sc_signal_rv<64> sigData, sigDataMem;

        // The clock that will drive the CPU and Memory
        sc_clock clk;

        // Connecting module ports with signals
        cache.Port_Func(sigFuncCache);
        cache.Port_Addr(sigAddr);
        cache.Port_Data(sigData);
        cache.Port_Done(sigDoneCache);

        cache.Port_MemFunc(sigFuncMem);
        cache.Port_MemAddr(sigAddrMem);
        cache.Port_MemData(sigDataMem);
        cache.Port_MemDone(sigDoneMem);

        mem.Port_MemFunc(sigFuncMem);
        mem.Port_MemAddr(sigAddrMem);
        mem.Port_MemData(sigDataMem);
        mem.Port_MemDone(sigDoneMem);

        cpu.Port_CpuFunc(sigFuncCache);
        cpu.Port_CpuAddr(sigAddr);
        cpu.Port_CpuData(sigData);
        cpu.Port_CpuDone(sigDoneCache);

        cache.Port_CLK(clk);
        cpu.Port_CLK(clk);
        mem.Port_CLK(clk);


        cout << "Running (press CTRL+C to interrupt)... " << endl;


        // Start Simulation
        sc_start();

        // Print statistics after simulation finished
        stats_print();
        // mem.dump(); // Uncomment to dump memory to stdout.
    }

    catch (exception &e) {
        cerr << e.what() << endl;
    }

    return 0;
}

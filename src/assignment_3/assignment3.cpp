#include <iostream>
#include <systemc.h>

#include "CPU.h"
#include "CACHE.h"
#include "BUS.h"
#include "MEMORY.h"
#include "psa.h"

using namespace std;

int sc_main(int argc, char *argv[]) {
    try {
        // Get the tracefile argument and create Tracefile object
        // This function sets tracefile_ptr and num_cpus
        init_tracefile(&argc, &argv);

        // init_tracefile changed argc and argv so we cannot use
        // getopt anymore.
        // The "-q" flag must be specified _after_ the tracefile.
        if (argc == 2 && !strcmp(argv[0], "-q")) {
            sc_report_handler::set_verbosity_level(SC_LOW);
        }

        sc_set_time_resolution(1, SC_PS);

        // Initialize statistics counters
        stats_init();

        // Number of CPUs and caches to create
        extern uint32_t num_cpus;

        vector<CPU*> cpus;
        vector<Cache*> caches;
        Memory *memory = new Memory("memory");
        Bus *bus = new Bus("bus");

        // The clock that will drive the CPU
        sc_clock clk;

        /* Initialize and connect Caches and CPUs */
        for (uint32_t i = 0; i < num_cpus; ++i) {
            log("top", "Creating CPU and Cache", i);
            
            cpus.push_back(new CPU(sc_gen_unique_name("cpu"), i));
            caches.push_back(new Cache(sc_gen_unique_name("cache"), i));

            // Connect instances
            cpus[i]->cache(*caches[i]);
            caches[i]->bus(*bus);
            caches[i]->cpu(*cpus[i]);

            cpus[i]->clk(clk);
            caches[i]->clk(clk);
            
            bus->add_cache(caches[i]);      
        }

        // Connect Memory and Bus
        bus->memory(*memory);
        memory->bus(*bus);

        // Connect Clock to all components
        bus->clk(clk);
        memory->clk(clk);


        // Start Simulation
        sc_start();

        // Print statistics after simulation finished
        stats_print();

        // Print Cache Bus Arbitration Waiting Time
        sc_time total_time = sc_time_stamp();
        cout << setw(10) << "Cache ID" << setw(20) << "Bus Wait Time" << setw(30) << "Percentage of Total Time" << endl;
        cout << "-------------------------------------------------------------" << endl;
        for (uint32_t i = 0; i < num_cpus; ++i) {
            uint64_t bus_waiting_time = caches[i]->get_time_waiting_for_bus_arbitration();
            double percentage = (bus_waiting_time / total_time.to_double()) * 100;
            cout << setw(10) << i << setw(20) << bus_waiting_time << setw(30) << percentage << "%" << endl;
        }

        // Print Memory Read and Write Count
        int read_count = memory->get_read_count();
        int write_count = memory->get_write_count();

        cout << "Memory read count: " << read_count << endl;
        cout << "Memory write count: " << write_count << endl;

        // Cleanup components
        for (uint32_t i = 0; i < num_cpus; ++i) {
            delete cpus[i];
            delete caches[i];
        }
        delete memory;
        delete bus;
    } catch (exception &e) {
        cerr << e.what() << endl;
    }

    return 0;
}

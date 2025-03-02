#ifndef MEMORY_H
#define MEMORY_H

#include <iostream>
#include <systemc.h>

#include "bus_slave_if.h"
#include "helpers.h"
#include "constants.h"
#include "psa.h"


class Memory : public bus_slave_if, public sc_module {
    /*
        Memory class that implements the bus_slave_if interface to allow
        communication between the Bus and Memory modules.

        The Memory class is responsible for handling requests from the Bus
        and sending responses back to the Bus module.

        The Memory module implements a simple memory model with a fixed
        latency for read and write operations and no actual data storage. 
        It also uses the Transaction Level Model to simulate communications,
        but does not require actual data to be sent back to the Bus module.

        The Memory class is implemented as a SystemC module and uses the SystemC
        TLM interface for communication with the Bus module.
    */
    public:
        SC_CTOR(Memory) : read_count(0), write_count(0) {
            // nothing to do here right now.
        }

        ~Memory() {
            // nothing to do here right now.
        }

        int read(uint64_t addr, Cache* requester) {
            /*
                Read data from the Memory. Called by Bus.

                Parameters:
                    addr: address to read from
                    requester: Cache module that is requesting the read
                
                Returns:
                    0
            */
            assert((addr & 0x3) == 0);
            log(name(), "READ from address", addr);
            wait(MEM_LATENCY);
            read_count++;
            return 0;
        }

        int write(uint64_t addr, Cache* requester) {
            /*
                Write data to the Memory. Called by Bus.

                Parameters:
                    addr: address to write to
                    requester: Cache module that is requesting the write
                
                Returns:
                    0
            */
            assert((addr & 0x3) == 0);
            log(name(), "WRITE to address", addr);
            wait(MEM_LATENCY);
            write_count++;
            return 0;
        }

        int write_invalidate(uint64_t addr, Cache* requester) {
            /*
                Implementation of the write_invalidate method from 
                the bus_slave_if interface.

                * Should add a new bus-to-memory interface method to only
                include read and write operations.

                Parameters:
                    addr: address to write to
                    requester: Cache module that is requesting the write
                
                Returns:
                    0
            */
            return 0;
        }

        int get_read_count() const {
            return read_count;
        }

        int get_write_count() const {
            return write_count;
        }

    private:
        int read_count;
        int write_count;
};
#endif

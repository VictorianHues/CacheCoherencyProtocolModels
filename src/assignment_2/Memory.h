#ifndef MEMORY_H
#define MEMORY_H

#include <iostream>
#include <systemc.h>

#include "bus_slave_if.h"
#include "helpers.h"
#include "constants.h"


class Memory : public bus_slave_if, public sc_module {
    public:

    SC_CTOR(Memory) {
        // nothing to do here right now.
    }

    ~Memory() {
        // nothing to do here right now.
    }

    int read(uint64_t addr, Cache* requester) {
        assert((addr & 0x3) == 0);
        log(name(), "read from address", addr);
        wait(MEM_LATENCY);
        return 0;
    }

    int write(uint64_t addr, Cache* requester) {
        assert((addr & 0x3) == 0);
        log(name(), "write to address", addr);
        wait(MEM_LATENCY);
        return 0;
    }
};
#endif

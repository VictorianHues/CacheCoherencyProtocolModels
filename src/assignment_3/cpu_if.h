#include <systemc.h>

#ifndef CPU_IF_H
#define CPU_IF_H

class cpu_if : public virtual sc_interface {
    public:
        virtual void read_response(uint64_t addr, uint64_t data) = 0;
        virtual void write_response(uint64_t addr) = 0;
};

#endif

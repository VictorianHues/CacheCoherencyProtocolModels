#include <systemc.h>

#ifndef BUS_SLAVE_IF_H
#define BUS_SLAVE_IF_H

class Cache;

class bus_slave_if : public virtual sc_interface {
    public:
    virtual int read(uint64_t addr, Cache* requester) = 0;
    virtual int write(uint64_t addr, Cache* requester) = 0;
    virtual int write_invalidate(uint64_t addr, Cache* requester) = 0;
};

#endif

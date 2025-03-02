#include <systemc.h>

#ifndef BUS_CACHE_IF_H
#define BUS_CACHE_IF_H

class Cache;

class bus_cache_if : public virtual sc_interface {
    public:
    virtual bool snoop(uint64_t addr, bool is_write) = 0;
    virtual int read(uint64_t addr, Cache* requester) = 0;
    virtual int write(uint64_t addr, Cache* requester) = 0;
};

#endif

#include <systemc.h>

#ifndef MEMORY_IF_H
#define MEMORY_IF_H

class Cache;

class memory_if : public virtual sc_interface {
    public:
    virtual bool system_busy() = 0;
    
    virtual void read_failed_snoop(uint64_t requester_id, uint64_t addr)= 0;
    virtual void read_write_allocate(uint64_t requester_id, uint64_t addr) = 0;
    virtual void write(uint64_t requester_id, uint64_t addr, uint64_t data) = 0;
    virtual void write_evicted(uint64_t requester_id, uint64_t addr, uint64_t data) = 0;
    virtual void write_through(uint64_t requester_id, uint64_t addr, uint64_t data) = 0;

    virtual void bus_arbitration_notification() = 0;
};

#endif

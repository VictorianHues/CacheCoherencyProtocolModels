#ifndef BUS_IF_H
#define BUS_IF_H

#include <systemc.h>

class bus_if : public virtual sc_interface {
    public:
        virtual bool system_busy() = 0;

        virtual void read(uint64_t requester_id, uint64_t addr) = 0;
        virtual void write_to_main_memory(uint64_t requester_id, uint64_t addr, uint64_t data) = 0;
        virtual void write_evicted_to_main_memory(uint64_t requester_id, uint64_t addr, uint64_t data) = 0;
        virtual void read_for_write_allocate(uint64_t requester_id, uint64_t addr) = 0;
        virtual void broadcast_invalidate(uint64_t requester_id, uint64_t addr) = 0;

        virtual void mem_read_write_allocate_complete(uint64_t requester_id, uint64_t addr, uint64_t data) = 0;
        virtual void mem_read_failed_snoop_complete(uint64_t requester_id, uint64_t addr, uint64_t data) = 0;
        virtual void mem_write_to_main_memory_complete(uint64_t requester_id, uint64_t addr) = 0;
        virtual void cache_snoop_read_response(uint64_t requester_id, uint64_t addr, uint64_t data) = 0;
};

#endif

#ifndef CACHE_IF_H
#define CACHE_IF_H

#include <systemc.h>

class cache_if : public virtual sc_interface {
    public:
        virtual bool system_busy() = 0;
        virtual uint64_t get_time_waiting_for_bus_arbitration() = 0;

        /* Requests from CPU */
        virtual void cpu_read(uint64_t addr) = 0;
        virtual void cpu_write(uint64_t addr) = 0;

        /* Responses from other Modules */
        virtual void snoop_read_response_cache(uint64_t addr, uint64_t data) = 0;
        virtual void snoop_read_response_mem(uint64_t addr, uint64_t data) = 0;
        virtual void snoop_invalidate_response(uint64_t addr) = 0;

        virtual void read_for_write_allocate_response(uint64_t addr, uint64_t data) = 0;
        virtual void write_to_main_memory_complete(uint64_t addr) = 0;
        virtual void write_through_response(uint64_t addr) = 0;

        virtual void bus_arbitration_notification() = 0;

        /* Snooping Functionality */
        virtual bool snoop_read(uint64_t requester_id, uint64_t addr) = 0;
        virtual void snoop_invalidate(uint64_t requester_id, uint64_t addr) = 0;
};

#endif

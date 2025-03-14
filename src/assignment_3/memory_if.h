#include <systemc.h>

#ifndef MEMORY_IF_H
#define MEMORY_IF_H

class Cache;

/**
 * Memory Interface
 * 
 * The Memory Interface is used to define the communication between the Memory and the Bus.
 * 
 */
class memory_if : public virtual sc_interface {
    public:
    virtual bool system_busy() = 0;
    
    /**
     * Read request from Cache for READ MISSES.
     * Caches Snoop the Bus for READS with matching CACHE LINES.
     * If no Cache has a matching Cache Line, then read from Main Memory.
     * 
     * @param requester_id The ID of the Cache that requested the READ.
     * @param addr The address of the Cache Line to READ.
     */
    virtual void read_failed_snoop(uint64_t requester_id, uint64_t addr)= 0;

    /**
     * Read request from Cache for WRITE MISSES with WRITE ALLOCATE.
     * 
     * @param requester_id The ID of the Cache that requested the READ.
     * @param addr The address of the Cache Line to READ.
     * 
     */
    virtual void read_write_allocate(uint64_t requester_id, uint64_t addr) = 0;

    /**
     * Write request from Cache for WRITES to MAIN MEMORY.
     * 
     * @param requester_id The ID of the Cache that requested the WRITE.
     * @param addr The address of the Cache Line to WRITE.
     * @param data The data to WRITE to Main Memory.
     */
    virtual void write(uint64_t requester_id, uint64_t addr, uint64_t data) = 0;

    /**
     * Notification from the Bus that it is available for communication.
     */
    virtual void bus_arbitration_notification() = 0;
};

#endif

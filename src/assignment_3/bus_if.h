#ifndef BUS_IF_H
#define BUS_IF_H

#include <systemc.h>

/** 
 * Bus Interface
 * 
 * The Bus Interface is used to define the communication between the Bus and Caches, and the Bus and Main Memory.
 */
class bus_if : public virtual sc_interface {
    public:
        /**
         * Checks if the Bus or the Memory are still processing requests.
         * 
         * @return bool True if the Bus or Memory are still processing requests, False otherwise.
         */
        virtual bool system_busy() = 0;

        /**
         * Pushes a READ request to the Bus from a Cache.
         * Results from READ MISSES.
         * 
         * @param requester_id The ID of the Cache that requested the READ.
         * @param addr The address of the Cache Line to READ.
         */
        virtual void read(uint64_t requester_id, uint64_t addr) = 0;

        /**
         * Pushes a WRITE TO MAIN MEMORY request to the Bus from a Cache.
         * Results from READ and WRITE MISSES that cause Cache Line Evictions.
         * 
         * @param requester_id The ID of the Cache that requested the WRITE.
         * @param addr The address of the Cache Line to WRITE.
         * @param data The data to WRITE to Main Memory.
         */
        virtual void write_to_main_memory(uint64_t requester_id, uint64_t addr, uint64_t data) = 0;

        /**
         * Pushes a READ FOR WRITE ALLOCATE request to the Bus from a Cache.
         * Results from WRITE MISSES that require data to be read from Main Memory.
         * 
         * @param requester_id The ID of the Cache that requested the READ.
         * @param addr The address of the Cache Line to READ.
         */
        virtual void read_for_write_allocate(uint64_t requester_id, uint64_t addr) = 0;

        /**
         * Broadcasts an INVALIDATE request to all Caches except the requester Cache.
         * Results from WRITE HITS that require other Caches to invalidate their Cache Lines.
         * 
         * @param requester_id The ID of the Cache that requested the WRITE.
         * @param addr The address of the Cache Line to INVALIDATE.
         */
        virtual void broadcast_invalidate(uint64_t requester_id, uint64_t addr) = 0;

        /**
         * RESPONSE from MAIN MEMORY after a READ for WRITE ALLOCATION.
         * read_for_write_allocate -> mem_read_write_allocate_complete
         * 
         * @param requester_id The ID of the Cache that requested the READ WRITE ALLOCATE
         * @param addr The address of the Cache Line to READ.
         * @param data The data read from Main Memory.
         */
        virtual void mem_read_write_allocate_complete(uint64_t requester_id, uint64_t addr, uint64_t data) = 0;

        /**
         * RESPONSE from MAIN MEMORY after a READ MISS into a SNOOP READ FAILURE.
         * read -> snoop_read -> mem_read_failed_snoop_complete
         * 
         * @param requester_id The ID of the Cache that requested the READ.
         * @param addr The address of the Cache Line to READ.
         * @param data The data read from Main Memory.
         */
        virtual void mem_read_failed_snoop_complete(uint64_t requester_id, uint64_t addr, uint64_t data) = 0;

        /**
         * RESPONSE from MAIN MEMORY after a direct WRITE to MAIN MEMORY.
         * write_to_main_memory -> mem_write_to_main_memory_complete
         * 
         * @param requester_id The ID of the Cache that requested the WRITE.
         * @param addr The address of the Cache Line to WRITE.
         */
        virtual void mem_write_to_main_memory_complete(uint64_t requester_id, uint64_t addr) = 0;

        /**
         * RESPONSE from a Cache after a SUCCESSFUL SNOOP READ request.
         * snoop_read -> cache_snoop_read_response
         * 
         * @param requester_id The ID of the Cache that requested the READ.
         * @param addr The address of the Cache Line to READ.
         * @param data The data read from the Cache.
         */
        virtual void cache_snoop_read_response(uint64_t requester_id, uint64_t addr, uint64_t data) = 0;

        /**
         * RESPONSE from a Cache after a SUCCESSFUL SNOOP READ ALLOCATE request.
         * snoop_read_allocate -> cache_snoop_read_allocate_response
         * 
         * @param requester_id The ID of the Cache that requested the READ.
         * @param addr The address of the Cache Line to READ.
         * @param data The data read from the Cache.
         */
        virtual void cache_snoop_read_allocate_response(uint64_t requester_id, uint64_t addr, uint64_t data) = 0;

        /**
         * Memory notifies the Bus that it is waiting for Bus Arbitration.
         */
        virtual void memory_notify_bus_arbitration() = 0;

        /**
         * Cache notifies the Bus that it is waiting for Bus Arbitration.
         * @param cache_id ID of the cache.
         */
        virtual void cache_notify_bus_arbitration(uint64_t cache_id) = 0;
};

#endif

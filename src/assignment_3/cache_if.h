#ifndef CACHE_IF_H
#define CACHE_IF_H

#include <systemc.h>
/**
 * Cache Interface
 * 
 * The Cache Interface is used to define the communication between the Cache and the Bus, and the Cache and the CPU.
 */
class cache_if : public virtual sc_interface {
    public:
        /* System Busy Check */
        virtual bool system_busy() = 0;

        /**
         * Get time spent waiting for Bus arbitration.
         */
        virtual uint64_t get_time_waiting_for_bus_arbitration() = 0;

        /**
         * READ REQUEST from CPU
         * 
         * @param addr The address of the Cache Line to READ.
         */
        virtual void cpu_read(uint64_t addr) = 0;

        /**
         * WRITE REQUEST from CPU
         * 
         * @param addr The address of the Cache Line to WRITE.
         */
        virtual void cpu_write(uint64_t addr) = 0;

        /**
         * RESPONSE from BUS after a READ request completed with a SUCCESSFUL SNOOP.
         * snoop_read -> cache_snoop_read_response -> snoop_read_response_cache
         * 
         * @param addr The address of the Cache Line to READ.
         * @param data The data read from the Cache.
         */
        virtual void snoop_read_response_cache(uint64_t addr, uint64_t data) = 0;

        /**
         * RESPONSE from BUS after a READ request completed with a UNSUCCESSFUL SNOOP
         * and resulting READ from MAIN MEMORY.
         * snoop_read -> cache_snoop_read_response -> snoop_read_response_mem
         * 
         * @param addr The address of the Cache Line to READ.
         * @param data The data read from Main Memory.
         */
        virtual void snoop_read_response_mem(uint64_t addr, uint64_t data) = 0;

        /**
         * RESPONSE from BUS after an INVALIDATE request on SNOOPED CACHE HITS.
         * snoop_invalidate -> cache_snoop_invalidate_response -> snoop_invalidate_response
         * 
         * @param addr The address of the Cache Line to INVALIDATE.
         */
        virtual void snoop_invalidate_response(uint64_t addr) = 0;

        /**
         * RESPONSE from BUS after a READ FOR WRITE ALLOCATION request.
         * read_for_write_allocate -> read_for_write_allocate_response
         * 
         * @param addr The address of the Cache Line to READ.
         * @param data The data read from Main Memory.
         */
        virtual void read_for_write_allocate_response(uint64_t addr, uint64_t data) = 0;

        /**
         * RESPONSSE from BUS after a WRITE TO MAIN MEMORY request.
         * write_to_main_memory -> write_to_main_memory_complete
         * 
         * @param addr The address of the Cache Line to WRITE.
         */
        virtual void write_to_main_memory_complete(uint64_t addr) = 0;

        /* Bus Arbitration notifier */
        virtual void bus_arbitration_notification() = 0;

        /** 
         * "Snoops" the Bus for READ requests caused by READ MISSES
         * 
         * Knows if other caches have the data in the Cache Line, and only sends a response 
         * to the Bus if the data is found in the Cache and it is the first Cache to find it.
         * 
         * @param requester_id The ID of the Cache that requested the READ.
         * @param addr The address of the Cache Line to READ.
         * @param data_already_snooped True if the data was already snooped, False otherwise. Prevents multiple 
         * snoops on the same data.
         * 
         * @return bool True if the Cache Line was found in the Cache, False otherwise.
         * */
        virtual bool snoop_read(uint64_t requester_id, uint64_t addr, bool data_already_snooped) = 0;

        /**
         * "Snoops" the Bus for READ requests caused by READS due to WRITE ALLOCATION. 
         * 
         * Knows if other caches have the data in the Cache Line, and only sends a response
         * to the Bus if the data is found in the Cache and it is the first Cache to find it.
         * 
         * @param requester_id The ID of the Cache that requested the READ.
         * @param addr The address of the Cache Line to READ.
         * @param data_already_snooped True if the data was already snooped, False otherwise. Prevents multiple
         * snoops on the same data.
         * 
         * @return bool True if the Cache Line was found in the Cache, False otherwise.
         */
        virtual bool snoop_read_allocate(uint64_t requester_id, uint64_t addr, bool data_already_snooped) = 0;
        
        /**
         * "Snoops" the Bus for WRITE requests caused by WRITE HITS,
         * requiring that other Caches with matching Cache Lines be INVALIDATED.
         * 
         * @param requester_id The ID of the Cache that requested the WRITE.
         * @param addr The address of the Cache Line to WRITE.
         */
        virtual void snoop_invalidate(uint64_t requester_id, uint64_t addr) = 0;
};

#endif

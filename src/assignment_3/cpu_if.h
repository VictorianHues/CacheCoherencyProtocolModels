#include <systemc.h>

#ifndef CPU_IF_H
#define CPU_IF_H

/**
 * CPU Interface
 * 
 * The CPU Interface is used to define the communication between the CPU and the Cache.
 * 
 */
class cpu_if : public virtual sc_interface {
    public:
        /**
         * Notification from the Cache that a READ request has been completed.
         * 
         * @param addr The address of the Cache Line that was READ.
         * @param data The data that was READ from the Cache Line.
         */
        virtual void read_response(uint64_t addr, uint64_t data) = 0;

        /**
         * Notification from the Cache that a WRITE request has been completed.
         * 
         * @param addr The address of the Cache Line that was WRITTEN.
         */
        virtual void write_response(uint64_t addr) = 0;
};

#endif

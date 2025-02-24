#include <systemc.h>

#ifndef CPU_CACHE_IF_H
#define CPU_CACHE_IF_H

/* NOTE: This interface is implemented by the cache. The CPU uses it to talk
 * to the cache. */
class cpu_cache_if : public virtual sc_interface {
    public:
    virtual int cpu_read(uint64_t addr) = 0;
    virtual int cpu_write(uint64_t addr) = 0;
};

#endif

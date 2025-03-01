#include <systemc.h>

#ifndef BUS_SLAVE_IF_H
#define BUS_SLAVE_IF_H

class Cache;

/* NOTE: Although this model does not have a bus, this bus slave interface is
 * implemented by the memory. The Cache uses this bus slave interface to
 * communicate directly with the memory. */
class bus_slave_if : public virtual sc_interface {
    public:
    virtual int read(uint64_t addr, Cache* requester) = 0;
    virtual int write(uint64_t addr, Cache* requester) = 0;
};

#endif

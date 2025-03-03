#include <systemc.h>

#include "request_response_struct.h"

#ifndef BUS_SLAVE_IF_H
#define BUS_SLAVE_IF_H

class Cache;

class bus_slave_if : public virtual sc_interface {
    public:
    virtual bool system_busy() = 0;
    virtual int read(RequestResponse req_res) = 0;
    virtual int write(RequestResponse req_res, uint64_t data) = 0;
    virtual int write_invalidate(RequestResponse req_res) = 0;
    virtual void notify_response(RequestResponse req_res) = 0;
};

#endif

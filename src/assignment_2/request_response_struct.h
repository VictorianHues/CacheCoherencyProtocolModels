#ifndef REQUEST_RESPONSE_STRUCT_H
#define REQUEST_RESPONSE_STRUCT_H

#include "constants.h"

struct RequestResponse {
    int cache_id;

    uint64_t addr;

    uint64_t tag = -1;
    int set_index;
    uint64_t byte_in_line;

    bool cache_hit = false;
    size_t cache_hit_index = -1;

    bool line_valid = false;

    enum RequestType { READ, WRITE, WRITE_INVALIDATE, INVALIDATE_SELF } request_type;
};

#endif
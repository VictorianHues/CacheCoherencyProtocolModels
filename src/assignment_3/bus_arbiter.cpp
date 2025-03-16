#include <systemc.h>
#include <vector>
#include <queue>
#include <random>

#include "bus_if.h"
#include "memory_if.h"
#include "CACHE.h"
#include "psa.h"
#include "BUS.h"


/**
 * Thread to process the Caches currently waiting for Bus access.
 * Uses a First-Come-First-Serve (FCFS) policy.
 * 
 * Prioritizes Memory requests over Cache requests.
 */
void Bus::bus_arbitration_thread() {
    while (true) {        
        if (memory_waiting == true) {
            log(name(), "MEMORY WAITING FOR BUS ARBITRATION");
            memory->bus_arbitration_notification();
            memory_waiting = false;
        }
        else {
            if (!cache_arbitration.empty()) {
                uint64_t arbitrated_cache_id = cache_arbitration.front();
                cache_arbitration.pop_front();

                log(name(), "ARBITRATED CACHE", arbitrated_cache_id);

                cache_list[arbitrated_cache_id]->bus_arbitration_notification();
            }
        }
        wait();
    }
}

/**
 * Memory notifies the Bus that it is waiting for Bus Arbitration.
 */
void Bus::memory_notify_bus_arbitration() {
    log(name(), "MEMORY NOTIFIED BUS ARBITRATION");

    memory_waiting = true;
}

/**
 * Cache notifies the Bus that it is waiting for Bus Arbitration.
 */
void Bus::cache_notify_bus_arbitration(uint64_t cache_id) {
    log(name(), "CACHE NOTIFIED BUS ARBITRATION on", cache_id);

    cache_arbitration.push_back(cache_id);
}

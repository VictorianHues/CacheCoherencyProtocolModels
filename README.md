# Cache Memory Simulation - SystemC Implementation

## Table of Contents

1. [Overview](#overview)
2. [Architectural Features](#architectural-features)
3. [System Components](#system-components)
4. [Execution Flow](#execution-flow)
5. [Compilation & Execution Instructions](#compilation--execution-instructions)
6. [Future Enhancements](#future-enhancements-assignment-2)
7. [Contributors](#contributors)
8. [References](#references)

## Overview

This project presents an advanced **32KB, 8-way set-associative L1 data cache** implemented in **SystemC**, designed to efficiently manage memory access latency within a computational architecture. The cache adheres to a **write-back** and **write-allocate** policy, interfacing with a main memory subsystem to resolve cache misses and enforce memory consistency. A CPU module issues memory access requests that initiate cache transactions, simulating realistic workload behaviors in a multiprocessor system.

## Architectural Features

- **32KB L1 data cache** with **8-way set associativity**
- **32-byte cache line granularity**
- **Least Recently Used (LRU) replacement algorithm**
- **Write-back policy** (modified data is written to memory only upon eviction)
- **Write-allocate strategy** (on a write miss, the full cache line is fetched into the cache before modification)
- **Single-cycle latency for cache hits**
- **100-cycle latency for main memory accesses**
- **Extensive diagnostic logging for cache operations and state transitions**

## System Components

### 1. **CPU Module**

- Generates **read and write memory access requests**.
- Issues **addresses and function codes** to interface with the cache.

### 2. **Cache Memory Module**

- Implements an **8-way set-associative caching mechanism**.
- Detects **cache hits, misses, and manages evictions**.
- Utilizes **LRU-based replacement policy** to optimize data retention.
- Enforces **write-back and write-allocate behaviors**.
- Engages with the **main memory module** for data retrieval and eviction handling.

### 3. **Main Memory Module**

- Models a **100-cycle latency for memory access**.
- Provides data retrieval for cache misses.
- Handles write-back requests from the cache.

## Execution Flow

1. **CPU Issues a Memory Request**
   - Transmits a **read or write request** to the cache.

2. **Cache Lookup and Resolution**
   - **Cache hit & valid**: Data is returned with **1-cycle latency**.
   - **Cache miss or invalid line**: Data is fetched from **main memory** incurring **100-cycle latency**.

3. **Read Processing**
   - **Cache hit**: Data is retrieved directly from cache.
   - **Cache miss**:
     - Main memory fetch is initiated.
     - Retrieved data is stored in cache.
     - Data is returned to CPU.

4. **Write Processing**
   - **Cache hit**:
     - Data is updated in the cache line.
     - Cache line is marked as **dirty**.
   - **Cache miss**:
     - The **full cache line** is fetched before modification (**write-allocate** policy).
     - Data is updated and the cache line is flagged **dirty**.

5. **Eviction Handling**
   - Identifies the **Least Recently Used (LRU) cache line** for replacement.
   - If the evicted line is **dirty**, it is **written back to memory**.
   - A new cache line is allocated and updated.

## Compilation & Execution Instructions

### **1. Compile the SystemC Project**

```sh
In Progress
```

### **2. Run the Simulation**

```sh
In Progress
```

## Future Enhancements (Assignment 2)

- Extend the cache design to support **multiprocessor cache coherence mechanisms**.
- Implement a **bus snooping protocol** for coherence enforcement.
- Optimize **cache-to-cache data transfers** to minimize main memory access overhead.

## Contributors

- **[Victoria Peterson]**

- Developed based on the framework by **Michiel W. van Tol, Mike Lankamp, Jony Zhang, Konstantinos Bousias, Simon Polstra**

## References

- **SystemC Standard Documentation**: https://www.accellera.org/

- **Course Lectures and Laboratory Assignments**

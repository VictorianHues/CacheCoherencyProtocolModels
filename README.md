# Multi-Core Processor Systems - Cache Coherency Assignments

## Table of Contents

- [Overview](#overview)
- [Assignments](#assignments)
  - [Assignment 1: Implementing a Single Cache](#assignment-1-implementing-a-single-cache)
  - [Assignment 2: Implementing a Multiprocessor Coherent Cache System](#assignment-2-implementing-a-multiprocessor-coherent-cache-system)
  - [Assignment 3: Implementing the MOESI Protocol](#assignment-3-implementing-the-moesi-protocol)
- [Installation & Setup](#installation--setup)
  - [Prerequisites](#prerequisites)
  - [Compilation](#compilation)
  - [Running the Simulation](#running-the-simulation)
  - [Trace Files](#trace-files)
- [Output & Logging](#output--logging)
- [Submission & Reports](#submission--reports)
- [References](#references)

## Overview

This repository contains implementations of various cache coherence protocols using SystemC for the "Multi-Core Processor Systems" course. The assignments focus on building a Level-1 Data-cache simulator, implementing cache coherence protocols, and evaluating their performance using trace files.

## Assignments

### Assignment 1: Implementing a Single Cache

- Develop a **32KB 8-way set-associative L1 Data Cache** with a **32-byte line size**.
- Implement **Least-Recently-Used (LRU) write-back replacement** and **allocate-on-write policy**.
- Simulate cache operations using **single processor trace files**.
- Print cache actions and state transitions to the console.
- Collect **hit/miss statistics** and print them after the simulation.

### Assignment 2: Implementing a Multiprocessor Coherent Cache System

- Extend Assignment 1 to support **multiple processors (1-8 processors)** with **shared memory architecture**.
- Implement a **bus snooping protocol** to ensure cache coherence.
- Use a **VALID-INVALID protocol** for cache coherence.
- Simulate different scenarios using provided trace files.
- Collect performance metrics:
  - Cache hit/miss rates
  - Main memory access rates
  - Bus contention (average time for bus acquisition)
  - Total execution time

### Assignment 3: Implementing the MOESI Protocol

- Extend Assignment 2 to support the **MOESI cache coherence protocol**.
- Implement **cache-to-cache transfers** for shared data consistency.
- Modify the bus system to accommodate the additional states.
- Perform the same experiments as in Assignment 2, analyzing performance differences.

## Installation & Setup

### Prerequisites

- **SystemC**: Ensure SystemC is installed on your system in the location `\home\Username` such that the file structure looks like `home\Username\local\systemc`
- **C++ Compiler**: Use a modern C++ compiler supporting C++11 or later.
- **Make**: Compilation is managed via a Makefile.

### Compilation

To compile the project, modify the `SYSTEMC_PATH` to target your systemc folder and run:
```sh
make
```

### Running the Simulation

To execute the cache simulator with a trace file:
```sh
./assignment_1.bin <trace_file>
./assignment_2.bin <trace_file>
./assignment_3.bin <trace_file>
```

### Trace Files

The provided trace files simulate various workloads:

- **debug** - Small traces for debugging
- **matrix multiplication** - 50x50 integer matrix multiplication
- **matrix-vector multiplication** - Multiplications with varying matrix sizes
- **1D FFT** - Fast Fourier Transform on 1024 integers

## Output & Logging

- Cache actions and state transitions are printed to the console.
- Hit/miss statistics are logged and displayed after the simulation.

## Submission & Reports

Each assignment submission includes:

1. **Source code** (must compile without warnings on a standard Linux system).
2. **Short report** (PDF format) discussing:
   - Problem statement
   - Implementation details
   - Performance analysis and findings
   - Comparison of different protocols

## References

- [AMD64 Architecture Programmer’s Manual](https://www.amd.com/content/dam/amd/en/documents/processor-tech-docs/programmer-references/24593.pdf)
- [CPU Cache Associativity](http://en.wikipedia.org/wiki/CPU_cache#Associativity)

---
For any questions, refer to the lab sessions or discussions on Canvas.


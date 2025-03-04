#!/usr/bin/env python3
import random
import sys

from trace_lib import Trace  # Ensure this matches your import

def generate_multi_cpu_tracefile(filename, num_procs, num_entries):
    trace = Trace(filename, num_procs)

    for i in range(num_entries):
        cpu_id = i % num_procs  # Assign requests round-robin
        addr = random.randint(0x1000, 0xFFFF) & ~0x3  # Random memory address

        if random.random() < 0.5:
            trace.read(addr)  # 50% chance of read
        else:
            trace.write(addr)  # 50% chance of write

    trace.close()

def generate_manual_tracefile(filename):
    trace = Trace(filename, 1)

    # Simulate memory operations
    for i in range(num_entries):
        cpu_id = i % num_procs  # Assign requests round-robin
        if i % 2 == 0:
            trace.write(0x100)
        else:
            trace.read(0x100)


    trace.close()

def generate_all_writes_same_address(filename, num_procs, num_entries):
    trace = Trace(filename, num_procs)

    for i in range(num_entries):
        cpu_id = i % num_procs  # Assign requests round-robin

        trace.write(0x1000)

    trace.close()

def generate_all_reads_same_address(filename, num_procs, num_entries):
    trace = Trace(filename, num_procs)

    for i in range(num_entries):
        cpu_id = i % num_procs  # Assign requests round-robin

        trace.read(0x1000)

    trace.close()

def generate_50_50_reads_writes_same_address(filename, num_procs, num_entries):
    trace = Trace(filename, num_procs)

    for i in range(num_entries):
        cpu_id = i % num_procs  # Assign requests round-robin

        if num_entries % 2 == 0:
            trace.read(0x1000)
        else:
            trace.write(0x1000)

    trace.close()


# Run from command line
if __name__ == "__main__":
    if len(sys.argv) != 4:
        print("Usage: python3 generate_trace.py <filename> <num_procs> <num_entries>")
        sys.exit(1)

    filename = sys.argv[1]
    num_procs = int(sys.argv[2])
    num_entries = int(sys.argv[3])

    generate_multi_cpu_tracefile(filename, num_procs, num_entries)
    #generate_manual_tracefile(filename)

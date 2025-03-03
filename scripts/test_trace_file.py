#!/usr/bin/env python3
import random
import sys

from trace_lib import Trace  # Ensure this matches your import

def generate_multi_cpu_tracefile(filename, num_procs, num_entries):
    trace = Trace(filename, num_procs)

    # Simulate multi-core memory operations
    for i in range(num_entries):
        cpu_id = i % num_procs  # Assign requests round-robin
        #addr = random.randint(0x1000, 0xFFFF) & ~0x3
        addr = 0x1000
        #trace.read(addr) 
        if random.random() < 0.95:
            trace.read(addr)  
        else:
            trace.write(addr)

    trace.close()
    print(f"Generated tracefile: {filename} with {num_procs} CPUs and {num_entries} entries")

def generate_manual_tracefile(filename):
    trace = Trace(filename, 1)

    # Simulate memory operations
    trace.read(0x100)
    trace.read(0x100)
    trace.read(0x100)
    trace.read(0x100)
    trace.read(0x100)
    trace.read(0x100)
    trace.write(0x100)
    trace.write(0x100)
    trace.read(0x100)
    trace.read(0x100)
    trace.read(0x100)
    trace.read(0x100)
    trace.read(0x100)
    trace.read(0x100)
    trace.read(0x100)
    trace.read(0x100)
    trace.read(0x100)
    trace.read(0x100)
    trace.read(0x100)
    trace.read(0x100)
    trace.read(0x100)
    trace.read(0x100)
    trace.read(0x100)
    trace.write(0x100)

    trace.close()



# Run from command line
if __name__ == "__main__":
    if len(sys.argv) != 4:
        print("Usage: python3 generate_trace.py <filename> <num_procs> <num_entries>")
        sys.exit(1)

    filename = sys.argv[1]
    num_procs = int(sys.argv[2])
    num_entries = int(sys.argv[3])

    #generate_multi_cpu_tracefile(filename, num_procs, num_entries)
    generate_manual_tracefile(filename)

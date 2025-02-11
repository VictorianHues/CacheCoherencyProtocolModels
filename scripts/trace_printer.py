#!/usr/bin/env python3

import struct
import sys
import argparse

from trace_lib import Trace_reader

def main():
    parser = argparse.ArgumentParser(
            description='Print trf trace')
    parser.add_argument('trace_file',
            help='Trace in trf format')
    parser.add_argument('-H', '--hex', action='store_true', default=False, help='Print addresses in hex')
    args = parser.parse_args()

    trace = Trace_reader(args.trace_file)
    print(trace.format, trace.num_procs)

    while True:
        e = trace.next()
        if not e:
            break
        (proc_id, e_type, e_addr) = e
        if args.hex:
            print(f'P{proc_id} {Trace_reader.type_to_string(e_type)} 0x{e_addr:x}')
        else:
            print(f'P{proc_id} {Trace_reader.type_to_string(e_type)} {e_addr}')

    trace.close()

if __name__ == "__main__":
    main()

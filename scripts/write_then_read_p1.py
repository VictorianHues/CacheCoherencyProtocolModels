#!/usr/bin/env python3

from trace_lib import Trace

t = Trace(__file__.replace('.py', '.trf'), 1)

t.write(0x20)   # P0: write
t.read(0x20)    # P0: read same data

t.close()


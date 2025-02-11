#!/usr/bin/env python3

from trace_lib import Trace

t = Trace(__file__.replace('.py', '.trf'), 1)

t.read(0x20)    # P0: read
t.read(0x20)    # P0: read same data again

t.close()


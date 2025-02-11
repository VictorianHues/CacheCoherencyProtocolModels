#!/usr/bin/env python3

# Example that shows how to generate a simple two processor trace.

from trace_lib import Trace

t = Trace(__file__.replace('.py', '.trf'), 2)

# 2 processor trace, so generate pairs of events for P0 and P1

# Write some data.
t.write(0x100)  # P0
t.write(0x200)  # P1

# Do nothing.
t.nop()     # P0
t.nop()     # P1

# Do nothing again.
t.nop()     # P0
t.nop()     # P1

# Read back the data.
t.read(0x100) # P0
t.read(0x200) # P1

t.close()

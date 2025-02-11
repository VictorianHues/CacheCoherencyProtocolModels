import struct

class Trace:
    (TYPE_NOP, TYPE_READ, TYPE_WRITE, TYPE_END, TYPE_BARRIER) = range(5)

    # use type_to_enum.index("R") to get the enum TYPE_READ value
    type_to_enum = "NRWEB"

    def __init__(self, filename, num_procs):
        self.num_procs = num_procs
        self.f = open(filename, "wb")
        self.f.write(b"5TRF") # trace file signature
        self.write32(self.num_procs)

    def write32(self, n):
        self.f.write(struct.pack('>I', n))

    def write64(self, n):
        self.f.write(struct.pack('>Q', n))

    def entry(self, t, addr):
        self.write64((t << 61) | (addr & ~(0b111 << 61)))

    def entry_str_type(self, str_type, addr):
        t = Trace.type_to_enum.index(str_type)
        self.entry(t, addr)

    def read(self, addr):
        self.entry(Trace.TYPE_READ, addr)

    def write(self, addr):
        self.entry(Trace.TYPE_WRITE, addr)

    def barrier(self):
        self.entry(Trace.TYPE_BARRIER, 0x0)

    def nop(self):
        self.entry(Trace.TYPE_NOP, 0x0)

    def close(self):
        for _ in range(self.num_procs):
            self.entry(Trace.TYPE_END, 0x0)
        self.f.close()

    def close_without_end(self):
        self.f.close()


class Trace_reader:
    address_mask = ~(0b111 << 61)   # type stored in upper three bits.
    map_type_to_char = "NRWEB"
    map_type_to_string = ["NOP", "READ", "WRITE", "END", "BARRIER"]

    def __init__(self, filename):
        self.f = open(filename, "rb")
        self.format = self.read32().decode('utf-8')
        if self.format != "5TRF":
            print(f"trace format error, got '{self.format}', expect '5TRF'")
            exit(1)
        self.num_procs = struct.unpack_from(">I", self.read32())[0]
        self.proc_id = 0

    def read32(self): return self.f.read(4)

    def read64(self): return self.f.read(8)

    def next(self):
        e = self.read64()
        if not e:
            return None # end of file
        current_proc_id = self.proc_id
        self.proc_id = (self.proc_id + 1) % self.num_procs

        # Unpack type (3 upper bit) and address (lower 61 bits).
        value = struct.unpack_from(">Q", e)[0]
        e_type = value >> 61
        e_addr = value & Trace_reader.address_mask

        return (current_proc_id, e_type, e_addr)

    @staticmethod
    def type_to_char(e_type):
        return Trace_reader.map_type_to_char[e_type]

    @staticmethod
    def type_to_string(e_type):
        return Trace_reader.map_type_to_string[e_type]

    def close(self):
        self.f.close()

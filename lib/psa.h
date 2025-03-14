/*
// Header file for the Parallel System Architectures Lab Session.
// Contains the TraceFile class. This class represents a file where
// traces of memory requests from a program's execution are stored.
// The class can be used to read such files and drive a simulator.
// Furthermore, it contains functions for keeping track of and printing
// statistics.
//
// -- STUDENTS SHOULD NOT NEED TO MODIFY THIS FILE --
//
// Author(s): Michiel W. van Tol, Mike Lankamp, Simon Polstra
*/

#ifndef PSA_H
#define PSA_H

#include <fstream>
#include <vector>

// Define fixed-size types
// Support non-compliant C99 compilers
#if defined(_MSC_VER)
typedef __int8 int8_t;
typedef __int16 int16_t;
typedef __int32 int32_t;
typedef __int64 int64_t;
typedef unsigned __int8 uint8_t;
typedef unsigned __int16 uint16_t;
typedef unsigned __int32 uint32_t;
typedef unsigned __int64 uint64_t;
#else
// We just hope that this compiler properly supports the C++ standard
#define __STDC_LIMIT_MACROS
#include <stdint.h>
#endif

/*
 * Initializes the Tracefile and sets the number of cpu's. It expects the
 * first argument from argv to be the Tracefile name, and modifies argv/argc
 * to remove this argument so that the user can add their own options and
 * argument parser after this function
 */
void init_tracefile(int *argc, char **argv[]);

/*
 * Initializes the statistic counters, needs to be run after init_tracefile
 * as it uses num_cpus to generate its datastructures.
 */
void stats_init();

// Removes and cleanes up the internal statistic counters
void stats_cleanup();

// Pretty-prints the contents of the statistic counters
void stats_print();

// Updates the internal statistic counters for given CPU
void stats_writehit(uint32_t cpuid);
void stats_writemiss(uint32_t cpuid);
void stats_readhit(uint32_t cpuid);
void stats_readmiss(uint32_t cpuid);

// Declaration of a constant to put a 64 bit wire in high impedance mode.
extern const char *float_64_bit_wire;

class TraceFile {
    public:
    // Data type of a memory request's operation type.
    enum EntryType {
        ENTRY_TYPE_NOP = 0x0,
        ENTRY_TYPE_READ = 0x1,
        ENTRY_TYPE_WRITE = 0x2,
        ENTRY_TYPE_END = 0x3, // End is only used internally
        ENTRY_TYPE_BARRIER = 0x4
    };

    // Data type of a memory request entry for a processor
    struct Entry {
        EntryType type;
        uint64_t addr;
    };

    // Constructor / Destructor
    TraceFile(const char *filename);
    ~TraceFile();

    // Closes the file
    void close();

    /*
     * Reads the next entry from the file for the processor specified in pid.
     * Parameter e is a reference to the Entry structure which will receive
     * the data.
     */
    bool next(uint32_t pid, Entry &e);

    // Determines if the end-of-file has been reached
    bool eof() const;

    // Returns the number of processors this file contains traces for
    uint32_t get_proc_count() const;

    private:
    const uint32_t entry_size = 8; // Trace element is 8 bytes.
    struct EntryInfo;

    std::ifstream m_input;
    std::vector<std::streampos> m_positions;
    std::vector<bool> m_waiting;
    uint32_t m_num_finished;
    std::streampos m_endstream;

    // Private copy constructor because no copies are allowed.
    TraceFile(const TraceFile &trf);
};

// Global value giving the number of CPU's in the simulation
extern uint32_t num_cpus;

// Global pointer to the TraceFile class of the opened Tracefile
extern TraceFile *tracefile_ptr;

#endif

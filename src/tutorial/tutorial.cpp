#include <systemc.h>
#include <exception>
#include <iostream>

using namespace std;

static const int MEM_SIZE = 512;



SC_MODULE ( Memory ) {
    public :
    enum Function { FUNC_READ , FUNC_WRITE } ; // Defines enum codes for read/write
    enum RetCode { RET_READ_DONE , RET_WRITE_DONE } ; // Defines enum codes for read/write done


    sc_in<bool> Port_CLK ; // Clock input

    sc_in<Function> Port_Func ; // Memory function input (read/write)
    sc_in<uint64_t> Port_Addr ; // Memory address input
    sc_out<RetCode> Port_Done ; // Memory done output (Uses RetCode enum)
    sc_inout_rv<64> Port_Data ; // Memory data input/output (Bidirectional)

    // Constructor for Memory module
    SC_CTOR ( Memory ) {
        SC_THREAD ( execute ) ; // Register the execute function with the kernel
        sensitive << Port_CLK . pos ( ) ; // Sensitivity list for the clock
        dont_initialize ( ) ; // Do not initialize the thread
        m_data = new uint64_t [ MEM_SIZE ] ; // Allocate memory for the data array
    }

    ~ Memory ( ) {
        delete [ ] m_data ; // Destructor for the Memory module
    }
    private :
    uint64_t * m_data ;

    void execute ( ) {
        while ( true ) {
            // Waits for a value change event on the Port_Func port
            wait ( Port_Func . value_changed_event ( ) ) ;

            Function f = Port_Func . read ( ) ; // Read the function enum code
            uint64_t addr = Port_Addr . read ( ) ; // Read the memory address
            uint64_t data = 0 ; // Initialize data variable

            if ( f == FUNC_WRITE ) { // If the function is a write
                data=Port_Data.read().to_uint64(); // Read the data from the port and convert to uint64
            }

            
            wait ( 100 ) ; // Simulate memory read/write delay

            if ( f == FUNC_READ ) { // If the function is a read enum code
                Port_Data.write ((addr < MEM_SIZE) ? m_data[addr] : 0); // If the address is within the memory size, write the data at the address to the port, otherwise write 0
                Port_Done.write ( RET_READ_DONE ); // Write the read done enum code to the done port

                wait ( ) ;

                // Write 64 Z ’ s to f l o a t the wi r e .
                Port_Data . write ("ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ"
                "ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ") ; // Write 64 Z's to float the wire
            } else {
                if ( addr < MEM_SIZE ) {
                    m_data[addr] = data ;
                }

                Port_Done.write(RET_WRITE_DONE) ;
            }
        }
    }
} ;



SC_MODULE ( CPU ) {
    public :
    sc_in<bool> Port_CLK ;

    sc_in<Memory::RetCode> Port_MemDone ;
    sc_out<Memory::Function> Port_MemFunc ;

    sc_out<uint64_t> Port_MemAddr ;
    sc_inout_rv<64> Port_MemData ;

    SC_CTOR ( CPU ) {
        SC_THREAD ( execute ) ;
        sensitive << Port_CLK . pos ( ) ;
        dont_initialize ( ) ;
    }

    private :
    void execute ( ) {
        while ( true ) {
            Memory::Function f = ( rand ( ) % 10) < 5 ? Memory::FUNC_READ :
            Memory::FUNC_WRITE ;

            uint64_t addr = rand ( ) % MEM_SIZE ;
            uint64_t data ;

            Port_MemAddr.write( addr ) ;
            Port_MemFunc.write( f ) ;

            if ( f == Memory::FUNC_WRITE ) {
                data = rand() ;
                Port_MemData.write( data ) ;

                wait() ;

                Port_MemData.write("ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ"
                    "ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ") ;
            }

            wait(Port_MemDone.value_changed_event()) ;

            if ( f == Memory::FUNC_READ ) {
                data = Port_MemData.read().to_uint64() ;
            }
                
            // Advance one c y c l e in s imulat ed time
            wait() ;
        }
    }
} ;


int sc_main ( int argc , char * argv [ ] ) {
    try {
        // I n s t a n t i a t e Modules
        Memory mem ( " main_memory " ) ;
        CPU _cpu_ ( " cpu " ) ;

        // Buf f e r s and S i g n a l s

        sc_buffer<Memory::Function> sigMemFunc ;
        sc_buffer<Memory::RetCode> sigMemDone ;
        sc_signal<uint64_t> sigMemAddr ;
        sc_signal_rv<64> sigMemData ;

        // The clock that will drive the CPU and Memory

        sc_clock clk ;

        // Connecting module po r t s with s i g n a l s

        mem.Port_Func ( sigMemFunc ) ;
        mem.Port_Addr ( sigMemAddr ) ;
        mem.Port_Data ( sigMemData ) ;
        mem.Port_Done ( sigMemDone ) ;
        
        _cpu_.Port_MemFunc ( sigMemFunc ) ;
        _cpu_.Port_MemAddr ( sigMemAddr ) ;
        _cpu_.Port_MemData ( sigMemData ) ;
        _cpu_.Port_MemDone ( sigMemDone ) ;

        mem.Port_CLK ( clk ) ;
        _cpu_.Port_CLK ( clk ) ;

        cout << " Running ( press CTRL +C to exit )... " << endl ;
        
        sc_start ( ) ;
    } catch ( exception& e ) {
        cerr << e . what ( ) << endl ;
    }
    return 0 ;
}
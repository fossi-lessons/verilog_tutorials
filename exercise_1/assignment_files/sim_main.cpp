// For std::unique_ptr
#include <memory>
#include <cstdint>

// Include common routines
#include <verilated.h>

// Include model header, generated from Verilating "top.v"
#include "Vmux_sim_top.h"

static void time_step(const std::unique_ptr<VerilatedContext> &contextp,
                    const std::unique_ptr<Vmux_sim_top> &top) {
    contextp->timeInc(1);  // 1 timeprecision period passes...
    top->eval();
}

static void init_context(const std::unique_ptr<VerilatedContext> &contextp,
                         int argc,
                         char ** argv) {
    // Set debug level, 0 is off, 9 is highest presently used
    // May be overridden by commandArgs argument parsing
    contextp->debug(0);

    // Randomization reset policy
    // May be overridden by commandArgs argument parsing
    contextp->randReset(2);

    // Verilator must compute traced signals
    contextp->traceEverOn(true);

    // Pass arguments so Verilated code can see them, e.g. $value$plusargs
    // This needs to be called before you create any model
    contextp->commandArgs(argc, argv);
}

// Modify as needed for debugging
static void check_output(const std::unique_ptr<Vmux_sim_top> &top,
                        uint8_t expected_value ) {
    if (top->data_out != expected_value) {
        printf("==ERROR==\n");
        printf("Wrong value when data_sel = %d\n", top->data_sel);
        printf("Expected: %hhu, Got: %hhu\n", expected_value, top->data_out);
        printf("=========\n");
    }

}

// Modify as needed for debugging
static void print_status(const std::unique_ptr<VerilatedContext> &contextp,
                        const std::unique_ptr<Vmux_sim_top> &top) {
    // Read outputs
    VL_PRINTF("[%" VL_PRI64 "d] data_sel=%hx data_out=%hhx\n",
            contextp->time(), top->data_sel, top->data_out);
}


int main(int argc, char** argv, char** env) {
    // Prevent unused variable warnings
    if (false && argc && argv && env) {}
    
    // Create logs/ directory in case we have traces to put under it
    Verilated::mkdir("logs");
    
    // Using unique_ptr is similar to
    // "VerilatedContext* contextp = new VerilatedContext" then deleting at end.
    const std::unique_ptr<VerilatedContext> contextp{new VerilatedContext};
    
    init_context(contextp, argc, argv);
    
    // Construct the Verilated model, from Vmux_sim_top.h generated from
    // Verilating "mux_sim_top".  
    // "TOP" will be the hierarchical name of the module.
    const std::unique_ptr<Vmux_sim_top> top{new Vmux_sim_top{contextp.get(), "TOP"}};

    // Set some initial data values
    top->data_0 = 1;
    top->data_1 = 2;
    top->data_2 = 3;
    top->data_3 = 4;
    top->data_sel = 0;

    /***************************************************************************
     * Check that all the inputs are selected appropriately
     **************************************************************************/

    time_step(contextp, top); // Kick the simulation
    print_status(contextp, top);

    // Check the output
    check_output(top, 1);

    contextp->timeInc(1);   // Advance time
    top->data_sel = 1;      // Modify the input signals
    top->eval();            // Update the signals
    print_status(contextp, top);

    // Check the output (in the same timestep)
    check_output(top, 2);
    
    contextp->timeInc(1);  // Advance time
    top->data_sel = 2;     // Modify the input signals
    top->eval();           // Update the signals
    print_status(contextp, top);

    // Check the output (in the same timestep)
    check_output(top, 3);
    
    contextp->timeInc(1);   // Advance time
    top->data_sel = 3;      // Modify the input signals
    top->eval();            // Update the signals
    print_status(contextp, top);

    // Check the output (in the same timestep)
    check_output(top, 4);


    /**************************************************************************
     * Check that the data output will change in the same timestep when
     * input changes
     **************************************************************************/
    contextp->timeInc(1);  // Advance time
    top->data_3 = 8;       // Modify the input signals
    top->eval();           // Update the signals

    check_output(top, 8);
    print_status(contextp, top);
    /**************************************************************************
     ***************************ADD MORE TESTS HERE****************************
     **************************************************************************/

    time_step(contextp, top);
    time_step(contextp, top);

    // Final model cleanup
    top->final();
    
    return 0;
}

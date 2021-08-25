// For std::unique_ptr
#include <memory>
#include <cstdint>

// Include common routines
#include <verilated.h>

// Include model header, generated from Verilating "top.v"
#include "Vlot_counter_top.h"

#define CLOCK_HALF_CYCLE_NS 5
#define CLOCK_CYCLE_NS (CLOCK_HALF_CYCLE_NS * 2)
#define MAX_CAPACITY 16

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

static void time_step(const std::unique_ptr<VerilatedContext> &contextp,
                    const std::unique_ptr<Vlot_counter_top> &top) {
    contextp->timeInc(1);  // 1 timeprecision period passes...
    top->eval();
}

static void clock_cycle(const std::unique_ptr<VerilatedContext> &contextp,
                    const std::unique_ptr<Vlot_counter_top> &top) {
    top->clk = !top->clk;
    for (int i = 0; i < CLOCK_HALF_CYCLE_NS; i++) {
        time_step(contextp, top);
    }
    top->clk = !top->clk;
    for (int i = 0; i < CLOCK_HALF_CYCLE_NS; i++) {
        time_step(contextp, top);
    }
}

static void check_output(const std::unique_ptr<VerilatedContext> &contextp,
                        const std::unique_ptr<Vlot_counter_top> &top,
                        uint32_t expected_count) {

    bool count_wrong = expected_count != top->count;
    bool full_wrong = ((top->count == MAX_CAPACITY) && (!top->full)) 
                    ||((top->count != MAX_CAPACITY) && (top->full));
    bool empty_wrong = ((top->count == 0) && (!top->empty))
                    || ((top->count != 0) && (top->empty));
    if (count_wrong | full_wrong | empty_wrong) {
        printf("==ERROR==\n");
        if (count_wrong) {
            printf("Wrong count\n");
            printf("Expected: %u, Got: %u\n", expected_count, top->count);
        }

        if (full_wrong) {
            printf("Wrong full flag\n");
            printf("Count is %u, but full flag is %d\n", top->count, top->full);
        }

        if (empty_wrong) {
            printf("Wrong empty flag\n");
            printf("Count is %u, but empty flag is %d\n", top->count, top->empty);
        }
    }
}

// Modify as needed for debugging
static void print_status(const std::unique_ptr<VerilatedContext> &contextp,
                        const std::unique_ptr<Vlot_counter_top> &top) {
    // Read outputs
    VL_PRINTF("[%" VL_PRI64 "d] inner_sensor: %d, outer_sensor: %d, count: %d, \
full: %d, empty: %d\n",
            contextp->time(), top->inner_sensor, top->outer_sensor, top->count,
            top->full, top->empty);
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
    // Verilating "log_counter_top".  
    // "TOP" will be the hierarchical name of the module.
    const std::unique_ptr<Vlot_counter_top> top{new Vlot_counter_top{contextp.get(), "TOP"}};

    // Set some initial data values
    top->inner_sensor = 0;
    top->outer_sensor = 0;
    top->clk = 0;
    top->rst = 1;

    clock_cycle(contextp, top); // Kick the simulation
    clock_cycle(contextp, top);
    
    print_status(contextp, top);
    top->rst = 0;
    clock_cycle(contextp, top);

    /***************************************************************************
     * One car enters
     **************************************************************************/
    top->outer_sensor = 1;

    clock_cycle(contextp, top);
    check_output(contextp, top, 0);

    top->inner_sensor = 1;

    clock_cycle(contextp, top);
    check_output(contextp, top, 0);

    top->outer_sensor = 0;

    clock_cycle(contextp, top);
    check_output(contextp, top, 0);

    top->inner_sensor = 0;

    clock_cycle(contextp, top);

    // Okay, great, check the current count value
    print_status(contextp, top);
    check_output(contextp, top, 1);
    
    /***************************************************************************
     * Fill up the lot
     **************************************************************************/
    for (int i = 0; i < MAX_CAPACITY-1; i++) {
        top->outer_sensor = 1;

        clock_cycle(contextp, top);
        check_output(contextp, top, i + 1);

        top->inner_sensor = 1;

        clock_cycle(contextp, top);
        check_output(contextp, top, i + 1);

        top->outer_sensor = 0;

        clock_cycle(contextp, top);
        check_output(contextp, top, i + 1);

        top->inner_sensor = 0;

        clock_cycle(contextp, top);

        // Okay, great, check the current count value
        print_status(contextp, top);
        check_output(contextp, top, i + 2);
    }
    
    /***************************************************************************
     * One car leaves
     **************************************************************************/
    top->inner_sensor = 1;

    clock_cycle(contextp, top);
    check_output(contextp, top, MAX_CAPACITY);

    top->outer_sensor = 1;

    clock_cycle(contextp, top);
    check_output(contextp, top, MAX_CAPACITY);

    top->inner_sensor = 0;

    clock_cycle(contextp, top);
    check_output(contextp, top, MAX_CAPACITY);

    top->outer_sensor = 0;

    clock_cycle(contextp, top);

    print_status(contextp, top);
    check_output(contextp, top, MAX_CAPACITY - 1);
    
    /***************************************************************************
     * Empty the lot
     **************************************************************************/

    for (int i = 0; i < MAX_CAPACITY-1; i++) {
        top->inner_sensor = 1;

        clock_cycle(contextp, top);
        check_output(contextp, top, MAX_CAPACITY - 1 - i);

        top->outer_sensor = 1;

        clock_cycle(contextp, top);
        check_output(contextp, top, MAX_CAPACITY - 1 - i);

        top->inner_sensor = 0;

        clock_cycle(contextp, top);
        check_output(contextp, top, MAX_CAPACITY - 1 - i);

        top->outer_sensor = 0;

        clock_cycle(contextp, top);

        print_status(contextp, top);
        check_output(contextp, top, MAX_CAPACITY - 2 - i);
    }
    
    /***************************************************************************
     * Check that we can hold sensors high for more than one cycle on enter
     **************************************************************************/
    top->outer_sensor = 1;

    clock_cycle(contextp, top);
    clock_cycle(contextp, top);
    clock_cycle(contextp, top);
    clock_cycle(contextp, top);
    check_output(contextp, top, 0);

    top->inner_sensor = 1;

    clock_cycle(contextp, top);
    clock_cycle(contextp, top);
    clock_cycle(contextp, top);
    check_output(contextp, top, 0);

    top->outer_sensor = 0;

    clock_cycle(contextp, top);
    clock_cycle(contextp, top);
    check_output(contextp, top, 0);

    top->inner_sensor = 0;

    clock_cycle(contextp, top);
    // Okay, great, check the current count value
    print_status(contextp, top);
    check_output(contextp, top, 1);
    
    /***************************************************************************
     * Check that we can hold sensors high for more than one cycle on exit
     **************************************************************************/
    top->inner_sensor = 1;

    clock_cycle(contextp, top);
    clock_cycle(contextp, top);
    check_output(contextp, top, 1);

    top->outer_sensor = 1;

    clock_cycle(contextp, top);
    clock_cycle(contextp, top);
    clock_cycle(contextp, top);
    clock_cycle(contextp, top);
    clock_cycle(contextp, top);
    check_output(contextp, top, 1);

    top->inner_sensor = 0;

    clock_cycle(contextp, top);
    clock_cycle(contextp, top);
    clock_cycle(contextp, top);
    check_output(contextp, top, 1);

    top->outer_sensor = 0;

    clock_cycle(contextp, top);

    print_status(contextp, top);
    check_output(contextp, top, 0);


    // Fill in more testing as needed
}

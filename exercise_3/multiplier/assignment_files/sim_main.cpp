// For std::unique_ptr
#include <memory>
#include <cstdint>
#include <cstdlib>

// Include common routines
#include <verilated.h>

// Include model header, generated from Verilating "top.v"
#include "Vmultiplier_top.h"

#define CLOCK_HALF_CYCLE_NS 5
#define CLOCK_CYCLE_NS (CLOCK_HALF_CYCLE_NS * 2)
#define CYCLE_TIMEOUT 1024

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
                    const std::unique_ptr<Vmultiplier_top> &top) {
    contextp->timeInc(1);  // 1 timeprecision period passes...
    top->eval();
}

static void half_clock_cycle(const std::unique_ptr<VerilatedContext> &contextp,
                    const std::unique_ptr<Vmultiplier_top> &top) {
    top->clk = !top->clk;
    for (int i = 0; i < CLOCK_HALF_CYCLE_NS; i++) {
        time_step(contextp, top);
    }
}

static void clock_cycle(const std::unique_ptr<VerilatedContext> &contextp,
                    const std::unique_ptr<Vmultiplier_top> &top) {
    half_clock_cycle(contextp, top);
    half_clock_cycle(contextp, top);
}

static void check_output(const std::unique_ptr<VerilatedContext> &contextp,
                        const std::unique_ptr<Vmultiplier_top> &top,
                        uint16_t expected_product) {
    if (top->resp_val == 0) {
        VL_PRINTF("[%" VL_PRI64 "d] resp not valid\n", contextp->time());
    }
    else {
        bool data_wrong = expected_product != top->resp_product;
        if (data_wrong) {
            VL_PRINTF("[%" VL_PRI64 "d] rd data wrong. Expected: %hx, Actual: %hx\n",
                    contextp->time(), expected_product, top->resp_product);
        }
    }
}

static void print_status(const std::unique_ptr<VerilatedContext> &contextp,
                        const std::unique_ptr<Vmultiplier_top> &top) {
    // Read outputs
    VL_PRINTF("[%" VL_PRI64 "d] req_val: %d A * B = %hhx * %hhx \
rd_resp_val: %d, product: %hx\n", 
            contextp->time(), top->req_val, top->req_operand_a, top->req_operand_b,
            top->resp_val, top->resp_product);
}

static void do_multiply(const std::unique_ptr<VerilatedContext> &contextp,
                        const std::unique_ptr<Vmultiplier_top> &top,
                        uint8_t operand_a, uint8_t operand_b, uint64_t timeout_cycles) {
    uint64_t cycle_count = 0;

    top->req_val = 1;
    top->req_operand_a = operand_a;
    top->req_operand_b = operand_b;

    half_clock_cycle (contextp, top);

    cycle_count = 0;
    while (!top->req_rdy) {
        cycle_count++;
        if (cycle_count == timeout_cycles) {
            VL_PRINTF("[%" VL_PRI64 "d] may have timed out waiting for req_rdy \
to go high\n", contextp->time());
        }
        clock_cycle(contextp, top);
    }
    half_clock_cycle(contextp, top);

    top->req_val = 0;
    top->resp_rdy = 1;

    half_clock_cycle(contextp, top);
    cycle_count = 0;
    while (!top->resp_val) {
        cycle_count++;
        if (cycle_count == timeout_cycles) {
            VL_PRINTF("[%" VL_PRI64 "d] may have timed out waiting for resp_val \
to go high\n", contextp->time());
        }
        clock_cycle(contextp, top);
    }
    check_output(contextp, top, operand_a * operand_b);
    half_clock_cycle(contextp, top);
    clock_cycle(contextp, top);
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
    const std::unique_ptr<Vmultiplier_top> top{new Vmultiplier_top{contextp.get(), "TOP"}};


    // Set some initial data values
    top->req_val = 0;
    top->req_operand_a = 0;
    top->req_operand_b = 0;
    
    top->resp_val = 0;
    top->resp_rdy = 1;

    top->clk = 0;
    top->rst = 1;
    
    clock_cycle(contextp, top); // Kick the simulation
    clock_cycle(contextp, top);
    
    print_status(contextp, top);
    top->rst = 0;
    clock_cycle(contextp, top);
    
    /***************************************************************************
     * Try just multiplying by 1
     **************************************************************************/
    printf("Run some basic test cases\n");
    half_clock_cycle(contextp, top);
    do_multiply(contextp, top, 4, 1, CYCLE_TIMEOUT);
    
    /***************************************************************************
     * Try just multiplying by 0
     **************************************************************************/
    do_multiply(contextp, top, 4, 0, CYCLE_TIMEOUT);
    
    /***************************************************************************
     * Try flipping the operands
     **************************************************************************/
    do_multiply(contextp, top, 1, 4, CYCLE_TIMEOUT);
    do_multiply(contextp, top, 0, 4, CYCLE_TIMEOUT);
    
    /***************************************************************************
     * Test all the possible combinations
     **************************************************************************/

    printf("Run exhaustive testing\n");
    for (uint16_t a = 0; a <= (uint16_t)0xff; a++) {
        for (uint16_t b = 0; b <= (uint16_t)0xff; b++) {
            do_multiply(contextp, top, (uint8_t)a, (uint8_t)b, CYCLE_TIMEOUT);
        }
    }
    
    /***************************************************************************
     * Make sure we can change the inputs while the request is in progress
     **************************************************************************/
    printf("Testing changing inputs while a request is in progress\n");

    top->req_val = 1;
    top->req_operand_a = 1;
    top->req_operand_b = 10;
    half_clock_cycle(contextp, top);
    while (!top->req_rdy) {
        clock_cycle(contextp, top);
    }
    half_clock_cycle(contextp, top);

    top->req_val = 0;
    top->resp_rdy = 1;
    clock_cycle(contextp, top);
    top->req_val = 1;
    top->req_operand_a = 0x32;
    top->req_operand_b = 0x16;

    clock_cycle(contextp, top);
    clock_cycle(contextp, top);
    top->req_val = 0;

    half_clock_cycle(contextp, top);
    while (!top->resp_val) {
        clock_cycle(contextp, top);
    }
    check_output(contextp, top, 10 * 1);
    half_clock_cycle(contextp, top);
    clock_cycle(contextp, top);
    
    /***************************************************************************
     * Make sure we can backpressure the input
     **************************************************************************/
    printf("Test backpressuring the input\n"); 

    top->req_val = 1;
    top->req_operand_a = 15;
    top->req_operand_b = 1;
    top->resp_rdy = 0;
    half_clock_cycle(contextp, top);
    
    while (!top->req_rdy) {
        clock_cycle(contextp, top);
    }

    while (!top->resp_val) {
        clock_cycle(contextp, top);
    }
    clock_cycle(contextp, top);
    clock_cycle(contextp, top);
    if (top->req_rdy) {
        printf("Error: engine is ready for a request when it shouldn't be\n");
    }
    half_clock_cycle(contextp, top);

    // Try changing the inputs
    top->req_val = 1;
    top->req_operand_a = 0x54;
    top->req_operand_b = 0x16;
    clock_cycle(contextp, top);
    half_clock_cycle(contextp, top);
    check_output(contextp, top, 15 * 1);
    half_clock_cycle(contextp, top);
    top->req_val = 0;
    top->resp_rdy = 1;
    clock_cycle(contextp, top);
    
    clock_cycle(contextp, top);
    clock_cycle(contextp, top);
    clock_cycle(contextp, top);

    return 0;
}


// For std::unique_ptr
#include <memory>
#include <cstdint>
#include <cstdlib>

// Include common routines
#include <verilated.h>

// Include model header, generated from Verilating "top.v"
#include "Vmem_wr_bypass_top.h"

#define CLOCK_HALF_CYCLE_NS 5
#define CLOCK_CYCLE_NS (CLOCK_HALF_CYCLE_NS * 2)
#define MAX_CAPACITY 8
#define CYCLE_TIMEOUT 8

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
                    const std::unique_ptr<Vmem_wr_bypass_top> &top) {
    contextp->timeInc(1);  // 1 timeprecision period passes...
    top->eval();
}

static void half_clock_cycle(const std::unique_ptr<VerilatedContext> &contextp,
                    const std::unique_ptr<Vmem_wr_bypass_top> &top) {
    top->clk = !top->clk;
    for (int i = 0; i < CLOCK_HALF_CYCLE_NS; i++) {
        time_step(contextp, top);
    }
}

static void clock_cycle(const std::unique_ptr<VerilatedContext> &contextp,
                    const std::unique_ptr<Vmem_wr_bypass_top> &top) {
    half_clock_cycle(contextp, top);
    half_clock_cycle(contextp, top);
}

static void check_output(const std::unique_ptr<VerilatedContext> &contextp,
                        const std::unique_ptr<Vmem_wr_bypass_top> &top,
                        uint8_t expected_rd_data) {
    if (top->rd_resp_val == 0) {
        VL_PRINTF("[%" VL_PRI64 "d] ERROR: rd resp not valid\n", contextp->time());
    }
    else {
        bool data_wrong = expected_rd_data != top->rd_resp_data;
        if (data_wrong) {
            VL_PRINTF("[%" VL_PRI64 "d] ERROR: rd data wrong. Expected: %hhx, \
Actual: %hhx\n",
                    contextp->time(), expected_rd_data, top->rd_resp_data);
        }
    }
}

static void print_status(const std::unique_ptr<VerilatedContext> &contextp,
                        const std::unique_ptr<Vmem_wr_bypass_top> &top) {
    // Read outputs
    VL_PRINTF("[%" VL_PRI64 "d] wr_req_val: %d mem[%x] <- %hhx \
rd_resp_val: %d, rd_resp_data: %hhx\n", 
            contextp->time(), top->wr_req_val, top->wr_req_addr, top->wr_req_data,
            top->rd_resp_val, top->rd_resp_data);
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
    const std::unique_ptr<Vmem_wr_bypass_top> top{
                                        new Vmem_wr_bypass_top{contextp.get(), "TOP"}};

    uint64_t cycle_count;

    std::srand(0);
    uint8_t ref_mem[MAX_CAPACITY];

    for (int i = 0; i < MAX_CAPACITY; i++) {
        ref_mem[i] = (uint8_t)(std::rand() % 256);
    }

    // Set some initial data values
    top->wr_req_val = 0;
    top->wr_req_addr = 0;
    top->wr_req_data = 0;
    
    top->rd_req_val = 0;
    top->rd_req_addr = 0;

    top->rd_resp_rdy = 1;

    top->clk = 0;
    top->rst = 1;
    
    clock_cycle(contextp, top); // Kick the simulation
    clock_cycle(contextp, top);
    
    print_status(contextp, top);
    top->rst = 0;
    clock_cycle(contextp, top);
    
    /***************************************************************************
     * Write some data
     **************************************************************************/
    half_clock_cycle(contextp, top);
    top->wr_req_val = 1;
    top->wr_req_addr = 0;
    top->wr_req_data = 0xab;
    ref_mem[0] = 0xab;
    print_status(contextp, top);

    half_clock_cycle(contextp, top);
    if (!(top->wr_req_rdy)) {
        VL_PRINTF("[%" VL_PRI64 "d] ERROR: wr_req_rdy not high when it should be\n",
                    contextp->time());
    }

    half_clock_cycle(contextp, top);
    top->wr_req_val = 0;
    clock_cycle(contextp, top);

    /***************************************************************************
     * Read some data
     **************************************************************************/
    top->rd_req_val = 1;
    top->rd_req_addr = 0;

    half_clock_cycle(contextp, top);
    if (!(top->rd_req_rdy)) {
        VL_PRINTF("[%" VL_PRI64 "d] ERROR: rd_req_rdy not high when it should be\n",
                    contextp->time());

    }
    half_clock_cycle(contextp, top);

    top->rd_req_val = 0;
    // Tick half a clock cycle, so we can check the output
    half_clock_cycle(contextp, top);
    print_status(contextp, top);
    check_output(contextp, top, ref_mem[0]);
    half_clock_cycle(contextp, top);
    clock_cycle(contextp, top);
    /***************************************************************************
     * Write and then read data from all the possible addresses
     **************************************************************************/

    for (int i = 1; i < MAX_CAPACITY; i++) {

        top->wr_req_val = 1;
        top->wr_req_addr = i;
        top->wr_req_data = ref_mem[i];
        print_status(contextp, top);

        half_clock_cycle(contextp, top);
        if (!(top->wr_req_rdy)) {
            VL_PRINTF("[%" VL_PRI64 "d] ERROR: wr_req_rdy not high when it should be\n",
                    contextp->time());
        }
        half_clock_cycle(contextp, top);
        top->wr_req_val = 0;

        top->rd_req_val = 1;
        top->rd_req_addr = i;

        half_clock_cycle(contextp, top);
        if (!(top->rd_req_rdy)) {
            VL_PRINTF("[%" VL_PRI64 "d] ERROR: rd_req_rdy not high when it should be\n",
                    contextp->time());
        }
        half_clock_cycle(contextp, top);

        top->rd_req_val = 0;
        half_clock_cycle(contextp, top);
        print_status(contextp, top);
        check_output(contextp, top, ref_mem[i]);
        half_clock_cycle(contextp, top);

        clock_cycle(contextp, top);
    }

    clock_cycle(contextp, top);
    clock_cycle(contextp, top);
    
    /***************************************************************************
     * Check that write data bypasses correctly
     **************************************************************************/
    ref_mem[0] = 0x65;

    top->wr_req_val = 1;
    top->wr_req_addr = 0;
    top->wr_req_data = 0x65;

    top->rd_req_val = 1;
    top->rd_req_addr = 0;

    half_clock_cycle(contextp, top);
    if (!(top->wr_req_rdy)) {
        VL_PRINTF("[%" VL_PRI64 "d] ERROR: wr_req_rdy not high when it should be\n",
                    contextp->time());
    }
    if (!(top->rd_req_rdy)) {
        VL_PRINTF("[%" VL_PRI64 "d] ERROR: rd_req_rdy not high when it should be\n",
                    contextp->time());
    }
    half_clock_cycle(contextp, top);

    top->rd_req_val = 0;
    top->wr_req_val = 0;
    half_clock_cycle(contextp, top);
    print_status(contextp, top);
    check_output(contextp, top, ref_mem[0]);
    half_clock_cycle(contextp, top);
    clock_cycle(contextp, top);
    
    /***************************************************************************
     * Check that you can backpressure rd resp
     **************************************************************************/
    // first check that if there is no valid response, we can set resp_rdy to 
    // low, but req_rdy is still high
    top->rd_resp_rdy = 0;
    clock_cycle(contextp, top);
    
    // then check that if there is a valid response and resp_rdy is low, then
    // req_rdy is also low
    top->rd_req_val = 1;
    top->rd_req_addr = 6;
    half_clock_cycle(contextp, top);
    if (top->rd_req_rdy != 1) {
        VL_PRINTF("[%" VL_PRI64 "d] ERROR: rd req not ready\n", contextp->time());
    }
    half_clock_cycle(contextp, top);

    half_clock_cycle(contextp, top);
    print_status(contextp, top);
    if (top->rd_req_rdy == 1) {
        VL_PRINTF("[%" VL_PRI64 "d] ERROR: rd req ready, but shouldn't be\n",
                contextp->time());
    }
    check_output(contextp, top, ref_mem[6]);


    clock_cycle(contextp, top);
    clock_cycle(contextp, top);
    clock_cycle(contextp, top);

    return 0;
}

`timescale 1ns/1ns
package lot_counter_pkg;

    typedef enum logic[2:0] {
        READY = 3'd0,
        // TODO: Fill in the rest of the states you need
    } state_e;

    localparam MAX_CAPACITY = 16;
    localparam COUNTER_W = $clog2(MAX_CAPACITY + 1);

endpackage

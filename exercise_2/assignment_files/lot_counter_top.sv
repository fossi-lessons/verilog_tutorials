`timescale 1ns/1ns
import lot_counter_pkg::*;
module lot_counter_top (
     input  clk
    ,input  rst

    ,input  outer_sensor
    ,input  inner_sensor

    ,output logic                   full
    ,output logic                   empty
    ,output logic   [COUNTER_W-1:0] count
);

    logic   [COUNTER_W-1:0] curr_counter;
    state_e                 curr_state;

    logic   [COUNTER_W-1:0] counter_next;
    state_e                 state_next;

    lot_counter_state_regs state_regs (
         .clk   (clk    )
        ,.rst   (rst    )

        ,.state_next    (state_next     )
        ,.counter_next  (counter_next   )

        ,.curr_counter  (curr_counter   )
        ,.curr_state    (curr_state     )
    );

    // TODO: instantiate the state logic module here
    
    initial begin
       if ($test$plusargs("trace") != 0) begin
          $display("[%0t] Tracing to logs/vlt_dump.vcd...\n", $time);
          $dumpfile("logs/vlt_dump.vcd");
          $dumpvars();
       end
       $display("[%0t] Model running...\n", $time);
    end
endmodule

`timescale 1ns/1ns
module mux_sim_top #(
     parameter DATA_W = 8
    ,parameter NUM_INPUTS = 4
    ,parameter SEL_W = $clog2(NUM_INPUTS)
)(
     input  [DATA_W-1:0]    data_0
    ,input  [DATA_W-1:0]    data_1
    ,input  [DATA_W-1:0]    data_2
    ,input  [DATA_W-1:0]    data_3
     
    ,input  [SEL_W-1:0]     data_sel

    ,output [DATA_W-1:0]    data_out

);

    mux_4 #(
         .DATA_W    (DATA_W )
    ) DUT (
         .data_0    (data_0     )
        ,.data_1    (data_1     )
        ,.data_2    (data_2     )
        ,.data_3    (data_3     )
                                
        ,.data_sel  (data_sel   )
                                
        ,.data_out  (data_out   )
    );
    // Print some stuff as an example
   initial begin
      if ($test$plusargs("trace") != 0) begin
         $display("[%0t] Tracing to logs/vlt_dump.vcd...\n", $time);
         $dumpfile("logs/vlt_dump.vcd");
         $dumpvars();
      end
      $display("[%0t] Model running...\n", $time);
   end

endmodule

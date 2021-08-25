`timescale 1ns/1ns
module mem_wr_bypass_top #(
     parameter DATA_W = 8
    ,parameter NUM_ELS = 8
    ,parameter ADDR_W = $clog2(NUM_ELS)
)(
     input clk
    ,input rst

    ,input  logic                   wr_req_val
    ,input  logic   [ADDR_W-1:0]    wr_req_addr
    ,input  logic   [DATA_W-1:0]    wr_req_data
    ,output logic                   wr_req_rdy

    ,input  logic                   rd_req_val
    ,input  logic   [ADDR_W-1:0]    rd_req_addr
    ,output logic                   rd_req_rdy

    ,output logic                   rd_resp_val
    ,output logic   [DATA_W-1:0]    rd_resp_data
    ,input  logic                   rd_resp_rdy
);

    mem_1r1w_sync_wr_bypass #(
         .DATA_W    (DATA_W )
        ,.NUM_ELS   (NUM_ELS)
        ,.ADDR_W    (ADDR_W )
    ) mem (
         .clk   (clk    )
        ,.rst   (rst    )
    
        ,.wr_req_val    (wr_req_val     )
        ,.wr_req_addr   (wr_req_addr    )
        ,.wr_req_data   (wr_req_data    )
        ,.wr_req_rdy    (wr_req_rdy     )
                                        
        ,.rd_req_val    (rd_req_val     )
        ,.rd_req_addr   (rd_req_addr    )
        ,.rd_req_rdy    (rd_req_rdy     )
                                        
        ,.rd_resp_val   (rd_resp_val    )
        ,.rd_resp_data  (rd_resp_data   )
        ,.rd_resp_rdy   (rd_resp_rdy    )
    );
   
    initial begin
        if ($test$plusargs("trace") != 0) begin
            $display("[%0t] Tracing to logs/vlt_dump.vcd...\n", $time);
            $dumpfile("logs/vlt_dump.vcd");
            $dumpvars();
        end
        $display("[%0t] Model running...\n", $time);
    end

endmodule

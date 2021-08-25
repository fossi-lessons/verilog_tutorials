module mem_1r1w_sync_wr_bypass #(
     parameter DATA_W = -1
    ,parameter NUM_ELS = -1
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


    // TODO: fill in bypassing logic and connect the basic mem
    mem_1r1w_sync #(
         .DATA_W    (DATA_W )
        ,.NUM_ELS   (NUM_ELS)
    ) inner_mem (
         .clk   (clk    )
        ,.rst   (rst    )
    
        ,.wr_val    ()
        ,.wr_addr   ()
        ,.wr_data   ()
    
        ,.rd_val    ()
        ,.rd_addr   ()
        ,.rd_data   ()
    );

endmodule

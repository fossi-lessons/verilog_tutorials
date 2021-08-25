//
// A synchronous memory with 1 read port and 1 write port
// 
module mem_1r1w_sync #(
     parameter DATA_W = -1
    ,parameter NUM_ELS = -1
    ,parameter ADDR_W = $clog2(NUM_ELS)
)(
     input clk
    ,input rst

    ,input  logic                   wr_val
    ,input  logic   [ADDR_W-1:0]    wr_addr
    ,input  logic   [DATA_W-1:0]    wr_data

    ,input  logic                   rd_val
    ,input  logic   [ADDR_W-1:0]    rd_addr
    ,output logic   [DATA_W-1:0]    rd_data
);

    logic [DATA_W-1:0]  mem [NUM_ELS-1:0];

    logic [ADDR_W-1:0] rd_addr_reg;

    //assign wr_rdy = 1'b1;
    //assign rd_rdy = 1'b1;
    
    assign rd_data = mem[rd_addr_reg];

    always_ff @(posedge clk) begin
        if (rd_val) begin
            rd_addr_reg <= rd_addr;
        end
    end

    always_ff @(posedge clk) begin
        if (wr_val) begin
            mem[wr_addr] <= wr_data;
        end
    end

    always_ff @(negedge clk) begin
        assert (rst || !(wr_val && rd_val && (wr_addr == rd_addr))) begin
            $error("Reading and writing from the same address %x\n", rd_addr);
        end
    end

endmodule

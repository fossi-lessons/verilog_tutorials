module multiplier_data #(
     parameter OPERAND_W = -1
    ,parameter PRODUCT_W = -1
)(
     input clk
    ,input rst
    
    ,input  logic   [OPERAND_W-1:0] req_operand_a
    ,input  logic   [OPERAND_W-1:0] req_operand_b

    ,output logic   [PRODUCT_W-1:0] resp_product
    
    // TODO: add in signals to/from control
);

    // TODO: fill in datapath logic
endmodule

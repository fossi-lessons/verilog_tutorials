module multiplier_top #(
     parameter OPERAND_W = 8
    ,parameter PRODUCT_W = 2 * OPERAND_W
)(
     input clk
    ,input rst

    ,input                  req_val
    ,input  [OPERAND_W-1:0] req_operand_a
    ,input  [OPERAND_W-1:0] req_operand_b
    ,output                 req_rdy

    ,output                 resp_val
    ,output [PRODUCT_W-1:0] resp_product
    ,input                  resp_rdy
);

    multiplier #(
         .OPERAND_W (OPERAND_W)
    ) test_multiplier (
         .clk   (clk    )
        ,.rst   (rst    )
    
        ,.req_val       (req_val        )
        ,.req_operand_a (req_operand_a  )
        ,.req_operand_b (req_operand_b  )
        ,.req_rdy       (req_rdy        )
                                        
        ,.resp_val      (resp_val       )
        ,.resp_product  (resp_product   )
        ,.resp_rdy      (resp_rdy       )
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

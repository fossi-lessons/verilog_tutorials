module multiplier #(
     parameter OPERAND_W = -1
    ,parameter PRODUCT_W = (2 * OPERAND_W) 
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

    multiplier_data #(
         .OPERAND_W (OPERAND_W  )
        ,.PRODUCT_W (PRODUCT_W  )
    ) data (
         .clk   (clk    )
        ,.rst   (rst    )
        
        ,.req_operand_a             (req_operand_a          )
        ,.req_operand_b             (req_operand_b          )
                                                            
        ,.resp_product              (resp_product           )
        
        /* TODO: add additional control signals here */
                                                            
    ); 

    multiplier_ctrl ctrl (
         .clk   (clk    )
        ,.rst   (rst    )
    
        ,.req_val                   (req_val                )
        ,.req_rdy                   (req_rdy                )
                                                            
        ,.resp_val                  (resp_val               )
        ,.resp_rdy                  (resp_rdy               )
                                                            
        ,.ctrl_data_store_inputs    (ctrl_data_store_inputs )
        ,.ctrl_data_iterate         (ctrl_data_iterate      )
        ,.data_ctrl_done            (data_ctrl_done         )
        
        /* TODO: add additional control signals here */
    );
endmodule

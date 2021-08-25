module mux_4 #(
     parameter DATA_W = 8
    ,parameter SEL_W = 2
)(
     input  [DATA_W-1:0]    data_0
    ,input  [DATA_W-1:0]    data_1
    ,input  [DATA_W-1:0]    data_2
    ,input  [DATA_W-1:0]    data_3

    ,input  [SEL_W-1:0]     data_sel

    ,output [DATA_W-1:0]    data_out
);

    // TODO: fill in your logic and module instantiations here

endmodule

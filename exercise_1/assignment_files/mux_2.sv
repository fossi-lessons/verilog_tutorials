module mux_2 #(
    parameter DATA_W = 8
)(
     input  [DATA_W-1:0]    data_0
    ,input  [DATA_W-1:0]    data_1

    ,input                  data_sel

    ,output [DATA_W-1:0]    data_out
);

    assign data_out = data_sel
                    ? data_1
                    : data_0;
endmodule

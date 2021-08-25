module multiplier_ctrl (
     input clk
    ,input rst

    ,input  logic   req_val
    ,output logic   req_rdy

    ,output logic   resp_val
    ,input  logic   resp_rdy

    // TODO: add in signals to/from datapath
);

    typedef enum logic[1:0] {
        // TODO: fill in states
    } state_e;

    state_e state_reg;
    state_e state_next;

    always_ff @(posedge clk) begin
        if (rst) begin
            state_reg <= READY;
        end
        else begin
            state_reg <= state_next;
        end
    end

    // TODO: fill in state machine control logic
    always_comb begin
        case (state_reg)
        endcase
    end
endmodule

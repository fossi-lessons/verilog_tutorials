// Do not modify this
import lot_counter_pkg::*;
module lot_counter_state_regs (
     input  clk
    ,input  rst

    ,input  state_e         state_next
    ,input  [COUNTER_W-1:0] counter_next

    ,output [COUNTER_W-1:0] curr_counter
    ,output state_e         curr_state
);

    state_e state_reg;

    logic   [COUNTER_W-1:0] counter_reg;

    assign curr_counter = counter_reg;
    assign curr_state = state_reg;

    always_ff @(posedge clk) begin
        if (rst) begin
            state_reg <= READY;
            counter_reg <= '0;
        end
        else begin
            state_reg <= state_next;
            counter_reg <= counter_next;
        end
    end

endmodule

// TODO: import the package here
module lot_counter_state_logic (
     input                          outer_sensor
    ,input                          inner_sensor
    
    ,output logic                   full
    ,output logic                   empty
    ,output logic   [COUNTER_W-1:0] count
    
    ,output         state_e         state_next
    ,output logic   [COUNTER_W-1:0] counter_next

    ,input          state_e         curr_state
    ,input          [COUNTER_W-1:0] curr_counter
);
    // TODO: implement all this logic
endmodule

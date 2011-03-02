
module wait_gen (
    output clk,
    output resetN,
    output ready);

reg res;
reg c;
reg rdy;

assign resetN = res;
assign clk = c;
assign ready = rdy;

initial
begin
    res = 1'b0;
    c = 1'b0;
    rdy = 1'b1;
    #150 res = 1'b1;
end

always
begin
    #50 c = ~c;
end

endmodule


module ff_7474 (
    input  clk ,
    input  d,
    input  clrN,
    input  prN,
    output q,
    output qN);
    
reg q;

assign qN = ~q;

always @(posedge clk or negedge clrN or negedge prN)
begin
    if (~clrN) begin
        q <= 1'b0;
    end else if (~prN) begin
        q <= 1'b1;
    end else begin
        q <= d;
    end
end
endmodule

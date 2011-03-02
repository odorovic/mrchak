
module latch_74373 (
    input [7:0] d,
    output [7:0] q,
    input oeN, le);
    
reg [7:0] data;
reg [7:0] q_buf;

assign q = q_buf;

initial
begin
    data = 8'h00;
end

always @(le, d, oeN)
begin
    if(le == 1) data = d;
    if(oeN == 1'b0) begin
        q_buf = data;
    end else begin
        q_buf = 8'hZZ;
    end
end
endmodule

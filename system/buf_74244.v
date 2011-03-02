
module buffer_74244 (
    input           oeN,
    input  [7:0]    a,
    output [7:0]    y);

reg [7:0] read_y;

assign y = read_y;

always @ (oeN, a) begin
    if (1'b0 == oeN) begin
        read_y <= a;
    end else begin
        read_y <= 8'hzz;
    end
end
endmodule

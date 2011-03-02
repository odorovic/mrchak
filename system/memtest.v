
`include "simplemem.v"

module memtest();

reg         csN;
reg         oeN;
reg         wrN;
reg  [9:0]  addr;
wire [7:0]  data;

simplemem U1 (
    csN,
    oeN,
    wrN,
    addr,
    data);

initial
begin
    $monitor($time, ": cs = %b, oe = %b, addr = %x, data = %x", csN, oeN, addr, data);
    csN = 1'b1;
    oeN = 1'b1;
    wrN = 1'b1;
    #10 addr = 10'h0;
    oeN = 1'b0;
    csN = 1'b0;
    #5 csN = 1'b1;
    oeN = 1'b1;
    $finish();
end

endmodule

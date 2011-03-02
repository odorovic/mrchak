
`include "wait_gen.v"

module waittest ();

wire clk;
wire reset;
wire ready;

wait_gen U1 (
    .clk    (clk),
    .resetN (reset),
    .ready  (ready));

initial
begin
    $dumpfile("logs/wait.vcd");
    $dumpvars(0, U1);
    #1000 $finish;

end
endmodule

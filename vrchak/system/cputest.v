`include "cpu8086.v"

module cputest();
reg         clk;
reg         reset;
reg         ready;
reg         intr;
reg         nmi;
wire        mn_mxN;
wire        hlda;
wire        intaN;
wire        ale;
wire        m_ioN;
wire        dt_rN;
wire        bheN;
wire        denN;
wire        rdN;
wire        wrN;
wire [3:0]  asbus;
wire [15:0] adbus;

cpu8086 U1 (
    clk,
    reset,
    ready,
    intr,
    nmi,
    mn_mxN,
    hlda,
    intaN,
    ale,
    m_ioN,
    dt_rN,
    bheN,
    denN,
    rdN,
    wrN,
    asbus,
    adbus);

initial 
begin
    $display("Pocinjemo simulaciju");
    clk = 0;
    #5  reset <= 1'b1;
    #20 reset <= 1'b0;
    #100 $finish();
end

always @ clk
begin
    #5 clk <= ~clk;
end


endmodule

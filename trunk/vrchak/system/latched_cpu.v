
`include "cpu8086.v"
`include "latch_74373.v"
`include "trans_74245.v"
`include "buf_74244.v"

module latched_cpu (
    input             clk,
    input             resetN,
    input             ready,
    input             intr,
    output            m_ioN,
    output            rdN,
    output            wrN,
    output            intaN,
    output            bheN,
    output [3:0]      a19to16,
    output [7:0]      a15to8,
    output [6:0]      a7to1,
    output            a0,
    output [7:0]      d15to8,
    output [7:0]      d7to0);

wire        m_ioN_ub;
wire        bheN_ub;
wire        rdN_ub;
wire        wrN_ub;
wire [3:0]  asbus;
tri  [15:0] adbus;

cpu8086 CPU(
    .clk    (clk),
    .reset  (resetN),
    .ready  (ready),
    .intr   (intr),
    .nmi    (1'b0),
    .mn_mxN (1'b1),
    .hlda   (1'b0),

    .intaN  (intaN),
    .ale    (ale),
    .m_ioN  (m_ioN_ub),
    .dt_rN  (dt_rN),
    .bheN   (bheN_ub),
    .denN   (denN),
    .rdN    (rdN_ub),
    .wrN    (wrN_ub),
    .asbus  (asbus),
    .adbus  (adbus));

wire [7:0] buf1_stub;

assign m_ioN = buf1_stub[7];
assign rdN = buf1_stub[6];
assign wrN = buf1_stub[5];

buffer_74244 BUF1(
    .oeN    (1'b0),
    .a      ({m_ioN_ub, rdN_ub, wrN_ub, 5'h00}),
    .y      (buf1_stub));
 
wire [7:0] latch1_stub;

assign bheN = latch1_stub[7];
assign a19to16 = latch1_stub[6:3];

latch_74373 LATCH1 (
    .le     (ale),
    .oeN    (1'b0),
    .d      ({bheN_ub ,asbus , 3'h0}),
    .q      (latch1_stub));

reg [15:0] lin2;
always @ adbus
begin
    lin2 <= adbus;
end

latch_74373 LATCH2 (
    .le     (ale),
    .oeN    (1'b0),
    .d      (adbus[15:8]),
    .q      (a15to8));

wire [7:0] addrlo;

assign a7to1 = addrlo[7:1];
assign a0 = addrlo[0];

latch_74373 LATCH3 (
    .le     (ale),
    .oeN    (1'b0),
    .d      (adbus[7:0]),
    .q      (addrlo));

trans_74245 TRANS1 (
    .g      (denN),
    .dir    (dt_rN),
    .a      (adbus[15:8]),
    .b      (d15to8));

trans_74245 TRANS2 (
    .g      (denN),
    .dir    (dt_rN),
    .a      (adbus[7:0]),
    .b      (d7to0));


endmodule

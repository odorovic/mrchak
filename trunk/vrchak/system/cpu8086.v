

module cpu8086 (
    input         clk,
    input         reset,
    input         ready,
    input         intr,
    input         nmi,
    input         mn_mxN,
    input         hlda,

    output        intaN,
    output        ale,
    output        m_ioN,
    output        dt_rN,
    output        bheN,
    output        denN,
    output        rdN,
    output        wrN,
    output [3:0]  asbus,

    inout  [15:0] adbus);


tri  [15:0] i_adbus;
wire [3:0]  i_asbus;
wire        i_bheN;
wire        i_m_ioN;
wire        i_wrN;
wire        i_rdN;

reg [15:0] adbuf;
reg [3:0]  asbuf;
reg        bheNbuf;
reg        m_ioNbuf;
reg        wrNbuf;
reg        rdNbuf;

always @ (i_adbus, denN, dt_rN)
begin
    if (denN|dt_rN) 
        adbuf <= i_adbus;
    else
        adbuf <= 16'hzzzz;
end

always @ (i_asbus) asbuf <= i_asbus;
always @ (i_bheN)  bheNbuf <= i_bheN;
always @ (i_m_ioN) m_ioNbuf <= i_m_ioN;
always @ (i_rdN) rdNbuf <= i_rdN;
always @ (i_wrN) wrNbuf <= i_wrN;

assign bheN  = bheNbuf;
assign adbus = adbuf;
assign asbus = asbuf;
assign m_ioN = m_ioNbuf;
assign rdN   = rdNbuf;
assign wrN   = wrNbuf;

initial
begin
    $cpu8086(
        clk,
        reset,
        ready,
        intr,
        nmi,
        mn_mxN,
        adbus,
        hlda,
        intaN,
        ale,
        i_m_ioN,
        dt_rN,
        i_bheN,
        denN,
        i_rdN,
        i_wrN,
        i_asbus,
        i_adbus);
    asbuf <= 4'b0000;
end
endmodule


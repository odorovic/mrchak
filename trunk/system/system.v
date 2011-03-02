`timescale 1ns/1ns

`include "latched_cpu.v"
`include "ff_7474.v"
`include "ram_62256.v"
`include "rom_27128.v"
`include "addr_dec.v"
`include "io_dec.v"
`include "wait_gen.v"
`include "charprinter.v"

module basic_system(
    input irq);

wire        ready;
wire        m_ioN;
wire        rdN;
wire        wrN;
wire        intaN;
wire        bheN;
wire [3:0]  a19to16;
wire [7:0]  a15to8;
wire [6:0]  a7to1;
wire        a0;
tri  [7:0]  d15to8;
tri  [7:0]  d7to0;
wire        ramselN;
wire        epromselN;
wire        ioselN;
wire        wrhN;
wire        wrlN;

latched_cpu LCPU (
    clk,
    resetN,
    ready,
    intr,
    m_ioN,
    rdN,
    wrN,
    intaN,
    bheN,
    a19to16,
    a15to8,
    a7to1,
    a0,
    d15to8,
    d7to0);

addr_dec DEC (
    .m_ioN     (m_ioN),
    .a19to16   (a19to16),
    .bheN      (bheN),
    .a0        (a0),
    .wrN       (wrN),
    .ramselN   (ramselN),
    .epromselN (epromselN),
    .wrhN       (wrhN),
    .wrlN       (wrlN));

io_dec IODEC (
    .m_ioN (m_ioN),
    .a     (a7to1),
    .a0    (a0),
    .selN  (ioselN));

charprinter CP (
    .csN    (ioselN),
    .wrN    (wrN),
    .data   (d7to0));

wait_gen CLKGEN (
    .clk       (clk),
    .resetN    (resetN),
    .ready     (ready));

defparam ROMLO.fname = "ROMLO";
rom_27128 ROMLO (
    epromselN,
    rdN,
    {a15to8, a7to1},
    d7to0);

defparam ROMHI.fname = "ROMHI";
rom_27128 ROMHI (
    epromselN,
    rdN,
    {a15to8, a7to1},
    d15to8);

ram_62256 RAMLOW (
    .csN    (ramselN),
    .oeN    (rdN),
    .wrN    (wrlN),
    .addr   ({a15to8, a7to1}),
    .data   (d7to0));

ram_62256 RAMHI (
    .csN    (ramselN),
    .oeN    (rdN),
    .wrN    (wrhN),
    .addr   ({a15to8, a7to1}),
    .data    (d15to8));

buffer_74244 INTADDRBUF (
    .oeN    (intaN),
    .a      (8'h10),
    .y      (d7to0));

ff_7474 INTTRIG (
     .clk   (irq),
     .d     (1'b1),
     .clrN  (intaN),
     .prN   (1'b1),
     .q     (intr),
     .qN    ());

endmodule

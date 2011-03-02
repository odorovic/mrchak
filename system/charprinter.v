
module charprinter (
    input        csN,
    input        wrN,
    input [7:0]  data);

initial
    $charprinter(csN, wrN, data);

endmodule


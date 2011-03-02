
module io_dec (
    input        m_ioN,
    input [6:0]  a,
    input        a0,
    output       selN);


reg selN;

always @ (a, a0, m_ioN)
begin
    if (a == 7'h38 && m_ioN == 1'b0  && a0 == 1'b0)
        selN <= 1'b0;
    else
        selN <= 1'b1;
end

endmodule

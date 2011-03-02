

module addr_dec (
    input        m_ioN,
    input [3:0]  a19to16,
    input        bheN,
    input        a0,
    input        wrN,
    output       ramselN,
    output       epromselN,
    output       wrhN,
    output       wrlN);

reg epromselN;
reg ramselN;
reg wrlN;
reg wrhN;

always @ (a19to16, a0, bheN, m_ioN, wrN)
begin
    if (a19to16 == 4'hf && m_ioN == 1'b1)
        epromselN <= 1'b0;
    else
        epromselN <= 1'b1;
    if (a19to16 == 4'h0 && m_ioN == 1'b1)
        ramselN <= 1'b0;
    else
        ramselN <= 1'b1;
    if(wrN == 1'b0)
    begin
        if(a0 == 1'b0)
            wrlN <= 1'b0;
        else
            wrlN <= 1'b1;
        if(bheN == 1'b0)
            wrhN <= 1'b0;
        else
            wrhN <= 1'b1;
    end
    else
    begin
        wrlN <= 1'b1;
        wrhN <= 1'b1;
    end
end

endmodule

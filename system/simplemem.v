
module simplemem (
        input        csN,
        input        oeN,
        input        wrN,
        input [9:0]  addr,
        inout [7:0]  data);

parameter fname = "mem.img";

reg [7:0] membyte [0:1023];
reg [7:0] rd_data;

assign data = (csN===1'b0 && oeN===1'b0) ? rd_data : 8'bz;

initial
    $readmemb(fname, membyte);

always @ (csN, wrN, addr)
begin
    if (csN === 1'b0) begin
        rd_data <= membyte[addr];
        if(wrN == 1'b0) begin
            membyte[addr] <= data;
        end
    end 
end
endmodule


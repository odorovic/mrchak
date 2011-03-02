
module ram_62256 (
    input        csN,
    input        oeN,
    input        wrN,
    input [14:0]  addr,
    inout [7:0]  data);


parameter MEM_SIZE = 32768;

reg [7:0] membyte [0:MEM_SIZE-1];
reg [7:0] rd_data;

integer k;

assign data = rd_data;

initial
    for (k = 0; k < MEM_SIZE ; k = k + 1)
    begin
        membyte[k] = 8'h00;
    end 

always @ (csN, oeN, addr)
begin
    if (csN == 1'b0 && oeN == 1'b0) begin
        rd_data <= membyte[addr];
    end else begin
        rd_data <= 8'hzz;
    end
end

always @ (posedge wrN)
begin
    if(csN == 1'b0) begin
        membyte[addr] <= data;
    end
end
endmodule


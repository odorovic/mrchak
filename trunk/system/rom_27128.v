
module rom_27128 (
        input         csN,
        input         oeN,
        input [14:0]  addr,
        inout [7:0]   data);

parameter fname = "mem.img";

reg [7:0] membyte [0:32767];
reg [7:0] rd_data;

reg [0:1024] fmt_string; 
reg [0:1024] image_fname; 

assign data = rd_data;

initial
begin
    fmt_string = {fname, "=%s"};
    if(! $value$plusargs(fmt_string, image_fname)) begin
        $display("Please specify image file for %s", fname);
        $finish;
    end
    $readmemh(image_fname, membyte);
end

always @ (csN, addr, oeN)
begin
    if (csN === 1'b0 && oeN == 1'b0) begin
        rd_data <= membyte[addr];
    end else begin
        rd_data <= 8'hzz;
    end
end
endmodule




module memiface (
        input         clk,

        input         reset,
        input         ready,

        output        ale,
        output        m_ioN,
        output        dt_rN,
        output        bheN
        output        denN
        output        rdN,
        output        wrN,

        inout  [15:0] adbus,
        output [3:0]  asbus,
        output [1:0]  aluop,
    
        //inside interface
        input         start,
        input  [15:0] addr_off,
        input  [3:0]  addr_seg,
        inout  [15:0] data,
        input         bytesize,
        input         interanl_mio,
        input         r,
        input         w,
        output        a);

always @ (negedge clk)
begin
    if (state == `IDLE and start == 1'b1) begin
        ale   <= 1'b1;
    end else if (state == `T1) begin
        ale   <= 1'b0;
    end
end

always @ (posedge clk)
begin
    case(state)
        `T1: begin
            adbus <= addr_off;
            asbus <= addr_seg;
            bheN  <= bytesize;
            m_ioN <= interanl_mio;
            state <= `T2;
        end
        `T2: begin
            adbus <= 16'bz;
            asbus <= 4'bz;
            rdN   <= `RW_ACTIVE;
            state <= `T3;

        end
        `T3: begin
            if(ready = 1'b1) begin
                state <= `T4;
            end else begin
                state <= `TWAIT
            end
        end
        `TWAIT: begin
            if(ready = 1'b1) begin
                state <= `T4;
            end else begin
                state <= `TWAIT
            end
        end
        `T4: begin
            data  <= adbus;
            state <= `FC
        end

    endcase
end


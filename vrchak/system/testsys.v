
`include "system.v"

module testsys ();

reg     irq;

basic_system U1 (irq);



initial
begin
    $dumpfile("logs/sys.vcd");
    $dumpvars(5, testsys);
    irq = 1'b0;
    #200000 irq = 1'b1;
    #400000 $finish;

end
endmodule


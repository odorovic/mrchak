
"""
demo for interactive simulation
"""
 
from myhdl import *
from myhdl_shell import debugger
import string
 
clk = Signal(bool(0))
count = Signal(intbv(0)[8:])
out = Signal(bool(0))
 
 
@instance
def clockgenerator():
    """
    generate a clock signal at the clk output
    """
    while(1):
        clk.next = not clk
        yield delay(1)
 
 
@always(clk)
def counter():
    """
    count the transitions at the clk input
    """
    count.next = (count + 1)%256
    if count == 0:
        out.next = not out.next
 
 
dbg = debugger(**locals())
 
Simulation(instances()).run()


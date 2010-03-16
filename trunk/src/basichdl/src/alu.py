from myhdl import Signal, delay, always, now, Simulation
from myhdl import enum
from myhdl import always_comb
from myhdl import instance
from myhdl import intbv

ADD,SUB, AND, OR, NOT = range(5)

default_ops = {ADD:lambda x,y: x+y,
               SUB:lambda x,y: x-y,
               AND:lambda x,y: x&y,
               OR:lambda x,y: x|y,
               NOT:lambda x,y: ~x}

def alu(oppcode, inA, inB, out, opp_bindings = default_ops):
    '''Jednostavna kombinatorna alu implementacija.
    
    I/O pinovi:
    ---------------------------------
    oppcode - kod operacije
    inA - Prvi operand. Jedini u slucaju unarnih operacija
    inB - Drugi operand. Ignorishe se za unarne operacije.
    out - Izlazni signal.
    
    Ponasanje:
    ---------------------------------
    Bihevioralna simulacija Alu jedinice.
    Na svaku promenu ulaznih signala, evaluira se funkcija 
    opps[oppcode](inA, inB). Ukoliko nije prosledjen
    '''
    
    @always_comb
    def onChange():
        for key, val in opp_bindings.items():
            print key, oppcode
            if key == oppcode:
                out.next = val(inA,inB)
        
    return onChange
            
def MP2(in0,in1,sel,out):
    assert (type(in0.val) == intbv and 
            type(in1.val) == intbv and 
            type(out.val) == intbv and
            type(sel.val) == inbv)
    assert len(in0.val) == len(in1.val) == len(out.val)

    @always_comb
    def onChange():
        if not sel.val:
            out.next = in0.val
        else:
            out.next = in1.val
    return onChange

def testBench():
    A = Signal(intbv(2)[8:0])
    B = Signal(intbv(1)[8:0])
    out = Signal(intbv(0)[8:0])
    opcode = Signal(intbv(0, 0, len(opp_bindings)))
    
    module = alu(opcode,A,B,out)
    
    @instance
    def test():
        for opp in range(len(opp_bindings)):
            opcode.next = intbv(opp)
            yield delay(10)
            print "A: %s, B: %s, opcode: %s, out: %s" % (
                hex(A), hex(B), hex(opcode), hex(out))
    return (test, module)


    

def ctrlUnit(ctrl, stsys,store, op1, op2, jsr, jump, rti, rts,
             not_halt, run, not_prekid,
             regdir, immreg, L1,L2, not_cond, grADRCOD, clk):
    '''Kontrolna jedinica sa brojacem koraka i dekoderom.
    
    Hdl modul sa kontrolnom jedinicom iz primer projekta sa sajta.
    '''
    #Unutrasnji signali:
    kmop2mp = Signal(intbv(0)[0:8])
    kmbranch2mp = Signal(intbv(0)[0:8])
    mp2cnt = Signal(intbv(0)[0:8])
    cnt2dc = Signal(intbv(0)[0:8])
    dcout = Signal(intbv(0)[0:256])
    cnt_ctrl2cnt_ld = Signal(bool(0))
    cnt_ctrl2cnt_inc = Signal(bool(0))
    brOPR = Signal(bool(0))
    branch = Signal(bool(0))
    val00 = Signal(bool(0))
    val08 = Signal(bool(0))
    val09 = Signal(bool(0))
    val0A = Signal(bool(0))
    val14 = Signal(bool(0))
    val1B = Signal(bool(0))
    val29 = Signal(bool(0))
    val3A = Signal(bool(0))
    val3B = Signal(bool(0))
    #komponente
    kmop = components.kmop(ctrl,stsys,store,op1,op2,jsr,jump,rti,rts,kmop2mp)
    kmbranch = components.kmbranch(val00,val08,val09,val0A,val14,
                                   val1B,val29,val3A,val3B, kmbranch2mp)
    mp = components.mp8_1(kmop2mp,kmbranch2mp,brOPR,mp2cnt)
    cnt = components.reg8(mp2cnt,cnt_ctrl2cnt_ld,cnt_ctrl2cnt_inc, cnt2dc, clk )
    dc = components.dc8(cnt2dc, dcout)
    op_signal_gen = components.op_signal_gen(dcout,)
    ctrl_signal_gen = components.ctrl_signal_gen(dcout, not_prekid, 
                                                 val00,val08,val09,val0A,val14,
                                                 val1B,val29,val3A,val3B )
    cnt_ctrl = components.cnt_ctrl()
    return instances()
'''
Created on Apr 5, 2009

@author: odor
'''

from myhdl import instance, delay, always, Signal, intbv

def EdgeDetector(sig, event):
    
    @always(sig.posedge)
    def detect():
        event()
    return detect

def ClkDriver(clk, period=10):
    lowTime = int(period/2)
    highTime = period - lowTime
    @instance
    def driveClk():
        while True:
            yield delay(lowTime)
            clk.next = 1
            yield delay(highTime)
            clk.next = 0
    return driveClk

def dva_reg(in1, in2, ld, out1, out2, clk):
    A = reg8('A', in1, ld, out1, clk)
    B = reg8('A', in2, ld, out2, clk)
    return (A, B)

def reg8(name, input, ld, out, clk):
    '''8-bitni registar.'''
    memory = {0 : intbv(0)[8:0],
              'buffer' : None}
    wrdelay = Signal(bool(0))
    inputbuffer = intbv(0)[8:0]
    
    @always(clk.posedge, wrdelay.posedge)
    def eval_reg():
        '''Generator vrsi sinhroni upis u registar.
        
        Na pozitivnu ivicu takta trenutna vrednost ulaza se pamti u inputbuffer
        i postavlja signal wrdelay.next na pozitivnu vrednost. U sledecem trenu 
        vremena, na pozitivnu ivicu wrdelay zaista pamtimo inputbuffer u memoriju.
        
        Razlog za ovo je sto zelimo da se pojavljivanje vrednosti u memoriji javi
        posle pozitivne ivice takta, a u isto vreme zelimo da struktura podataka za
        memoriju ne bude signal.
        '''
        if wrdelay.val:
            print 'izvrsavamo upis'
            wrdelay.next = bool(0)
            memory[0] = memory['buffer']
            out.next = memory[0]
            return
        if ld.val:
            print 'izvrsavamo ld'
            memory['buffer'] = input.val
            wrdelay.next = bool(1)
    return eval_reg

def regTestBench():
    
    clk = Signal(intbv(0)[1:0])
    inA = Signal(intbv(5)[8:0])
    AtoB = Signal(intbv(0)[8:0])
    ldA = Signal(intbv(0)[1:0])
    ldB = Signal(intbv(0)[1:0])
    out = Signal(intbv(0)[8:0])
    A = reg8('A', inA, ldA, AtoB, clk)
    B = reg8('B', AtoB, ldB, out, clk)
    rezA = []
    rezB = []
    @instance
    def test():
        rezA.append(AtoB.val)
        rezB.append(out.val)
        yield delay(1)
        ldA.next = intbv(1)[1:0]
        rezA.append(AtoB.val)
        rezB.append(out.val)
        yield delay(1)
        clk.next = intbv(1)[1:0]
        rezA.append(AtoB.val)
        rezB.append(out.val)
        yield delay(1)
        rezA.append(AtoB.val)
        rezB.append(out.val)
        yield delay(1)
        rezA.append(AtoB.val)
        rezB.append(out.val)
        yield delay(1)
        rezA.append(AtoB.val)
        rezB.append(out.val)
        yield delay(1)
        rezA.append(AtoB.val)
        rezB.append(out.val)
        print rezA
    return (test, A,B)
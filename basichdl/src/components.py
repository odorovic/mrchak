'''
Created on Apr 5, 2009

@author: odor
'''

import hdl 
from myhdl import instance, delay, always, Signal, intbv, always_comb, instances

def PROBE(sig):
    '''Komponenta koja detektuje uzlaznu ivicu signala na koji je vezana.'''
    @always(sig.posedge)
    def detect():
        print "Detektovan posedge: %s" % sig._name
        evt = hdl.PosEdgeEvt(sig)
        hdl.process_event(evt)
    return detect

def CLOCK(clk, period=10):
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

def MUX2(in0, in1, S, out):
    '''Dvoulazni multiplekser.'''
    
    @always_comb
    def eval_mux():
        print "evaluiramo mux"
        if S == 0:
            out.next = in0
        elif S == 1:
            out.next = in1
        else:
            assert False, "S signal multipleksera nije ni 1 ni 0. Weird."
    return eval_mux

def REG(input, ld, clk, out):
    '''8-bitni registar.'''
    memory = {0 : intbv(0)[8:0],
              'buffer' : None}
    reg_delay = 5
    
    @instance
    def eval_reg():
        '''Generator vrsi sinhroni upis u registar.
        
        Na pozitivnu ivicu takta trenutna vrednost ulaza se pamti u inputbuffer
        i postavlja signal wrdelay.next na pozitivnu vrednost. U sledecem trenu 
        vremena, na pozitivnu ivicu wrdelay zaista pamtimo inputbuffer u memoriju.
        
        Razlog za ovo je sto zelimo da se pojavljivanje vrednosti u memoriji javi
        posle pozitivne ivice takta, a u isto vreme zelimo da struktura podataka za
        memoriju ne bude signal.
        '''
        while True:
            yield clk.posedge
            memory['buffer'] = input.val
            yield delay(reg_delay)
            out.next = memory['buffer']
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
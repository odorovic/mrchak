'''
Created on Apr 4, 2009

@author: odor
'''

import unittest
import myhdl
from myhdl import Signal, now
from os import path

from simserver import simrunner

from builtins import components

def logicgate(name, inA, inB, out, func=None):
    
    assert len(inA)==len(inB)==len(out)
    @always_comb
    def eval_gate():
        out.next = func(inA.val, inB.val)
    return eval_gate


def andor(name, inA, inB, andout, orout):
    
    and1 = logicgate(path.join(name, 'and1'), inA, inB, andout, func=myand)
    or1 =  logicgate(path.join(name, 'or1'), inA, inB, orout, func=myor)
    return (and1, or1)

def opunit(name, inA, inB, ldA, ldB, opselect, out, clk):
    #izlazni signali andor modula
    andout = Signal(intbv(0))
    andout.name = path.join(name, 'andout')
    orout = Signal(intbv(0))
    orout.name = path.join(name, 'orout')
    #ulazni signali andor modula
    A2andor = Signal(intbv(0))
    A2andor.name = path.join(name, 'A2andor')
    B2andor = Signal(intbv(0))
    B2andor.name = path.join(name, 'B2andor')
    andor1 = andor(path.join(name, 'andor1'), A2andor, B2andor, andout, orout)
    A = reg8(path.join(name, 'A'), inA, ldA, A2andor, clk)
    B = reg8(path.join(name, 'B'), inA, ldB, A2andor, clk)
    
    mux1 = mux(path.join(name, 'MUX'), andout,orout, opselect, out)
    return (A,B, andor1, mux1)
        
def ctrlunit(start, ldA, ldB, opselect, fc, clk):
    states = ['BEGIN', 'LOAD']
    
    state = Signal(intbv(1))
    @always(clk.posedge)
    def eval_module():
        if state.val == intbv(1):
            state.next = state.val+1
            ldA.next = 1
            ldB.next = 1
            fc.next = intbv(1)
            return
        if state == 2 :     #Na nadolazetju ivicu hvatamo ld signale
            state.next = state.val+1
            ldA.next = 0
            ldB.next = 0
            opseletc.next = intbv(0)
            return
        if state == 3:  #ovde je izlazni raspoloziva izlazna vrednost
            state.next = intbv(1)
            fc.next = intbv(1)
    return eval_module

def device(name):
    inA = Signal(intbv(42)[8:0])
    inB = Signal(intbv(42)[8:0])
    ldA = Signal(intbv(0)[1:0])
    ldB = Signal(intbv(0)[1:0])
    opselect = Signal(intbv(0)[1:0])
    
    ctrlunit1 = ctrlunit(path.join(name, 'ctrlunit1'))
    opunit1 = opunit(path.join(name, 'ctrlunit'))

class TestClockStep(unittest.TestCase):
    def test(self):
        clk = Signal(0)
        clk._name = 'clk'
        hwinstance = (components.ClkDriver(clk, period = 10))
        print dir(simrunner)
        runner = simrunner.SimRunner(hwinstance)
        runner.step()
        t1 = now()
        runner.step()
        t2 = now()
        assert t2-t1 == 10
    
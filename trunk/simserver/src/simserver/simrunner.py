'''Omotac oko koda simulacije. 

Paznja!: Iako nije eksplicitno postavljeno, zbog ogranicenja myhdl simulatora,
SimRunner klasa se sme instancirati samo jednom (poor man's singleton)

Takodje, nije sav myhdl kod pogodan za simulaticju u ovom simulatroru. Specificno,
signali i memorije kojima zelimo pristup prilikom rada modaju da budu eksplicitno i 
jedinstveno imenovani.

Konvencija imenovanja signala je da se isti imenuju kao cvorovi u fajl sistemu, tj
kao '/' razdvojene putanje od korena, gde je svako ime sadrzavajuce komponenete.

Created on Apr 2, 2009

@author: odor
'''

import sys
import myhdl
from builtins.components import EdgeDetector

CLK_DURATION = 100



class SimRunner(object):
    signals = {}    #svi signali, referencirani po url-u
    memories = {}   #svi memorijski elementi, referencirani po url-u i adresi.
    def __init__(self, hwinstance):
        '''Kreira instancu simulacije nad datim myhdl hardverskim modulom.
        
        Argumenti:
            hwmodule : function -- myhdl hardverski modul za simulaciju.
        '''
        self.hwinstance = hwinstance
        self.clkposedge = 0
        self.clk = 0
        from myhdl import _simulator
        SimRunner.signals = dict([(sig._name, sig) for sig in _simulator._signals])
        print SimRunner.signals
        self.start()
        
    
    def clkevent(self):
        self.clkposedge = 1
        self.clk = self.clk+1
        
    def start(self):
        '''Inicira simlaciju sa datim hardverskim modulom'''
        self.sim = myhdl.Simulation((self.hwinstance, 
                                     EdgeDetector(SimRunner.signals['clk'], 
                                                  self.clkevent))) 
        
    def step(self):
        '''Pokrece simulaciju za jedan period takta unapred'''
        while True:
            self.sim.run(1)
            if self.clkposedge:
                self.clkposedge = 0
                return
        
    def goto(self, clknum):
        '''Pomera simulaciju do prilozenog takta.'''
        assert clknum >= self.clk
        togo = self.clk - clknum
        for i in range(togo):
            self.step()
    
    def listsignalnames(self):
        '''Vraca listu identifikatora svih signala u simulaciji'''
        return [key for key in SimRunner.signals.keys()]
    
    def getsignalstate(self, name):
        '''Vraca trenutno stanje signala datog imena.'''
        return SimRunner.signals[name].val
    
    def getmemories(self):
        '''Vraca listu svih memorijskih modula koje simulacija oglasava'''
        pass
    
    def getmemory(self, name, address=0):
        '''Vraca sadrzaj prilozene adrese prilozene memorije.'''
        pass
        
    def setmemory(self, name, address, val):
        pass
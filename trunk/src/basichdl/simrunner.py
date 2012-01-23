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
from components import PROBE
import hdl

CLK_DURATION = 100

class SimRunner(object):
    signals = {}    #svi signali, referencirani po url-u
    memories = {}   #svi memorijski elementi, referencirani po url-u i adresi.
    def __init__(self, parent, hwinstance):
        '''Kreira instancu simulacije nad datim myhdl hardverskim modulom.
        
        Argumenti:
            hwmodule : function -- myhdl hardverski modul za simulaciju.
        '''
        self.hwinstance = hwinstance
        self.flags = {'clk':False, 'instruction_begin': False, 'halt':False}
        hdl.bind(hdl.PosEdgeEvt, self.onClk, signame='clk')
        hdl.bind(hdl.PosEdgeEvt, self.onInst, signame='instruction_begin')
        hdl.bind(hdl.PosEdgeEvt, self.onHalt, signame='halt')
        self.start()
        self.parent = parent
    
    def onClk(self, evt):
        self.flags['clk'] = True
        hdl._clk = hdl._clk + 1
        
    def onInst(self, evt):
        self.flags['instruction_begin'] = True
        
    def onHalt(self, evt):
        self.flags['instruction_begin'] = True
        raise Exception("Simulacija zavrshena")
        print "HALT!"
        
    def start(self):
        '''Inicira simlaciju sa datim hardverskim modulom'''
        self.sim = myhdl.Simulation(self.hwinstance) 
    
    
    
    def step(self, clkname=None):
        '''Pokrece simulaciju za jedan period takta unapred'''
        assert not clkname or clkname in self.flags.keys()
        watchdog = 0
        while True:
            self.sim.run(1)
            hdl.flush_evts()
            #sporedni efekti svih dogadjaja su se se desili,
            #sada proveravamo uslove ispadanja.
            if clkname == None or self.flags[clkname] == True :
                #self.sim.run(5)
                for key in self.flags:
                    self.flags[key] = False
                return
            watchdog = watchdog + 1
            if watchdog == 10000:
                raise Exception("Mrtva petlja.")
    
    def getsignal(self, name, namespace):
        try:
            global_sigs = hdl._signals
            local_sigs = hdl._local_sig_bindings[namespace]
            try:
                return local_sigs[name]
            except KeyError:
                return global_sigs[name]
        except KeyError, e:
            not_found = e.args[0]
            if not_found == namespace:
                raise KeyError('Ne postoji dati namespace: %s' % namespace)
            else:
                raise KeyError('Ne postoji signal: %s.%s' % (namespace, name))   
    
    def get_sig_state(self,name, namespace):
        '''Pokusava da vrati stanje signala po imenu u zadatom modulu'''
        return self.getsignal(name, namespace).val
            
    def onSimCommand(self, evt):
        print "SimRunner.onSimCommand"
        try:
            commandname, arg = evt.command.split()
        except ValueError:
            commandname = evt.command
            arg = None
        self.step(arg)
        

'''
Created on May 2, 2009

@author: odor
'''

import os
import sys
import simplejson
import adapters
from basichdl import hdl
from basichdl import simrunner

class DataSource(object):  
    '''Interfejs klasa'''      
    def items(self):
        return self._items

class TrivialJsonDs(DataSource):
    '''Izvor podataka za iscrtavanje.
    
    Vrsi konverziju spoljnjeg opisa sheme u listu objekata koji se mogu iscrtati na 
    FloatCanvas instanci.
    
    Za sada je spoljin tip podataka hard-wired na Json objekat proizvedem schdump alatom.
    
    '''

    def __init__(self, schjson):
        '''Konstruktor DataSorsa.
        
        Izvor konteksta za crtanje. Spaja izvor topologije za crtanje i simulatora 
        koji odredjuje stanje elemenata za crtanje.
        
        Argumenti:
            schjson - filename ili file object : ulazna shema.
            simserver - SimServer instanca : Interfejs ka simulatoru.
        '''
        self.sch = self.read_data(schjson)

    def read_data(self, schjson):
        schdict = None
        if isinstance(schjson, basestring):
            try:
                f = open(schjson, 'r')
                schdict = self._read_json(f)
            except IOError, e:
                raise e  
        elif hasattr(schjson, 'read'):
            schdict = self._read_json(schjson)
        #odavde se propagira IOError sa nepronadjenim fajlom, kao i bilo sta
        #sto baca simplejson, uglavnom loshe formiran json
        return schdict
    
    def _read_json(self, f):
        return simplejson.load(f)
    
    def _items(self, sch):
        self.signals = []
        for submodule in sch['submodules'] + sch['graphics']:
            for entity in submodule['symboldata']:
                for adp in adapters.adapter(entity):
                    yield adp
            for attribute in submodule['attributes'].values():
                for adp in adapters.adapter(attribute):
                    yield adp
        for signal in sch['signals']:
            signalobj = adapters.Signal(signal)
            self.signals.append(signalobj)
            yield signalobj

    def items(self):
        '''Iterator kroz sve elemente za crtanje'''
        return self._items(self.sch)
    
    def onStateChange(self, evt):
        print "Datasource.onStateChange, boli me kita, btw."
        
                
class ProjectDs(TrivialJsonDs):
    '''Datasors koji radi sa projektom -- skupom fajlova koji opisuju simulaciju.'''
    
    def __init__(self, prj_path):
        self.manifest = self.read_manifest(prj_path)
        self.schs = {}
        for schem in self.manifest['schnames']:
            sch = self.read_data(os.path.join(prj_path, 'out', schem.lower() + '.json'))
            self.schs[schem] = sch
        self.init_sim()
        self.current_sch = self.manifest['entry-point']
    
    def init_sim(self):
        '''Inicijalizuje instancu simulatora nad hardverom'''
        self.init_mems()
        sys.path.append(os.path.join(self.manifest['path'],'out'))
        pymodulename = self.manifest['hardware']
        pymodule = __import__(pymodulename)
        hwname = self.manifest['entry-point']
        hw = getattr(pymodule, hwname)()
        self.simulator = simrunner.SimRunner(self, hw)
        
    def init_mems(self):
        '''Inicijalizuje memorije.'''
        mem_path = os.path.join(self.manifest['path'], 'out', 'mem')
        mems = self.manifest['mems']
        for mem in mems:
            memf = open(os.path.join(mem_path, mem+'.mem'))
            mem_desc = simplejson.load(memf)
            hdl.register_memory(mem_desc)
    
    def read_manifest(self, path, manifest_fname='manifest.json'):
        def inorder(lst):
            keys = []
            for item in lst:
                if isinstance(item, basestring):
                    keys.append(item)
                else:
                    keys = keys + inorder(item)
            return keys 
        f = open(os.path.join(path, manifest_fname), 'r')
        manifest =  simplejson.load(f)
        manifest['schnames'] = inorder(manifest['tree'])
        manifest['path'] = path
        return manifest
    
    def items(self):
        sch = self.schs[self.current_sch]
        itemgen = self._items(sch)
        return itemgen
    
    def onStateChange(self, evt):
        print "Datasource.onStateChange"
        for signal in self.signals:
            sigstate = self.simulator.get_sig_state(signal.name, self.current_sch)
            signal.set_state(sigstate)
    
    def getsignal(self, name):
        '''Vraca signal po imenu, lokalno pa globalono trazenje'''
        return self.simulator.getsignal(name, self.current_sch)
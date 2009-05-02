'''
Created on May 2, 2009

@author: odor
'''

import os
import simplejson
import adapters

class DataSource(object):        
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
        for submodule in sch['submodules']:
            for entity in submodule['symboldata']:
                yield adapters.adapter(entity)

    def items(self):
        '''Iterator kroz sve elemente za crtanje'''
        return self._items(self.sch)
        
                
class ProjectDs(TrivialJsonDs):
    '''Datasors koji radi sa projektom -- skupom fajlova koji opisuju simulaciju.'''
    
    def __init__(self, prj_path):
        self.manifest = self.read_manifest(prj_path)
        self.schs = {}
        for schem in self.manifest['schnames']:
            sch = self.read_data(os.path.join(prj_path, 'out', schem + '.json'))
            self.schs[schem] = sch
        self.current_sch = self.manifest['entry-point'] 
    
    def read_manifest(self, path, manifest_fname='manifest.json'):
        def inorder(dct):
            keys = []
            for key, val in dct.items():
                keys.append(key)
                if val != None:
                    keys.append(inorder(val))
            return keys 
        f = open(os.path.join(path, manifest_fname), 'r')
        manifest =  simplejson.load(f)
        manifest['schnames'] = inorder(manifest['tree'])
        return manifest
    
    def items(self):
        sch = self.schs[self.current_sch]
        return self._items(sch)
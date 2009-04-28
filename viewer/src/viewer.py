'''Modul za iscrtavanje shema.

Ovaj modul definishe klase/funkcije za isctavanje Shema u wx prozore.
Format shema je json enkoding kakav proizvodi schdump, dok je Widget od kojeg
polazimo sa implementacijom wx-ov floatcanvas
'''

import wx
import simplejson
from wx.lib.floatcanvas import NavCanvas
from wx.lib.floatcanvas import FloatCanvas
import os

import adapters

class Signal(object):
    '''Reprezentacija signal objekta.
    
    Signali se sastoje iz liste netova i tekstualnih labela. Signal je vezan za
    SimProxy instancu, od koje preuzima trenutnu vrednost, a takodje se i prijavljuje 
    na stateUpdate dogadjaje koje SimProksi generishe.
    '''
    class Wire(object):
        '''Prikaz zice'''
        pass
    class Label(object):
        '''Prikaz labele sa statusom'''
        pass

    def __init__(self):
        self.name = None
        self.value = None
        
class Symbol(object):
    '''Reprezentacija simbola na shemi.
    
    Simboli su prikazi na shemi koji imaju jedno jedino pridruzeno stanje.
    Poseduju geometrijski kontekst koji treba iscrtati, i to je to.
    '''
    def __init__(self):
        pass
    
    def loaddict(self, symboldict):
        '''ucitava dict sa serijalizovanim opisom simbola u instancu.'''
        
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
        if type(schjson) == str:
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
    
    def items(self):
        '''Iterator kroz sve elemente za crtanje'''
        for submodule in self.sch['submodules']:
            for entity in submodule['symboldata']:
                yield adapters.adapter(entity)
        
class TrivialDs(DataSource):
    def __init__(self):
        self._items = [adapters.Line({'base_attrs' : {'line':[[10,10],[10,20]],
                                                    'color': 4}}),
                        adapters.Box({'base_attrs' : {'point': [11,11],
                                                      'width': 20,
                                                      'height' : 5,
                                                      'color': 5}})]

    
class Viewer(wx.Frame):
    '''Viewer klasa.
    
    Prikazuje zeljenu shemu na floatcanvas.NavCanvas podklasi.
    '''
    def __init__(self, datasource, *args, **kwargs):
        '''Konstruktor Viewera.'''
        wx.Frame.__init__(self, *args, **kwargs)
        self.Canvas = NavCanvas.NavCanvas(self,
                                          Debug = 0,
                                          BackgroundColor = adapters.pallete['BACKGROUND_COLOR']).Canvas
        self.datasource = datasource
        self.populate()
        self.Show()
        self.Canvas.ZoomToBB()

    
    def populate(self):
        '''Iscrtava ekran na osnovu data-sorsa.'''
        assert self.datasource != None
        for element in self.datasource.items():
            self.Canvas.AddObject(element)
            
    def onStateChange(self):
        '''Hendler za promenu stanja sheme koju Viever prikazuje'''
        pass
    
    def onSchematicChange(self, schemevent):
        '''Hendler koji se poziva u slucaju promene topologije sheme'''
        pass
        
if __name__ == '__main__':
    app = wx.App(False)
    frame = Viewer(TrivialJsonDs('drawtest.json'), 
                   None,
                   title='Schematic Viewer',
                   size=(640,480))
    app.MainLoop()
        

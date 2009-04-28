'''
Created on Mar 14, 2009

@author: odor
'''

import schparse
import entities
import schematic
import sys
from json import serialize
from json import JsonSerializable

class NotConnected(Exception):
    pass
     
def parsef(filename):
    raise Error("Not Implemented") #TODO

def parse(file):
    if type(file) == str:
        raise "NotImplemented" #TODO otvaranje fajla sa PATH i sl
    if not hasattr(file, 'readlines'):
        raise "Treba nam objekat sa readlines metodom."
    linesgen = (line.strip() for line in file.readlines())
    return _parse(linesgen)

def _parse(linesgen):
    '''Parsuje generator linija sch fajla'''
    entitylist = schparse._parse(linesgen)
    schematiclist = merge_nets(entitylist)
    schematiclist = expand_components(schematiclist)
    return schematiclist

def _translate(symbol, pnt):
    '''Translira sve entitete u simbolu za pnt.'''
    for entity in symbol.symboldata:
        if hasattr(entity, 'point'):
            entity.base_attrs['point'] = entity.point + pnt
        elif hasattr(entity, 'line'):
            entity.base_attrs['line'] = entity.line + pnt

def expand_components(entitylist):
    '''Prolazi kroz listu entiteta i vrshi ekspanziju komponenata.
    
    '''
    retlst = []
    for entity in entitylist:
        if isinstance(entity, entities.Component):
            sym = Symbol(entity)
            #shiftovati sve tacke unutar simbola za poziciju entitija.
            _translate(sym, entity.point)
            retlst.append(sym)
        else:
            retlst.append(entity)
    return retlst

def merge_nets(schemlist, net_cls = entities.Net):
    '''Spaja logicki spojene Net entitete u NetList instance
    
    Prolazi kroz listu entiteta, Net instance spaja u Logicki spojene NetList instance,
    a sve ostalo ostavlja nepromenjeno. Vratja novu listu.
    
    Argumenti:
        schemlist : list(entites.SchematicEntity, [ Other types ]) - Lista
            sa reprezentacijom svih elemenata na shemi. Ova funkcija radi samo nad
            instancama klase date net_cls argumenta, nad kojima izvrsava merge alg 
            dok ostale bezbedno ignorise.
        net_cls : class - Klasa ad cijim instancima vrsimo obradu. Podrazumevana 
        je entites.Net.
    Povratni tip:
        List(schematic.NetList [Other types])
    '''
    retlst = [ item for item in schemlist if not isinstance(item, net_cls) ]    #sve sto nije net.
    nets = [ item for item in schemlist if isinstance(item, net_cls) ]    #sve sto jeste net.
    netlists = []
    while len(nets) > 0:
        net = nets.pop()
        doesnt_belong = []
        belongs_to = []
        for netlist in netlists:
            if netlist.touches(net):
                netlist.append(net)
                belongs_to.append(netlist)
            else:
                doesnt_belong.append(netlist)
        if belongs_to != []:
            netlists = doesnt_belong + [Netlist.join(belongs_to)]
        else:
            netlists = doesnt_belong + [Netlist().append(net)]
    return retlst + netlists

def isnetlist(item):
    return isinstance(item, Netlist)
def isinpin(item):
    return hasattr(item, 'type') and item.type == 'INPUT'
def isoutpin(item):
    return hasattr(item, 'type') and item.type == 'OUTPUT'
def ismodule(item):
    return (isinstance(item, Symbol) and 
            not isinpin(item) and 
            not isoutpin(item))
def isattribute(item):
    return isinstance(item, entities.Text)
    
def set_io(item, schemdict):
    '''Dodeljuje input i output signale simbolu.
    
    Za sve pinove simbola pronalzimo netlist koji isti dodiruje.
    Pinovi za sada moraju imati eksplicitno oznacen smer.
    '''
    for pin in item.pins:
        for netlist in schemdict['signals']:
            for net in netlist.nets:
                if pin.point.touches(net.line):
                    if pin.ext_attrs['pintype'].attr == 'in':
                        item.inputs.append(netlist)
                        break   #prelazimo na sledeci pin
                    elif pin.ext_attrs['pintype'].attr == 'out':
                        item.outputs.append(netlist)
                        break   #prelazimo na sledeci pin
                    else:
                        #ovde smo najebali, posto ne znamo dovoljno o pinu.
                        #TODO -- iz ovoga se moze ponekad oporaviti.
                        raise SchSyntaxError("Nismo u stanju da odredomo smer pina.")
    return item

def clasify(schemlist):
    '''Uzima listu formiranih Net/BusListova i komponenti i pravi dict sheme.'''
    schematic = Schematic(schemlist)
    return schematic

class Schematic(JsonSerializable):
    '''Hijerarhijska reprezentacija elektricne sheme.'''
    def __init__(self, schemlist):
        '''Kreira Shemu iz liste sadrzanih podelementata.
        
        Argumenti:
            schemlist : list -- Lista Simbola i NetLista koji cine shemu.
        Povratna vrednost : 
            Schematic instanca.
        '''
        lst = [item for item in schemlist]
        self.in_pins = [item for item in lst if isinpin(item)]
        self.out_pins = [item for item in lst if isoutpin(item)]
        self.signals = [item for item in lst if isnetlist(item)]
        self.input_signals = []
        for signal in self.signals:
            for pin in self.in_pins:
                try:
                    self.connect(signal, pin)
                    self.input_signals.append(signal)
                except NotConnected:
                    pass

        self.output_signals = []
        for signal in self.signals:
            for pin in self.out_pins:
                try:
                    self.connect(signal, pin)
                    self.output_signals.append(signal)
                except NotConnected:
                    pass
        
        self.internal_signals = [item for item in self.signals if 
                                     item not in self.input_signals and
                                     item not in self.output_signals]
        self.submodules  = []
        for item in lst:
            if ismodule(item):
                submodule = self.set_io(item)
                self.submodules.append(submodule)
    
        self.attributes = dict([(item.key, item.attr) for item in lst if isattribute(item)])        
        
    def set_io(self, item):
        '''Dodeljuje input i output signale simbolu.
        
        Za sve pinove simbola pronalzimo netlist koji isti dodiruje.
        Pinovi za sada moraju imati eksplicitno oznacen smer.
        '''
        for pin in item.pins:
            for netlist in self.signals:
                for net in netlist.nets:
                    if pin.point.touches(net.line):
                        if pin.ext_attrs['pintype'].attr == 'in':
                            item.inputs.append(netlist)
                            break   #prelazimo na sledeci pin
                        elif pin.ext_attrs['pintype'].attr == 'out':
                            item.outputs.append(netlist)
                            break   #prelazimo na sledeci pin
                        else:
                            #ovde smo najebali, posto ne znamo dovoljno o pinu.
                            #TODO -- iz ovoga se moze ponekad oporaviti.
                            raise SchSyntaxError("Nismo u stanju da odredomo smer pina.")
        return item
        
    def connect(self, signal, pin):
        '''Pokusava da poveze netlist instancu sa pin simbolom.
        
        Prvo proverava da li u stvari postoji fizicka veza izmedju ova dva.
        Ako postoji, pregovara netname. Za sada toliko.
        '''
        if signal.touches(pin):
            #Ukoliko nije vec definisan, signal preuzima netname pina
            if signal.netname == None:
                signal.netname = pin.pinlabel
        else:
            raise NotConnected
        
class Symbol(JsonSerializable):
    '''Simbol na shemi.
    
    Ova klasa poseduje topoloski kontekst, u upotrebi za generaciju simulatora i
    kontekst za crtanje, u upotrebi za gui. Kreira parsiranjem Component instance.
    
    Polja:
        type : string - Klasifikator tipa komponente. Moze biti ili jedan od 
            built in tipova ili "submodule" koji predstavlja korisnicki definisanu
            komponenentu. Vazniji tipovi: 
            - "input_pin" : Specijalni simbol za vezu koju tekuci simbol importuje
            - "output_pin" : Specijalni simbol za vezu koju tekuci simbol eksportuje
            - "submodule"  : Simbol modula ispod u hijerarhiji. Za sada mi se
                cini da je transparentno da li je submodul generisan u ovom alatu
                ili je built-in
        pins : entities.Pin - Lista pinova kojima se Simbol povezuje sa netlistama.
            TODO: za sada pretpostavljamo da entities.Pin ima touches() metod.
            videti sta tjemo oko toga.
        symboldata: list(entities.SchematicEntity) - Sirova lista Schematic entiteta.
        Sadrzi dovoljno informacija za kreiranje grafickog konteksta
        
    '''
    def __init__(self, component):
        '''Pravi Simbol od komponente. 
        
        Za sada nas od elemenata simbola zanimaju samo 
        koordinate connection tacaka pinova.
        '''
        self.symboldata = schparse.parse(component.basename)
        self.pins = [item for item in self.symboldata 
                          if isinstance(item, entities.Pin) ]
        self.point = component.point
        self.type = component.device.attr
        if hasattr(component, 'pinlabel'):
            self.pinlabel = component.pinlabel.attr
        self.inputs = []
        self.outputs = []
        
    def touches(self, line):
        for pin in self.pins:
            if pin.point.touches(line):
                return True
        return False

class Netlist(JsonSerializable):
    '''Skup logicki povezanih netova.'''
    def __init__(self):
        self.nets = set()
        self.netname = None
        
    def __eq__(self, netlst):
        return self.nets == netlst.nets
    
    def __repr__(self):
        '''String repreznetacija signala.
        
        Za sada nas izgleda samo zanima ime mreze i sirina magistrale (default=1)
        '''
        return "{'netname': %s, 'width' %s}" % (self.netname, 1)
        
    def append(self, net):
        self.nets.add(net)
        return self
        
    def touches(self, net):
        '''Proverava da li Nelist dodiruje Net dat argumentom.
        
        Ovo se moze desiti ili ako su netlist i net fizicki spojeni, ili ako
        imaju isto ime.
        '''
        if hasattr(net, 'netname') and net.netname == self.netname:
            return True
        if hasattr(net, 'line'):
            '''Tj. ako smo dobili liniju'''
            for mynet in self.nets:
                if mynet.line.touches(net.line):
                    return True
            return False
        elif hasattr(net, 'touches'):
            for mynet in self.nets:
                if net.touches(mynet.line):
                    return True
        elif hasattr(net, '__iter__') and hasattr(net.__iter__().next(), 'touches'):
            '''Argument nema potrebni interfejs.
            Da nije iterabilan?
            '''
            for othernet in net:
                for mynet in self.nets:
                    if othernet.touches(mynet.line):
                        return True
            return False
        else:
            raise AttributeError("SRANJE")
                
    
    @staticmethod
    def join(netlists):
        '''Spaja dva netlista u jedan'''
        newset = set()
        for netlist in netlists:
            newset = newset.union(netlist.nets)
        retnetlist = Netlist()
        retnetlist.nets = newset
        return retnetlist 

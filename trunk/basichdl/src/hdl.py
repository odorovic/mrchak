'''Modul sa nashim ekstenzijama myhdla.
Created on May 10, 2009

@author: odor
'''

import myhdl

ALLOW_DUPLICATES = False
_signals = {}

_pending_evts = []

_evt_bindings = {}

def signal_by_name(name):
    return _signals[name]

def signal_by_ref(ref):
    return ref._name

def process_event(evt):
    '''Registruje dogadjaj u processing loop.
    
    Postavlja instancu SimEvent klase na listu cekanja.  
    '''
    _pending_events.append(evt)
    
def flush_evts():
    '''Poziva hendlere svih dogadjaja na redu za cekanje i prazni red.'''
    for evt in _pending_evts:
        for handler, signame in _evt_bindings[evt.__class__]:
            if signame == None or evt.signame == signame:
                handler(evt) 
    
def bind(evttype, fun, signame=None):
    '''Povezuje dobgadjaj sa callback fjom.
    
    Opcioni argument signame predstavlja ime izvorisnog signala.
    '''
    try:
        _evt_bindings[evttype]
    except KeyError:
        _evt_bindings[evttype] = []
    _evt_bindings[evttype].append((fun, signame))

class SimEvent(object):
    '''Jednostavni dogadjaji u simuaciji'''
    pass

class PosEdgeEvt(SimEvent):
    '''Signal obavestava o uzlaznoj ivici signala'''
    def __init__(self, sig):
        self.signame = signal_by_ref(sig)

def signal(name, *args, **kwargs):
    '''Dekorator oko myhdl signala.
    
    Nasa verzija keshira sve instancirane signale, sto omogucava njihovo
    referenciranje po imenu kasnije.
    '''
    try:
        s = _signals[name]
        assert ALLOW_DUPLICATES, 'Signal sa istim imenom instaniciran dva puta.'
        return s
    except KeyError:
        s = myhdl.Signal(*args, **kwargs)
        s._name = name
        _signals[name] = s
        return s
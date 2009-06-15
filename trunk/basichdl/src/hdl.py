'''Modul sa nashim ekstenzijama myhdla.
Created on May 10, 2009

@author: odor
'''

import myhdl

ALLOW_DUPLICATES = False
_signals = {}

_clk = 0

'''Recnik lokalnih imenovanja. Svaki myhdl modul je duzan da prijavi lokalne signale.'''
_local_sig_bindings = {}

_pending_evts = []

_evt_bindings = {}

_memories = {}

def signal_by_name(name):
    return _signals[name]

def signal_by_ref(ref):
    return ref._name

def now(clk=False):
    if not clk:
        return myhdl.now()
    else:
        return _clk

def process_event(evt):
    '''Registruje dogadjaj u processing loop.
    
    Postavlja instancu SimEvent klase na listu cekanja.  
    '''
    _pending_evts.append(evt)
    
def flush_evts():
    '''Poziva hendlere svih dogadjaja na redu za cekanje i prazni red.'''
    global _pending_evts
    for evt in _pending_evts:
        for handler, signame in _evt_bindings[evt.__class__]:
            if signame == None or evt.signame == signame:
                handler(evt)
    _pending_evts = []
    
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
        
def register_locals(namespace, bindings):
    assert not _local_sig_bindings.has_key(namespace)
    #ovo je jedino mesto gde proveravamo jedinstvenost shema.
    _local_sig_bindings[namespace] = bindings

def register_reg(desc):
    pass

def register_memory(desc):
    '''Datasource ovde registruje memorije koje su na raspolaganju'''
    _memories[desc['name']] = desc

def request_memory(desc):
    '''Trazi pristup memoriji. Ista je morala biti dodata.
    
    Ovaj zahtev vishe sluzi za proveru gresaka nego bilo sta drugo.
    '''
    pass
    
def read_memory(name, addr):
    print _memories
    mem_desc = _memories[name]
    assert len(addr) == mem_desc['addrwidth'],'%s, %s' %(len(addr), mem_desc['addrwidth'])
    width = mem_desc['wordwidth']
    for seg in mem_desc['segments']:
        if addr >= seg['start'] and addr < seg['start'] + seg['length']:
            offset = addr - seg['start']
            word = myhdl.intbv(int(seg['words'][offset],16))[width:0]
            return word
    raise Exception("Nije pronadjena rech u memoriji.")
    
def signal(name, sigtype, *args, **kwargs):
    '''Dekorator oko myhdl signala.
    
    Nasa verzija keshira sve instancirane signale, sto omogucava njihovo
    referenciranje po imenu kasnije.
    '''
    try:
        s = _signals[name]
        assert ALLOW_DUPLICATES, 'Signal sa istim imenom instaniciran dva puta.'
        return s
    except KeyError:
        if sigtype == 'sig':
            s = myhdl.Signal(*args, **kwargs)
        elif sigtype == 'tri':
            s = myhdl.Tristate(*args, **kwargs)
        else:
            raise Exception("Nije prepozbnat tip signala: %s" % sigtype)
        s._name = name
        _signals[name] = s
        return s
'''
Created on Mar 3, 2009

@author: odor
'''

import os

import configuration

from entities import create_entity
import entities

class InitializationError(Exception): pass

from entities import OpenBrace, ClosedBrace, SchSyntaxError

path = configuration.SYMBOL_PATHS

def path_open(filename, mode):
    for dirname in path:
        try:
            f = open(os.path.join(dirname,filename),mode)
            return f
        except IOError:
            pass
    raise IOError('Symbol %s not found' % filename)

def _parse(linegen):
    version = linegen.next()    #TODO validacija ovoga.
    #frame = linegen.next()    
    #obavezno izvaditi drugu liniju schematic 
    #fajlova (ona sadrzi frame radnog papira)
    cargo = {'in': linegen, 'out':[]}
    sm = StateMachine()
    return sm.run(cargo)

def parse(filename):
    '''Parsira shemu i vratja listu Entiteta.'''
    f = path_open(filename, 'r')
    gen = (token.strip() for token in f.xreadlines())
    return _parse(gen)

class StateMachine:
    '''Templejt state mashine.
    
    Iz Text Processing in Python od Dejvida Meca.
    Ako radi posao zasto menjati.
    '''
    def __init__(self):
        self.handlers = [state_read_token, 
                         state_read_args,
                         eof]
        self.startState = state_read_token
        self.endStates = [eof]

    def add_state(self, handler, end_state=0):
        self.handlers.append(handler)
        if end_state:
            self.endStates.append(name)

    def set_start(self, handler):
        self.startState = handler

    def run(self, cargo=None):
        if not self.startState:
            raise InitializationError, \
                    "must call .set_start() before .run()"
        if not self.endStates:
            raise InitializationError, \
                    "at least one state must be an end_state"
        handler = self.startState
        while 1:
            (newState, cargo) = handler(cargo)
            if newState in self.endStates:
                    newState(cargo)
                    return cargo['out']
            elif newState not in self.handlers:
                    raise RuntimeError, "Invalid target %s" % newState
            else:
                    handler = newState
                    
def state_read_token(cargo):
    '''Stateless read line.
    
    State u kojem se nalazimo prilikom citanja 
    prve linije nekog entiteta. 
    
    Mogutji prelazi su:
    --read_token - Ako pricitamo neki od single-line entiteta.
    --read-args - U slucaju da je procitan token "{"
    --EOF - ako smo stigli do kraja fajla 
    '''
    try:
        token = cargo['in'].next()
        entity = create_entity(token, cargo['in'])
        cargo['out'].append(entity)
        return state_read_token, cargo
    except OpenBrace:
        '''Fabrika entiteta baca open brace na '{'''
        return state_read_args, cargo
    except StopIteration:
        return eof, cargo

def state_read_args(cargo):
    '''U unutreasnjosti argument bloka.
    
    Stanje u kojem se nalazimo posle chitanja "{".
    unutra arg bloka svi entiteti su stringovi, te ih citamo
    i smestamo u entity-dict poslednjeg procitanog entiteta 
    pre "{" sve dok ne naletimo na "}"
    
    Mogutji preliz stanja:
    --read_token - ako procitamo "}" token
    '''
    def args(cargo):
        try:
            while True:
                token = cargo['in'].next()
                entity = create_entity(token, cargo['in'])
                
                yield entity.key, entity
        except StopIteration:
            raise SchSyntaxError, 'Neocekivani kraj fajla. Ocekivano "}" ili text-entity'
        except ClosedBrace:
            raise StopIteration
    for key, arg in args(cargo):
        cargo['out'][-1].ext_attrs[key] = arg
    return state_read_token, cargo

def eof(cargo):
    '''Zavrsni state prilikom kraja fajla.
    
    Jedina namena je da signalizira state-mashini da treba da ispadne.
    '''
    return lambda cargo: None
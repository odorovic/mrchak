'''Helper modul za serijalizaciju u json format.
Created on Apr 1, 2009

@author: odor
'''

import json

def serialize(obj, **kwargs):
    serialized = _serialize(obj)
    return json.dumps(serialized, **kwargs)

def _serialize(obj):
    '''Serijalizuje objekat u recnik serijalizabilan u simplejson.
    
    u slucaju neke od built in klasa direktno podrzanih za enkodovanje
    (stringovi, brojevi) vraca se direktna vrednost. U slucaju podrznih kontejnera
    (liste, recnici) rekurzivno se serijalizuju sadrzani objekti istom metodom,
    dok se u slucaju JsonSerializable instanci zove njihov serialize metod.
    Argumenti:
        obj  -- objekat koji treba serijalizovati. Moze biti ili objekat nase klase
            koja je serijalizabilna 'serialize' metodom ili built-in type koji 
            simplejson automatski enkoduje (str, int, float, long, dict, list)
    '''
    jsonnums = (int,float,long)
    jsonatoms = (str,) + jsonnums
    if hasattr(obj, '__serialize__'):
        return obj.__serialize__()
    elif isinstance(obj, dict):
        retobj = {}
        for key, val in obj.items():
            assert isinstance(key, jsonatoms)  #dozvoljni json kljucevi
            retobj[str(key)] = _serialize(val)
        return retobj
    elif isinstance(obj, jsonatoms):
        return obj
    elif hasattr(obj, '__iter__'):
        #Ovo moramo zvati tek posle stringova i dictova. Hvata set i list, i mozda
        #jos stogod nepozeljno.
        return [_serialize(item) for item in obj]
    else:
        ret = str(obj)
        return  ret


class JsonSerializable(object):
    '''Mixin dodaje serijalizabilnost klasi.'''
    def __serialize__(self):
        '''Generise Json-enkodabilnu reprezentaciju simbola.'''
        jsondict = {}
        for key, val in self.__dict__.items():
            jsondict[key] = _serialize(val)
        return jsondict

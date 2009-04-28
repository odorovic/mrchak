'''Test Helper modula za serijalizaciju u json format.
Created on Apr 1, 2009

@author: odor
'''

import unittest
from schtools import json

class SanityCheck(unittest.TestCase):
    '''Proverava infrastrukturu za serijalizaciju u nekim bazicnim slucajevima.'''
    def testSimpleDict(self):
        dct = {'a':'string', 'b': [1,2,3], 'c': {'1':1,'2':[1,2,3]}}
        rez = dict(dct)
        assert dct == rez
        jsn = json.serialize(dct) # --> json string
        dct = json.loads(jsn)
        assert dct == rez
        
    def testSimpleList(self):
        assert True
    
    def testSet(self):
        assert True
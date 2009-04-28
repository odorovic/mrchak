'''
Created on Apr 11, 2009

@author: odor
'''

import unittest
from symtools import symgen



test_schematic = {'attributes' : {'device':'ANDWRAP', 'name' : 'AndWrap'},
                  'in_pins': [{'pinlabel': 'inA'},
                              {'pinlabel': 'inB'}],
                  'input_signals': [{'netname': 'inA', 'width': 1}, 
                                    {'netname': 'inB', 'width': 1}],
                  'internal_signals': [],
                  'out_pins': [{'pinlabel':'OUT'}],
                  'output_signals': [{'netname': 'OUT', 'width': 1}],
                  'signals': [{'netname': 'OUT', 'width': 1},
                              {'netname': 'inA', 'width': 1},
                              {'netname': 'inB', 'width': 1}],
                  'submodules': [{'type':'7400',
                                  'inputs' : [{'netname': 'inA', 'width': 1}, 
                                              {'netname': 'inB', 'width': 1}]}]}

class TestExtractPins(unittest.TestCase):
    def test(self):
        print dir(symgen)
        rez = symgen.extract_pins(test_schematic)
        for pin in rez:
            print pin
        assert True
        
class TestSymbolDumps(unittest.TestCase):
    def test(self):
        rez = symgen.symbol_dumps(test_schematic)
        print rez
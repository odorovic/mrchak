'''
Created on Mar 6, 2009

@author: odor
'''
import unittest
from schtools.schparse import state_read_token
from schtools.schparse import state_read_args
import schtools.schparse

class TestStateReadToken(unittest.TestCase):
    def setUp(self):
        self.net_token = ["N 45200 47300 45200 48400 4"].__iter__()
        self.text_token = ["T 44600 47700 5 10 0 0 0 0 1",
                           "device=7408",].__iter__()
        self_ctoken = ["C 43900 46800 1 0 0 7408-1.sym",
                       "{",        
                       "T 44200 47700 5 10 1 1 0 0 1",
                       "refdes=U?",
                       "T 44600 49100 5 10 0 0 0 0 1",
                       "footprint=DIP14",
                       "}"].__iter__()
    def testReadToken(self):
        cargo = {'in': self.net_token, 'out': []}
        new_state, new_cargo = state_read_token(cargo)
        state_ok = self.assertEqual(new_state, state_read_token)
        cargo_ok = cargo is not None
        return state_ok and cargo_ok

class TestParseSchematic(unittest.TestCase):
    pass
    
if __name__ == "__main__":
    unittest.main()

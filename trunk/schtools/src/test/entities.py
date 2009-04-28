'''Unit testovi za modul entites
Created on Mar 6, 2009

@author: odor
'''

import unittest
from schtools import entities

class DirectValidInstantiation(unittest.TestCase):
    '''Testira instanciranje entiteta na validan unos'''
    def setUp(self):
        self.net_vals = ('N', 45200, 47300, 45200, 48400, 4)
        self.net_token = " ".join(map(str, self.net_vals))
        self.net_attrnames = (item[0] for item in entities.Net.subtoken_mapping)
        
        self.text_vals = (('T', 44600, 47700, 5, 10, 0, 0, 0, 0, 1),
                          ('quick brown fox',))
        self.text_token = " ".join(map(str, self.text_vals[0]))
        self.text_attrnames = (item[0] for item in entities.Text.subtoken_mapping)
        self.text_generator = self.text_vals[1].__iter__()

    def testCreateNet(self):
        n = entities.Net(self.net_token)
        print type(n.line)
        print type(n.line.points[0])
        print n.base_attrs
        
    def testCreateText(self):
        t = entities.Text(self.text_token, self.text_generator)
        valid = True
#        for key, val in zip(self.net_attrnames, self.net_vals):
#            valid = valid and t.base_attrs[key] == val
#        valid = valid & (t.base_attrs['text'] == self.text_vals[1][0])
        assert valid
            
class DirectInvalidInstantiation(unittest.TestCase):
    '''Testira nelegalne instanciajcije'''
    pass

class TestGetEntities(unittest.TestCase):
    '''Testira da li se valisno formira rechnik klasa.'''
    pass
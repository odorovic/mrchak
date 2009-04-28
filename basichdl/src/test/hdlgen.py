'''
Created on Mar 16, 2009

@author: odor
'''

import unittest

from schtools import hdlgen 

class TestGenerateHdl(unittest.TestCase):
    def setUp(self):
        '''Postavljamo nepotpun opis kontrolne jedinice.
        
        Generisani HDL netje biti validan, ali je dovoljno da vidimo da 
        li radi.
        '''
        self.module = {'name' : 'ctrlUnit',
                  'inputs' : [{'netname':'ctrl','width':1},
                              {'netname' : 'stsys' , 'width' : 1 },
                              {'netname' : 'store' , 'width' : 1 },
                              {'netname' : 'op1' , 'width' : 1 },
                              {'netname' : 'op2' , 'width' : 1 },
                              {'netname' : 'jsr' , 'width' : 1 },
                              {'netname' : 'jump' , 'width' : 1 },
                              {'netname' : 'rti' , 'width' : 1 },
                              {'netname' : 'rts' , 'width' : 1 },
                              {'netname' : 'not_halt' , 'width' : 1 },
                              {'netname' : 'not_intr' , 'width' : 1 }],
                  'outputs' : [],
                  'internal_signals' : [],
                  'submodules' : [{'name' : 'kmop',
                                   'type' : 'Kmop',
                                   'inputs': [{'netname':'ctrl','width':1},
                                               {'netname' : 'stsys' , 'width' : 1 },
                                               {'netname' : 'store' , 'width' : 1 },
                                               {'netname' : 'op1' , 'width' : 1 },
                                               {'netname' : 'op2' , 'width' : 1 },
                                               {'netname' : 'jsr' , 'width' : 1 },
                                               {'netname' : 'jump' , 'width' : 1 },
                                               {'netname' : 'rti' , 'width' : 1 },
                                               {'netname' : 'rts' , 'width' : 1 },
                                               {'netname' : 'not_halt' , 'width' : 1 },
                                               {'netname' : 'not_intr' , 'width' : 1 }],
                                   'outputs': ['kmop2mp',]},
                                   {'name' : 'kmbranch',
                                    'type' : 'Kmbranch',
                                    'inputs': ['val00',
                                               'val08',
                                               'val09',
                                               'val0A',
                                               'val14',
                                               'val1B',
                                               'val29',
                                               'val3A',
                                               'val3B',],
                                    'outputs': ['kmbranch2mp']},
                                    {'name' : 'mp',
                                     'type' : 'Mp8_1',
                                     'inputs' : ['kmop2mp',
                                                 'kmbranch2mp',
                                                 'brOPR',
                                                 'enable'],
                                     'outputs' : ['mp2cnt']},
                                    {'name' : 'cnt',
                                     'type' : 'Reg8',
                                     'inputs' : ['mp2cnt',
                                                 'cnt_ctrl2cnt_ld',
                                                 'cnt_ctrl2cnt_inc',
                                                 'clk'],
                                     'outputs' : ['cnt2dc']},
                                     {'name' : 'dc',
                                      'type' : 'dc8', 
                                      'inputs': ['cnt2dc'],
                                      'outputs' : ['dcout']}]}
    def testDemoCodeGen(self):
        print hdlgen.generate_hdl(self.module)
        return True
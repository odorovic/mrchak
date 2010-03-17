'''
Created on Mar 19, 2009

@author: odor
'''

import pprint
import unittest
import schtools.geometry as g
from schtools import json
from schtools import schematic

import simplejson

class TestNet(object):
    '''Fixture klasa koja oponasha entites.Net'''
    def __init__(self, line, netname = None):
        self.line = line
        self.netname = netname
    def __eq__(self, net):
        return self.line == ner.line
        
        
class TestComponent(object):
    '''Fixture za entites.Component'''
    def __init__(self):
        pass
    
    
class TestPin(object):
    def __init__(self, pnt):
        self.conn_pnt = pnt
        


class TestPointToPointLineMerge(unittest.TestCase):
    def setUp(self):
        self.parallel = [TestNet(g.Line(g.Point(1,1),g.Point(2,1))),
                         TestNet(g.Line(g.Point(2,1),g.Point(3,1)))]
        self.ort = [TestNet(g.Line(g.Point(1,1),g.Point(2,1))),
                         TestNet(g.Line(g.Point(2,1),g.Point(2,2)))]
        self.pNetlist = schematic.Netlist()
        self.pNetlist.append(TestNet(g.Line(g.Point(1,1),g.Point(2,1))))
        self.pNetlist.append(TestNet(g.Line(g.Point(2,1),g.Point(3,1))))
    def testMergeParallelNets(self):
        netlists = schematic.merge_nets(self.parallel, TestNet)
        assert netlists[0] == self.pNetlist
        
class TestClasify(unittest.TestCase):
    '''Testira algoritam translacije liste Schematic entiteta u recnik'''
    def setUp(self):
        '''Postavljamo probnu shemu.
        
        Ovo je jednostavna shema sa 3 io pina i jednim nand kolom.
        '''
        self.schematic = ['v 20080127 1',
                          'C 43000 43700 1 0 0 input-1.sym',
                          '{',
                          'T 43000 44000 5 10 0 0 0 0 1',
                          'device=INPUT',
                          'T 43000 44000 5 10 0 0 0 0 1',
                          'pinlabel=inB',
                          '}',
                          'N 43800 43800 44300 43800 4',
                          'C 44300 43500 1 0 0 7400-1.sym',
                          '{',
                          'T 44800 44400 5 10 0 0 0 0 1',
                          'device=7400',
                          'T 44600 44400 5 10 1 1 0 0 1',
                          'refdes=U1',
                          'T 44800 45750 5 10 0 0 0 0 1',
                          'footprint=DIP14',
                          '}',
                          'C 43000 44100 1 0 0 input-1.sym',
                          '{',
                          'T 43000 44400 5 10 0 0 0 0 1',
                          'device=INPUT',
                          'T 43000 44400 5 10 0 0 0 0 1',
                          'pinlabel=inA',
                          '}',
                          'N 43800 44200 44300 44200 4',
                          'C 44900 42800 1 0 0 output-1.sym',
                          '{',
                          'T 45000 43100 5 10 0 0 0 0 1',
                          'device=OUTPUT',
                          'T 45000 43100 5 10 0 0 0 0 1',
                          'pinlabel=OUT',
                          '}',
                          'N 45600 44000 45600 43300 4',
                          'N 45600 43300 44700 43300 4',
                          'N 44700 43300 44700 42900 4',
                          'N 44700 42900 44900 42900 4']

    def testClasify(self):
        schemlist = schematic._parse(self.schematic.__iter__())
        schemdict = schematic.clasify(schemlist)
        ser = json.serialize(schemdict)
        print
        print simplejson.dumps(simplejson.loads(ser), indent=4)
        assert True
        
'''
Created on Mar 19, 2009

@author: odor
'''

import unittest
import schtools.geometry as g

class TestPoint(unittest.TestCase):
    def setUp(self):
        self.p1 = g.Point(1,2)
        self.p2 = g.Point(2,2)
        self.line = g.Line(g.Point(1,1),g.Point(1,3))
        self.invline = g.Line(g.Point(1,3),g.Point(1,1))
        
    def testTouches(self):
        assert self.p1.touches(self.invline)
        assert self.p1.touches(self.line)
    def testNotTouches(self):
        assert not self.p2.touches(self.invline)
        assert not self.p2.touches(self.line)

class TestLine(unittest.TestCase):
    def setUp(self):
        self.l1 = g.Line(g.Point(1,1), g.Point(3,1))
        self.l2 = g.Line(g.Point(5,2), g.Point(2,2))
        self.l3 = g.Line(g.Point(2,1),g.Point(2,5))
        self.l4 = g.Line(g.Point(3,1),g.Point(3,5))
        
    def testTouches(self):
        assert self.l1.touches(self.l3)
    def testDoesntTouch(self):
        assert not self.l1.touches(self.l2)
        assert not self.l2.touches(self.l4)
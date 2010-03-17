'''
Created on Apr 5, 2009

@author: odor
'''
import unittest

from myhdl import Signal, Simulation, intbv, now
from builtins.components import ClkDriver, EdgeDetector

class Event(object):
    def notify(self):
        print "now: %s. Notified." % (now(),)

class Test(unittest.TestCase):


    def testClkDriver(self):
        clk = Signal(0)
        sim = Simulation(ClkDriver(clk), EdgeDetector(clk, Event() ))
        print
        for i in range(20):
            sim.run(1)
            print "now: %s. clk: %s" % (now(), clk.val)


if __name__ == "__main__":
    #import sys;sys.argv = ['', 'Test.testClkDriver']
    unittest.main()
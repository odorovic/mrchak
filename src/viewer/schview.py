#!/usr/bin/python
'''
Created on Jun 4, 2009

@author: odor
'''

import wx
import sys
from viewer import Viewer
from datasource import TrivialJsonDs

def __main__():
    f = open(sys.argv[1])
    ds = TrivialJsonDs(f)
    app = wx.App()
    viewer = Viewer(ds, None)
    app.MainLoop()
    
if __name__ == '__main__':
    __main__()
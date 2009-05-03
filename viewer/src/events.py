'''Dogadjaji specificni za aplikaciju
Created on May 3, 2009

@author: odor
'''

import wx

class SchChangedEvent(wx.PyCommandEvent):
    '''Dogadjaj promene selektovane sheme.'''
    
    type = wx.NewEventType()
    binder = wx.PyEventBinder(type, 1)
    
    def __init__(self, id):
        wx.PyCommandEvent.__init__(self, SchChangedEvent.type, id)
        
EVT_SCH_CHANGED = SchChangedEvent.binder 

class SimStateEvent(wx.PyCommandEvent):
    '''Dogadjaj promene stanja simulatora.'''
    type = wx.NewEventType()
    binder = wx.PyEventBinder(type, 1)
    
    def __init__(self, id):
        wx.PyCommandEvent.__init__(self, SimStateEvent.type, id)

EVT_SIM_STATE = SimStateEvent.binder

class SimCommandEvent(wx.PyCommandEvent):
    '''Dogadjaj unete komande simulatoru.'''
    type = wx.NewEventType()
    binder = wx.PyEventBinder(type, 1)
    
    def __init__(self, id):
        wx.PyCommandEvent.__init__(self, SimCommandEvent.type, id)

EVT_SIM_CMD = SimCommandEvent.binder
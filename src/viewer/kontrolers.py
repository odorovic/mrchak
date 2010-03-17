'''
Created on Apr 25, 2009

@author: Kuzmanovic Nenad
'''

import wx
import sys
import events
from basichdl import hdl
from wx.py.shell import ShellFrame

class NavBase(wx.Frame):
    '''Bazna klasa za mashinski generisani glade interfejs'''
    def __init__(self, *args, **kwds):
        self.datasource = kwds['datasource']
        del kwds['datasource']
        wx.Frame.__init__(self, *args, **kwds)
        
    def onStepTime(self, event): # wxGlade: NavFrame.<event_handler>
        print "Event handler `onStepTime' not implemented!"
        evt = events.SimCommandEvent(self.GetId())
        evt.command = 'step'
        self.GetEventHandler().ProcessEvent(evt)

    def onStepClk(self, event): # wxGlade: NavFrame.<event_handler>
        evt = events.SimCommandEvent(self.GetId())
        evt.command = 'step clk'
        self.GetEventHandler().ProcessEvent(evt)

    def onStepInst(self, event): # wxGlade: NavFrame.<event_handler>
        evt = events.SimCommandEvent(self.GetId())
        evt.command = 'step instruction_begin'
        self.GetEventHandler().ProcessEvent(evt)

    def onStepProg(self, event): # wxGlade: NavFrame.<event_handler>
        evt = events.SimCommandEvent(self.GetId())
        evt.command = 'step halt'
        self.GetEventHandler().ProcessEvent(evt)   
    
    def init(self):
        tree = self.datasource.manifest['tree']
        root = self.tree.AddRoot(tree[0])
        try:
            self.AddTreeNodes(root, tree[1])
        except IndexError:
            pass    #stablo nema podstabla
        self.tree.Expand(root)
        self.Show(True)
    
    def AddTreeNodes(self, parentItem, items):
        for item in items:
            if isinstance(item, basestring):
                self.tree.AppendItem(parentItem, item)
            else:
                newItem = self.tree.AppendItem(parentItem, item[0])
                self.AddTreeNodes(newItem, item[1])   

    def GetItemText(self, item):
        if item:
            return self.tree.GetItemText(item)
        else:
            return ""

    def onSelect(self, event):
        print "NavBase.onSelect()"
        evt = events.SchChangedEvent(self.GetId())
        evt.newname = self.GetItemText(event.GetItem())
        self.GetEventHandler().ProcessEvent(evt)
        
    def onStep(self, evt):
        print "NavBase.OnStep"
        newevt = events.SimCommandEvent(self.GetId())
        newevt.command = 'step'
        self.GetEventHandler().ProcessEvent(newevt)
        
    def onShell(self, evt):
        print "NavBase.onShell"
        frame = ShellFrame(parent=self)
        frame.Show()
        
    def onStateChange(self, evt):
        print 'NavFrame.onStateChange'
        time = hdl.now()
        clk = hdl.now(clk=True)
        statusbar_fields = ["", "clk: %s" %clk, "vreme: %s" % time]
        for i in range(len(statusbar_fields)):
            self.statusbar.SetStatusText(statusbar_fields[i], i)
        evt.Skip()
        
class SigViewBase(wx.Frame):
    def __init__(self, *args, **kwds):
        self.datasource = kwds['datasource']
        del kwds['datasource']
        wx.Frame.__init__(self, *args, **kwds)
    
    def init(self):
        columns = ['name','width', 'value']
        for index, text in enumerate(columns):
            self.list.InsertColumn(index, text)
        self.Show(True)
        self.signals = {}
    
    def addSignal(self, signal):
        item = signal._name, len(signal), signal.val
        index = self.list.InsertStringItem(sys.maxint, item[0])
        self.signals[signal._name] = index , signal
        for col, text in enumerate(item[1:]):
            self.list.SetStringItem(index, col+1, hex(text))
    
    def onStateChange(self, evt):
        for index, signal in self.signals.values():
            if signal.val == None:
                rep = 'Z'
            else:
                rep = hex(signal.val)
            self.list.SetStringItem(index, 2, rep)     
    
    def onSigAdd(self, event): # wxGlade: SigViewFrame.<event_handler>
        print "Event handler `onSigAdd' not implemented!"
        event.Skip()

    def onSigRemove(self, event): # wxGlade: SigViewFrame.<event_handler>
        print "Event handler `onSigRemove' not implemented!"
        event.Skip()

    def onRevert(self, event): # wxGlade: SigViewFrame.<event_handler>
        print "Event handler `onRevert' not implemented!"
        event.Skip()
        
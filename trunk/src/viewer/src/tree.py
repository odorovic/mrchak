'''
Created on May 17, 2009

@author: odor
'''

import wx
tree = ['sranje', ['big_one',['1','2','2']]]

class SchemSelectorPanel(wx.Panel):
    def __init__(self, parent):
        wx.Panel.__init__(self, parent, -1, size=(400,500))
        self.tree = wx.TreeCtrl(self)
        root = self.tree.AddRoot("wx.Object")
        self.AddTreeNodes(root, tree)
    
    def AddTreeNodes(self, parentItem, items):
        for item in items:
            if type(item) == str:
                self.tree.AppendItem(parentItem, item)
            else:
                newItem = self.tree.AppendItem(parentItem, item[0])
                self.AddTreeNodes(newItem, item[1])


class TestFrame(wx.Frame):
    def __init__(self):
        wx.Frame.__init__(self, None,
            title="simple tree", size=(400,500))
        self.tree = wx.TreeCtrl(self)
        root = self.tree.AddRoot("wx.Object")
        self.AddTreeNodes(root, tree)
        self.Bind(wx.EVT_TREE_ITEM_EXPANDED,
                  self.OnItemExpanded,
                  self.tree)
        self.Bind(wx.EVT_TREE_ITEM_COLLAPSED,
                  self.OnItemCollapsed,
                  self.tree)
        self.Bind(wx.EVT_TREE_SEL_CHANGED,
                  self.OnSelChanged, self.tree)
        self.Bind(wx.EVT_TREE_ITEM_ACTIVATED,
                  self.OnActivated, self.tree)
        self.tree.Expand(root)
          
    def GetItemText(self, item):
        if item:
            return self.tree.GetItemText(item)
        else:
            return ""

    def AddTreeNodes(self, parentItem, items):
        for item in items:
            if type(item) == str:
                self.tree.AppendItem(parentItem, item)
            else:
                newItem = self.tree.AppendItem(parentItem, item[0])
                self.AddTreeNodes(newItem, item[1])
          
    def OnItemExpanded(self, evt):
        print "OnItemExpanded: ", self.GetItemText(evt.GetItem())
    
    def OnItemCollapsed(self, evt):
        print "OnItemCollapsed:", self.GetItemText(evt.GetItem())
    
    def OnSelChanged(self, evt):
        print "OnSelChanged:     ", self.GetItemText(evt.GetItem())
    
    def OnActivated(self, evt):
        print "OnActivated:      ", self.GetItemText(evt.GetItem())
    
app = wx.PySimpleApp(redirect=False)
frame = TestFrame()
frame.Show()
app.MainLoop()

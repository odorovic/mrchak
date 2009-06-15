#!/usr/bin/env python
# -*- coding: utf-8 -*-
# generated by wxGlade 0.6.3 on Wed May 20 23:15:21 2009

import wx
import sys

# begin wxGlade: extracode
from kontrolers import NavBase
from kontrolers import SigViewBase
# end wxGlade

ID_STEP_TIME = wx.NewId()
ID_STEP_CLK = wx.NewId()
ID_STEP_INST = wx.NewId()
ID_STEP_PROG = wx.NewId()

class NavFrame(NavBase):
    def __init__(self, *args, **kwds):
        # begin wxGlade: NavFrame.__init__
        kwds["style"] = wx.DEFAULT_FRAME_STYLE
        NavBase.__init__(self, *args, **kwds)
        self.panel_1 = wx.Panel(self, -1)
        
        # Menu Bar
        self.frame_1_menubar = wx.MenuBar()
        self.File = wx.Menu()
        self.quit = wx.MenuItem(self.File, wx.NewId(), "Quit", "", wx.ITEM_NORMAL)
        self.File.AppendItem(self.quit)
        self.frame_1_menubar.Append(self.File, "File")
        self.View = wx.Menu()
        self.Shell = wx.MenuItem(self.View, wx.NewId(), "Shell", "", wx.ITEM_CHECK)
        self.View.AppendItem(self.Shell)
        self.frame_1_menubar.Append(self.View, "View")
        self.SetMenuBar(self.frame_1_menubar)
        # Menu Bar end
        self.statusbar = self.CreateStatusBar(3, 0)
        
        # Tool Bar
        self.toolbar = wx.ToolBar(self, -1)
        self.SetToolBar(self.toolbar)
        self.toolbar.AddSeparator()
        self.toolbar.AddLabelTool(ID_STEP_TIME, "time+", wx.Bitmap("/home/odor/download/tango-icon-theme-0.8.90/22x22/actions/appointment-new.png", wx.BITMAP_TYPE_ANY), wx.NullBitmap, wx.ITEM_NORMAL, "Sledeci vrem. kvant.", "Sledeci vrem. kvant.")
        self.toolbar.AddLabelTool(ID_STEP_CLK, "clk+", wx.Bitmap("/home/odor/download/tango-icon-theme-0.8.90/22x22/actions/go-next.png", wx.BITMAP_TYPE_ANY), wx.NullBitmap, wx.ITEM_NORMAL, "Sledeci takt", "Sledeci takt")
        self.toolbar.AddLabelTool(ID_STEP_INST, "inst+", wx.Bitmap("/home/odor/download/tango-icon-theme-0.8.90/22x22/actions/go-jump.png", wx.BITMAP_TYPE_ANY), wx.NullBitmap, wx.ITEM_NORMAL, "Sledeca instrukcija", "Sledeca instrukcija")
        self.toolbar.AddLabelTool(ID_STEP_PROG, "prog+", wx.Bitmap("/home/odor/download/tango-icon-theme-0.8.90/22x22/actions/go-last.png", wx.BITMAP_TYPE_ANY), wx.NullBitmap, wx.ITEM_NORMAL, "Kraj programa.", "Kraj programa.")
        self.toolbar.AddSeparator()
        self.toolbar.AddLabelTool(wx.NewId(), "item", wx.Bitmap("/home/odor/download/tango-icon-theme-0.8.90/22x22/apps/utilities-system-monitor.png", wx.BITMAP_TYPE_ANY), wx.NullBitmap, wx.ITEM_NORMAL, "", "")
        self.toolbar.AddLabelTool(wx.NewId(), "item", wx.Bitmap("/home/odor/download/tango-icon-theme-0.8.90/22x22/apps/utilities-terminal.png", wx.BITMAP_TYPE_ANY), wx.NullBitmap, wx.ITEM_NORMAL, "", "")
        self.toolbar.AddLabelTool(wx.NewId(), "item", wx.Bitmap("/home/odor/download/tango-icon-theme-0.8.90/22x22/mimetypes/image-x-generic.png", wx.BITMAP_TYPE_ANY), wx.NullBitmap, wx.ITEM_NORMAL, "", "")
        # Tool Bar end
        self.static_line_1 = wx.StaticLine(self.panel_1, -1)
        self.tree = wx.TreeCtrl(self.panel_1, -1, style=wx.TR_HAS_BUTTONS|wx.TR_NO_LINES|wx.TR_DEFAULT_STYLE|wx.SUNKEN_BORDER)

        self.__set_properties()
        self.__do_layout()

        self.Bind(wx.EVT_MENU, self.onShell, self.Shell)
        self.Bind(wx.EVT_TOOL, self.onStepTime, id=ID_STEP_TIME)
        self.Bind(wx.EVT_TOOL, self.onStepClk, id=ID_STEP_CLK)
        self.Bind(wx.EVT_TOOL, self.onStepInst, id=ID_STEP_INST)
        self.Bind(wx.EVT_TOOL, self.onStepProg, id=ID_STEP_PROG)
        self.Bind(wx.EVT_TREE_BEGIN_DRAG, self.onSelect, self.tree)
        self.Bind(wx.EVT_TREE_SEL_CHANGED, self.onSelect, self.tree)
        # end wxGlade

    def __set_properties(self):
        # begin wxGlade: NavFrame.__set_properties
        self.SetTitle("Kontroler SImulatora")
        self.SetSize((300, 300))
        self.statusbar.SetStatusWidths([-1, -1, -1])
        # statusbar fields
        statusbar_fields = ["", "clk: 0", "vreme: 0"]
        for i in range(len(statusbar_fields)):
            self.statusbar.SetStatusText(statusbar_fields[i], i)
        self.toolbar.Realize()
        # end wxGlade

    def __do_layout(self):
        # begin wxGlade: NavFrame.__do_layout
        sizer_1 = wx.BoxSizer(wx.VERTICAL)
        sizer_2 = wx.BoxSizer(wx.VERTICAL)
        sizer_2.Add(self.static_line_1, 0, wx.EXPAND, 0)
        sizer_2.Add(self.tree, 1, wx.EXPAND, 0)
        self.panel_1.SetSizer(sizer_2)
        sizer_1.Add(self.panel_1, 1, wx.EXPAND, 0)
        self.SetSizer(sizer_1)
        self.Layout()
        # end wxGlade

# end of class NavFrame


class SigViewFrame(SigViewBase):
    def __init__(self, *args, **kwds):
        # begin wxGlade: SigViewFrame.__init__
        kwds["style"] = wx.DEFAULT_FRAME_STYLE
        SigViewBase.__init__(self, *args, **kwds)
        self.panel_1 = wx.Panel(self, -1)
        
        # Tool Bar
        self.toolbar = wx.ToolBar(self, -1)
        self.SetToolBar(self.toolbar)
        self.toolbar.AddSeparator()
        self.toolbar.AddLabelTool(ID_STEP_TIME, "add", wx.Bitmap("/home/odor/download/tango-icon-theme-0.8.90/22x22/actions/list-add.png", wx.BITMAP_TYPE_ANY), wx.NullBitmap, wx.ITEM_NORMAL, "Dodaj signal", "Dodaj signal")
        self.toolbar.AddLabelTool(ID_STEP_CLK, "remove", wx.Bitmap("/home/odor/download/tango-icon-theme-0.8.90/22x22/actions/list-remove.png", wx.BITMAP_TYPE_ANY), wx.NullBitmap, wx.ITEM_NORMAL, "Ukloni signal", "Ukloni signal")
        self.toolbar.AddLabelTool(ID_STEP_INST, "revert", wx.Bitmap("/home/odor/download/tango-icon-theme-0.8.90/22x22/actions/edit-clear.png", wx.BITMAP_TYPE_ANY), wx.NullBitmap, wx.ITEM_NORMAL, "Ponisti izmene", "Ponisti izmene signala.")
        # Tool Bar end
        self.static_line_1 = wx.StaticLine(self.panel_1, -1)
        self.list = wx.ListCtrl(self.panel_1, -1, style=wx.LC_REPORT|wx.SUNKEN_BORDER)

        self.__set_properties()
        self.__do_layout()

        self.Bind(wx.EVT_TOOL, self.onSigAdd, id=ID_STEP_TIME)
        self.Bind(wx.EVT_TOOL, self.onSigRemove, id=ID_STEP_CLK)
        self.Bind(wx.EVT_TOOL, self.onRevert, id=ID_STEP_INST)
        # end wxGlade

    def __set_properties(self):
        # begin wxGlade: SigViewFrame.__set_properties
        self.SetTitle("Kontroler SImulatora")
        self.SetSize((300, 300))
        self.toolbar.Realize()
        # end wxGlade

    def __do_layout(self):
        # begin wxGlade: SigViewFrame.__do_layout
        sizer_1 = wx.BoxSizer(wx.VERTICAL)
        sizer_2 = wx.BoxSizer(wx.VERTICAL)
        sizer_2.Add(self.static_line_1, 0, wx.EXPAND, 0)
        sizer_2.Add(self.list, 1, wx.EXPAND, 0)
        self.panel_1.SetSizer(sizer_2)
        sizer_1.Add(self.panel_1, 1, wx.EXPAND, 0)
        self.SetSizer(sizer_1)
        self.Layout()
        # end wxGlade

if __name__ == "__main__":
    app = wx.PySimpleApp(0)
    wx.InitAllImageHandlers()
    frame_1 = NavFrame(None, -1, "")
    app.SetTopWindow(frame_1)
    frame_1.Show()
    app.MainLoop()

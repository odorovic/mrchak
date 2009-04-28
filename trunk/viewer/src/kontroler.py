'''
Created on Apr 25, 2009

@author: Kuzmanovic Nenad
'''

import wx

class Kontroler(wx.App):

    def OnInit(self):
        frame = MyFrame("Kontroler", (50, 60), (600, 150),
                        style=wx.DEFAULT_FRAME_STYLE ^ (wx.RESIZE_BORDER | wx.MAXIMIZE_BOX))
        frame.Show()
        self.SetTopWindow(frame)
        return True
    
class MyFrame(wx.Frame):
    
    def __init__(self, title, pos, size, style):
        
        wx.Frame.__init__(self, None, -1, title, pos, size, style)
        
        self.inc = 1 # Inkrementiranje clocka #
        
        self.buildMenu()
        
        panel = wx.Panel(self, -1)
        sizer = wx.FlexGridSizer(rows=1)
        panel.SetSizer(sizer)

        self.buttonStep = wx.Button(panel, label="Step")
        self.Bind(wx.EVT_BUTTON,self.OnStep,self.buttonStep)
        sizer.Add(self.buttonStep)
        
        self.buttonGoTo = wx.Button(panel, label = "GoTo")
        self.Bind(wx.EVT_BUTTON,self.OnGoTo,self.buttonGoTo)
        sizer.Add(self.buttonGoTo)
        
        self.buttonStep.SetDefault()
        self.buttonGoTo.SetDefault()
        
        a = wx.StaticText(panel, -1, "Step size", (210, 5))
        b = wx.StaticText(panel, -1, "Go To", (320, 5))
        sizer.Add(a)
        sizer.Add(b)
        
        sampleList = ['1', 'Mem_clk', 'CPU_clk']
        self.Lista = wx.ComboBox(panel, -1, "1", (200, 25), wx.DefaultSize,sampleList, wx.CB_DROPDOWN)
        self.Bind(wx.EVT_COMBOBOX, self.OnList, self.Lista)
        sizer.Add(self.Lista)
        
        self.goToField = wx.TextCtrl(panel, -1, "314",pos = (310,25) ,size = (60, 20))
        sizer.Add(self.goToField)
        
        self.c = wx.StaticText(panel, -1, "CLK:", pos = (205,50))
        self.clk = wx.StaticText(panel, -1, "0", pos = (230,50))
        sizer.Add(self.c)
        sizer.Add(self.clk)
        
        panel.Fit()


    def buildMenu(self):
        menuFile = wx.Menu()
        menuFile.Append(1, "&About This...")
        menuFile.AppendSeparator()
        menuFile.Append(2, "E&xit")
        menuBar = wx.MenuBar()
        menuBar.Append(menuFile, "&File")
        self.SetMenuBar(menuBar)
        self.CreateStatusBar()
        self.SetStatusText("How can I please you ser?")
        self.Bind(wx.EVT_MENU, self.OnAbout, id=1)
        self.Bind(wx.EVT_MENU, self.OnQuit, id=2)

        
        
        
    def OnStep(self,event):
        Y = self.clk.GetLabelText()
        X = int(Y)
        X = X + self.inc
        Y = str(X)
        self.clk.SetLabel(Y)
        
    def OnGoTo(self,event):
        Y = self.goToField.GetLabel()   #Potreban oporavak ili zastita od upisa stringa u GoToFIled
        self.clk.SetLabel(Y)            #tj kad neko upise "asdfasdf" i onda hoce Step.
                                        #Probaj ako mi ne verujes
                                        #al svejedno izbacuje kad se to uradi :)
                                        
    def OnList(self,event):
        if self.Lista.GetLabel()=='1':
            self.inc = 1
            self.SetStatusText("Step By Step")
        else:
            if self.Lista.GetLabel()=='CPU_clk':
                self.inc = 100
                self.SetStatusText("CPU_clk")
            else:
                self.inc = 50
                self.SetStatusText("MEM_clk")
                
        
        
        
            
    
    def OnQuit(self, event):
        self.Close()
        
    def OnAbout(self, event):
        wx.MessageBox("This window is property of Kuzma so FUCK OFF!!",
                      "Kuzma The King", wx.OK | wx.ICON_INFORMATION, self)
        
if __name__ == '__main__':
    ktr = Kontroler(False)
    ktr.MainLoop()
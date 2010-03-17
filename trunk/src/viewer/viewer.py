'''Modul za iscrtavanje shema.

Ovaj modul definishe klase/funkcije za isctavanje Shema u wx prozore.
Format shema je json enkoding kakav proizvodi schdump, dok je Widget od kojeg
polazimo sa implementacijom wx-ov floatcanvas
'''

import wx
import simplejson
from wx.lib.floatcanvas import NavCanvas
from wx.lib.floatcanvas import FloatCanvas
import os
import sys

import events
import adapters
import datasource
import kontrolers
import glade

properties = {}

class Viewer(wx.Frame):
    '''Viewer klasa.
    
    Prikazuje zeljenu shemu na floatcanvas.NavCanvas podklasi.
    '''
    def __init__(self, datasource, *args, **kwargs):
        '''Konstruktor Viewera.'''
        wx.Frame.__init__(self, *args, **kwargs)
        NC = NavCanvas.NavCanvas(self,
                                 Debug = 0,
                                 BackgroundColor = adapters.pallete['BACKGROUND_COLOR'])
        self.Canvas = NC.Canvas
        self.datasource = datasource
        self.populate()
        
        S = wx.BoxSizer(wx.VERTICAL)
        S.Add(NC, 1, wx.EXPAND)
        self.SetSizer(S)
        wx.CallAfter(self.Canvas.ZoomToBB)
        self.Show()
    
    def populate(self):
        '''Iscrtava ekran na osnovu data-sorsa.'''
        assert self.datasource != None
        self.Canvas.ClearAll()
        for element in self.datasource.items():
            self.Canvas.AddObject(element)
            try:
                element.onAfterAdd()
            except:
                pass
        self.datasource.onStateChange(None)
    
    def onStateChange(self, state):
        '''Hendler za promenu stanja sheme koju Viewver prikazuje'''
        
        wx.CallAfter(self.Canvas.Draw, **{'Force':True})
        state.Skip()
    
    def onSchematicChange(self, schevent):
        '''Hendler koji se poziva u slucaju promene topologije sheme'''
        print 'onschchange'
        newname = schevent.newname
        self.datasource.current_sch = newname
        self.populate()
        wx.CallAfter(self.Canvas.ZoomToBB)
    
class AorApp(wx.App):
    '''Centralna tacka celokupne aplikacije.
    
    Aor simulator se sastoji iz sledecih komponenata:
    Viewer -- gui prikaz sheme koja se simulira
    Selector -- gui pretrazivac sadrzaja procesora
    Controller -- gui Applet za kontrolu toka ismulacije
    Simulator -- Interfejs ka myhdl simulaciji
    Datasource -- Pasivni izvor topologije hardvera.
    '''
    
    @classmethod
    def get_instance(cls):
        return properties['instance']
    
    @classmethod
    def set_instance(cls, instance):
        properties['instance'] = instance
    
    def __init__(self, prj_path='/home/odor/workspace/gschem/procesor'):
        wx.App.__init__(self)
        AorApp.set_instance(self)
        print AorApp.get_instance()
        print properties
        self.datasource = datasource.ProjectDs(prj_path)
        self.navframe = glade.NavFrame(None, wx.ID_ANY, '', datasource = self.datasource)
        self.sigviewframe = glade.SigViewFrame(None, wx.ID_ANY, '', datasource = self.datasource)
        self.viewer = Viewer(self.datasource, 
                             self.navframe,
                             title='Schematic Viewer',
                             size=(700,480))
        
        self.simulator = self.datasource.simulator
        self.sigviewframe.init()
        self.navframe.init()
        
        self.Bind(events.EVT_SCH_CHANGED, self.viewer.onSchematicChange)
        self.Bind(events.EVT_SIM_CMD, self.onSimCommand, self.navframe)
        #self.Bind(events.EVT_SIM_STATE, self.viewer.onStateChange)
        #self.Bind(events.EVT_SIM_STATE, navframe.onStateChange)
        self.Bind(events.EVT_SIG_VIEW, self.onSigView, self.viewer.Canvas)
        
        self.MainLoop()
    
    def onSigView(self, evt):
        sig = self.datasource.getsignal(evt.signame)
        self.sigviewframe.addSignal(sig)
    
    def onSimCommand(self, evt):
        print "AorApp.onSimCommand"
        self.simulator.onSimCommand(evt)
        self.datasource.onStateChange(evt)
        self.viewer.onStateChange(evt)
        self.navframe.onStateChange(evt)
        self.sigviewframe.onStateChange(evt)
        
if __name__ == '__main__':
    #sys.stderr = open(os.devnull, "w")
    AorApp()
        

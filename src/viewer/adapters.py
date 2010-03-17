'''Adapteri oblika za crtanje.

Skup klasa adaptera izmedju FloatCanvas primitiva za crtanje i gschem primitiva.
'''

import sys
from wx.lib.floatcanvas import FloatCanvas
from schtools import entities
import events
import myhdl
from math import sin,cos
from viewer import AorApp

try:
    from viewer import adapters
except ImportError:
    adapters = sys.modules[globals()['__name__']]

colors = {0: 'BACKGROUND_COLOR',
          1: 'PIN_COLOR',
          2: 'NET_ENDPOINT_COLOR',
          3: 'GRAPHIC_COLOR',
          4: 'NET_COLOR',
          5: 'ATTRIBUTE_COLOR',
          6: 'LOGIC_BUBBLE_COLOR',
          7: 'DOTS_GRID_COLOR',
          8: 'DETACHED_ATTRIBUTE_COLOR',
          9: 'TEXT_COLOR',
          10: 'BUS_COLOR',
          11: 'SELECT_COLOR',
          12: 'BOUNDINGBOX_COLOR',
          13: 'ZOOM_BOX_COLOR',
          14: 'STROKE_COLOR',
          15: 'LOCK_COLOR',
          16: 'OUTPUT_BACKGROUND_COLOR',
          17: 'FREESTYLE1_COLOR',
          18: 'FREESTYLE2_COLOR',
          19: 'FREESTYLE3_COLOR',
          20: 'FREESTYLE4_COLOR',
          21: 'JUNCTION_COLOR',
          22: 'MESH_GRID_MAJOR_COLOR',
          23: 'MESH_GRID_MINOR_COLOR'}

tango = {
                  'Butter': ['#fce94f', '#edd400', '#c4a000'],
                  'Orange': ['#fcaf3e', '#f57900', '#ce5c00'],
                  'Chocolate': ['#e9b96e', '#c17d11', '8#f5902'],
                  'Chameleon': ['#8ae234', '#73d216', '#4e9a06'],
                  'Sky Blue':  ['#729fcf', '#3465a4', '#204a87'],
                  'Plum': ['#ad7fa8', '#75507b', '#5c3566'],
                  'Scarlet Red': ['#ef2929', 'cc0000', '#a40000'],
                  'Aluminium': ['#eeeeec', '#d3d7cf', '#babdb6',
                                '#888a85', '#555753', '#2e3436']
                  
                  }

geda_pallete = {'BACKGROUND_COLOR' : 'BLACK',
           'PIN_COLOR' : 'WHITE',
           'NET_ENDPOINT_COLOR' : 'RED',
           'GRAPHIC_COLOR' : 'GREEN',
           'NET_COLOR' : 'BLUE',
           'ATTRIBUTE_COLOR' : 'YELLOW',
           'LOGIC_BUBBLE_COLOR' : 'CYAN',
           'DOTS_GRID_COLOR' : 'GRAY',
           'DETACHED_ATTRIBUTE_COLOR' : 'RED',
           'TEXT_COLOR' : 'GREEN',
           'BUS_COLOR' : 'GREEN',
           'SELECT_COLOR' : 'ORANGE',
           'BOUNDINGBOX_COLOR' : 'ORANGE',
           'ZOOM_BOX_COLOR' : 'GREEN',
           'STROKE_COLOR' : 'GREEN',
           'LOCK_COLOR' : 'GREEN',
           'OUTPUT_BACKGROUND_COLOR' : 'GREEN',
           'FREESTYLE1_COLOR' : 'GREEN',
           'FREESTYLE2_COLOR' : 'GREEN',
           'FREESTYLE3_COLOR' : 'GREEN',
           'FREESTYLE4_COLOR' : 'GREEN',
           'JUNCTION_COLOR' : 'YELLOW',
           'MESH_GRID_MAJOR_COLOR' : 'GREEN',
           'MESH_GRID_MINOR_COLOR' : 'GREEN'}

tango_pallete = {'BACKGROUND_COLOR' : tango['Aluminium'][5],
           'PIN_COLOR' : '#eeeeec',
           'NET_ENDPOINT_COLOR' : '#cc0000',
           'GRAPHIC_COLOR' : '#73d216',
           'NET_COLOR' : '#3465a4',
           'ATTRIBUTE_COLOR' : '#fce94f',
           'LOGIC_BUBBLE_COLOR' : '#729fcf',
           'DOTS_GRID_COLOR' : '#888a85',
           'DETACHED_ATTRIBUTE_COLOR' : '#cc0000',
           'TEXT_COLOR' : '#73d216',
           'BUS_COLOR' : '#cc0000',
           'SELECT_COLOR' : '#f57900',
           'BOUNDINGBOX_COLOR' : '#73d216',
           'ZOOM_BOX_COLOR' : '#cc0000',
           'STROKE_COLOR' : '#cc0000',
           'LOCK_COLOR' : '#cc0000',
           'OUTPUT_BACKGROUND_COLOR' : '#cc0000',
           'FREESTYLE1_COLOR' : '#cc0000',
           'FREESTYLE2_COLOR' : '#cc0000',
           'FREESTYLE3_COLOR' : '#cc0000',
           'FREESTYLE4_COLOR' : '#cc0000',
           'JUNCTION_COLOR' : '#fce94f',
           'MESH_GRID_MAJOR_COLOR' : '#cc0000',
           'MESH_GRID_MINOR_COLOR' : '#cc0000',
           'SIGNAL_LOW' : tango['Sky Blue'][2],
           'SIGNAL_HIGH': tango['Scarlet Red'][0],
           'SIGNAL_BUS' : tango['Plum'][1],
           'SIGNAL_HIGHZ': tango['Chameleon'][0]}

pallete = tango_pallete

alignments = {0: 'bl',
           1: 'cl',
           2: 'tl',
           3: 'bc',
           4: 'cc',
           5: 'tc',
           6: 'br',
           7: 'cr',
           8: 'tr',}

def get_color(index):
    return pallete[colors[index]]

def get_name(repr):
    '''Vraca ime klase koja treba da reprezentuje json objekat.
    
    Glavani deo selekcije se odradjuje poredjenjem base_attrs.type polja sa 
    kodovima polja iz entitet klasa. Posle toga imamo hack koji diferencira
    (valjda jednom zasvagda) Text i Atribute.
    '''
    typecode = repr['base_attrs']['type']
    typename = entities.entity_dict[typecode].__name__
    #Budutji ja: IZVINI!!!!!!!!
    if typename == 'Text':
        if repr['base_attrs'].has_key('key') and repr['base_attrs'].has_key('attr'):
            return 'Attribute'
        else:
            return 'Text'
    else:
        return typename

def _adapter(repr):
    try:
        typename = get_name(repr)
        cls = getattr(adapters, typename)
        return cls(repr)
    except AttributeError, err:
        raise Exception('Ne postoji adapter za trazenu klasu: %s' % typename)
    
def adapter(repr):
    '''Fabrika adaptera.'''
    try:
        yield _adapter(repr)
        if repr.has_key('ext_attrs'):
            for extattr in repr['ext_attrs'].values():
                yield _adapter(extattr) 
    except AttributeError, err:
        raise Exception('Ne postoji adapter za trazenu klasu: %s' % typename)

class Line(FloatCanvas.Line):
    '''Adapter izmedju gschem linije i floatcanvas linije.'''
    def __init__(self, repr):
        '''Instancijra liniju na osnovu json reprezentacije.
        
        Argumenti:
            repr : dict --Pogledati aor2.schtools.entites.Line za referencu 
                o izgledu repr recnika.
        '''
        attrs = repr['base_attrs']
        line = attrs['line']
        color = get_color(attrs['color'])
        FloatCanvas.Line.__init__(self, line, LineColor=color, )
        
class Circle(FloatCanvas.Circle):
        def __init__(self, repr):
            attrs = repr['base_attrs']
            point = attrs['point']
            diameter =  2 * attrs['radius']
            color = get_color(attrs['color'])
            FloatCanvas.Circle.__init__(self, point, diameter, LineColor=color)

class Arc(FloatCanvas.Arc):
        def __init__(self, repr):
            def triangulate(pnt, radius, angle):
                def rad(deg):
                    return deg * 0.0174532925
                
                x,y = pnt
                nx = x + radius * cos(rad(angle))
                ny = y + radius * sin(rad(angle))
                return nx, ny
                
            attrs = repr['base_attrs']
            point = attrs['point']
            radius =  attrs['radius']
            startangle =  attrs['startangle']
            sweepangle =  attrs['sweepangle']
            endangle = (startangle + sweepangle) % 360
            startpnt = triangulate(point, radius, startangle)
            endpnt = triangulate(point, radius, endangle)
            color = get_color(attrs['color'])
            FloatCanvas.Arc.__init__(self, 
                                     startpnt, 
                                     endpnt,
                                     point, 
                                     LineColor=color)
            
class Pin(Line):
    def __init__(self, repr):
        Line.__init__(self, repr)
        
class Box(FloatCanvas.Rectangle):
    '''Adapter izmedju gschem boxa i floatcanvas recta.'''
    def __init__(self, repr):
        attrs = repr['base_attrs']
        point = attrs['point']
        wh = (attrs['wh'])
        color = get_color(attrs['color'])
        FloatCanvas.Rectangle.__init__(self, point, wh, LineColor=color)   
        
class Text(FloatCanvas.ScaledTextBox):
    '''Adapter izmedju gschem Texta i floatcanvas TextBoxa.'''
    def __init__(self, repr):
        '''Konstruktor texta.
        
        Za sada samo dampuejemo text atribut iz recnika.ipak, izgleda da sam
        text resio dosta nedosledno. Tu tje morati da se radi.
        '''
        attrs = repr['base_attrs']
        point = attrs['point']
        size = int(attrs['size']) * 15
        color = get_color(attrs['color'])
        if attrs['visibility'] == 1:
            text = attrs['text']
        else:
            text = ''
        align = alignments[attrs['alignment']]
        FloatCanvas.ScaledTextBox.__init__(self,
                                           text,
                                           point,
                                           size,
                                           Color=color,
                                           PadSize = 0,
                                           LineColor= None,
                                           LineSpacing = 0,
                                           Position = align)

class Attribute(Text):
    '''Adapter izmedju atributa i TextBoxa.
    
    Atribut je key=val tekstualni par, sa slicnom json reprezentacijom kao Text.
    
    Dokumentacija seriajlizovane reprezentcije:
    show_name_value = int atribut koji definise nacin crtanja atributa.
        SHOW NAME VALUE = 0 (show both name and value of an attribute)
        SHOW VALUE = 1 (show only the value of an attribute)
        SHOW NAME = 2 (show only the name of an attribute)
    '''
    def __init__(self, repr):        
        attrs = repr['base_attrs']
        text_mapping = {0: attrs['text'],
                        1: attrs['attr'],
                        2: attrs['key']}
        show_name_value = attrs["show_name_value"]
        attrs['text'] = text_mapping[show_name_value]
        Text.__init__(self, repr)
        
def fhex(value):
    if value == None:
        return 'Z'
    digits = len(value) / 4 + 1
    padding = ''
    repr = hex(value)
    assert repr.startswith('0x'), repr
    repr = repr[2:]
    if repr.endswith('L'):
        repr = repr[:-1]
    else:
        print '!!!Repr signala: %s' % repr
    if len(repr) != digits:
        padding = '0' * ((len(repr)-digits)/4 + 1)
    return '0x' + padding + repr

def fbin(value):
    if value == None:
        return 'Z'
    repr = myhdl.bin(value)
    padding = '0' * (len(value) - len(repr))
    return '0b' + padding + repr    

def fctrlsigs(value):
    '''Ovde cinimo nepojmljivi zlocin protiv single point of knoledge.
    
    Po pedeseti put listamo raspolozive signale. Dem. Dear future me: I'm sorry.
    '''
    signals = (None, 'MARin', 'MBRin','MBRout', 'write', 'read',
               'INTRaccept','IVTPout','IADDRout', 'G', 'ALUadd', 'ALUdec', 
               'ALUinc', 'ALUop', 'ALUout', 'ALUtrans', 'Yin', 'YselM1',
                'Ysignex', 'PSWupdateIT', 'PSWin', 'PSWout','PSWupdateNZCV',
                'PSWinta','IR1in','IR2in','IR3in','IR4in','IR2out','IR3out',
                'IR4out','regsel2', 'PCin','PCout',None,None,'SPin','SPout', 
                'REGin', 'REGout', 'TEMPinHIGH','TEMPinLOW','TEMPout',
                'TEMPswap','TEMP2inHIGH','TEMP2inLOW','TEMP2out','TEMP2swap')
    active = []
    opsigword = value[48:0]
    for bit, signame in zip(opsigword, signals):
        if bit == True:
            active.append(signame)
    return ','.join(active)
    
class Viewer(Text):
    def __init__(self, repr):
        formats = {'0x':fhex, '0b': fbin, 'uMEM' : fctrlsigs}
        attrs = repr['base_attrs']
        self.format = formats[attrs['attr']]
        self.value = 0
        self.name = None
        Text.__init__(self, repr)
    
    def setValue(self, val):
        self.value = val
        if self.name:
            name = self.name + ': '
        else:
            name = ''
        self.SetText('%s%s' % (name, self.format(self.value)))
    
class Net(Line):
    def __init__(self, repr):
        Line.__init__(self, repr)
        
        
class Signal(FloatCanvas.Group):
    '''Reprezetacija signala.
    
    Signal se crta kao grupa svih svojih zica i tekstualnih atributa.
    Za razliku od svih ostalih adaptera koji su pasivni, signali menjaju izgled,
    odrazavajuci stanje simulatora.
    '''
    def __init__(self, repr):
        self.name = repr['netname']
        try:
            self.width = int(repr['width'])
        except KeyError:
            self.width = 1
        self.nets = []
        self.attributes = []
        self.viewers = []
        for netrepr in repr['nets']:
            self.nets.append(Net(netrepr))
            for key, val in netrepr['ext_attrs'].items():
                if key == 'viewer':
                    self.viewers.append(Viewer(val))
                else:
                    self.attributes.append(Attribute(val))
        if self.width > 1:
            for net in self.nets:
                net.SetLineWidth(4)
        self.dirty = True
        self.value = 0
        try:
            self.set_state(int(repr['value'])) 
        except KeyError:
            pass
        FloatCanvas.Group.__init__(self, self.nets+self.attributes+self.viewers)
    
    def onAfterAdd(self):
        def onClick(evt):
            print "onCLick u signalu"
            evt = events.SignalViewEvent(self._Canvas.GetId())
            evt.signame = self.name
            self._Canvas.GetEventHandler().ProcessEvent(evt)            
        self.Bind(FloatCanvas.EVT_FC_LEFT_DCLICK, onClick)
    
    def SetColor(self, color):
        for net in self.nets:
            net.SetColor(color)
    
    def set_state(self, state):
        '''Menja stanje signala.
        '''
        colors = {0: pallete['SIGNAL_LOW'], 
                  1: pallete['SIGNAL_HIGH'],
                  'bus': pallete['SIGNAL_BUS'], 
                  'high-Z': pallete['SIGNAL_HIGHZ']}
        newcolor = None
        if self.value != state:
            self.dirty = True
            if state == None:
                newcolor = colors['high-Z']
            elif state < 2:
                newcolor = colors[int(state)]
            else:
                newcolor = colors['bus']
            self.value = state
            self.SetColor(newcolor)
            for viewer in self.viewers:
                viewer.setValue(state)

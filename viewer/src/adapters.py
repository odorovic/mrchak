'''Adapteri oblika za crtanje.

Skup klasa adaptera izmedju FloatCanvas primitiva za crtanje i gschem primitiva.
'''

from wx.lib.floatcanvas import FloatCanvas
from schtools import entities
from viewer import adapters

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

pallete = {'BACKGROUND_COLOR' : 'BLACK',
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

def get_color(index):
    return pallete[colors[index]]

def get_name(typecode):
    return entities.entity_dict[typecode].__name__

def adapter(repr):
    '''Fabrika adaptera.'''
    try:
        typename =get_name(repr['base_attrs']['type'])
        cls = getattr(adapters, typename)
    except AttributeError, err:
        raise Exception('Ne postoji adapter za trazenu klasu: %s' % typename)
    return cls(repr)

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
        FloatCanvas.Line.__init__(self, line, LineColor=color)
        
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
        size = int(attrs['size']) * 20
        color = get_color(attrs['color'])
        text = attrs['text']
        FloatCanvas.ScaledTextBox.__init__(self,
                                           text,
                                           point,
                                           size,
                                           Color=color,
                                           LineColor=None,
                                           Position = 'bl')

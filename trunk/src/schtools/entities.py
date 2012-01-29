'''Fabrika i baza validnih entiteta u sch fajlu.
'''

import inspect
import entities as ents
import geometry as g
import jsonWrapper
class OpenBrace(Exception): pass
class ClosedBrace(Exception): pass
class SchSyntaxError(Exception): pass

def char(arg):
    '''"Konvertuje" string u char
    
    U praksi samo baca error ako arg nije str ili ako nije duzine 1
    '''
    arg = str(arg)
    assert(len(arg) == 1)
    return arg

def string(arg):
    '''Vrshi validaciju i konverziju u string
    '''
    return str(arg)

def get_entities():
    '''Registruje sve podklase SchematicEntity iz modula
    
    Poziva se prilikom uvoza modula i prijavljuje sve Entitete
    u dict gde je kljuc pretrazivanja njhov type_id.
    '''
    module = ents
    symbols = (getattr(module, name) for name in dir(module))
    entities = (entity for entity in symbols 
                if inspect.isclass(entity) and
                entity is not SchematicEntity and
                issubclass(entity, SchematicEntity))
    return dict((entity.type_id, entity) 
                for entity in entities)

def create_entity(token, generator = None):
    '''Factory metoda. 
    
    Na osnovu tokena instancira proizvoljan SchematicEntity
    '''
    if token == "{":
        raise OpenBrace
    if token == "}":
        raise ClosedBrace
    type_id = token.split()[0]
    cls = entity_dict[type_id]
    return cls(token, generator)    

def _parse_token(xsubtoken, grammar):
    '''Formira jedan atribut entiteta.'''
    name, cons = grammar[0:2]
    subnodes = grammar[2:]
    val = None
    if subnodes == ():
        '''Ako smo u leaf node'''
        val = cons(xsubtoken.next())
    else:
        args = [_parse_token(xsubtoken, subnode)[1] for subnode in subnodes ]
        val = cons(*args)
    return name, val

class SchematicEntity(jsonWrapper.JsonSerializable):
    def __new__(cls, token, generator = None):
        '''Kondiciono instancira klasu'''
        if token.split()[0] == cls.type_id:
            return object.__new__(cls, token, generator)
        else:
            raise ImportError, "Neadekvatan type_id"
    def __init__(self, token, generator = None):
        token_gen = token.split().__iter__()
        self.base_attrs = {}
        self.ext_attrs = {}
        
        for item in self.subtoken_mapping:
            name, value = _parse_token(token_gen, item)
            self.base_attrs[name] = value
        
        self.setUp(token, generator)
    def __getattr__(self, key):
        '''Pristup atributima u base_attrs i ext_attrs recnicima'''
        try:
            return self.base_attrs[key]
        except KeyError:
            try:
                return self.ext_attrs[key]
            except KeyError:
                raise AttributeError("Kljuc %s nije pronadjen." % (key,))
    def setUp(self, token, generator = None):
        '''Instance-specificna inicijalizacija
        
        Obavlja specificnu inicijalizaciju entiteta. Npr. Text entiteti
        citaju "lines" sledecih tokena.
        '''
        pass
    
    def __serialize__(self):
        return jsonWrapper._serialize(self.__dict__)
        
class Text(SchematicEntity):
    '''Tekstualni entitet. Sadrzi niz linija teksta'''
    type_id = 'T'
    subtoken_mapping = (('type', char),
                        ('point', g.Point,("x", int),("y", int)),
                        ('color', int),
                        ('size', int),
                        ('visibility', int),
                        ('show_name_value', int),
                        ('angle', int),
                        ('alignment', int),
                        ('num_lines', int))
    def setUp(self, token, generator):
        '''Inicijalizacija specificna za text entitet
        
        Chita self.num_lines broj linija i dodaje ih kao svoj text.
        '''
        text = []
        for i in range(self.num_lines):
            text.append(generator.next())
        text = "\n".join(text)
        self.base_attrs['text'] = text
        if self.num_lines == 1 and '=' in self.text:
            key, attr = self.text.split('=')
            self.base_attrs['key'] = key
            self.base_attrs['attr'] = attr

class Net(SchematicEntity):
    '''Zhica'''
    type_id = 'N'
    subtoken_mapping = (('type', char),
                        ('line', g.Line,('p1', g.Point, ('x1', int),('y1', int)),
                                        ('p2', g.Point, ('x2', int),('y2', int))),
                        ('color', int))
    def setUp(self, token, generator):
        pass

class Bus(SchematicEntity):
    '''Magistrala'''
    type_id = 'U'
    subtoken_mapping = (("type", char),
                        ('line', g.Line,('p1', g.Point, ('x1', int),('y1', int)),
                                        ('p2', g.Point, ('x2', int),('y2', int))),
                        ("color", int),
                        ("ripperdir", int))

class Component(SchematicEntity):
    '''Predefinisan komponenta'''
    type_id = 'C'
    subtoken_mapping = (("type", char),
                        ("point", g.Point, ('x', int),('y', int)),
                        ("selectable", int),
                        ("angle", int),
                        ("mirror", int),
                        ("basename", string))

class Version(SchematicEntity):
    '''VErzija'''
    type_id = 'v'
    subtoken_mapping = (("version", int),
                        ("fileformat_version", int))
                        
class Line(SchematicEntity):
    '''Linija'''
    type_id = 'L'
    subtoken_mapping = (("type", char),
                        ('line', g.Line,('p1', g.Point, ('x1', int),('y1', int)),
                                        ('p2', g.Point, ('x2', int),('y2', int))),
                        ("color", int),
                        ("width", int),
                        ("capstyle", int),
                        ("dashstyle", int),
                        ("dashlength", int),
                        ("dashspace", int))
    
class Picture(SchematicEntity):
    '''SLIKA'''
    type_id = 'G'
    subtoken_mapping = (("type", char),
                        ('point', g.Point,("x", int),("y", int)),
                        ("width", int),
                        ("height", int),
                        ("angle", int),
                        ("mirrored", char),
                        ("embedded", char),
                        ("filename", string),
                        ("encoded_picture_data", string),
                        ("encoded_picture_end", string))
                                                
class Box(SchematicEntity):
    '''box'''
    type_id = 'B'
    subtoken_mapping = (("type", char),
                        ('point', g.Point,("x", int),("y", int)),
                        ('wh', g.Point,("width", int),("height", int)),
                        ("color", int),
                        ("width", int),
                        ("capstyle", int),
                        ("dashstyle", int),
                        ("dashlength", int),
                        ("dashspace", int),
                        ("filltype", int),
                        ("fillwidth", int),
                        ("angle1", int),
                        ("pitch1", int),
                        ("angle2", int),
                        ("pitch2", int))

class Circle(SchematicEntity):
    '''Krug'''
    type_id = 'V'
    subtoken_mapping = (("type", char),
                        ('point', g.Point,("x", int),("y", int)),
                        ("radius", int),
                        ("color", int),
                        ("width", int),
                        ("capstyle", int),
                        ("dashstyle", int),
                        ("dashlength", int),
                        ("dashspace", int),
                        ("filltype", int),
                        ("fillwidth", int),
                        ("angle1", int),
                        ("pitch1", int),
                        ("angle2", int),
                        ("pitch2", int))                        
class Arc(SchematicEntity):
    '''Arc'''
    type_id = 'A'
    subtoken_mapping = (("type", char),
                        ('point', g.Point,("x", int),("y", int)),
                        ("radius", int),
                        ("startangle", int),
                        ("sweepangle", int),
                        ("color", int),
                        ("width", int),
                        ("capstyle", int),
                        ("dashstyle", int),
                        ("dashlength", int),
                        ("dashspace", int))
                  
class Pin(SchematicEntity):
    '''pin'''
    type_id = 'P'
    subtoken_mapping = (("type", char),
                        ('line', g.Line,('p1', g.Point, ('x1', int),('y1', int)),
                                        ('p2', g.Point, ('x2', int),('y2', int))),
                        ("color", int),
                        ("pintype", int),
                        ("whichend", int))
    def setUp(self, token, generator):
        '''Na osnovu whichend atributa postavljamo point
        
        Connection point (conn_pnt) je tacka na koju se netovi
        povezuju sa pinom.
        '''
        self.base_attrs['point'] = self.line.points[self.whichend]

class Path(SchematicEntity):
    '''path'''
    type_id = 'H'
    subtoken_mapping = (("type", char),
                        ("color", int),
                        ("width", int),
                        ("capstyle", int),
                        ("dashstyle", int),
                        ("dashlength", int),
                        ("dashspace", int),
                        ("filltype", int),
                        ("fillwidth", int),
                        ("angle1", int),
                        ("pitch1", int),
                        ("angle2", int),
                        ("pitch2", int),
                        ("num_lines", int))                        

class Font(SchematicEntity):
    '''font'''
    type_id = 'F'
    subtoken_mapping = (("type", char),
                        ("character", char),
                        ("width", int),
                        ("flag", int))

    
entity_dict = get_entities()
'''Recnik svih instancibilnih entiteta u modulu

Kljuc pretrage je type polje jednog tokena u sch fajlu. Koristi se u create_entity
Fabrici kao metod za lociranje odgovarajuce klase.
'''

    

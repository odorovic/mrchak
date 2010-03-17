'''Jedonstavan paket za 2D-integer-taxicab geometriju.'''

from json import _serialize, JsonSerializable

class Point(JsonSerializable):
    def __init__(self, x, y):
        self.x = x
        self.y = y
    
    def touches(self, line):
        '''Proverava da li tacka dodiruje liniju'''

        if self. x == line.x1 and self.x == line.x2:
            y1,y2 = sorted((line.y1, line.y2))
            return self.y >= y1 and self.y <= y2
        elif self.y == line.y1 and self.y == line.y2:
            x1,x2 = sorted((line.x1, line.x2))
            return self.x >= x1 and self.x <= x2
        else:
            return False
    def __cmp__(self, pnt):
        '''Poredjenje dve tacke u taxicab geometriji.
        
        Dve tacke su jednake ako imaju iste koordinate, a ako nisu, veca je ona sa 
        vecim zbigorm koordinata.
        '''
        if not isinstance(pnt, Point):
            return False
        if self.x == pnt.x and self.y == pnt.y:
            return 0
        elif self.x + self.y > pnt.x + pnt.y:
            return 1
        else:
            return -1
        
    def __str__(self):
        return str((self.x, self.y))
    
    def __repr__(self):
        return str(self)
    
    def __add__(self, pnt):
        return Point(self.x+pnt.x, self.y+pnt.y)
    
    def __hash__(self):
        '''Hash f-ja geometrijske tacke.
        
        Tezimo da napravimo takav hash koji reflektuje medjusobni odnos dve tacke.
        Od dve tacke, vetji hash tje imati vetja tacka.
        
        Od dve tacke vetja je udaljenija od koordinatnog pocetka. Ako su te razdaljine 
        iste, vetaj tacka ima vetju x koordinatu.
        '''
        x = self.x / 100
        y = self.y / 100
        distance = (x + y) * 1000
        hash = distance + x
        return hash
    
    def __serialize__(self):
        return [_serialize(self.x), _serialize(self.y)]
        
class Line(JsonSerializable):
    def __init__(self, p1 , p2):
        '''p1,p2 --> Point instance'''
        self.points = (p1,p2)
    def __getattr__(self, name):
        '''Omogucavamo line.x1 vrstu sintakse'''
        if len(name) == 2:
            coord, index = [char for char in name]
            if not (coord in ('x', 'y') and index in ('1','2')):
                raise AttributeError()
            else:
                return getattr(self.points[int(index)-1], coord)
        else:
            raise AttributeError()
        
    def __cmp__(self, line):
        '''Poredjenje dve linije u taxicab geometriji.
        
        Linije su jednake ako su im obe tacke jednake, a veca linija je ona sa 
        vecom geometrijskom sredinom. Ako su geom sredine iste, vraca se self > line
        '''
        if not isinstance(line, Line):
            return False
        p11,p12 = self.points
        p21,p22 = line.points
        if p11 == p21 and p12 == p22:
            return 0
        else:
            g1 = p11 + p12
            g2 = p21 + p22
            rez = g1.__cmp__(g2)
            if rez == 0:
                return 1
            else:
                return rez
    def __str__(self):
        return '<Line : ( %s, %s )>' % self.points
    
    def __repr__(self):
        return str(self)
    
    def __hash__(self):
        '''Hash linije. 
        
        Definise se kao hash centralne tacke konkatovan na hash vetje tacke.
        Ovo znaci da tje hashovi linija biti tipa long
        '''
        p1, p2 = self.points
        center = (p1 + p2) / 2
        hash = center.__hash__() * 10000000 + p2.hash
        
    def __add__(self, pnt):
        p1,p2 = self.points
        return Line(p1+pnt, p2+pnt)
    
    def __serialize__(self):
        p1, p2 = self.points
        return [_serialize(p1), _serialize(p2)]
        
    def touches(self, line):
        '''Proverava da li linija dodiruje liniju
        
        Neoptimalno, proverava tacke svaku sa svakom.
        '''
        tch = False
        for p in self.points:
            tch = tch or p.touches(line)
        for p in line.points:
            tch = tch or p.touches(self)
        return tch
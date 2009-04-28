'''MyHdl code generator.

Generates MyHdl structural components from 
'''

import sys, string

class CodeGeneratorBackend:

    def begin(self, tab="\t"):
        self.code = []
        self.tab = tab
        self.level = 0

    def end(self):
        return string.join(self.code, "")

    def write(self, string):
        self.code.append(self.tab * self.level + string)

    def indent(self):
        self.level = self.level + 1

    def dedent(self):
        if self.level == 0:
            raise SyntaxError, "internal error in code generator"
        self.level = self.level - 1
    
    def newline(self):
        self.write('\n')

def generate_hdl(schematic):
    '''Convrets a schematic.Schematic object to myhdl structural module.
    
    '''
    c = CodeGeneratorBackend()
    c.begin( tab = " "*4)
    module_docstring = """'''Automatski generisani kod: schtools.hdlgen.generate_hdl'''"""
    c.write(module_docstring)
    c.newline()
    c.write('import myhdl')
    c.newline()
    modulename = schematic['name']
    arglist = ', '.join([name for name, width in 
                         schematic['inputs']+schematic['outputs']])
    c.write('def %s(%s):' % (modulename, arglist))
    c.newline()
    c.indent()
    for signalname, width in schematic['internal_signals']:
        if width == 1:
            opts = 'bool(0)'
        else:
            opts = 'intbv(0)[0:%d]' % (width,)
        c.write('%s = Signal(%s)' % (signalname, opts))
        c.newline()
    c.newline()
    
    for module in schematic['submodules']:
        name = module['name']
        type = module['type']
        arglist = ', '.join(module['inputs'] + module['outputs']) 
        c.write('%s = %s(%s)' % (name,type,arglist))
        c.newline()
    c.newline()
    c.write('return instances()')
    c.dedent()
    return c.end()
    
    
#!/usr/bin/python
'''MyHdl code generator.

Generates MyHdl structural components from 
'''

import sys
import string
import getopt
import simplejson

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

def generate_imports():
    c = CodeGeneratorBackend()
    c.begin(tab = " "*4)
    docstr = """'''Automatski genereisan modul. Izbacuemo importe.'''"""
    c.write(docstr)
    c.newline()
    c.write('import myhdl')
    c.newline()
    c.write('from basichdl.builtins.hdl import signal')
    c.newline()
    c.write('from basichdl.builtins.components import *')
    c.newline()
    c.newline()
    return c.end()
    
def generate_hdl(schematic):
    '''Convrets a schematic.Schematic object to myhdl structural module.
    
    Returns a string representation od myhdl structural module. 
    '''
    c = CodeGeneratorBackend()
    c.begin( tab = " "*4)
    module_docstring = """'''Automatski generisani kod: schtools.hdlgen.generate_hdl'''"""
    c.write(module_docstring)
    c.newline()
    c.newline()
    modulename = schematic['attributes']['name']
    arglist = ', '.join([name for name in 
                         schematic['input_signals']+schematic['output_signals']])
    c.write('def %s(%s):' % (modulename, arglist))
    c.newline()
    c.indent()
    for signalname in schematic['internal_signals']:
        signalobj = [signal for signal in schematic['signals'] 
                     if signal['netname'] == signalname][0]
        value = signalobj.has_key('value') and signalobj['value'] or "0"
        width = 1 #TODO citanje sirine iz jsona
        
        opts = '"%s", intbv(%s)[%d:0]' % (signalname, value, width)
        c.write('%s = signal(%s)' % (signalname, opts))
        c.newline()
    c.newline()
    
    modulenames = []
    for module in schematic['submodules']:
        name = module['attributes']['refdes']['base_attrs']['attr']
        modulenames.append(name)
        type = module['type']
        arglist = ', '.join(module['inputs'] + module['outputs']) 
        c.write('%s = %s(%s)' % (name,type,arglist))
        c.newline()
    c.newline()
    c.write('return %s' % ', '.join(modulenames))
    c.dedent()
    return c.end()

usage='''hdlgen [-m|-i|-a] file[s]'''

class Args(object):
    opts = ((('m', 'module-only'), False, 'Skriptra generishe samo telo myhdl modula (py f-ja)'),
            (('i', 'imports-only'), False, 'Skripta generishe samo neophodne imprte.'),
            (('a', 'generate-all'), False, 'Emituje modul i sve importe.'))
    
    def generate_optargs(self):
        '''Generates necesarry arguments for getopt call.
        
        '''
        def _genopts(seq, index, connector):
            return [opt[0][index] + (opt[1] and connector or '') for opt in seq ]
        
        def generate_shortopts():
            return ''.join(_genopts(Args.opts, 0, ':'))
        
        def generate_longopts():
            return _genopts(Args.opts, 1, '=')
        
        shortopts = generate_shortopts()
        longopts = generate_longopts()
        return shortopts, longopts
    
    
    def __init__(self):
        self.set_defaults()
        argv = sys.argv[1:]
        opts, args = getopt.getopt(argv, *self.generate_optargs())
        self.override_io(opts, args)
        self.override_generation(opts)
        
    def set_defaults(self):
        self.inputs = [sys.stdin]
        self.output = sys.stdout
        self.dump_imports = True
        self.dump_body = True
    
    def override_io(self, opts, args):
        '''Overrides default input/output file(s) settings'''
        assert len(args) != 1, 'Neadekvatna upotreba. Treba nam ime i ulaznog i izlaznog fajla.'
        if len(args) >= 2:
            output_fname = args[-1]
            input_fnames = args[:-1]
            #Ako je stdin ulazni fajl, bi trebalo da imamo i druge ulazne fajlove. Valjda
            assert len(input_fnames) == 1 or '-' not in input_fnames
            self.inputs = [fname == '-' and sys.stdin or open(fname, 'r') 
                           for fname in input_fnames]
            self.output = output_fname == '-' and sys.stdout or open(output_fname, "w")
            
    def override_generation(self, opts):
        '''overrides code generation settings.'''
        for opt, arg in opts:
            if opt == 'm':
                self.dump_body = True
                self.dump_imports =False
                return True
            elif opt == 'i':
                self.dump_body = True
                self.dump_imports =False
                return True
            elif opt == 'a':
                self.dump_body = True
                self.dump_imports =True
                return True
        return False
    
def __main__():
    '''Skripta za generisanje hdla.
    
    Opcije:
        -m : Generise se samo myhdl modul, bez importa
        -i : Samo dampuje neophodne importe
        -a : funkcionalan py modul (-m + -i)
    '''
    args = Args()
    if args.dump_imports == True:
        args.output.write(generate_imports())
    for input in args.inputs:
        args.output.write(generate_hdl(simplejson.load(input)))
       
    
    
if __name__ == '__main__':
    __main__()
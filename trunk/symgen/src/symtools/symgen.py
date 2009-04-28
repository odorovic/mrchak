#!/usr/bin/python
'''Generishe simbol za datu shemu.

@author: odor
'''

import os
import sys
import simplejson

template_symbol = {'options' : {'wordswap': 'yes',
                       'rotate_labels' : 'no',
                       'sort_labels' : 'yes',
                       'generate_pinseq' : 'no',
                       'sym_width' : 2600,
                       'pinwidthvertical' : 400,
                       'pinwidthhorizontal' : 600},
        'geda_attr' : {'version':'20060113 1',
                        'name':None,
                        'device':None,
                        'refdes':'U?',
                        'footprint':'DIP14',
                        'description':None,
                        'documentation':None,
                        'author':None,
                        'dist-license':'DWTFYW',
                        'use-license':'DWTFYW',
                        'numslots':0},
        'pins' : []}

#[pins]
#-----------------------------------------------------
#pinnr    seq    type    style    posit.    net    label    
#-----------------------------------------------------

def extract_pins(schematic):
    '''Iz sheme vadi dve sortirane liste pinova. Ulazni i izlazni'''
    inpins = schematic['in_pins']
    outpins = schematic['out_pins']
    for pin in inpins:
        pin['position'] = 'l'
        pin['type'] = 'in'
    for pin in outpins:
        pin['position'] = 'r'
        pin['type'] = 'out'
    pins = inpins + outpins
    for pinnum, pin in enumerate(pins):
        pin['style'] = 'line'
        pin['pinnum'] = pinnum + 1
        pin['pinseq'] = pin['pinnum']
    return  inpins + outpins

def make_pinfield(pindict):
    '''Iz recnika sa opisima pinova pravi listu atributa u tragesym formatu
    
    Za dati recnik sa opisom pina porlazi kroz listu atributa fieldmapping
    I generise listu sa datim atributima.
    >>>make_pinfield({'pinnum' : 42, 'type': in, 'pinlabe': 'sranje'})
    [42, None, 'in', None, None, None, 'sranje'] 
    '''
    fieldmapping = ['pinnum','pinseq','type','style','position','net','pinlabel']
    pinfield = []
    for field in fieldmapping:
        val = pindict.get(field)
        pinfield.append(val)
    return pinfield

def extract_attrs(symboldict):
    '''Cita glogale atriburte sheme i kombinuje ih sa default.'''
    attrs = dict(template_symbol['geda_attr'])
    for key, val in symboldict['attributes'].items():
        attrs[key] = val
    return attrs

def dict_dumps(dct, FS='=',RS='\n'):
    '''Serijalizuje recnik u key-value pair string.'''
    records = []
    for key, val in dct.items():
        records.append(FS.join((str(key),str(val))))
    return RS.join(records)

def symbol_dumps(symboldict):
    '''Generise string koji opisuje simbol, takav da ga moze procitati tragesym util
    
    Od recnika symbol sa opsom simbola pravi string u .ini formatu koji moze da 
    procita tragesym util. Za izgled fajla pogledati data poddirektorijum tekuceg
    projekta. 
    '''
    def str(arg):
        if arg != None:
            import __builtin__
            return __builtin__.str(arg)
        else:
            return ''
    tragesym_file = '\n\n[options]\n%s\n\n[geda_attr]\n%s\n\n[pins]\n%s\n'
    options = dict_dumps(template_symbol['options'])
    geda_attr = dict_dumps(extract_attrs(symboldict))
    pinfields = []
    pindicts = extract_pins(symboldict)
    for pindict in pindicts:
        pinfield = [str(item) for item in make_pinfield(pindict)]
        pinfields.append('\t'.join(pinfield))
    pins = '\n'.join(pinfields)
    return tragesym_file % (options, geda_attr, pins)

usage = \
'''Symgen 0.01. 
Usage: symgen.py input output
'''
    
def main():
    try:
        in_filename = sys.argv[1]
        out_filename = sys.argv[2]
        input = open(in_filename).read()
        schin, schout = os.popen2('schdump')
        schin.write(input)
        schin.close()
        schjson = schout.read()
        f = open("/tmp/proba.json", 'w')
        f.write(schjson)
        f.close()
        symboldict = simplejson.loads(schjson)
        output = symbol_dumps(symboldict)
        tmpfilename = "/tmp/symgen.ini"
        tmpfile = open(tmpfilename, "w")
        tmpfile.write(output)
        tmpfile.close()
        os.system("tragesym %s %s" % (tmpfilename, out_filename))
    except IOError, e:
        print usage
    
    
if __name__ == '__main__':
    main()
    
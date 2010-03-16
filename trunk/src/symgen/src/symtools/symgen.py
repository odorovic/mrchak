#!/usr/bin/python
'''Generishe simbol za datu shemu.

@author: odor
'''

import os
import sys
import simplejson

template_symbol = {'options' : {'wordswap': 'yes',
                       'rotate_labels' : 'no',
                       'sort_labels' : 'no',
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

def update_attrs(pin, offset = 0):
    try:
        pinnum = int(pin['attributes']['pinnumber']['base_attrs']['attr']) + offset
        pinseq = int(pin['attributes']['pinseq']['base_attrs']['attr']) + offset    
    except:
        print pin['attributes']['pinlabel']
        for key in pin['attributes'].keys():
            print key
    pinnum = int(pin['attributes']['pinnumber']['base_attrs']['attr']) + offset
    pinseq = int(pin['attributes']['pinseq']['base_attrs']['attr']) + offset
    pin['pinnum'] = pinnum
    pin['pinseq'] = pinseq
    pin['style'] = 'line'

def extract_pins(schematic):
    def cmppin(pin1, pin2):
        return cmp(int(pin1['pinnum']),int(pin2['pinnum']))
    '''Iz sheme vadi dve sortirane liste pinova. Ulazni i izlazni'''
    inpins = schematic['in_pins']
    outpins = schematic['out_pins']
    reverse = False
    if not reverse:
        inpos = 'l'
        intype = 'in'
        inoffset = 0
        outpos = 'r'
        outtype = 'out'
        outoffset = 0
    else:
        inpos = 'r'
        intype = 'out'
        inoffset = 10
        outpos = 'l'
        outtype = 'in'
        outoffset = -31
    for pin in inpins:
        pin['position'] = inpos
        pin['type'] = intype
        update_attrs(pin, inoffset)
    for pin in outpins:
        pin['position'] = outpos
        pin['type'] = outtype
        update_attrs(pin, outoffset)
    pins = sorted(inpins, cmppin) + sorted(outpins, cmppin)
    return pins

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
        input = open(in_filename)
        symboldict = simplejson.load(input)
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
    
#!/usr/bin/python
'''Cli aplikacija za parsovanje i dampovanje sch shema.'''



import os
import sys

import json
import schematic
import configuration

usage = \
'''
schdump - Alat za parsiranje .sch fajlova gschem editora. Ulazne sch fajlove retvara 
u hijerarhijsku json reprezentaciju pogodniju za dalju obradu. Za sada podrzani modovi
rada su "topoloski" i "potpuni". Topoloski (podrazumevani) opisuje samo logicku 
povezanost belemenata, dok potpuni (neimplemntirani) izbacuje i potpun opis svih 
entiteta u sch fajlu, 

Opcije:
    -t  -- Opciono. Izlazni json sadrzi samo topoloski kontekst seme. Pogodno
        npr. za ulaz u generatore hdl koda.
    -a  -- Opciono. Izlazni json sadrzi (gotovo)potpuni opis celokupne seme.
        Ideja je da imamo dovoljno informacija da pouzdano ponovo nacrtamo 
        celokupnu semu. NIJE IMPLEMENTIRANO.
    -o <filename>   -- Opciono. Upisuje izlaz u imenovani fajl umesto na
        standardni izlaz. NIJE IMPLEMENTIRANO.
    
    -f <filename>   -- Opciono. Cita ulaz iz fajla umesto sa standardnog ulaza. 
        NIJE IMPLEMENTIRANO.
'''

class Args(object):
    '''Parser command line i enviroment varijabli.
    
    Sadrzi samo konstruktor koji radi arsihg i sanity-checking svih argumenata.
    '''
    
    def __init__(self):
        if len(sys.argv) == 1:
            self.input = sys.stdin
            self.output = sys.stdout
        elif len(sys.argv) == 2:
            raise Exception("Treba nam ili 0 ili 2 argumenta.")
        else:
            self.input = self.open(sys.argv[1], 'r')
            self.output = self.open(sys.argc[2], 'w')
        self.dump_method = 'all'
    
    def open(self, fname, mode):
        default_stream = {'r':sys.stdin, 'w':sys.stdout}
        if fname == '-':
            return default_stream[mode]
        else:
            return open(fname, mode)
        

def __main__():
    configuration.SYMBOL_PATHS.append(os.path.join(os.getcwd(),'symbols'))
    args = Args()
    schemlist = schematic.parse(args.input)
    obj = schematic.clasify(schemlist)
    jsonrepr = json.serialize(obj, indent=2)
    args.output.write(jsonrepr)

     
if __name__ == '__main__':
    __main__()

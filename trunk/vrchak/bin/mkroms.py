#!/usr/bin/python

import os
import re
from os import path
from optparse import OptionParser


def parse_args():
    usage = 'Usage: %prog [options] file'
    parser = OptionParser(usage=usage)
    parser.add_option('-l', '--low-bank-fname', dest = 'lobank', 
            default='romlo.img', metavar='FILE',
            help='Write contents of lower memory bank to %default.')
    parser.add_option('-u', '--high-bank-fname', dest = 'hibank',
            default='romhi.img', metavar='FILE',
            help='Write contents of higher memory bank to %default.')
    parser.add_option('-s', '--symbol-file', dest='symfname',
            default='symbols.txt', metavar='FILE',
            help='Path to a map file, as outputed from nasm.')
    parser.add_option('-a', '--rom_addr', dest='romaddr',
            type='int', default=0x0, metavar='ADDR',
            help='Start address of program address space. Defaults to 0x0.')
    (options, args) = parser.parse_args()
    if (len(args) != 1):
        parser.error('Incorrect number of arguments.')
    return (options, args)

def parse_symbol_table(fname):
    f = open(fname)
    lines = f.read()
    p = re.compile('Program origin .*\n\n([0-9A-F]*)')
    m = p.search(lines)
    if not m:
        raise Exception("Invalid map file.")
    print int(m.group(1), 16)
    return int(m.group(1), 16) 


def split_bytes(l):
    lobytes = []
    hibytes = []
    i = 0
    for c in l:
        if not i % 2:
            lobytes.append(c)
        else:
            hibytes.append(c)
        i = i+1
    return lobytes, hibytes

def read_segment(fname, start_off, size):
    if not path.isfile(fname):
        raise IOError(os.stderr, "No such file: %s" % fname)
    f = open(fname, 'rb')
    f.seek(start_off)
    seg = f.read(size)
    return seg

def dump_readh_image(fname, img, start_addr):
    f = open(fname, "w")
    print >> f, '@%x' % start_addr, '//', start_addr
    for c in img:
        print >> f, "%02x" % ord(c)
    print >> f
    f.close()

def calculate_addresses(start_addr, rom_start_addr):
    rom_offset = start_addr / 2
    return rom_offset

def main():
    (options, args) = parse_args()
    start_addr = parse_symbol_table(options.symfname)
    romoff = calculate_addresses(start_addr, options.romaddr)
    seg = read_segment(args[0], 0, -1)
    lo, hi = split_bytes(seg)
    dump_readh_image(options.lobank, lo, romoff)
    dump_readh_image(options.hibank, hi, romoff)


if __name__ == '__main__':
    main()

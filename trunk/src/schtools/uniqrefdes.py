#!/usr/bin/python
'''
Created on Jun 1, 2009

@author: odor
'''

import sys
import re

cnt = 1
def refenum(pattern):
    global cnt
    retstr = 'refdes=U%d' % cnt
    cnt = cnt + 1
    return retstr

def __main__():
    input = open(sys.argv[1])
    #output = open(sys.argv[2])
    for line in input.readlines():
        subsline = re.sub('^refdes=U.{0,3}$', refenum, line)
        print subsline,
        
if __name__ == '__main__':
    __main__()
#!/usr/bin/python
'''Alatka koja sve projekte iz pydeva eksportuje na pytohnpath

Pretrazuje date direktorijume i svaki poddirektorijum koji sadrzi src poddirektorijum
linkuje na direktorijum na pytohnpathu.

Created on Apr 18, 2009

@author: odor
'''

import sys
import os

def clean_links(path):
    prevpath = os.getcwd()
    os.chdir(path)
    for item in os.listdir(path):
        if os.path.islink(item):
            os.unlink(item)

def export_worksapce_paths(workspace_path, pypath = 'lib'):
    '''Eksportuje src foldere iz wspejsa na pythonpath'''
    os.chdir(workspace_path)
    prjdirs = []
    for dirname in os.listdir('.'):
        if os.path.isdir(dirname) and 'src' in os.listdir(dirname):
            abspath = os.path.abspath(os.path.join(dirname, 'src'))
            prjdirs.append((dirname, abspath))
    assert os.path.isdir(pypath)
    os.chdir(pypath)
    assert os.getcwd() in sys.path  #da li je pypayh u sys.path? 
    clean_links(os.getcwd())
    for libname, libpath in prjdirs:
        if not '__init__.py' in os.listdir(libpath):
            open(os.path.join(libpath, '__init__.py'), 'w').close()
        os.symlink(libpath, libname)
            
def __main__():
    assert len(sys.argv) >= 2
    assert os.path.isdir(sys.argv[1])
    export_worksapce_paths(sys.argv[1])
    
if __name__ == '__main__':
    __main__()
            
    
    
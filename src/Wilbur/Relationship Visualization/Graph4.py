'''Prints out files that have same hashes in a format as required by GraphVisualization2.py
Variable 'path' specifies the directory that is to be scanned'''

import os
import numpy as np
import operator
from py_essentials import hashing as hs


path = '../Desktop'

'''same hash'''
filetimes = []

files = []

'''scanning the directory'''

for r, d, f in os.walk(path):
    f = [file for file in f if not (file[0] == '.' or file[0] == '_')]
    d[:] = [dr for dr in d if not (dr[0] == '.' or dr[0] == '_')]
    for file in f:
            filepath = os.path.join(r, file)
            filepath2 = filepath.split('/')
            filename = os.path.splitext(filepath2[-1])
            filename = os.path.splitext(filename[0])
            files.append((filepath,filename[0]))
            


files = np.asarray(files) #So that indexing with a list will work
hashes = []

for f in files:
    hashes.append(hs.fileChecksum(f[0], "md5"))

i=-1
j=-1

for hash1 in hashes:
    i = i+1
    j=-1
    for hash2 in hashes:
        j = j+1
        if hash1==hash2 and files[i][0]!=files[j][0]:
            f1mtime = str(os.path.getmtime(os.path.abspath(files[i][0])))
            f2mtime = str(os.path.getmtime(os.path.abspath(files[j][0])))
            print('content' + '^'+ os.path.abspath(files[i][0]) + '^' + files[i][1] + '^' + f1mtime + '^' + os.path.abspath(files[j][0]) + '^' + files[j][1] + '^' + f2mtime) 
            
            
        

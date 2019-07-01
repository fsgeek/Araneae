'''Prints out files that have similar names in a format as required by GraphVisualization2.py
Variable 'path' specifies the directory that is to be scanned'''

import os
import numpy as np
import sklearn.cluster
import textdistance
from datasketch import MinHash, MinHashLSH
import operator
import magic
import time
from sklearn.cluster import KMeans
import matplotlib.pyplot as plt

path = '../Desktop'

'''name similarity'''
filetimes = []

'''scanning the directory'''
files = []
for r, d, f in os.walk(path):
    f = [file for file in f if not (file[0] == '.' or file[0] == '_')]
    d[:] = [dr for dr in d if not (dr[0] == '.' or dr[0] == '_')]
    for file in f:
            filepath = os.path.join(r, file)
            filepath2 = filepath.split('/')
            filename = os.path.splitext(filepath2[-1])
            filename = os.path.splitext(filename[0])
            files.append((filepath,filepath2[-1]))
            


files = np.asarray(files) #So that indexing with a list will work



'''forming links between files with similar name'''
for f1 in files:
    for f2 in files:
        distance1 = textdistance.jaro.distance(f1[1],f2[1])
        distance2 = textdistance.levenshtein.distance(f1[1],f2[1]) 
        if ((distance1<=0.30 and f1[0]!=f2[0] and distance2<0.75*min(len(f1[1]),len(f2[1]),8)) or distance1<=0.15 or distance2<=0.25*min(len(f1[1]),len(f2[1]),8)) and f1[0]!=f2[0]:
            f1mtime = str(os.path.getmtime(os.path.abspath(f1[0])))
            f2mtime = str(os.path.getmtime(os.path.abspath(f2[0])))
            print('name' + '^'+ os.path.abspath(f1[0]) + '^' + f1[1] + '^' + f1mtime +'^' + os.path.abspath(f2[0]) + '^' + f2[1] + '^' + f2mtime)#+ ',' + str(1/(0.1+distance1+distance2/(len(f1[1])+len(f1[1]) ))))
            
            
        

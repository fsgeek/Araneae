'''Prints out files that have similar content in a format as required by GraphVisualization2.py
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
root_path = 'relationship_root2'

        
'''content similarity'''


text_files = []

'''scanning the directory'''
for r, d, f in os.walk(path):
    f = [file for file in f if not (file[0] == '.' or file[0] == '_')]
    d[:] = [dr for dr in d if not (dr[0] == '.' or dr[0] == '_')]
    for file in f:
            filepath = os.path.join(r, file)
            mime = magic.Magic(mime=True)
            file_type = mime.from_file(filepath) 
            major_minor = file_type.split('/')
            
            if major_minor[0] == 'text' :
                text_files.append(filepath)
documents =[]
for f in text_files:
    documents.append((open(f).read(),f))

lsh = MinHashLSH(threshold=0.3, num_perm=128)

for f in documents:
    setdoc = set(f[0].split())  
    m = MinHash(num_perm=128)
    for d in setdoc:
        m.update(d.encode('utf8'))
    lsh.insert(f[1], m)


results = []
for doc in documents:
    setdoc = set(doc[0].split())    
    m = MinHash(num_perm=128)
    for d in setdoc:
        m.update(d.encode('utf8'))
    result = lsh.query(m)
    results.append((doc[1],result))


'''forming links between files with similar content'''

for result in results:
    for r in result[1]:
        if r!=result[0]: 
            f1mtime = str(os.path.getmtime(os.path.abspath(r)))
            f2mtime = str(os.path.getmtime(os.path.abspath(result[0])))
            print('content' + '^'+ os.path.abspath(r) + '^' + r.split('/')[-1] + '^' + f1mtime +'^'+ os.path.abspath(result[0]) + '^' + result[0].split('/')[-1] + '^' + f2mtime)#+ ',' + str(3))
           

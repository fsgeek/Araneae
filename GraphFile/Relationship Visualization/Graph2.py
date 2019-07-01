'''Prints out files that have similar modification times in a format as required by GraphVisualization2.py
Variable 'path' specifies the directory that is to be scanned
Uses k-means clustering'''

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

'''time similarity'''

file_threshhold = 10
timeroot_path = 'time_clustering'


filetimes = []
files =[]
i = 0


#scanning directory
for r, d, f in os.walk(path):
    f = [file for file in f if not (file[0] == '.' or file[0] == '_')]
    d[:] = [dr for dr in d if not (dr[0] == '.' or dr[0] == '_')]
    for file in f:

        filepath = os.path.join(r, file)
        filepath2 = filepath.split('/')
        last_modif_time = int(os.path.getmtime(filepath))
        files.append((filepath,last_modif_time))
        filetimes.append([last_modif_time])
        i = i+1

kmeans = KMeans(n_clusters=int(i/file_threshhold) +1, random_state=0).fit(filetimes)
labels = kmeans.labels_




for j in range(int(i/file_threshhold)+1) : #iterating through all clusters
    idx = [] 
    for i in range(0, len(labels)) : 
        if labels[i] == j : 
            idx.append(i) 


    filesj = [files[i] for i in idx]
    
    for f1 in filesj:
        for f2 in filesj:
            if  f1[0]!=f2[0] :
                f1mtime = str(os.path.getmtime(os.path.abspath(f1[0])))
                f2mtime = str(os.path.getmtime(os.path.abspath(f2[0])))
                print('time' + '^'+ os.path.abspath(f1[0]) + '^' + f1[0].split('/')[-1] + '^' + f1mtime + '^'+ os.path.abspath(f2[0]) + '^' + f2[0].split('/')[-1] + '^' + f2mtime )#+ ',' + str(3))
               


    

    

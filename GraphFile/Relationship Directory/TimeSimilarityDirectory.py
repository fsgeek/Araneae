'''Another program ('RelationshipDirectory.py') creates a 'copy' of directory specified with the variable 'path' with symlinks to all the files inside a folder with the same name as the files.
This Program creates a Directory that contains symlinks to all the files with similar modification timestamps. 
These symlinks are in a Directory which is a sibling to the file symlink.
Specify the path of directory to be scanned using the variable 'path' 
''''

import os, time
import operator
import shutil
from sklearn.cluster import KMeans
import numpy as np
import matplotlib.pyplot as plt

file_threshhold = 15
timeroot_path = 'time_clustering'


# directory being scanned
path = '../FileSystemTest'
root_path = 'relationship_root2'

filetimes = []
files =[]
i = 0


'''scanning the directory'''
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

print(i)
kmeans = KMeans(n_clusters=int(i/file_threshhold) +1, random_state=0).fit(filetimes)
labels = kmeans.labels_


print(kmeans.labels_)

for j in range(int(i/file_threshhold)+1) : 
    idx = [] 
    for i in range(0, len(labels)) : 
        if labels[i] == j : 
            idx.append(i) 


    filesj = [files[i] for i in idx]
    
    for f1 in filesj:
        split_f1 = f1[0].split('/')[2:]
        for f2 in filesj:
            if  f1[0]!=f2[0] :
                split_f2 = f2[0].split('/')[2:]
                print(f1[0])
                print(f2[0])
                print('\n')
                try:
                    os.symlink(os.path.abspath(os.path.join(root_path, *split_f2)),os.path.join(root_path, *split_f1, 'similar_time', '-'.join(split_f2[::-1])) )
                except FileExistsError:
                    print("File already exists")

    
    

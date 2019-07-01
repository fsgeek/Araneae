'''Clusters files that have similar last modofication times based on k-means clustering algorithm
Creates a directory 'time_clustering' to show the cluster of files formed
'''
import os, time
import operator
import shutil
from sklearn.cluster import KMeans
import numpy as np
import matplotlib.pyplot as plt

file_threshhold = 15 # Inversely proportional to number of clusters
timeroot_path = 'time_clustering'

# root directory
try:
    os.mkdir(timeroot_path)
except FileExistsError:
    print("Directory already exists")

# directory being scanned
path = '../Desktop'


filetimes = []
files =[]
i = 0



for r, d, f in os.walk(path):
    f = [file for file in f if not file[0] == '.']
    d[:] = [dr for dr in d if not dr[0] == '.']
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



    filetimesj = [filetimes[i] for i in idx]
    filesj = [files[i] for i in idx]
    filesj.sort(key = operator.itemgetter(1))

    time_start = time.ctime(filesj[0][1]).split()
    time_end = time.ctime(filesj[-1][1]).split()
    time_dir_name = (time_start[1] + '_' + time_start[2] + '_' + time_start[4]) + '-' + (time_end[1] + '_' + time_end[2] + '_' + time_end[4])
    time_dir_path = os.path.join(timeroot_path,time_dir_name)
    try:
        os.mkdir(time_dir_path)
    except FileExistsError:
        print("Directory already exists")
    i=0
    for f in filesj:
        fname = f[0].split('/')[-1]

        try:
            os.symlink(os.path.abspath(f[0]), os.path.join(time_dir_path,str(i)+'-'+fname))
        except FileExistsError:
            print("File already exists")
        i = i+1


    plt.plot(filetimesj, len(filetimesj) * [1], "x")

plt.show()

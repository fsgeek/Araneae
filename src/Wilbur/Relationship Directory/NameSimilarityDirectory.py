'''Another program ('RelationshipDirectory.py') creates a 'copy' of directory specified with the variable 'path' with symlinks to all the files inside a folder with the same name as the files.
This Program creates a Directory that contains symlinks to all the files with similar name. 
These symlinks are in a Directory which is a sibling to the file symlink.
Specify the path of directory to be scanned using the variable 'path' 
''''

import os
import numpy as np
import sklearn.cluster
import textdistance

path = '../FileSystemTest'
root_path = 'relationship_root2'

filetimes = []

files = []
for r, d, f in os.walk(path):
    f = [file for file in f if not (file[0] == '.' or file[0] == '_')]
    d[:] = [dr for dr in d if not (dr[0] == '.' or dr[0] == '_')]
    for file in f:
            filepath = os.path.join(r, file)
            filepath2 = filepath.split('/')
            filename = os.path.splitext(filepath2[-1])
            filename = os.path.splitext(filename[0])
            print(filename)
            last_modif_time = os.path.getmtime(filepath)
            files.append((filepath,filename[0]))
            


files = np.asarray(files) #So that indexing with a list will work




for f1 in files:
    split_f1 = f1[0].split('/')[2:]
    for f2 in files:
        distance1 = textdistance.jaro.distance(f1[1],f2[1])
        distance2 = textdistance.levenshtein.distance(f1[1],f2[1]) 
        if ((distance1<=0.30 and f1[0]!=f2[0] and distance2<0.75*min(len(f1[1]),len(f2[1]),8)) or distance1<=0.15 or distance2<=0.25*min(len(f1[1]),len(f2[1]),8)) and f1[0]!=f2[0]:
            split_f2 = f2[0].split('/')[2:]
            print(f1[1])
            print(f2[1])
            print(distance1)
            print(distance2)
            print('\n')
            try:
                os.symlink(os.path.abspath(os.path.join(root_path, *split_f2)),os.path.join(root_path, *split_f1, 'similar_name', '-'.join(split_f2[::-1])) )
            except FileExistsError:
                print("File already exists")




    

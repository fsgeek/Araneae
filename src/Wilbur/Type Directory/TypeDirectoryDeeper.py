'''File type analysis: Using shell commands one can find the file type and default application of a given file. The directory view was filtered based on the file type and symlinks were provided to be able to see the entire directory when needed. Example of a path : text/c++/Desktop/Folder1/File1.cpp. This may also be useful. Mobile phones usually just separate files based on their types and have no hierarchy structure. On a related note, files in the downloads and thrash folder are not stored hierarchically and it gets very difficult to find a file. Tools may be created to make this easy.''' 

''''TypeDirectoryDeeper.py' creates the directory 'type_root''''

import os
import operator
import shutil
import magic

# function to create necessary location directories
def create_loc_dir(path, dir, orig_path):
    if len(dir)>0:
        dir_path = os.path.join(path,dir[0])
        orig_path = os.path.join(orig_path,dir[0])
        try:
            os.mkdir(dir_path)

        except FileExistsError:
            print("Directory already exists")
        print(orig_path)
        print(dir_path) 
        try:
            os.symlink(orig_path, os.path.join(os.path.abspath(dir_path),dir_path.split('/')[-1] + '-main'))

        except FileExistsError:
            print("Link already exists")    
        
        create_loc_dir(dir_path,dir[1:],orig_path)
        
# directory being scanned
path = '../FileSystemTest'


files = []
typeroot_path = 'type_root'

# root directory
try:
    os.mkdir(typeroot_path)
except FileExistsError:
    print("Directory already exists")

# r=root, d=directories, f = files
#getting all files in the directory
for r, d, f in os.walk(path):
    f = [file for file in f if not file[0] == '.']
    d[:] = [dr for dr in d if not dr[0] == '.']
    for file in f:
            filepath = os.path.join(r, file)
            filepath2 = filepath.split('/')
            files.append((filepath, filepath2[-1]))



type_dir_names = []
type_dir_paths = []
type_dir_subpaths = []

# creating a list of folders needed
for f in files:
   
    
    mime = magic.Magic(mime=True)

    file_type = mime.from_file(f[0]) 
    major_minor = file_type.split('/')
    type_dir_subpaths.append(os.path.join(typeroot_path, major_minor[0]))
    type_dir_paths.append(os.path.join(typeroot_path, file_type))



# creting all the folders with symlinks to previous and next folder

for tdsp in type_dir_subpaths:
    try:
        os.mkdir(tdsp)
    except FileExistsError:
        print("Directory already exists")

for tdp in type_dir_paths:
    try:
        os.mkdir(tdp)
    except FileExistsError:
        print("Directory already exists")
        
j =0


# creting links to all the files with location heirarchy
for f in files:

    location_dir_path = f[0].split('/')[2:-1]
    create_loc_dir(type_dir_paths[j], location_dir_path,os.path.abspath(path))
    try:
        os.symlink(os.path.abspath(f[0]), os.path.join(type_dir_paths[j], *location_dir_path, f[1]))
    except FileExistsError:
        print("File already exists")

    j = j+1

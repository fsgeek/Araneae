''' Creates a 'copy' of the Directory specified by the variable 'path' by creating symlinks to files
Files are contained in a folder with the same name as file
This Directory is then acted upon by other programs in this forder to create links to similar files
'''

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
        
        
        create_loc_dir(dir_path,dir[1:],orig_path)
        
# directory being scanned
path = '../FileSystemTest'


files = []
root_path = 'relationship_root2'

# root directory
try:
    os.mkdir(root_path)
except FileExistsError:
    print("Directory already exists")

# r=root, d=directories, f = files
#getting all files in the directory
for r, d, f in os.walk(path):
    f = [file for file in f if not ((file[0] == '.' or file[0] == '_'))]
    d[:] = [dr for dr in d if not (dr[0] == '.' or dr[0] == '_')]
    for file in f:
            filepath = os.path.join(r, file)
            filepath2 = filepath.split('/')
            files.append((filepath, filepath2[-1]))




for f in files:

    location_dir_path = f[0].split('/')[2:]
    create_loc_dir(root_path, location_dir_path,os.path.abspath(path))
    try:
        os.symlink(os.path.abspath(f[0]), os.path.join(root_path, *location_dir_path, f[1]))
    except FileExistsError:
        print("File already exists")

    os.mkdir(os.path.join(root_path, *location_dir_path, 'similar_name'))
    os.mkdir(os.path.join(root_path, *location_dir_path, 'similar_time'))
    mime = magic.Magic(mime=True)
    file_type = mime.from_file(f[0]) 
    major_minor = file_type.split('/')
    if major_minor[0] == 'text' :
        os.mkdir(os.path.join(root_path, *location_dir_path, 'similar_content'))   

    

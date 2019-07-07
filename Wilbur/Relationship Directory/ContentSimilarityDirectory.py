'''Another program ('RelationshipDirectory.py') creates a 'copy' of directory specified with the variable 'path' with symlinks to all the files inside a folder with the same name as the files.
This Program creates a Directory that contains symlinks to all the files with similar content. 
These symlinks are in a Directory which is a sibling to the file symlink.
Specify the path of directory to be scanned using the variable 'path' 
''''

from datasketch import MinHash, MinHashLSH
import textdistance
import numpy as np
import os
import operator
import magic


path = '../FileSystemTest'
root_path = 'relationship_root2'

text_files = []

for r, d, f in os.walk(path):
    f = [file for file in f if not (file[0] == '.' or file[0] == '_')]
    d[:] = [dr for dr in d if not (dr[0] == '.' or dr[0] == '_')]
    for file in f:
            filepath = os.path.join(r, file)
            mime = magic.Magic(mime=True)
            file_type = mime.from_file(filepath) 
            major_minor = file_type.split('/')
            
            if major_minor[0] == 'text' :
            	text_files.append(os.path.abspath(filepath))
documents =[]
for f in text_files:
    print(f)
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




for result in results:
    split_result = result[0].split('/')[4:]
    for r in result[1]:
        split_path = r.split('/')[4:]
        if r!=result[0]:
            try:
                os.symlink(os.path.abspath(os.path.join(root_path, *split_path)),os.path.join(root_path, *split_result, 'similar_content', '-'.join(split_path[::-1])) )
            except FileExistsError:
                print("File already exists")

	

 

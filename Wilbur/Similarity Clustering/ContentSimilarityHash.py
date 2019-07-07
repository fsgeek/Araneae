'''Prints out text files that have similar content by putting them in similarity sensitive hash tables
'''

from datasketch import MinHash, MinHashLSH
import textdistance
import numpy as np
import os
import operator
import magic


path = '../Desktop'

text_files = []

for r, d, f in os.walk(path):
    f = [file for file in f if not file[0] == '.']
    d[:] = [dr for dr in d if not dr[0] == '.']
    for file in f:
            filepath = os.path.join(r, file)
            mime = magic.Magic(mime=True)
            file_type = mime.from_file(filepath) 
            major_minor = file_type.split('/')
            
            if major_minor[0] == 'text' :
            	text_files.append(os.path.abspath(filepath))


documents = [(open(f).read(),f) for f in text_files]
lsh = MinHashLSH(threshold=0.4, num_perm=128)

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
	results.append(result)

results = np.array(results) 
results = (np.unique(results)) 


for r in results:
	print(r)
	print('\n')
 

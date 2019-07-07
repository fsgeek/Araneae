'''Prints out text files that have same content by using filecmp library - it compares file metadeta and hashes
'''

import filecmp
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
            
            if major_minor[0] == 'text' and major_minor[1] == 'plain':
            	text_files.append(os.path.abspath(filepath))
            	print(filepath)

i=0
print(len(text_files))
for f1 in text_files:
    for f2 in text_files:
        if filecmp.cmp(f1,f2) == True and f1!=f2:
            i=i+1
            print(f1)
            print(f2)
            print('\n')


print(i)

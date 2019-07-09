'''Prints out text files that have similar content by using Levenshtein string similarity metric
'''

import textdistance
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


documents = [(open(f).read(),f) for f in text_files]

i=0
print(len(documents))
for f1 in documents[0:5]:
    for f2 in documents[5:10]:
        similarity = textdistance.levenshtein.similarity(f1[0],f2[0])
        if similarity > 0 :
            i=i+1
            print(f1[1])
            print(f2[1])
            print(similarity)
            print('\n')


print(i)

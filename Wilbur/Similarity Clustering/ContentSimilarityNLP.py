'''Prints out text files that have similar content by using sklearn NLP library
'''
from sklearn.feature_extraction.text import TfidfVectorizer
import os
import operator
import shutil
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


documents = [open(f).read() for f in text_files]
tfidf = TfidfVectorizer().fit_transform(documents)
# no need to normalize, since Vectorizer will return normalized tf-idf
pairwise_similarity = tfidf * tfidf.T
count=0

for i, j in zip(*pairwise_similarity.nonzero()):
    if(pairwise_similarity[i,j]>0.5 and i!=j) :
        print(text_files[i])
        print(text_files[j])
        print(pairwise_similarity[i,j])
        print('\n')
        count = count+1

print(count) 



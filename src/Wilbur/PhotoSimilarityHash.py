'''Test program for image similarity'''
from PIL import Image
import imagehash
import os

path = 'Photos'

photos = []

for r, d, f in os.walk(path):
    for file in f:
            filepath = os.path.join(r, file)
            print(filepath)
            photohash = imagehash.average_hash(Image.open(filepath))
            photos.append((filepath,photohash))



for p1 in photos:
    for p2 in photos:
        similarity = p1[1] - p2[1]
        if similarity == 0 and p1[0]!=p2[0]:
            print(p1[0])
            print(p2[0])
            print(similarity)
            print('\n')


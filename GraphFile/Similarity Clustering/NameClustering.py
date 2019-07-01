'''Prints out files that have similar names and location path using Jaro string similarity and Levenshtein string similarity
Uses sklearn clustering library
'''

# from sklearn.cluster import KMeans
# import numpy as np
# X = [[1], [2], [3], [4], [5], [0]]
# kmeans = KMeans(n_clusters=2, random_state=0).fit(X)
# print(kmeans.labels_)
import os
import numpy as np
import sklearn.cluster
import textdistance

path = '../Desktop'

filenames = []
filepaths = []
filetimes = []

files = []
for r, d, f in os.walk(path):
    f = [file for file in f if not file[0] == '.' ]
    d[:] = [dr for dr in d if not dr[0] == '.']
    for file in f:
            filepath = os.path.join(r, file)
            filepath2 = filepath.split('/')
            filename = os.path.splitext(filepath2[-1])
            filename = os.path.splitext(filename[0])
            last_modif_time = os.path.getmtime(filepath)
            files.append(filepath2[-1])
            filepaths.append(filepath)
            filenames.append(filename[0])
            filetimes.append(last_modif_time)


files = np.asarray(files) #So that indexing with a list will work
filenames = np.asarray(filenames) #So that indexing with a list will work
filepaths = np.asarray(filepaths) #So that indexing with a list will work
filetimes = np.asarray(filetimes) #So that indexing with a list will work

namesimilarity = np.array([[textdistance.jaro.similarity(f1,f2) for f1 in filenames] for f2 in filenames])
#timesimilarity = - (np.array([[(f1-f2)*(f1-f2) for f1 in filetimes] for f2 in filetimes]))
locationsimilarity = 0.5*np.array([[textdistance.levenshtein.similarity(f1,f2) for f1 in filepaths] for f2 in filepaths])

namesimilarity = namesimilarity/np.mean(namesimilarity)
#timesimilarity = timesimilarity/np.mean(timesimilarity)
locationsimilarity = locationsimilarity/np.mean(locationsimilarity)



similarity =  namesimilarity  + locationsimilarity #+ timesimilarity

affprop = sklearn.cluster.AffinityPropagation(affinity="precomputed", damping=0.5)
affprop.fit(similarity)
for cluster_id in np.unique(affprop.labels_):
    exemplar = files[affprop.cluster_centers_indices_[cluster_id]]
    cluster = files[np.nonzero(affprop.labels_==cluster_id)]
    cluster_str = ", ".join(cluster)
    print(" - *%s:* %s" % (exemplar, cluster_str))
    print("\n")

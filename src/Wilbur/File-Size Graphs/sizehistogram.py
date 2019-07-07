'''Create a histogram of file sizes''''
import os
import numpy as np
import matplotlib.mlab as mlab
import matplotlib.pyplot as pl


path = '../FileSystemTest'

filesizes = []

for r, d, f in os.walk(path):
    for file in f:
            filepath = os.path.join(r, file)
            filesizes.append(os.path.getsize(filepath)/1000000)

pl.title('File Size Histogram')
pl.xlabel('Size (mega bytes)')
pl.ylabel('Number of files')
pl.hist(filesizes, bins=np.logspace(np.log10(0.00001),np.log10(max(filesizes)), 50))
pl.gca().set_xscale("log")
pl.show()
pl.show()



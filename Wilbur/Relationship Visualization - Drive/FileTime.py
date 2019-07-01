# Used by 'ContainsRelationship.sh' to compute folder modification times as the time when the last file mofification happened among the files that it contains

import sys
import os
import numpy as np

 # '''Get File/Folder last modification time (lmt)lmt for folder is the lmt of the file in it that was modified the last '''

def getmtime(path):
	mtime = 0
	if os.path.isdir(path): 
		for r, d, f in os.walk(path):
			f = [file for file in f if not (file[0] == '.' or file[0] == '_')]
			d[:] = [dr for dr in d if not (dr[0] == '.' or dr[0] == '_')]
			for file in f:
				filepath = os.path.join(r, file)
				filemtime = os.path.getmtime(os.path.abspath(filepath))
				if filemtime>mtime:
					mtime = filemtime
	else:
		mtime = os.path.getmtime(os.path.abspath(path))
	print(mtime)

getmtime(sys.argv[1]) 

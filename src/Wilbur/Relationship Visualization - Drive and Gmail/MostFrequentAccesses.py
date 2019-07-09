import os
import time
from operator import itemgetter

accesslog = (open('accesslog').read())

log_sts = accesslog.split('\n')

accesses = {}

currentTimesinceEpoc = time.time()

mark = 0 #log statements only start in line 5

user = input("Enter user name: ")

for log_st in log_sts:
	try:
		log_split = log_st.split(' ')
		fsentity = log_split[6]
		if log_split[10] == user:
			if accesses.get(fsentity) == None :
				accesses[fsentity] =1
			else:
				accesses[fsentity] = accesses[fsentity] + 1
	except IndexError:
		continue

accesslist =[]

for access in accesses:
	accesslist.append([access, accesses[access]])
	
currentTimesinceEpoc = str(time.time())

accesslist = sorted(accesslist, key=itemgetter(1), reverse=True)
i=0

dirmtime = '0' #modif time for most recent access folder

for fsobj in accesslist:
	try:
		i = i+1
		if i ==6:
			break
		mtime = str(os.path.getmtime(os.path.abspath(fsobj[0])))
		if (float(mtime)>float(dirmtime)):
			dirmtime = mtime
	except FileNotFoundError:
		continue

print('location' + '^' + 'dir' + 'root' + '^' + 'Root'  + '^' + currentTimesinceEpoc + '^' + 'dir'+'mostfrequentaccess'+ '^' + 'Most Frequent Access'  + '^' + dirmtime)

i=0

for fsobj in accesslist:
	try:
		i = i+1
		if i ==6:
			break
		name = fsobj[0].split('/')[-1]

		mtime = str(os.path.getmtime(os.path.abspath(fsobj[0])))
		if os.path.isdir(os.path.abspath(fsobj[0])):
			print('location' + '^' + 'dir' + 'mostfrequentaccess'+ '^' + 'Most Frequent Access' + '^' + str(0) + '^' + 'dir'+fsobj[0]+ '^' + name  + '^' + mtime)
		else:
			print('location' + '^' + 'dir' + 'mostfrequentaccess'+ '^' + 'Most Frequent Access' + '^' + str(0) + '^' + fsobj[0]+ '^' + name  + '^' + mtime)
	except FileNotFoundError:
		continue

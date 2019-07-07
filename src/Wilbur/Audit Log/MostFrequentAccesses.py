import os
from operator import itemgetter

accesslog = (open('accesslog').read())

log_sts = accesslog.split('\n')

accesses = {}

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


print(sorted(accesslist, key=itemgetter(1), reverse=True))

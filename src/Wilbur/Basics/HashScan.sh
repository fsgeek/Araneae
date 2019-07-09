#Scans a directory and prints out file hashes (md5sum) into a file 'Hashes'
#Pass the directory address as an argument to the function 'scan'

scan()
{
	local path=$1
  	for deeperPath in $path/*
	do
	 
   	 
	 

		if [ -d "${deeperPath}" ] ; then
		
		scan $deeperPath; stat -c '%N' $path; 
		else
			stat -c '%i' $deeperPath >> Hashes    #inode
			echo $deeperPath >> Hashes     	      #name
			md5=($(md5sum $deeperPath))
			echo $md5 >> Hashes 		      #hash
        fi
	
	
         
	done
}



>  Hashes #clean the file
scan "../col226" 


# Scans a directory and prints out file and folder attributes (name, time, extension, inode) into a file 'ObjectAttributes'
# Pass the directory address as an argument to the function 'scan'

scan()
{
	local path=$1
  	for deeperPath in $path/*
	do
	 
   	 
	 stat -c '%i' $deeperPath >> ObjectAttributes  #inode
	 deeperpathname="${deeperPath##*/}"
	 echo $deeperpathname >> ObjectAttributes      #name
	 extension="${deeperpathname##*.}"
	 if [ -d "${deeperPath}" ] ; then
		echo "directory" >> ObjectAttributes
	 else echo $extension >> ObjectAttributes      #extension
	 fi	
	 file $deeperPath -b >> ObjectAttributes       #type
	 md5=($(md5sum ObjectAttributes))
	 echo $md5 >> ObjectAttributes 		       #hash
     	 stat -c '%x' $deeperPath >> ObjectAttributes  #last access
	 stat -c '%y' $deeperPath >> ObjectAttributes  #last modification
	 stat -c '%z' $deeperPath >> ObjectAttributes  #last change

		if [ -d "${deeperPath}" ] ; then
		
		scan $deeperPath; stat -c '%N' $path; 
		
		fi
	
	
         
	done
}



>  ObjectAttributes
scan "../col226" 


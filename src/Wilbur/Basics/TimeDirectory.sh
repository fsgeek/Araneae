# Scans a directory and creates a time directory structure of the form 'time/modification_year/modification_month/file' with symlinks to files
# Pass the directory address as an argument to the function 'scan'

scan()
{
	local path=$1
  	for deeperPath in $path/*
	do
	 
   	 
	 

		if [ -d "${deeperPath}" ] ; then
		
		scan $deeperPath; stat -c '%N' $path; 
		else
			last_modification_year=$(date -d @$( stat -c %Y $deeperPath ) +%Y) #last modification
			last_modification_month=$(date -d @$( stat -c %Y $deeperPath ) +%m) #last modification
			mkdir -p "time/$last_modification_year/$last_modification_month"
			deeperpathname="${deeperPath##*/}"
			inode=$(stat -c '%i' $deeperPath)
			realpath=$(realpath $deeperPath)
			ln -s $realpath "time/$last_modification_year/$last_modification_month/$inode$deeperpathname"
			
       		fi
	  
         
	done
}

scan "../col226" 


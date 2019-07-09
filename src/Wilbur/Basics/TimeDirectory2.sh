# File for some test purpose

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
			last_modification=$(date -d @$( stat -c %Y $deeperPath ) +%Y%m%d)
			echo $last_modification
        fi
	  
     	if [ -d "${deeperPath}" ] ; then
		
		scan $deeperPath; stat -c '%N' $path; 
		else
			stat -c '%i' $deeperPath >> Hashes    	  #inode
			echo $deeperPath >>  TimeSort     	  #name
			md5=($(md5sum $deeperPath))
			echo $md5 >>  TimeSort		          #hash
			last_modification=$(date -d @$( stat -c %Y $deeperPath ) +%Y%m%d)
			echo $last_modification
			
        fi    
	done
}

>  TimeSort
scan "../col226" 


# Scans a directory and and creates a two dimensional (location and time) directory structure  with symlinks to files
# Eg of paths would be - 'root2/year-2019/Desktop/Folder1/month-04/SubFolder1/day-22/File' , 'root2/year-2019/Desktop/Folder1/month-04/SubFolder1/day-22/File'
# Pass the directory address as an argument to the function 'scan'

create_tree() #creates the 2-d directory structure
{
	local timepath="$1"
	local locationpath="$2"
	local realpath="$3"
	local name="$4"
	

	local timetail="${timepath#/*/}"
	local timehead="${timepath%/$timetail}"
	local locationtail="${locationpath#/*/}"
	local locationhead="${locationpath%/$locationtail}"
	
		
	if [ "$timehead" != "$timetail" ]; then
		mkdir -p "time$timehead"
		cd "time$timehead"
		create_tree "/$timetail" "$locationpath" "$realpath" "$name" "$5" "$6"
		cd ../..
	else 
		if [ "$5" = "1" ]; then
		if [ "$locationhead" != "$locationtail" ]; then
		mkdir -p "time$timehead"
		cd "time$timehead"
		create_tree "$timetail" "$locationpath" "$realpath" "$name" "0" "$6"
		cd ../..
		fi
		fi	
	fi
	
		
	if [ "$locationhead" != "$locationtail" ]; then
		mkdir -p "location$locationhead"
		cd "location$locationhead"
		create_tree "$timepath" "/$locationtail" "$realpath" "$name" "$5" "$6"
		cd ../..
	else 
		if [ "$6" = "1" ]; then
		if [ "$timehead" != "$timetail" ]; then
		mkdir -p "location$locationhead"
		cd "location$locationhead"
		create_tree "$timepath" "$locationtail" "$realpath" "$name" "$5" "0"
		cd ../..
		fi
		fi	
	fi

	if [ "$locationhead" = "$locationtail" ]; then
		if [ "$timehead" = "$timetail" ]; then
			ln -s "$realpath" "$name"
		fi
	fi

	
}

scan() #scans the given directory
{
	local path=$1
	
	
  	for deeperPath in $path/*
	do
	 
   	 

		if [ -d "${deeperPath}" ] ; then
		
		scan $deeperPath; stat -c '%N' $path; 
		else
			local last_modification_year=$(date -d @$( stat -c %Y $deeperPath ) +%Y) #last modification
			local last_modification_month=$(date -d @$( stat -c %Y $deeperPath ) +%m) #last modification
			local last_modification_day=$(date -d @$( stat -c %Y $deeperPath ) +%d) #last modification
			local deeperpathname="${deeperPath##*/}"
			local inode=$(stat -c '%i' $deeperPath)
			local name="$deeperpathname-$inode" 
			local realpath=$(realpath $deeperPath)
			 

			cd root2
			create_tree "/year-$last_modification_year/month-$last_modification_month/day-$last_modification_day/$deeperpathname" "${deeperPath//.}" "$realpath" "$name" "1" "1"
			cd ..

        fi
	  
         
	done
	
}
mkdir root2
scan "../Desktop/col226" 
scan "../Desktop/Project" 
./TimeDirectorySummarize.sh

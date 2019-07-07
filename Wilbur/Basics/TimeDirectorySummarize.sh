# Acts on the Directory structure created by 'TimeDirectory3.sh'. Both of these together form the tree summarization of 2-D location and time hierarchy.
# The function 'summarize' would try to find nodes in the tree hierarchy with more than 'merge_threshhold' files and call the function 'merge'
# 'merge' would create a node containing all the files in its children
# 'make_unique' would choode one of time or location hierarchy to expand each node based on how many children each dimension has - the lesser the better
# 'make clean' would remove intermediary directories 'time' and 'location'
# 'make_cleaner' would combine folders with single child to that child

merge()
{
	local path=$1
	local realpath=$(realpath $path)
	local containsobjects=($(ls $realpath))
	local symlinks=($(find $realpath -type l -print ))

	for symlink in "${symlinks[@]}"
	do
		local basename=$(basename $symlink)
		ln -sf $(readlink -f $symlink) "$realpath/$basename"
	done
	for object in "${containsobjects[@]}"
	do
		if [ -d "$realpath/$object" ] ; then
		rm -r "$realpath/$object"
		fi
	done
	
}
summarize()
{
	local path=$1
  	for deeperpath in $path/*
	do
		local realpath=$(realpath $deeperpath)
		local number_links=$(find $realpath -type l -print | wc -l)
		
	    if [ $number_links -gt $merge_threshhold ] ; then
	    	
			summarize "$deeperpath"
		else 
			echo "$deeperpath"
			merge "$deeperpath"
        fi

     done

      local number_links_parent=$(find $path -type l -print | wc -l)
		
	   if [ $number_links_parent -lt $merge_threshhold ] ; then
	   merge "$path"
	   fi
}

make_unique()
{
	
	local path=$1
	echo $path
	local location_size=$merge_threshhold
	local time_size=$merge_threshhold
	local flag="0"
  	for deeperpath in $path/*
	do
		local basename=$(basename $deeperpath)
		if [ $basename = "location" ]; then
			location_size=$(ls $deeperpath | wc -l)
			flag="1"
		fi
		if [ $basename = "time" ]; then
			time_size=$(ls $deeperpath | wc -l)
			flag="1"
		fi

     done
     
     
     if [ $flag = "1" ]; then
     	if [ $time_size -lt $location_size ]; then
     		rm -r "$path/location"
     	else
     		rm -r "$path/time"
     	fi
     fi
     for deeperpath in $path/*
	 do
	 	if [ -d "${deeperpath}" ] ; then
		make_unique "$deeperpath"
		fi

     done

}

make_clean() #to choose remove location/time directory names
{
	
	local path=$1
	echo $path
	
	
  	for deeperpath in $path/*
	do
		local basename=$(basename $deeperpath)
		if [ $basename = "location" ]; then
			mv  $path/location/*  $path
			rm -r  "$path/location"
		fi
		if [ $basename = "time" ]; then
			mv  $path/time/*  $path
			rm -r  "$path/time"
		fi

     done
     
     for deeperpath in $path/*
	 do
     if [ -d "${deeperpath}" ] ; then
     	make_clean $deeperpath
     fi
     done
    

}
make_cleaner() #to remove directories with single children
{
	local path=$1
	echo $path
	
	
  	
		
		local directory_size=$(ls $path | wc -l)
		
		if [ $directory_size -eq 1 ]; then
			local child=$(ls $path)
			for deeperpath1 in $path/*
			do
				local deeper_directory_size=$(ls $deeperpath1 | wc -l)
				if [ -d "${deeperpath1}" ] ; then
					for deeperpath2 in $deeperpath1/*
					do
					local basename=$(basename $deeperpath2)
					mv  $deeperpath2  $path/$child-$basename
					done
				fi
			done
			if [ -d "$path/$child" ] ; then
			rm -r  "$path/$child"
			fi
			
		fi
		

    
     local directory_size=$(ls $path | wc -l)
     if [ $directory_size -eq 1 ]; then
     local child=$(ls $path)
     if [ -d "$path/$child" ] ; then
		make_cleaner "$path"
	 fi
	 else
     for deeperpath in $path/*
	 do
     if [ -d "${deeperpath}" ] ; then
     	
     	make_cleaner "$deeperpath"
     fi
     done
 	 fi
}
merge_threshhold=20;
summarize "root2" 
make_unique "root2"
make_clean "root2"
make_cleaner "root2"

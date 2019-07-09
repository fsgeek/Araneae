# Shell script to remove directories with single child by merging them with that child
# Pass the path to the directory that needs to be scanned for such sub-directories as an argument to 'scan'

make_clean() #to remove directories with single children
{
	local path="$1"
	
	local directory_size=$(ls "$path" | wc -l)
		
		if [ $directory_size -eq 1 ]; then
			local child=$(ls "$path")
			for deeperpath1 in "$path"/*
			do
				local deeper_directory_size=$(ls "$deeperpath1" | wc -l)
				if [ -d "${deeperpath1}" ] ; then
					for deeperpath2 in "$deeperpath1"/*
					do
					local basename="$(basename "$deeperpath2")"

						cp -r "$(realpath "$deeperpath2")"  "$(realpath "$path/$child-$basename")"
				
					done
				fi
			done
			if [ -d "$path/$child" ] ; then
			rm -r  "$path/$child"
			fi
			
		fi
		

    
     local directory_size=$(ls "$path" | wc -l)
     if [ $directory_size -eq 1 ]; then
     local child="$(ls "$path")"
     if [ -d "$path/$child" ] ; then
		make_clean "$path"
	 fi
	 else
     for deeperpath in "$path"/*
	 do
     if [ -d "${deeperpath}" ] ; then
     	if ! [ -L "${deeperpath}" ] ; then #not a link
     		make_clean "$deeperpath"
     	fi
     fi
     done
 	 fi
}

make_clean "time_root_deeper_time"

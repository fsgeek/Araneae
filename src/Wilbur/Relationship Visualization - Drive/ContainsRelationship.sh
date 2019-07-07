# Shell script to scan a directory and print out all 'contains' relationship in a file named 'graphdata5' in a specific format as needed by 'GraphVisualization2.py'
# Pass the directory address as an argument to the function 'scan'
# Run this after DriveAndSystemRelations.py 
scan()
{
	local path="$1"
	
  	for deeperpath in "$path"/*
	do
	 inodepath="$(stat -c '%i' "$path")" 
   	 abspath="$(realpath "$path")"
	 inodedeeperpath="$(stat -c '%i' "$deeperpath")" 
	 absdeeperpath="$(realpath "$deeperpath")"
	 pathname="${path##*/}";
	 deeperpathname="${deeperpath##*/}";
	 pathmtime="$(python3 FileTime.py "$path")"
	 deeperpathmtime="$(python3 FileTime.py "$deeperpath")"
	 if [ -d "${deeperpath}" ] ; then
	 	echo "location^dir$abspath^$pathname^$pathmtime^dir$absdeeperpath^$deeperpathname^$deeperpathmtime" >> graphdata5
	 else
	 	echo "location^dir$abspath^$pathname^$pathmtime^$absdeeperpath^$deeperpathname^$deeperpathmtime" >> graphdata5
	 fi
	
	
		if [ -d "${deeperpath}" ] ; then
		
			scan "$deeperpath"; stat -c '%N' "$path"; 
		fi
	done
}


scan "../../Desktop" 



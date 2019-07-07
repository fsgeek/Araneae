# Shell script to scan a directory and print out all 'contains' relationship in a file named 'ContainsRelationship (file/folder contained is printed below the folder it is contained in)'
# Pass the directory address as an argument to the function 'scan'

scan()
{
	local path=$1
  	for deeperPath in $path/*
	do
	 
   	 stat -c '%i' $path >> ContainsRelationship
	 pathname="${path##*/}";
	 echo $pathname >> ContainsRelationship
	 stat -c '%i' $deeperPath >> ContainsRelationship
	 deeperpathname="${deeperPath##*/}";
	 echo $deeperpathname >> ContainsRelationship
	 
	
	
		if [ -d "${deeperPath}" ] ; then
		
		scan $deeperPath; stat -c '%N' $path; 
		fi
	done
}



>  ContainsRelationship
scan "../col226" 



//Reads the file 'Hashes' which contains the hash values of files in a directory
//Finds out files that have the same hash and writes them one below the other in the file 'HashRelationship'

#include<iostream>
#include<stdlib.h>
#include<vector>
#include <fstream>
#include <algorithm>
#include <cstring>
#include <string.h>
#include<bits/stdc++.h> 
#include<tuple>
using namespace std;

int totalAttributes = 3;
int inodeAttributeNumber = 0;
int nameAttributeNumber = 1;
int hashAttributeNumber = 2;
bool sortbysec(const tuple <string,string,string> &a, 
              const tuple <string,string,string> &b) 
{ 
    return ( get<2>(a) <  get<2>(b)); 
} 
  
int main()
{ 
	
	string line;
   	ifstream File ("Hashes");
	vector<tuple <string,string,string> > inodeAndHash;	

	 if (File.is_open())
	 {
	 	long i=0;
	 	string inode;
	 	string name;
	 	string hash;
	 	while(getline(File, line))
	 	{

	 		line.erase(remove_if(line.begin(), line.end(), ::isspace),line.end());
	 		if(i%totalAttributes == inodeAttributeNumber)
	 		{inode = line;}
	 		if(i%totalAttributes == nameAttributeNumber)
	 		{name = line;}
	 		if(i%totalAttributes == hashAttributeNumber)
	 		{hash = line;}
	 		if ((i%totalAttributes == totalAttributes-1))
	 		{inodeAndHash.push_back(make_tuple(inode,name,hash));}
	 		i++;

	 	}

	 	sort(inodeAndHash.begin(), inodeAndHash.end(), sortbysec); 
	 	ofstream FileOut;
        FileOut.open ("HashRelationship");
 		
	 	for (long i=0; i<inodeAndHash.size(); i++) 
    	{ 
    		for (long j=i+1; j<inodeAndHash.size(); j++)
    			if (!strcmp((get<2>(inodeAndHash[i])).c_str(),(get<2>(inodeAndHash[j])).c_str()))
    			{
    				FileOut<< get<0>(inodeAndHash[i])<< "\n"
             		<< get<1>(inodeAndHash[i])<< "\n";
             		
    				
             		FileOut<< get<0>(inodeAndHash[j])<< "\n"
             		<< get<1>(inodeAndHash[j])<<"\n";
             		

             		FileOut<< get<0>(inodeAndHash[j])<< "\n"
             		<< get<1>(inodeAndHash[j])<< "\n";
    				
             		FileOut<< get<0>(inodeAndHash[i])<< "\n"
             		<< get<1>(inodeAndHash[i])<<"\n";
             		
             	}
             	
   		} 
   		FileOut.close();

	 }
	 else 
     {
        cerr << "Couldn't open file.\n";
     }
return 0;
}



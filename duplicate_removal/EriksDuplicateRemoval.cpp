
//#include "stdafx.h"				//there is nothing in this header

#include <stdio.h>
#include <dirent.h>
#include <sys/stat.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <chrono>

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
using namespace std;
	//Compile: 			gcc <name of this file>.cpp -o <nameOfThisFile>.exe
	//Run: 				<nameOfThisFile> parent_dir(full path) filename(full path)...

#define false 0
#define true  1

	int duplicateCount = 0;
	int j =0;

	int FindDuplicates(char* path, char* fileName);
	int CompareFiles(char* originalFile, char* currFile);
	vector<string> dupPaths;

	int main(int argc, char *argv[])
	{
		//Two additional arguments are expected: Parent dir, file to find duplicates of...
		

		if (argc != 3)
		{
			printf("Usage: %s 'Base Directory' 'File Name'\n", argv[0]);
			return -1;
		}

		//argv[1] = base dir, argv[2] = file to find duplicates of; e.g argv[1] = /home,
		//argv[2] = "file.txt"...

		
//		printf("Searching...");
		
		FindDuplicates(argv[1], argv[2]);
		
//		if(duplicateCount<=0)
//			printf("There are no duplicates \n");
//		if(duplicateCount>1)
//			printf("\nThe duplicates are \n");
//		else
//			printf("\nThe duplicate is \n");

		for(int i = 0; i<duplicateCount; i++){
			cout << (dupPaths[i]) << endl;
		}
		return 0;
	}

	void printSeaching()
	{
		if(j == 500)
			printf("\rSearching   ");
		if(j == 1000)
			printf("\rSearching.  ");
		if(j == 1500)
			printf("\rSearching.. ");
		if(j == 2000)
		{
			printf("\rSearching...");
			j = 0;
		}
		j++;
	}

	int FindDuplicates(char* path, char* fileName)
	{
		DIR *dir;
		struct dirent *dp;
		struct dirent *result;
		struct stat statp;

		char absoluteFilePath[255];

		if ((dir = opendir(path)) == NULL)		//opens the path into dir
		{
			//possibly trying to open shortcut or corrupt folder typically.
			//printf("Failed to open directory %s \n",path);
			return -1;
		}


		while ((dp = readdir(dir)) != NULL)
		{

			//readdir can return . and .. which we should ignore...
			if (strcmp(dp->d_name, ".") && strcmp(dp->d_name, ".."))
			{
				//find file full path, relative to base path. e.g, a /home/file.txt...

				//copy path to absoluteFilePath...
				strcpy(absoluteFilePath, path);

				//append / at end...
				strcat(absoluteFilePath, "/");

				//append filename to path...
				strcat(absoluteFilePath, dp->d_name);

				//check if the current file is actually file or dir...
				stat(absoluteFilePath, &statp);

				if (S_ISDIR(statp.st_mode))		//is a directory...
				{
					//recurse through this dir...
					FindDuplicates(absoluteFilePath, fileName);
				}
				else if (S_ISREG(statp.st_mode))	//is a file...
				{
					//updates the searching graphic when analyzing a file
					printSeaching();
					//compare current file name with the file specified by user...
					if (strcmp(fileName, absoluteFilePath))
					{
						if (CompareFiles(fileName, absoluteFilePath))
						{
							//yes, duplicate; print it...
//							printf("This is a duplicate! %s\n", absoluteFilePath);
							dupPaths.push_back(absoluteFilePath);
							duplicateCount++;
						}
					}

				}		//end else if (S_ISREG(statp.st_mode))...

			}	    //if (strcmp(dp->d_name, ".") && strcmp(dp->d_name,".."))...
		}	    //end while...

		closedir(dir);
		return 0;

	}


	int CompareFiles(char* originalFile, char* currFile)
	{
		//two step comparison: (1) first check size; if not same, return false.
		//If equal, (2) compare file content.If equal, return true, false otherwise...

		struct stat statOriginal, statCurr;
		stat(originalFile, &statOriginal);
		stat(currFile, &statCurr);

		//Step 1...
		if ((int)statOriginal.st_size != (int)statCurr.st_size)  //size not same...
			return false;
	
		FILE* fdOriginal;
		if (fdOriginal = fopen(originalFile, "r")) {
			if (fdOriginal == NULL) { fputs("File error", stderr); return false; }
		}
		else return false;        								//error occurred, not sure if duplicate
		FILE* fdCurr;
		if (fdCurr = fopen(currFile, "r")) {
			if (fdCurr == NULL) { fputs("File error", stderr); return false; }
		}
		else return false;

		int chunkSize = 1024, objsRead;
		char *bufferOriginal = (char*)malloc(chunkSize * sizeof(char));
		if (bufferOriginal == NULL) { fputs("Memory error for buff orig", stderr); exit(2); }

		char *bufferCurr = (char*)malloc(chunkSize * sizeof(char));
		if (bufferCurr == NULL) { fputs("Memory error for buff curr", stderr); exit(2); }


		while (true)
		{
			//read file in chunk...
			//std::size_t fread( void* buffer, std::size_t size, std::size_t count, std::FILE* stream );
			objsRead = fread(bufferOriginal, sizeof(char), chunkSize , fdOriginal);
			if (objsRead <= 0)
				break;		//end of file...

			objsRead = fread(bufferCurr, sizeof(char), objsRead, fdCurr);

			//compare buffer...
			if (strcmp(bufferOriginal, bufferCurr))	    //if content not matching...
				return false;
		}

		return true;
	}

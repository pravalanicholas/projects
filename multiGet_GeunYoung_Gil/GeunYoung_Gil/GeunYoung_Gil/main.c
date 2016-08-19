/// \file main.c
///
/// \mainpage Multi-GET
///
/// \section intro Program Introduction
/// - This program implements the application which revieve chunks of data from remote resource by using libcurl.
///
/// Major <b>Multi-GET</b> features to note are:
/// 
/// -# Usage: GeunYoung_Gil [OPTIONS] url
///				-o string
///					Write output to <file> instead of default
///				-parallel
///					Download chunks in parallel instead of sequentally
///				-n number
///					Set number of chunks (example: -n 6)
///				-s number
///					Set size of a chunk (example: -s 10000)
///
/// -# File is downloaded in 4 chunks - default
/// -# first 4 MiB of the file downloaded from the server - default
/// -# File is retrieved with GET requests
/// -# Support parallel download 
/// -# Support custimizing number of chunks and size of a chunk 
/// -# Reference from https://curl.haxx.se/libcurl/c/
///
/// \section notes Special Release Notes
/// <b>Nothing</b>
///
/// \section requirements Style and Convention Requirements
///
/// \todo 
///
/// \bug <b>Nothing</b>
///
/// \section version Current version of this Class
/// <ul>
/// <li>\authors  GeunYoung Gil
/// <li>\version  1.00.00</li>
/// <li>\date     AUG.16.2016</li>
/// <li>\pre      Nothing
/// <li>\warning  Nothing
/// <ul>

#include <stdio.h>
#include "multiGet.h"

#define MIN_ARGS 2		// minimum arguments
#define MAX_URL_LEN 100	// max url length
#define DEFAULT_NUM_CHUNKS 4	// default number of chunks
#define DEFAULT_SIZE_CHUNK 1000000	// default size of a chunk

/// <summary>
/// Print the usage information
/// </summary>
/// <returns>0</returns>
int usageInfo()
{
	printf("\nUsage: GeunYoung_Gil [OPTIONS] url\n");
	printf("\t-o string \n");
	printf("\t\tWrite output to <file> instead of default \n");
	printf("\t-parallel \n");
	printf("\t\tDownload chunks in parallel instead of sequentally\n");
	printf("\t-n number \n");
	printf("\t\tSet number of chunks (example: -n 6) \n");
	printf("\t-s number \n");
	printf("\t\tSet size of a chunk (example: -s 10000) \n");

	return 0;
}

/// <summary>
/// Validate arguments for command and save the information from argument.
/// Saved argument is used for application
/// If it pass the validation, it will return 1
/// </summary>
int checkCommand(int argc, char* argv[],char* destFile, int* isParallel, char* url, int* numOfChunks, int* sizeOfChunk)
{
	if (argc < MIN_ARGS)
		return usageInfo();

	int numOfArgs = argc - 1;
	
	// validate options
	int i;
	for (i = 1; i < numOfArgs; ++i)
	{
		if (strcmp("-o", argv[i]) == 0)
		{
			if ((i + 1 < numOfArgs) && (*argv[i+1] != '-'))
			{
				memset(destFile, '\0', sizeof(destFile));
				strcpy(destFile, argv[++i]);
			}
			else 
			{
				printf("missing output file!");
				return usageInfo();
			}			
		}
		else if (strcmp("-parallel", argv[i]) == 0)
		{
			*isParallel = 1;
		}
		else if (strcmp("-n", argv[i]) == 0)
		{
			if ((i + 1 < numOfArgs) && (*argv[i + 1] != '-'))
			{
				sscanf(argv[++i], "%d", numOfChunks);
			}
			else
			{
				printf("Wrong number of chunks!");
				return usageInfo();
			}
		}
		else if (strcmp("-s", argv[i]) == 0)
		{
			if ((i + 1 < numOfArgs) && (*argv[i + 1] != '-'))
			{
				sscanf(argv[++i], "%d", sizeOfChunk);
			}
			else
			{
				printf("Wrong size of chunk!");
				return usageInfo();
			}
		}
		else
		{
			printf("Undefined option!\n");
			return usageInfo();
		}
	}

	// validate url
	if ((*argv[numOfArgs] == '-') ||  (i > numOfArgs))
	{
		printf("Invalid url!\n");
		return usageInfo();	
	}
	else
	{
		strcpy(url, argv[numOfArgs]);	// save url
	}

	return 1;
}

/// <summary>
/// Main function to execute application. It support parallel and serial downloads
/// </summary>
int main(int argc, char* argv[])
{
	char destFile[MAX_URL_LEN] = "384MB.jar";
	int isParallel = 0;
	char url[MAX_URL_LEN] = { '\0' };
	int numOfChunks = DEFAULT_NUM_CHUNKS;
	int sizeOfChunk = DEFAULT_SIZE_CHUNK;
	int isFailed = 0;

	if (checkCommand(argc, argv, destFile, &isParallel, url, &numOfChunks, &sizeOfChunk) == 1)
	{
		if (isParallel == 1)	// parallel mode
		{
			isFailed = parallelDownload(destFile, url, numOfChunks, sizeOfChunk);
		}
		else                    // serial mode
		{
			isFailed = serialDownload(destFile, url, numOfChunks, sizeOfChunk);
		}

		if (isFailed == 0)
		{
			printf("\nCompleted transmission.\n");
		}
		cleanCurlResource();	// clean global curl resource
	}

	return 0;
}


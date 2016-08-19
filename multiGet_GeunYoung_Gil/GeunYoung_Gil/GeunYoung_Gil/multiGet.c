/// \file multiGet.h
/// \brief 
/// - This source file contains the functions which used for MultiGET 

#include "multiGet.h"

/// <summary>
/// It download chunks of data from remote resource in serial.
/// It request each chunk with the range of lower and upper bound by order.
/// And each chunk stored in memory by order and write all received data in memory to the file.
/// </summary> 
int serialDownload(char* destFile, char* url, int numOfChunks, int sizeOfChunk)
{
	CURL** curlHandle = (CURL**)calloc(numOfChunks, sizeof(CURL*));
	struct MemoryStruct* chunk = (struct MemoryStruct*)calloc(numOfChunks, sizeof(struct MemoryStruct));
	int upperBound = 0;	// range
	int lowerBound = 0;

	// download chunks by order
	for (int i = 0; i < numOfChunks;++i)
	{
		upperBound = sizeOfChunk * (i + 1) - 1;
		curlHandle[i] = curl_easy_init();	// return CURL eash handle
		chunk[i].memory = malloc(1);  // will be grown as needed by the realloc above 
		chunk[i].size = 0;    // no data at this point 

		// set range and memory to receive
		if (setCurlOpt(curlHandle[i],lowerBound,upperBound,url,(void *)&chunk[i]) != 1)
		{
			printf("\nError: fail to create curl handle!\n");
			return 1;
		}

		CURLcode res = curl_easy_perform(curlHandle[i]);	// receive data
		if (res != CURLE_OK) {
			fprintf(stderr, "curl_easy_perform() failed: %s\n",
				curl_easy_strerror(res));	
			return 1;
		}

		lowerBound = upperBound + 1;
		curl_easy_cleanup(curlHandle[i]);
	}

	saveToFile(destFile, chunk, numOfChunks);	// save to file

	// free resource
	free(curlHandle);
	free(chunk);

	return 0;
}

/// <summary>
/// It download chunks of data from remote resource in parallel by using multi handle.
/// It set each chunk with the range of data and receive each chunk in parallel.
/// And each chunk stored in memory by order and write all received data in memory to the file.
/// </summary> 
int parallelDownload(char* destFile, char* url, int numOfChunks, int sizeOfChunk)
{
	CURL** curlHandle = (CURL**)calloc(numOfChunks, sizeof(CURL*));
	CURLM* multi_handle;
	struct MemoryStruct* chunk = (struct MemoryStruct*)calloc(numOfChunks, sizeof(struct MemoryStruct));
	int upperBound = 0;
	int lowerBound = 0;
	int still_running;

	multi_handle = curl_multi_init();	// initialize multi handler

	for (int i = 0; i < numOfChunks; ++i)
	{
		upperBound = sizeOfChunk * (i + 1) - 1;
		curlHandle[i] = curl_easy_init();	// return CURL eash handle
		chunk[i].memory = malloc(1);  // will be grown as needed by the realloc above
		chunk[i].size = 0;    // no data at this point 	

		if (setCurlOpt(curlHandle[i], lowerBound, upperBound, url, (void *)&chunk[i]) != 1)
		{
			printf("\nError: fail to create curl handle!\n");
			return 1;
		}

		lowerBound = upperBound + 1;
		curl_multi_add_handle(multi_handle, curlHandle[i]);	// add handle to multi handle
	}

	do {
		curl_multi_perform(multi_handle, &still_running);	// request chunks in parallel
	} while (still_running);

	saveToFile(destFile, chunk, numOfChunks);

	// free reource
	curl_multi_cleanup(multi_handle);

	for (int i = 0; i < numOfChunks; ++i)
	{
		curl_easy_cleanup(curlHandle[i]);
	}

	free(curlHandle);
	free(chunk);

	return 0;
}

/// <summary>
/// It set the option for a chunk.
/// url, range of data, write memory call back function, data to store
/// </summary> 
int setCurlOpt(CURL* curlHandle, int lowerBound, int upperBound, char* url, void* chunk)
{
	if (curlHandle)
	{
		curl_easy_setopt(curlHandle, CURLOPT_URL, url);	// set the url
		char range[MAX_STR_LEN];
		sprintf(range, "%d-%d", lowerBound, upperBound);
		curl_easy_setopt(curlHandle, CURLOPT_RANGE, range);	// set byte range to request
		curl_easy_setopt(curlHandle, CURLOPT_WRITEFUNCTION, writeMemory); // set call back function to write chunk
		curl_easy_setopt(curlHandle, CURLOPT_WRITEDATA, chunk);

		return 1;
	}
	
	return 0;
}

/// <summary>
/// Call back function to write data to memory with data content and data size
/// </summary> 
static size_t writeMemory(void *contents, size_t size, size_t nmemb, void* userp)
{
	size_t realsize = size * nmemb;
	struct MemoryStruct *mem = (struct MemoryStruct *)userp;

	mem->memory = realloc(mem->memory, mem->size + realsize + 1);

	// out of memory
	if (mem->memory == NULL) {	
		printf("not enough memory\n");
		return 0;
	}

	memcpy(&(mem->memory[mem->size]), contents, realsize);
	mem->size += realsize;
	mem->memory[mem->size] = 0;

	return realsize;
}

/// <summary>
/// save all received data in memory to file in binary
/// </summary> 
int saveToFile(char* destFile, struct MemoryStruct* chunk, int numOfChunks)
{
	FILE* fp = fopen(destFile, "wb");

	for (int i = 0; i < numOfChunks; ++i)
	{
		fwrite(chunk[i].memory,1,chunk[i].size,fp);
	}
	fclose(fp);
	cleanChunksMem(chunk, numOfChunks);
}

/// <summary>
/// free the memory which stored data from remote resource
/// </summary> 
void cleanChunksMem(struct MemoryStruct* chunk, int numOfChunks)
{
	for (int i = 0; i < numOfChunks; ++i)
	{
		free(chunk[i].memory);
	}
}

/// <summary>
/// free global curl resource
/// </summary> 
void cleanCurlResource()
{
	curl_global_cleanup();
}

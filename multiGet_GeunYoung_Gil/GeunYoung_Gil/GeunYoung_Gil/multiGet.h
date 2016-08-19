/// \file multiGet.h
/// \brief 
/// - This source file contains the functions which used for MultiGET 

#pragma once
#include <curl/curl.h>
#include <stdio.h>

#pragma warning(disable: 4996)

#define MAX_STR_LEN 100

// memory to store a received chunk
struct MemoryStruct {
	char *memory;
	size_t size;
};

int setCurlOpt(CURL* curlHandle, int lowerBound, int upperBound, char* url, void* chunk);
int serialDownload(char* destFile, char* url, int numOfChunks, int sizeOfChunk);
int parallelDownload(char* destFile, char* url, int numOfChunks, int sizeOfChunk);
static size_t writeMemory(void *contents, size_t size, size_t nmemb, void* userp);
int saveToFile(char* destFile, struct MemoryStruct* chunk, int numOfChunks);
void cleanCurlResource();
void cleanChunksMem(struct MemoryStruct* chunk, int numOfChunks);


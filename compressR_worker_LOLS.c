/*
 * CS214: Systems Programming, Fall 2016
 * Assignment 2
 * compressR_worker_LOLS.c
 * Baljit Kaur & Amy Guinto
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <ctype.h>

char* convert_its(int num) {
	int size = 0, numSize = num;

	while(numSize != 0) {
		size++;
		numSize /= 10;
	}

	char *buf = malloc(size+1);

	int i, remainder;
	for(i = 0; i < size; i++) {
		remainder = num % 10;
		num /= 10;
		buf[size - (i+1)] = '0' + remainder;
	}
	buf[size] = '\0';

	return buf;
}

char * getNewFileName(char *file, int part) {
	char * newFile = (char *)malloc(strlen(file) + 7);
	
	int i, pos = 0;
	char c;
	for(i = 0; i < strlen(file); i++) {
		c = file[i];
		if(c == '.') {
			pos = i;
			break;
		}
	}

	if(part == -1)
		sprintf(newFile, "%s_LOLS", file);
	else
		sprintf(newFile, "%s_LOLS%d", file, part);

	newFile[pos] = '_';

	return newFile;
}

int main(int argc, char **argv) {
	// char *file, int start, int size, int part, int numParts

	// get arguments
	int fd = open(argv[1], O_RDONLY);
	int start = atoi(argv[2]);
	int size = atoi(argv[3]);
	int part = atoi(argv[4]);
	int numParts = atoi(argv[5]);

	// compression
	char ret[size+1];
	lseek(fd, start, SEEK_SET);
	char string[size+1];
	read(fd, string, size);
	string[size] = '\0';

	int i;
	// replace non-alphabetic characters with a space (print out warning message)
	for(i = 0; i < strlen(string); i++) {
		if(!isalpha(string[i])) {
			if(!isspace(string[i]))
				printf("Warning: Non-alphabetic character encountered: %c\n", string[i]);
			string[i] = ' ';
		}
	}

	char c = string[0];
	int count = 1, pos = 0, retCount = 0;
	while(pos < size) {
		if(pos < size-1 && c == string[pos+1]) 
			count++;
		else if(isspace(string[pos+1])) {
			// do nothing if space is encountered (just want to advance)
		}
		else {
			if(count == 1)
				ret[retCount++] = c;
			else if(count == 2) {
				ret[retCount++] = c;
				ret[retCount++] = c;
			}
			else if(count < 10){
				if(!isspace(c)) {
					ret[retCount++] = '0' + count;
					ret[retCount++] = c;
				}
				
			} else {
				if(!isspace(c)) {
					char *buf = convert_its(count);
					int j = 0;
					while(buf[j] != '\0') {
						ret[retCount++] = buf[j++];
					}
					ret[retCount++] = c;
				}
			}
			count = 1;
			c = string[pos+1];
		}
		pos++;
	}
	ret[retCount] = '\0';

	if(numParts == 1)
		part = -1;

	// write to file
	char *newFileName = getNewFileName(argv[1], part);
	FILE *newFile = fopen(newFileName, "w");
	fprintf(newFile, "%s", ret);

	close(fd);
	fclose(newFile);
}
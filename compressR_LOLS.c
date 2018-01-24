/*
 * CS214: Systems Programming, Fall 2016
 * Assignment 2
 * compressR_LOLS.c
 * Baljit Kaur & Amy Guinto
 */

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

/* Defines text colors for error and usage messages */
#define COLOR_RED		"\x1b[31m"
#define COLOR_BLUE		"\x1b[34m"
#define COLOR_RESET		"\x1b[0m"

/*
 * Checks for correct number of arguments
 * If incorrect number (i.e. not two), prints an error and usage message
 */
int checkInput(int argc, char **argv) {
	if(argc != 3) {
		fprintf(stderr, COLOR_RED "ERROR: Incorrect number of arguments." COLOR_RESET "\n");
		fprintf(stdout, COLOR_BLUE "Usage: ./compressR_LOLS \"<file to compress>\" <number of parts>" COLOR_RESET "\n");
 		return 0;
	} else
		return 1;
}

// Converts int to corresponding char*
char* convert_itos(int num) {
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

/*
 * Checks if arguments are fit for use by workers
 * Starts and ends workers
 */
void compressR_LOLS(char *file, int numParts) {
	int fd = open(file, O_RDONLY);
	if(fd > 0) {
		if(numParts < 1) {
			fprintf(stderr, COLOR_RED "Error: Need to compress with 1 or more parts" COLOR_RESET "\n");
			return;
		}

		// fork child processes specified number of times
		int pid = 1, part = 0;
		while(pid != 0 && part < numParts) {
			pid = fork();
			part++;
		}
		part--;

		if(pid == 0) {	// child process
			char *args[7];

			int fileSize = lseek(fd, 0, SEEK_END);
			lseek(fd, 0, SEEK_SET);

			int overage = 0;
			if(fileSize%numParts != 0)
				overage = fileSize%numParts;

			int readMin = fileSize/numParts;

			int start = 0, size = readMin;
			if(part == 0) {
				size += overage;
			} else {
				start = (part)*readMin + overage;
			}

			// check if number of partitions exceeds size of file
			if(fileSize < numParts) {
				if(part == 0)
					fprintf(stderr, COLOR_RED "Error: More partitions than characters" COLOR_RESET "\n");
				return;
			}

			// exec
			args[0] = "./compressR_worker_LOLS";
			args[1] = file;
			args[2] = convert_itos(start);
			args[3] = convert_itos(size);
			args[4] = convert_itos(part);
			args[5] = convert_itos(numParts);
			args[6] = NULL;

			execv(args[0], args);
		}
		else if(pid > 0) {	// parent process
			int i;
			for(i = 0; i < numParts; i++) {
				wait(NULL);
			}
		}
		else {	// fork failed
			fprintf(stderr, COLOR_RED "Error: fork() failed" COLOR_RESET "\n");
				return;
		}

	} else {
		fprintf(stderr, COLOR_RED "Error opening file." COLOR_RESET "\n");
	}
	
	close(fd);
}

/*
 * Checks input, goes to compressR_LOLS if correct number of arguments
 */
int main(int argc, char **argv) {
	if(checkInput(argc, argv)) {
		char *file = argv[1];
		int numParts = atoi(argv[2]);

		compressR_LOLS(file, numParts);
	}
}
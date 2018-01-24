#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <string.h>
#include <pthread.h>
#include <errno.h>

struct argstocompress{
	char *filename;
	int numofchars;
	int beginIndex;
	char *destfile;
	int numworkers;
};
void *compress(void *arg);
void *compressLOLS(char *filename, int numofworkers);

int main(int argc, char *argv[]) {
	compressLOLS("test_small_50.txt", 10);
	return 0;
}

void *compress(void *arg){
	struct argstocompress *arg_struct = (struct argstocompress*) arg;
	FILE *fp;
	int count = 0;
	
	fp = fopen(arg_struct->destfile, "wb+");
	if(fp == NULL){
		printf("Error opening file!\n");
	}
	//printf("compress destname: %s\n", arg_struct->destfile);
	
	FILE * file = fopen(arg_struct->filename, "rb");
	if(file == NULL){
		printf("Error opening main file!\n");
	}
	//printf("compress filename: %s\n", arg_struct->filename);
	//printf("Error: %d \n", errno);
	char string[arg_struct->numofchars + 1];
	fseek(file, arg_struct->beginIndex, SEEK_SET);
	fread(string, sizeof(char), arg_struct->numofchars, file);
	string[arg_struct->numofchars] = '\0';
	int num = 0;
	count = 0;
	char ret[arg_struct->numofchars];
	ret[0] = '\0';
	
	if(strlen(string) <= 2){
		fprintf(fp, string);
		pthread_exit(0);
	}
	while (num < strlen(string)){
		if((string[num] == string[num + 1]) && (string[num] == string[num + 2])){
			num += 2;
			count += 3;
			while((string[num + 1] == string[num]) && (num < strlen(string))){
				num++;
				count++;
			}
			char buffer[arg_struct->numofchars];
			sprintf(buffer, "%d", count);
			strcat(ret, buffer);
		}
		else{
			if (string[num] == string[num + 1]){
				strncat(ret, &string[num], 1);
				strncat(ret, &string[num + 1], 1);
				num += 2;
			}
			else{
				strncat(ret, &string[num], 1);
				num++;
			}
		}
		count = 0;
	}
	ret[arg_struct->numofchars] = '\0';
	fprintf(fp, ret);
	printf("Result printed to file.\n");
	fclose(fp);
	pthread_exit(0);
}

void *compressLOLS(char *filename, int numberofworkers){
	int size;
	FILE *file = fopen(filename, "rb");
	if(file == NULL){
		printf("Error opening main file in compressLOLS!\n");
	}
	fseek(file, 0, SEEK_END);
	size = ftell(file);
	rewind(file);
	fclose(file);
	if(numberofworkers > size){
		printf("Error: more workers than characters.");
		return;
	}
	
	int count = 0;
	while(filename[count] != '.'){
		count++;
	}
	
	int i;
	char *mainfile = malloc(sizeof(strlen(filename)));
	strcpy(mainfile, filename);
	
	int numofchars = size/numberofworkers;
	int extra = size % numberofworkers;
	int index = 0;
	
	struct argstocompress args[numberofworkers];
	pthread_t tid[numberofworkers];
	//int worker = 0;
	
	for (i = 0; i < numberofworkers; i++){
		args[i].destfile = malloc(sizeof(char) * (count + 11));
		strncpy(args[i].destfile, filename, count);
		if (numberofworkers == 1 && i == 0){
			strcat(args[i].destfile, "_txt_LOLS");
		}else if(numberofworkers > 1){
			strcat(args[i].destfile, "_txt_LOLS");
			char buffer[4]; //change into more sutible number
			sprintf(buffer, "%d", i);
			strcat(args[i].destfile, buffer);
		}
		FILE *fd = fopen(args[i].destfile, "wb+");
		if(fd == NULL){
			printf("Error opening file after creation\n");
		}
		printf("destfile: %s\n", args[i].destfile);
		args[i].filename = (char *)malloc(sizeof(char) * strlen(mainfile));
		if (i < numberofworkers){
			if (i == 0){
				strcpy(args[i].filename, mainfile);
				args[i].numofchars = numofchars + extra;
				args[i].beginIndex = index;
				args[i].numworkers = numberofworkers;
				pthread_attr_t attr;
				pthread_attr_init(&attr);
				pthread_create(&(tid[i]), &attr, compress, &args[i]);
				index += numofchars;
				index += extra;
			}
			else{
				strcpy(args[i].filename, mainfile);
				args[i].numofchars = numofchars;
				args[i].beginIndex = index;
				args[i].numworkers = numberofworkers;
				pthread_attr_t attr;
				pthread_attr_init(&attr);
				pthread_create(&(tid[i]), &attr, compress, &args[i]);
				index += numofchars;
			}
		}
		//free(args[i].destfile);
	}
	
	for(i = 0; i < numberofworkers; i++){
		pthread_join(tid[i], NULL);
	}
 	pthread_exit(0);
}


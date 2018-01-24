ALL: process thread

thread:
	gcc -c compressT_LOLS.c
	gcc -pthread -o compressT_LOLS compressT_LOLS.c

process:
	gcc -c compressR_worker_LOLS.c
	gcc -o compressR_worker_LOLS compressR_worker_LOLS.c
	gcc -c compressR_LOLS.c
	gcc -o compressR_LOLS compressR_LOLS.c

clean:
	rm ./compressR_LOLS
	rm ./compressR_worker_LOLS
	rm ./compressT_LOLS
	rm -rf *.o
	rm -rf *_txt_LOLS*
// compiled with : gcc -o memcpy memcpy.c -m32 -lm
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <sys/mman.h>
#include <math.h>

// rdstc is getting the current CPU time - we use it to check how much time it took for something to run
unsigned long long rdtsc(){
        asm("rdtsc");
}

// Just copy byte by byte from src to dst - pretty simple
char* slow_memcpy(char* dest, const char* src, size_t len){
	int i;
	for (i=0; i<len; i++) {
		dest[i] = src[i];
	}
	return dest;
}

char* fast_memcpy(char* dest, const char* src, size_t len){
	size_t i;
	// 64-byte block fast copy
	if(len >= 64){
		i = len / 64;
		len &= (64-1);
		while(i-- > 0){
			__asm__ __volatile__ (
			"movdqa (%0), %%xmm0\n"
			"movdqa 16(%0), %%xmm1\n"
			"movdqa 32(%0), %%xmm2\n"
			"movdqa 48(%0), %%xmm3\n"
			"movntps %%xmm0, (%1)\n"
			"movntps %%xmm1, 16(%1)\n"
			"movntps %%xmm2, 32(%1)\n"
			"movntps %%xmm3, 48(%1)\n"
			::"r"(src),"r"(dest):"memory");
			dest += 64;
			src += 64;
		}
	}

	// byte-to-byte slow copy
	if(len) slow_memcpy(dest, src, len);
	return dest;
}

int main(void){

	setvbuf(stdout, 0, _IONBF, 0);  // Setting stdout to have no buffer
	setvbuf(stdin, 0, _IOLBF, 0);  // Setting stdin to have no buffer

	printf("Hey, I have a boring assignment for CS class.. :(\n");
	printf("The assignment is simple.\n");

	printf("-----------------------------------------------------\n");
	printf("- What is the best implementation of memcpy?        -\n");
	printf("- 1. implement your own slow/fast version of memcpy -\n");
	printf("- 2. compare them with various size of data         -\n");
	printf("- 3. conclude your experiment and submit report     -\n");
	printf("-----------------------------------------------------\n");

	printf("This time, just help me out with my experiment and get flag\n");
	printf("No fancy hacking, I promise :D\n");

	unsigned long long t1, t2; // t1 and t2 will hold the times
	int e;  // Will be used to get memcpy amounts
	char* src;  // Will hold the src data
	char* dest;  // Memory from src will be copied to it
	unsigned int low, high;  // Just to get input from the user
	unsigned int size;  // Every size the user will insert will be inserted to it before it will be added to "sizes"
	// allocate memory

	// addr is null (the first arg) - so the kernel will choose the address
	// mmap will allocate 0x4000 bytes
	// the 3rd arg is the protection - there are 4 options:
	// #define	PROT_NONE	0x00	/* no permissions */
        // #define	PROT_READ	0x01	/* pages can be read */
        // #define	PROT_WRITE	0x02	/* pages can be written */
        // #define	PROT_EXEC	0x04	/* pages can be executed */
        // 0|1|2|4 = 7 - so the memory is rwx :)
	// MAP_PRIVATE - will make the memory copy-on-write
	// MAP_ANONYMOUS - this means the mapping is in memory, no fd, no offset - it's not backed by any file
	// the fd is -1 and the offset is 0 (because we are using MAP_ANONYMOUS)
	char* cache1 = mmap(0, 0x4000, 7, MAP_PRIVATE|MAP_ANONYMOUS, -1, 0);
	char* cache2 = mmap(0, 0x4000, 7, MAP_PRIVATE|MAP_ANONYMOUS, -1, 0);
	
	src = mmap(0, 0x2000, 7, MAP_PRIVATE|MAP_ANONYMOUS, -1, 0);  // The explanation of cache is pretty much the same, just the size is smaller

	size_t sizes[10];  // All the sizes the user gave 
	int i=0;  // Used just the way you think

	// This loop is not that intersting - just getting sizes from the user to copy each test
	// setup experiment parameters
	for(e=4; e<14; e++){	// 2^13 = 8K
		low = pow(2,e-1);
		high = pow(2,e);
		printf("specify the memcpy amount between %d ~ %d : ", low, high);
		scanf("%d", &size);
		if( size < low || size > high ){
			printf("don't mess with the experiment.\n");
			exit(0);
		}
		sizes[i++] = size;
	}

	sleep(1);
	printf("ok, lets run the experiment with your configuration\n");
	sleep(1);

	// run experiment
	for(i=0; i<10; i++){
		size = sizes[i];
		printf("experiment %d : memcpy with buffer size %d\n", i+1, size);
		dest = malloc( size );

		memcpy(cache1, cache2, 0x4000);		// to eliminate cache effect
		t1 = rdtsc();
		slow_memcpy(dest, src, size);		// byte-to-byte memcpy
		t2 = rdtsc();
		printf("ellapsed CPU cycles for slow_memcpy : %llu\n", t2-t1);

		memcpy(cache1, cache2, 0x4000);		// to eliminate cache effect
		t1 = rdtsc();
		fast_memcpy(dest, src, size);		// block-to-block memcpy
		t2 = rdtsc();
		printf("ellapsed CPU cycles for fast_memcpy : %llu\n", t2-t1);
		printf("\n");
	}

	printf("thanks for helping my experiment!\n");
	printf("flag : ----- erased in this source code -----\n");
	return 0;
}

